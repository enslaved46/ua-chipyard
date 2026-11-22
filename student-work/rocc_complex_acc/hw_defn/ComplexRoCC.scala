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

  val doScalarMul           = funct === 1.U
  val doRdScalarMULImagComp = funct === 2.U

  val doMAC           = funct === 3.U  // replies with real
  val doRdMACImag     = funct === 4.U 
  val doMACclr        = funct === 5.U  //
  val macReg = 1.U(7.W)
  // val doRdMACReal     = funct === 5.U


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

  // internal register
  val accRealReg    = RegInit(0.S(64.W))  // 33 for complex mul and log2(4) = 35 bits for accumulator for 4 complex numx
  val accImagReg    = RegInit(0.S(64.W))

  // z1 = a+bi                         //
  // z2 = c+di                        //
  // z3 = z1+ze = (a + bi) + (c + di)  //
  //                                  //
  // ADD                              //
  //                                  //

  private def complexAdd(a: SInt, b: SInt, c: SInt, d: SInt, outputWidth : Int): UInt = {
    // add prodce wrap around
    val addRealRes   = (a +& c)   //  +&  prouuces extra bit, carry over, we don't do carry over, we wrap around
    val addImagRes   = (b +& d)
    // generic witdh
    val addResult   = Cat(addRealRes(outputWidth-1,0).asUInt, addImagRes(outputWidth-1,0).asUInt) // pack two 17 bit signed  number as unsigned because, we don't want sign extension of real to 64 bits, might cause some shifting opearation errors
    addResult // this is same as return addResult in c
                    // val scalarAddResult  = packAddRes.pad(xLen)
  }

  val scalarAddResult = complexAdd(a,b,c,d,17)

  private def complexMul16(a: SInt, b: SInt, c: SInt, d: SInt): (SInt,SInt) = { // two returns
                                                                                // Qm.n × Qj.q =Q(m+j).(n+q)
                                                                                // Qm.n+Qj.q=Q(max(m,j)+1).max(n,q)

    val mulRealRes   = (a * c) -& (b * d)   //  +&  prouuces extra bit 33 bits
    val mulImagRes   = (a * d) +& (b * c)  // 33 bits
    (mulRealRes, mulImagRes) // returned tuples
                             // val packMulRes   = Cat(mulRealRes(32,0), mulImagRes(32,0))
                             // val scalarMulResult  = packMulRes
  }

  val (mulRealResult, mulImagResult)       = complexMul16(a, b, c, d)


  // create registers
  val savedMulImag  = RegInit(0.S(33.W))
  val savedMulValid = RegInit(false.B)

  // cmd.valid is high when we recive a vlaid command int the accelerator
  // cmd.ready is hight when acc is ready to take new commands
  // cmd.fire is high when both of the cmd.valid and cmd.reayd is high
  //we have need to wait one more clock, idk i hope the output comes right away just with the comibnational delay
  // in c sim, output came out in one clk
  when(cmd.fire && doScalarMul) {
    savedMulImag  := mulImagResult
    savedMulValid := true.B
  }

  object State extends ChiselEnum {
    val sIdle, sColdStrt, sExec, sClrAccum = Value
  }

  val state = RegInit(State.sIdle)  // init state with idle
  val macOp = RegInit(false.B)

  // sequential
  val (mulRealMAC, mulImagMAC) = complexMul16(a, b, c, d)
  val pkdAccum                 = complexAdd(accRealReg, accImagReg, mulRealMAC, mulImagMAC, outputWidth = 64)

  val accReal = Mux(!macOp, mulRealMAC.pad(64), pkdAccum(127, 64).asSInt)
  val accImag = Mux(!macOp, mulImagMAC.pad(64), pkdAccum(63, 0).asSInt)

  when(cmd.fire){
    when(doMACclr){
      accImagReg := 0.S
      accRealReg := 0.S
      // pkdAccum   := 0.S
      macOp      := false.B  // keep sending
    }.elsewhen (doMAC){
        accImagReg := accImag
        accRealReg := accReal
        macOp := true.B
      }.elsewhen (doRdMACImag){
        macOp := false.B  // keep sending
      }
    }


  //
  // setup response
  val doResp    = cmd.bits.inst.xd // && !doMAC
  val stallResp = doResp && !io.resp.ready
  //val stallLoad = !io.mem.req.ready  // mem unused


  //
  // prepare response to the processor, set IO bus
  //
  // i don't know what it does
  cmd.ready         :=  !stallResp // && !stallLoad  use this if we write to a mem
  // only pass the response when processor has insturcted us to and the op code is for us
  io.resp.valid     := cmd.valid && cmd.bits.inst.xd  //same as cmd.fire
  io.resp.bits.rd   := cmd.bits.inst.rd  // take destination given from OP code


  // 10.U          // UInt value 10, width inferred
  // 10.U(8.W)     // 8-bit UInt value 10
  // (-10).S       // Signed value -10
  // (-10).S(16.W) // 16-bit signed value -10

  // io.busy := cmd.valid || busy.reduce(_||_)
 val acceleratorOut = MuxLookup(
    funct,                  // selector      with funt out <= addRees when "000" like this in vhdl
    0.U(xLen.W)             // defualt mux val,  64 bit 0 out if funct is not sensible is given
  )(Seq(
    0.U -> scalarAddResult.pad(xLen),         //  0.U -> scalarAddResult.pad(xLen) key value pair , when unsided 0 is given in funct
    1.U -> mulRealResult.pad(xLen).asUInt,
    2.U -> savedMulImag.pad(xLen).asUInt,
    3.U -> accReal.asUInt,
    4.U -> accImagReg.asUInt // simdMulResult.pad(xLen)
  ))

  // seelect output based on the funct, opcode
  io.resp.bits.data :=   acceleratorOut      //addResult.pad(xLen) // comes from has coreparameter, resolves to 64 bits
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
