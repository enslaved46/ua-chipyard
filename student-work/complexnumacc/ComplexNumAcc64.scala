// ComplexNumAcc64.scala -- RoCC accelerator: 32-bit complex add / multiply plus storage slots
//
// WHAT CHANGED FROM ComplexNumAcc.scala
//   1. Wider numbers. One complex number now fills a whole 64-bit register:
//        real      = bits 63..32  (signed 32-bit)
//        imaginary = bits 31..0   (signed 32-bit)
//      (ComplexNumAcc used 16 + 16 bits inside 32 bits, so it wrapped past +/-32,767.
//       Now it only wraps past +/-2,147,483,647.)
//   2. NEW: n = 4 storage slots inside the accelerator. Each slot holds one packed
//      64-bit complex number, so a running total can stay in the accelerator.
//
// INSTRUCTIONS (custom-0 opcode, funct3 = 7 -> uses rd, rs1, rs2). funct7 picks the operation:
//   funct7 = 0 : ACMUL  rd = rs1 * rs2                          complex multiply
//   funct7 = 1 : AADD   rd = rs1 + rs2                          complex add
//   funct7 = 2 : ACWR   slot[rs2] = rs1,  rd = rs1              write a number into a slot
//   funct7 = 3 : ACRD   rd = slot[rs2]                          read a slot
//   funct7 = 4 : ACACC  slot[rs2] = slot[rs2] + rs1,  rd = the new slot value   add into a slot
// For ACWR, ACRD and ACACC the slot number is the low 2 bits of rs2 (0 to 3).
// An unknown funct7 returns 0.
package chipyard

import chisel3._
import chisel3.util._
import org.chipsalliance.cde.config.Parameters
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

// "n" is the number of storage slots (the course ROCC.scala has the same parameter, default 4).
class ComplexNumAcc64(opcodes: OpcodeSet, val n: Int = 4)(implicit p: Parameters) extends LazyRoCC(opcodes) {
  override lazy val module = new ComplexNumAcc64ModuleImp(this)
}

class ComplexNumAcc64ModuleImp(outer: ComplexNumAcc64)(implicit p: Parameters)
    extends LazyRoCCModuleImp(outer) {

  // ---- the storage: n registers, each 64 bits, all starting at 0 -------------
  val regfile = RegInit(VecInit(Seq.fill(outer.n)(0.U(64.W))))

  // ---- state: idle -> (compute on cmd) -> respond, same as ComplexNumAcc -----
  val sIdle :: sResp :: Nil = Enum(2)
  val state  = RegInit(sIdle)
  val rdReg  = Reg(UInt(5.W))     // which destination register gets the answer
  val resReg = Reg(UInt(64.W))    // the 64-bit packed answer (was 32 bits)

  val cmd   = io.cmd
  val funct = cmd.bits.inst.funct // funct7 field: picks the operation

  // ---- unpack the two operands: x = a + bj (rs1), y = c + dj (rs2) -----------
  val x = cmd.bits.rs1(63, 0)
  val y = cmd.bits.rs2(63, 0)
  val a = x(63, 32).asSInt        // real part of the first number
  val b = x(31, 0).asSInt         // imaginary part of the first number
  val c = y(63, 32).asSInt        // real part of the second number
  val d = y(31, 0).asSInt         // imaginary part of the second number

  // ---- multiply: (ac - bd) + (ad + bc)j, keep the low 32 bits of each part ---
  val ac = a * c                  // 64-bit signed
  val bd = b * d
  val ad = a * d
  val bc = b * c
  val mulRe = (ac - bd)(31, 0)
  val mulIm = (ad + bc)(31, 0)
  val mulOut = Cat(mulRe, mulIm)

  // ---- add: (a + c) + (b + d)j (32-bit add wraps) ----------------------------
  val addRe = (a + c).asUInt
  val addIm = (b + d).asUInt
  val addOut = Cat(addRe, addIm)

  // ---- storage: which slot, what it holds, and slot + rs1 --------------------
  val idxBits = log2Ceil(outer.n)
  val idx     = y(idxBits - 1, 0)          // slot number = low bits of rs2
  val slotVal = regfile(idx)               // the number stored in that slot
  val accRe   = (slotVal(63, 32).asSInt + a).asUInt
  val accIm   = (slotVal(31, 0).asSInt + b).asUInt
  val accOut  = Cat(accRe, accIm)

  // ---- decode funct7 and pick the answer -------------------------------------
  val isMul = funct === 0.U
  val isAdd = funct === 1.U
  val isWr  = funct === 2.U
  val isRd  = funct === 3.U
  val isAcc = funct === 4.U

  val result = MuxCase(0.U(64.W), Seq(
    isMul -> mulOut,
    isAdd -> addOut,
    isWr  -> x,
    isRd  -> slotVal,
    isAcc -> accOut
  ))

  // ---- handshake: accept one command when idle, answer on the next cycle -----
  cmd.ready := state === sIdle
  when(cmd.fire) {
    rdReg  := cmd.bits.inst.rd
    resReg := result
    state  := sResp
    when(isWr)  { regfile(idx) := x }       // ACWR: store rs1
    when(isAcc) { regfile(idx) := accOut }  // ACACC: store slot + rs1
  }

  io.resp.valid     := state === sResp
  io.resp.bits.rd   := rdReg
  io.resp.bits.data := resReg
  when(io.resp.fire) { state := sIdle }

  // ---- unused interfaces: no memory access, no interrupts --------------------
  io.busy      := state =/= sIdle
  io.interrupt := false.B

  io.mem.req.valid := false.B
  io.mem.req.bits  := DontCare
}
