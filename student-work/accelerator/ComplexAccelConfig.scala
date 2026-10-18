// ComplexAccelConfig.scala -- bolts ComplexAccel onto the course baseline
// WITHOUT editing CourseRocketConfig.
//
// Check patches/clamp8_cconfig.scala for: the package line, the imports, and
// exactly how it references CourseRocketConfig. Mirror it.
package chipyard

import org.chipsalliance.cde.config.{Parameters, Config}   // older: freechips.rocketchip.config
import freechips.rocketchip.tile._
import freechips.rocketchip.diplomacy._

class WithComplexAccel extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq(
    (p: Parameters) => {
      val acc = LazyModule(new ComplexAccel(OpcodeSet.custom0)(p))
      acc
    }
  )
})

// Config you build with:  make CONFIG=ComplexAccelConfig
// (guess at the base name -- match whatever clamp8_cconfig.scala does)
class ComplexAccelConfig extends Config(
  new WithComplexAccel ++
  new BaselineConfig
)

