// ComplexNumAccW.scala -- RoCC accelerator: v2 plus a multiply-accumulate that never wraps
//
// WHAT CHANGED FROM ComplexNumAcc64.scala (version 2)
//   Version 2 kept a running total as ONE complex number in a slot (two 32-bit halves),
//   so each half still wrapped at +/-2,147,483,647.
//   Version 3 (this file) adds one instruction, ACMAC, that keeps the running total as
//   TWO separate full 64-bit numbers:
//        slot 0 = real total      (a whole 64-bit signed number, no packing)
//        slot 1 = imaginary total (a whole 64-bit signed number, no packing)
//   Each product is added in at full width, so the totals stay exact far beyond 32 bits.
//   You read the totals back with ACRD, once for slot 0 and once for slot 1.
//
// INSTRUCTIONS (custom-0 opcode, funct3 = 7 -> uses rd, rs1, rs2). funct7 picks the operation:
//   funct7 = 0 : ACMUL  rd = rs1 * rs2                  complex multiply (32-bit parts, wraps)   [same as v2]
//   funct7 = 1 : AADD   rd = rs1 + rs2                  complex add (32-bit parts, wraps)        [same as v2]
//   funct7 = 2 : ACWR   slot[rs2] = rs1, rd = rs1       write a full 64-bit value into a slot    [same as v2]
//   funct7 = 3 : ACRD   rd = slot[rs2]                  read a full 64-bit slot                  [same as v2]
//   funct7 = 4 : ACACC  slot[rs2] += rs1 (two 32-bit halves), rd = new value                     [same as v2]
//   funct7 = 5 : ACMAC  NEW. product = rs1 * rs2 (complex, exact, not cut to 32 bits)
//                       slot[0] = slot[0] + real(product)     (64-bit add)
//                       slot[1] = slot[1] + imaginary(product) (64-bit add)
//                       rd = the new slot[0] (real total)
// For ACWR, ACRD and ACACC the slot number is the low 2 bits of rs2 (0 to 3).
// ACMAC always uses slots 0 and 1 and ignores the slot bits. An unknown funct7 returns 0.
package chipyard

import chisel3._
import chisel3.util._
import org.chipsalliance.cde.config.Parameters
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

// "n" is the number of storage slots. ACMAC needs at least 2 (slots 0 and 1).
class ComplexNumAccW(opcodes: OpcodeSet, val n: Int = 4)(implicit p: Parameters) extends LazyRoCC(opcodes) {
  require(n >= 2, "ComplexNumAccW needs at least 2 storage slots")
  override lazy val module = new ComplexNumAccWModuleImp(this)
}

class ComplexNumAccWModuleImp(outer: ComplexNumAccW)(implicit p: Parameters)
    extends LazyRoCCModuleImp(outer) {

  // ---- the storage: n registers, each 64 bits, all starting at 0 -------------
  val regfile = RegInit(VecInit(Seq.fill(outer.n)(0.U(64.W))))

  // ---- state: idle -> (compute on cmd) -> respond ----------------------------
  val sIdle :: sResp :: Nil = Enum(2)
  val state  = RegInit(sIdle)
  val rdReg  = Reg(UInt(5.W))     // which destination register gets the answer
  val resReg = Reg(UInt(64.W))    // the 64-bit answer

  val cmd   = io.cmd
  val funct = cmd.bits.inst.funct // funct7 field: picks the operation

  // ---- unpack the two operands: x = a + bj (rs1), y = c + dj (rs2) -----------
  val x = cmd.bits.rs1(63, 0)
  val y = cmd.bits.rs2(63, 0)
  val a = x(63, 32).asSInt        // real part of the first number
  val b = x(31, 0).asSInt         // imaginary part of the first number
  val c = y(63, 32).asSInt        // real part of the second number
  val d = y(31, 0).asSInt         // imaginary part of the second number

  // ---- products (each is a 64-bit signed number) -----------------------------
  val ac = a * c
  val bd = b * d
  val ad = a * d
  val bc = b * c

  // ---- ACMUL: keep the low 32 bits of each part (wraps, same as v2) ----------
  val mulOut = Cat((ac - bd)(31, 0), (ad + bc)(31, 0))

  // ---- AADD: 32-bit add of each part (wraps, same as v2) ---------------------
  val addOut = Cat((a + c).asUInt, (b + d).asUInt)

  // ---- storage: which slot, what it holds, and slot + rs1 (ACACC, same as v2) -
  val idxBits = log2Ceil(outer.n)
  val idx     = y(idxBits - 1, 0)          // slot number = low bits of rs2
  val slotVal = regfile(idx)               // the number stored in that slot
  val accRe   = (slotVal(63, 32).asSInt + a).asUInt
  val accIm   = (slotVal(31, 0).asSInt + b).asUInt
  val accOut  = Cat(accRe, accIm)

  // ---- ACMAC (NEW): add the exact product into slot 0 (real) and slot 1 (imag) -
  // Each slot is read as one whole signed 64-bit number and the product part is added
  // at full 64-bit width, so nothing is cut to 32 bits.
  val macRe = (regfile(0).asSInt + (ac - bd)).asUInt   // new real total
  val macIm = (regfile(1).asSInt + (ad + bc)).asUInt   // new imaginary total

  // ---- decode funct7 and pick the answer -------------------------------------
  val isMul = funct === 0.U
  val isAdd = funct === 1.U
  val isWr  = funct === 2.U
  val isRd  = funct === 3.U
  val isAcc = funct === 4.U
  val isMac = funct === 5.U

  val result = MuxCase(0.U(64.W), Seq(
    isMul -> mulOut,
    isAdd -> addOut,
    isWr  -> x,
    isRd  -> slotVal,
    isAcc -> accOut,
    isMac -> macRe
  ))

  // ---- handshake: accept one command when idle, answer on the next cycle -----
  cmd.ready := state === sIdle
  when(cmd.fire) {
    rdReg  := cmd.bits.inst.rd
    resReg := result
    state  := sResp
    when(isWr)  { regfile(idx) := x }       // ACWR: store rs1
    when(isAcc) { regfile(idx) := accOut }  // ACACC: store slot + rs1
    when(isMac) {                           // ACMAC: update both totals
      regfile(0) := macRe
      regfile(1) := macIm
    }
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
