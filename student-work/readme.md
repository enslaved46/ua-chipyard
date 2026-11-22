# All the necessacys dependency and files are brough to this dir

# Matlab Model source code is included in
 - student-work/models/matlab_model

# RISC Baseline path
 - student-work/models/software_imp
 - types and complex function defined in
    - complex.h
 - test programs are
    - complex_vector_add_test.c
    - complex_vector_mul_test.c
    - complex_vector_arithematic.c : scalar add and mul test, dont use this
    - complex_arithematic_type.c   : scalar add and mul test using type defined in comple.h , use this program

# CMakee Build Commands and istructions

## SW Baseline Add
- `cmake -S ./ -B ./rocc_complex_acc/build-sw-add/ -DTEST_SELECT=sw_cmplx_add`

## SW Baseline MUL
- `cmake -S ./ -B ./rocc_complex_acc/build-sw-mul/ -DTEST_SELECT=sw_cmplx_mul`

## Accelerator MAC
- `cmake -S ./ -B ./rocc_complex_acc/build-sw-mac/ -DTEST_SELECT=sw_cmplx_mac`


## Accelerator ADD
- `cmake -S ./ -B ./rocc_complex_acc/build-rocc-add/ -DTEST_SELECT=rocc_cmplx_add`

## Accelerator MUL
- `cmake -S ./ -B ./rocc_complex_acc/build-rocc-mul/ -DTEST_SELECT=rocc_cmplx_mul`

## Accelerator MAC
- `cmake -S ./ -B ./rocc_complex_acc/build-rocc-mac/ -DTEST_SELECT=rocc_cmplx_mac`


## Clean traget
- `cmake --build ./rocc_complex_acc/build-pure-sw/ --target clean`

## executable is producedin ./rocc_complex_acc/

## to run executable, example for add
- `/workspace/chipyard/sims/verilator/simulator-chipyard.harness-BaselineConfig  /workspace/student-work/rocc_complex_acc/build-pure-sw/cmplx_v_add.riscv`

## Build hardware
cd /workspace/chipyard/sims/verilator
make -j$(nproc) CONFIG=ComplexRoCCConfig

## use more veliatero thread
cd /workspace/chipyard/sims/verilator
make -j32 CONFIG=ComplexRoCCConfig VERILATOR_THREADS=8
make -j32 CONFIG=BaselineConfig VERILATOR_THREADS=8

# fast sim eample use time stdbuf
time stdbuf -oL /workspace/chipyard/sims/verilator/simulator-chipyard.harness-ComplexRoCCConfig  rocc_cmplx_mul.riscv | tee ../result/10_mul_tst_hw_no_print.txt

## increase stack size
ulimit -s 65536