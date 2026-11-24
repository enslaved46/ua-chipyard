// ComplexNumAccWConfig.scala -- attaches ComplexNumAccW to the BaselineConfig core
//
// Build it with:  make CONFIG=ComplexNumAccWConfig
package chipyard

import org.chipsalliance.cde.config.{Parameters, Config}
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

// Adds the accelerator to the list of RoCC accelerators, using the custom-0 opcode.
class WithComplexNumAccW extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq(
    (p: Parameters) => {
      val acc = LazyModule(new ComplexNumAccW(OpcodeSet.custom0)(p))
      acc
    }
  )
})

// The finished design: BaselineConfig plus the accelerator.
class ComplexNumAccWConfig extends Config(
  new WithComplexNumAccW ++
  new BaselineConfig
)
