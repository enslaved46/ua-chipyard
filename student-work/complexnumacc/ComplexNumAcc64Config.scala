// ComplexNumAcc64Config.scala -- attaches ComplexNumAcc64 to the BaselineConfig core
//
// Build it with:  make CONFIG=ComplexNumAcc64Config
package chipyard

import org.chipsalliance.cde.config.{Parameters, Config}
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

// Adds the accelerator to the list of RoCC accelerators, using the custom-0 opcode.
class WithComplexNumAcc64 extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq(
    (p: Parameters) => {
      val acc = LazyModule(new ComplexNumAcc64(OpcodeSet.custom0)(p))
      acc
    }
  )
})

// The finished design: BaselineConfig plus the accelerator.
class ComplexNumAcc64Config extends Config(
  new WithComplexNumAcc64 ++
  new BaselineConfig
)
