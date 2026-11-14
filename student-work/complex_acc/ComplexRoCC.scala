package chipyard

import org.chipsalliance.cde.config.{Field, Parameters, Config}
import chisel3._
import chisel3.util._
import chisel3.experimental.IntParam

import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

import freechips.rocketchip.rocket.{
  MStatus, HellaCacheIO, TLBPTWIO, CanHavePTW, CanHavePTWModule,
  SimpleHellaCacheIF, M_XRD, PTE, PRV, M_SZ
}
import freechips.rocketchip.tilelink.{
  TLNode, TLIdentityNode, TLClientNode, TLMasterParameters, TLMasterPortParameters
}

import chipyard.{TestSuitesKey, TestSuiteHelper}

// class ECEx62Accumulator(opcodes: OpcodeSet, val n: Int = 4)(implicit p: Parameters) extends LazyRoCC(opcodes) {
//   override lazy val module = new ECEx62AccumulatorModuleImp(this)
// }

// needed for integration , opcodes routing and system connect
// this attaches to the Rocket CHip, wrapper to connect to the processor
class ComplexRoCC(opcodes: OpcodeSet)(implicit p: Parameters)    extends LazyRoCC(opcodes) {
  override lazy val module = new ComplexRoCCModuleImp(this)
}


// class ECEx62AccumulatorModuleImp(outer: ECEx62Accumulator)(implicit p: Parameters) extends LazyRoCCModuleImp(outer)
//     with HasCoreParameters {

// used for registers, handshake , contorl and arithematic
// this is our accelrator
class ComplexRoCCModuleImp(outer: ComplexRoCC)(implicit p: Parameters)    extends LazyRoCCModuleImp(outer)
    with HasCoreParameters {

  //    31          25 24       20 19       15 14  13  12 11        7 6          0
  // +--------------+-----------+-----------+---+---+---+------------+------------+
  // | funct[6:0]   |    rs2    |    rs1    | xd|xs1|xs2|     rd     |   opcode   |
  // +--------------+-----------+-----------+---+---+---+------------+------------+
  //      7 bits        5 bits      5 bits    1   1   1     5 bits       7 bits

  // opcode : for custom0, 0xB = 1011
  // rd [7-11 ] The destination CPU register
  // xs2 (bit 12, 1 bit): Set to 1 (SS and DSS).Tells the CPU to read the 64-bit value from rs2 and send it on cmd.bits.rs2.
  // xs1 (bit 13, 1 bit): Set to 1 (SS and DSS). Tells the CPU to read the 64-bit value from rs1 and send it on cmd.bits.rs1.
  // xd (bit 14, 1 bit): Set to 0 for SS (write, load, add) and 1 for DSS (read). for accum, ROCC.scala our Chisel code reads this exact bit via val doResp = cmd.bits.inst.xd.

  // rs1 (bits [19:15], 5 bits): The source CPU register ID holding your first operand (data, 0, ptr, or addend).

  // rs2 (bits [24:20], 5 bits): The source CPU register ID holding your second operand (idx).

  // funct[6:0] (bits [31:25], 7 bits): Your custom function code:

    // 0000000 (0) = accum_write
    // 0000001 (1) = accum_read
    // 0000010 (2) = accum_load
    // 0000011 (3) = accum_add


  // for scalar , we expect the opcode comes with the destination register addr

  val cmd = Queue(io.cmd)
  val funct = cmd.bits.inst.funct // [31:25] , 32 bit inst

  // val addr = cmd.bits.rs2(log2Up(outer.n)-1,0)
  val doScalarAdd = funct === 0.U
  val doScalarMul = funct === 1.U

  val doLoadVect  = funct === 2.U
  val doWriteVect = funct === 3.U

  val doVectAdd = funct === 4.U
  val doVectMul = funct === 5.U

  // comes from LazyRoccModuleImp

  // io.cmd       // commands and operands from Rocket
  // io.resp      // results returned to Rocket
  // io.mem       // optional cache/memory interface
  // io.busy      // accelerator status
  // io.interrupt // optional interrupt

  // io.cmd.bits.rs1
  // io.cmd.bits.rs2
  // io.cmd.bits.inst.funct

  
  // SInt(16.W)
  
  //----------------------------//
  // BIT SLICING FROM OP CODE   //
  //----------------------------//
  // .asSInt reinterprets 16 bits as two's complement

  val a = cmd.bits.rs1(31, 16).asSInt
  val b = cmd.bits.rs1(15, 0).asSInt

  //z2 = c+di
  val c = cmd.bits.rs2(31, 16).asSInt
  val d = cmd.bits.rs2(15, 0).asSInt

  // z1 = a+bi                         //
  // z2 = c+di                        //
  // z3 = z1+ze = (a + bi) + (c + di)  //
  //                                  //
  // ADD                              //
  //                                  //

  // add prodce wrap around
  val addRealRes   = (a+c)   //  +&  prouuces extra bit, carry over, we don't do carry over, we wrap around
  val addImagRes   = (b+d)
  val addResult = Cat(addRealRes, addImagRes)

  val doResp    = cmd.bits.inst.xd
  val stallResp = doResp && !io.resp.ready
  val stallLoad = !io.mem.req.ready



  //
  // prepare response to the processor, set IO bus
  //
  // i don't know what it does
  cmd.ready     := !stallLoad && !stallResp

  io.resp.bits.data := addResult.pad(xLen) // comes from has coreparameter, resolves to 64 bits
                                           // take destination given from OP code
                                           // io.resp.bits.rd   := cmd.bits.inst.rd

  // only pass the response when processor has insturcted us to and the op code is for us
  io.resp.valid     := cmd.valid && cmd.bits.inst.xd  //same as cmd.fire
}

//  tells Chipyard: “Add my accelerator.”
// WithComplexRoCC: reusable modification
class WithComplexRoCC(opcodes: OpcodeSet = OpcodeSet.custom0)
    extends Config((site, here, up) => {
      case BuildRoCC => up(BuildRoCC) ++ Seq((p: Parameters) =>
        LazyModule(new ComplexRoCC(opcodes)(p)))
    })

/** BaselineConfig plus the packed-complex RoCC accelerator. */
//  ComplexRoCCConfig tells Chipyard: “Build the baseline processor with my accelerator added.”
class ComplexRoCCConfig extends Config(
  new WithComplexRoCC ++  // add accelerator
    new BaselineConfig      // use baseline processor
)
