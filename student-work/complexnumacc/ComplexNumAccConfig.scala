// ComplexNumAccConfig.scala -- puts ComplexNumAcc on top of the course BaselineConfig
//
// TAGS compare this file with the bottom of the course ROCC.scala:
//   // KEPT = same as ROCC.scala    // RENAMED = only the name changed    // CHANGED = different on purpose
package chipyard                                           // KEPT

import org.chipsalliance.cde.config.{Parameters, Config}   // KEPT (ROCC.scala also imports Field)
import freechips.rocketchip.tile._                         // KEPT
import freechips.rocketchip.diplomacy._                    // KEPT

// RENAMED: ROCC.scala calls this WithAccumulator.
// KEPT: the same pattern, "add one more accelerator to Rocket's RoCC list".
// CHANGED: ROCC.scala lets you pass an opcode (op: OpcodeSet = OpcodeSet.custom0); we just fix it to custom0.
class WithComplexNumAcc extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq(                  // KEPT
    (p: Parameters) => {
      val acc = LazyModule(new ComplexNumAcc(OpcodeSet.custom0)(p))   // RENAMED (ROCC.scala: new ECEx62Accumulator(op, n = 4)(p))
      acc
    }
  )
})

// RENAMED: ROCC.scala calls this ROCCTest.
// CHANGED: ROCCTest = WithAccumulator ++ WithNHugeCores(1) ++ AbstractConfig  (a "huge" Rocket core).
//          Ours = WithComplexNumAcc ++ BaselineConfig, so the core is exactly the one in
//          our software baseline and the cycle counts can be compared fairly.
// Build it with:  make CONFIG=ComplexNumAccConfig
class ComplexNumAccConfig extends Config(
  new WithComplexNumAcc ++
  new BaselineConfig
)
