#!/bin/bash
# accel.sh -- helper to install/build/run the complex-number accelerator work.
# Run INSIDE the Chipyard container:   bash /workspace/student-work/accelerator/scripts/accel.sh <step>
#
# Steps:
#   install   copy the C tests into chipyard/tests and register them in CMakeLists.txt
#   tests     build all test binaries (software + accelerator versions)
#   sw        run the SOFTWARE test + benchmark on CourseRocketConfig (baseline numbers)
#   build-hw  build the accelerator simulator (slow the first time)   [needs Scala to compile]
#   hw        run the ACCELERATOR test + benchmark on ComplexAccelConfig
#   all-sw    install + tests + sw
set -e

SW=/workspace/student-work/accelerator
CY=/workspace/chipyard
TESTS=$CY/tests
SIM=$CY/sims/verilator
OUT=/workspace/student-work/results

load_env() { cd $CY && source env.sh >/dev/null 2>&1; }

do_install() {
  cp $SW/tests/acomplex.h $SW/tests/acomplex_test.c $SW/tests/complex_bench.c $TESTS/
  if ! grep -q "complex_bench_sw" $TESTS/CMakeLists.txt; then
    cat >> $TESTS/CMakeLists.txt << 'CM'

# ---- complex-number accelerator project (Kody) ----
add_executable(acomplex_test_sw  acomplex_test.c)
add_executable(acomplex_test_acc acomplex_test.c)
target_compile_definitions(acomplex_test_acc PRIVATE USE_ROCC)
add_executable(complex_bench_sw  complex_bench.c)
add_executable(complex_bench_acc complex_bench.c)
target_compile_definitions(complex_bench_acc PRIVATE USE_ROCC)
add_dump_target(acomplex_test_sw)
add_dump_target(acomplex_test_acc)
add_dump_target(complex_bench_sw)
add_dump_target(complex_bench_acc)
CM
    echo "Registered targets in tests/CMakeLists.txt"
  else
    echo "CMakeLists.txt already has the targets"
  fi
}

do_tests() {
  load_env
  mkdir -p $TESTS/build && cd $TESTS/build
  cmake .. && make
  ls -1 $TESTS/build/*.riscv
}

run_sim() {  # run_sim <config> <binary-name>
  load_env
  cd $SIM
  ./simulator-chipyard.harness-$1 $TESTS/build/$2.riscv
}

case "$1" in
  install)  do_install ;;
  tests)    do_tests ;;
  sw)       mkdir -p $OUT
            run_sim BaselineConfig acomplex_test_sw | tee $OUT/sw_test.txt
            run_sim BaselineConfig complex_bench_sw | tee $OUT/sw_bench.txt ;;
  build-hw) load_env; cd $SIM && make CONFIG=ComplexAccelConfig ;;
  hw)       mkdir -p $OUT
            run_sim ComplexAccelConfig acomplex_test_acc | tee $OUT/hw_test.txt
            run_sim ComplexAccelConfig complex_bench_acc | tee $OUT/hw_bench.txt ;;
  all-sw)   do_install; do_tests; "$0" sw ;;
  *) sed -n '2,12p' "$0" ;;
esac

