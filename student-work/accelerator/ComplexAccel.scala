// ComplexAccel.scala  --  RoCC accelerator implementing ACMUL and AADD
//
// DRAFT: written from the standard Chipyard RoCC pattern (AccumulatorExample).
// Compare imports / package / file placement against patches/clamp8 before
// building -- Chipyard versions differ (see notes in README).
//
// Instruction set (custom-0 opcode, funct3 = 7 -> uses rd, rs1, rs2):
//   funct7 = 0 : ACMUL  rd = rs1 * rs2   complex multiply
//   funct7 = 1 : AADD   rd = rs1 + rs2   complex add
// Operand format: real = bits 31:16, imag = bits 15:0, signed 16-bit each.
// Overflow wraps (low 16 bits kept), matching the Matlab / C baseline.
package chipyard

import chisel3._
import chisel3.util._
import org.chipsalliance.cde.config.Parameters   // older Chipyard: freechips.rocketchip.config.Parameters
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

class ComplexAccel(opcodes: OpcodeSet)(implicit p: Parameters) extends LazyRoCC(opcodes) {
  override lazy val module = new ComplexAccelModuleImp(this)
}

class ComplexAccelModuleImp(outer: ComplexAccel)(implicit p: Parameters)
    extends LazyRoCCModuleImp(outer) {

  // ---- state: idle -> (compute on cmd) -> respond ----------------------
  val sIdle :: sResp :: Nil = Enum(2)
  val state  = RegInit(sIdle)
  val rdReg  = Reg(UInt(5.W))
  val resReg = Reg(UInt(32.W))

  val cmd   = io.cmd
  val funct = cmd.bits.inst.funct

  // ---- unpack operands: x = a + bj (rs1), y = c + dj (rs2) -------------
  val x = cmd.bits.rs1(31, 0)
  val y = cmd.bits.rs2(31, 0)
  val a = x(31, 16).asSInt
  val b = x(15, 0).asSInt
  val c = y(31, 16).asSInt
  val d = y(15, 0).asSInt

  // ---- multiply: (ac - bd) + (ad + bc)j  -- 4 multipliers, 1 stage ------
  val ac = a * c            // 32-bit signed
  val bd = b * d
  val ad = a * d
  val bc = b * c
  val mulRe = (ac - bd)(15, 0)   // keep low 16 bits => wraparound
  val mulIm = (ad + bc)(15, 0)

  // ---- add: (a + c) + (b + d)j  (16-bit non-expanding add wraps) --------
  val addRe = (a + c).asUInt
  val addIm = (b + d).asUInt

  val isMul = funct === 0.U
  val outRe = Mux(isMul, mulRe, addRe)
  val outIm = Mux(isMul, mulIm, addIm)

  // ---- handshake ---------------------------------------------------------
  cmd.ready := state === sIdle
  when(cmd.fire) {
    rdReg  := cmd.bits.inst.rd
    resReg := Cat(outRe, outIm)
    state  := sResp
  }

  io.resp.valid     := state === sResp
  io.resp.bits.rd   := rdReg
  io.resp.bits.data := resReg          // zero-extended to xLen
  when(io.resp.fire) { state := sIdle }

  // ---- unused interfaces (no memory access, no interrupts) ---------------
  io.busy           := state =/= sIdle
  io.interrupt      := false.B
  io.mem.req.valid  := false.B
  io.mem.req.bits   := DontCare
}

