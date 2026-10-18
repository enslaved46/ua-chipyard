#!/bin/bash
# vec.sh - build (with Ashish's CMake) and run the vector add / multiply benchmark
# Run INSIDE the container:  bash /workspace/student-work/accelerator/scripts/vec.sh
# Sizes 50, 500, 5000, one timed pass (same as Ashish's tests).
# Software -> BaselineConfig.  Accelerator -> ComplexAccelConfig.
# Logs: /workspace/student-work/results/vec_<add|mul>_<sw|hw>_<Config>_N<size>.txt
set -e
CY=/workspace/chipyard
SW=/workspace/student-work
SIM=$CY/sims/verilator
OUT=$SW/results

cd $CY && source env.sh >/dev/null 2>&1
mkdir -p $OUT $SW/build

for cfg in BaselineConfig ComplexAccelConfig; do
  if [ ! -x $SIM/simulator-chipyard.harness-$cfg ]; then
    echo "MISSING simulator: $SIM/simulator-chipyard.harness-$cfg"
    echo "Copy it from /workspace/student-work/ into $SIM/ first."
    exit 1
  fi
done

SUMMARY=$OUT/vec_summary.txt
: > $SUMMARY

for op in add mul; do
  for n in 50 500 5000; do
    for mode in sw hw; do
      cfg=BaselineConfig
      if [ $mode = hw ]; then cfg=ComplexAccelConfig; fi
      tag=vec_${op}_${mode}_N${n}
      bdir=$SW/build/$tag
      log=$OUT/vec_${op}_${mode}_${cfg}_N${n}.txt
      echo "=== $op $mode N=$n on $cfg ==="
      if ! cmake -S $SW -B $bdir -DTEST_SELECT=vec_bench -DVEC_OP=$op -DVEC_MODE=$mode -DVEC_N=$n > $bdir.build.txt 2>&1; then
        cat $bdir.build.txt; echo "CMAKE CONFIGURE FAILED for $tag"; exit 1
      fi
      if ! cmake --build $bdir --target all >> $bdir.build.txt 2>&1; then
        cat $bdir.build.txt; echo "BUILD FAILED for $tag"; exit 1
      fi
      $SIM/simulator-chipyard.harness-$cfg $bdir/$tag.riscv 2>&1 | tee $log
      if ! grep -q "^RESULT" $log; then echo "RUN FAILED: no RESULT line in $log"; exit 1; fi
      cyc=$(awk '/^v(add|mul)_/{print $2}' $log)
      ins=$(awk '/^v(add|mul)_/{print $3}' $log)
      if [ -z "$cyc" ]; then echo "RUN FAILED: no cycle row in $log"; exit 1; fi
      mm=$(grep -o 'mismatches=[0-9]*' $log)
      echo "$op $mode N=$n cycles=$cyc instret=$ins $mm" >> $SUMMARY
      if [ $mode = sw ]; then swcyc=$cyc; fi
      if [ $mode = hw ]; then
        awk -v s=$swcyc -v h=$cyc -v op=$op -v n=$n 'BEGIN{printf "SPEEDUP %s N=%s  sw/hw = %.2fx\n", op, n, s/h}' | tee -a $SUMMARY
      fi
    done
  done
done

echo
echo "################ SUMMARY ################"
cat $SUMMARY
