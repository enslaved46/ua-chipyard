// ComplexNumAcc.scala -- RoCC accelerator: complex ADD and complex MULTIPLY
//
// HOW TO READ THE TAGS (they compare this file with the course ROCC.scala):
//   // KEPT     = same as ROCC.scala (the standard RoCC pattern)
//   // RENAMED  = same idea as ROCC.scala, only the name changed
//   // CHANGED  = same job as in ROCC.scala but done differently here
//   // NEW      = not in ROCC.scala; this is our complex-number logic
//   // REMOVED  = something ROCC.scala has that we do not need
//
// THE FIELDS OF OUR INSTRUCTION (one 32-bit instruction, split into boxes):
//   opcode  = custom-0 (0001011)  "this one is for the accelerator"
//   rd      = DESTINATION register: where the answer goes
//   rs1     = SOURCE register 1: holds the first complex number
//   rs2     = SOURCE register 2: holds the second complex number
//   funct3  = 7 = binary 111 = three switches xd, xs1, xs2, all ON:
//               xd  = 1 : send an answer back (to rd)
//               xs1 = 1 : use the value in rs1
//               xs2 = 1 : use the value in rs2
//             funct3 says WHICH REGISTERS the instruction uses. It does NOT choose the operation.
//   funct7  = which OPERATION to do (separate box from funct3):
//               0 = ACMUL, 1 = AADD
//
// Instructions (custom-0 opcode, funct3 = 7, uses rd, rs1, rs2):
//   funct7 = 0 : ACMUL  rd = rs1 * rs2   complex multiply
//   funct7 = 1 : AADD   rd = rs1 + rs2   complex add
// Operand format (same as complex.h): real = bits 31..16, imag = bits 15..0,
// both signed 16-bit. Results wrap (low 16 bits kept).
package chipyard                                     // KEPT

import chisel3._                                     // KEPT
import chisel3.util._                                // KEPT
import org.chipsalliance.cde.config.Parameters       // CHANGED: ROCC.scala also imports Field and Config here; we do not need them in this file
import freechips.rocketchip.tile._                   // KEPT
import freechips.rocketchip.diplomacy._              // KEPT
// REMOVED: ROCC.scala also imports the rocket memory classes (HellaCacheIO, M_XRD, ...),
//          the tilelink classes, and IntParam. We never touch memory, so we skip them.

// RENAMED: ROCC.scala calls this ECEx62Accumulator.
// CHANGED: ROCC.scala has an extra parameter "val n: Int = 4" (the number of accumulator slots). We have no slots, so no n.
class ComplexNumAcc(opcodes: OpcodeSet)(implicit p: Parameters) extends LazyRoCC(opcodes) {
  override lazy val module = new ComplexNumAccModuleImp(this)   // RENAMED
}

// RENAMED: ROCC.scala calls this ECEx62AccumulatorModuleImp.
// CHANGED: ROCC.scala also says "with HasCoreParameters" (it needs xLen for its storage). We have no storage, so we drop it.
class ComplexNumAccModuleImp(outer: ComplexNumAcc)(implicit p: Parameters)
    extends LazyRoCCModuleImp(outer) {

  // REMOVED: val regfile = Mem(outer.n, UInt(xLen.W))   (the 4 accumulator slots)
  // REMOVED: val busy = RegInit(VecInit(...))           (the "waiting for memory" flags)

  // NEW: two states, waiting for a command (sIdle) or holding an answer to send back (sResp)
  val sIdle :: sResp :: Nil = Enum(2)
  val state  = RegInit(sIdle)
  val rdReg  = Reg(UInt(5.W))    // NEW: remembers which register gets the answer
  val resReg = Reg(UInt(32.W))   // NEW: holds the packed 32-bit answer

  val cmd   = io.cmd             // CHANGED: ROCC.scala uses Queue(io.cmd) (a small waiting line); we use the command directly
  val funct = cmd.bits.inst.funct   // KEPT: the funct7 field (0 = multiply, 1 = add here)

  // REMOVED: addr, doWrite, doRead, doLoad, doAccum, memRespTag (the accumulator's four operations)

  // NEW: unpack the operands. x = a + bj (from rs1), y = c + dj (from rs2)
  //      (ROCC.scala only uses rs1 as one plain number, "addend")
  val x = cmd.bits.rs1(31, 0)
  val y = cmd.bits.rs2(31, 0)
  val a = x(31, 16).asSInt   // NEW: real part of the first number
  val b = x(15, 0).asSInt    // NEW: imaginary part of the first number
  val c = y(31, 16).asSInt   // NEW: real part of the second number
  val d = y(15, 0).asSInt    // NEW: imaginary part of the second number

  // NEW: multiply, (ac - bd) + (ad + bc)j
  //      (ROCC.scala's datapath is only "accum + addend")
  val ac = a * c
  val bd = b * d
  val ad = a * d
  val bc = b * c
  val mulRe = (ac - bd)(15, 0)   // keep low 16 bits (wraparound)
  val mulIm = (ad + bc)(15, 0)

  // NEW: add, (a + c) + (b + d)j
  val addRe = (a + c).asUInt
  val addIm = (b + d).asUInt

  val isMul = funct === 0.U      // CHANGED: ROCC.scala compares funct to 0,1,2,3 for write/read/load/accumulate; we only need 0 = multiply
  val outRe = Mux(isMul, mulRe, addRe)   // NEW: pick multiply or add
  val outIm = Mux(isMul, mulIm, addIm)

  // CHANGED: ROCC.scala's cmd.ready depends on memory/response stalls.
  //          Ours is simpler: accept a new command only when idle.
  cmd.ready := state === sIdle
  when(cmd.fire) {                       // KEPT: "fire" = command valid and accelerator ready
    rdReg  := cmd.bits.inst.rd           // NEW: remember where the answer goes
    resReg := Cat(outRe, outIm)          // NEW: pack, real on top, imaginary on bottom
    state  := sResp
  }

  // CHANGED: same three response signals as ROCC.scala (valid, rd, data), different values
  io.resp.valid     := state === sResp   // ROCC.scala: cmd.valid && doResp && no stalls
  io.resp.bits.rd   := rdReg             // ROCC.scala: cmd.bits.inst.rd
  io.resp.bits.data := resReg            // ROCC.scala: accum (the old stored value). Ours is the computed answer
  when(io.resp.fire) { state := sIdle }  // NEW: answer delivered, back to idle

  io.busy      := state =/= sIdle        // CHANGED: ROCC.scala: cmd.valid || any slot waiting on memory
  io.interrupt := false.B                // KEPT

  // CHANGED: ROCC.scala fills in a full memory request (address, tag, load command, size...).
  //          We never use memory, so we turn the request off.
  io.mem.req.valid := false.B
  io.mem.req.bits  := DontCare           // avoids an "unconnected wire" build error
}
