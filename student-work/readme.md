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
    - complex_arithematic_type.c : scalar add and mul test using type defined in comple.h , use this program 



# CMakee Build Commands and istructions 

## Vector Add
- `cmake -S ./ -B ./rocc_complex_acc/build-pure-sw/ -DTEST_SELECT=vector_add`
- `cmake --build ./rocc_complex_acc/build-pure-sw/ --target-all`


## Vector MUL
- `cmake -S ./ -B ./rocc_complex_acc/build-pure-sw/ -DTEST_SELECT=vector_add`
- `cmake --build ./rocc_complex_acc/build-pure-sw/ --target-all`


## Simple complex arithemetic and funciton check
### this function needs update, i've not used complex type
- `cmake -S ./ -B ./rocc_complex_acc/build-pure-sw/ -DTEST_SELECT=complex_arith`
- `cmake --build ./rocc_complex_acc/build-pure-sw/ --target all`


## Accelerator SCALAR CADD
- `cmake -S ./ -B ./rocc_complex_acc/build-rocc-acc/ -DTEST_SELECT=rocc_cmplx_scalar_add`

## Clean traget
- `cmake --build ./rocc_complex_acc/build-pure-sw/ --target clean`

## executable is producedin ./build_riscv dir

## to run executable, example for add
- `/workspace/chipyard/sims/verilator/simulator-chipyard.harness-BaselineConfig  /workspace/student-work/rocc_complex_acc/build-pure-sw/cmplx_v_add.riscv`

