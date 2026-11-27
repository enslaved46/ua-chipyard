#!/usr/bin/env bash
set -e

ulimit -s unlimited
mkdir -p ./rocc_complex_acc/result

SIM_HW="/workspace/chipyard/sims/verilator/simulator-chipyard.harness-ComplexRoCCConfig"
SIM_SW="/workspace/chipyard/sims/verilator/simulator-chipyard.harness-BaselineConfig"

for VEC_SIZE in 50 500 5000; do
    echo "============================================================"
    echo " Building & Simulating MAX_VECT = ${VEC_SIZE}"
    echo "============================================================"

    #ake here
    # Configure all 4 targets with MAX_VECT=${VEC_SIZE} and DEBUG_PRINT=0
    cmake -S ./ -B ./rocc_complex_acc/build-sw-add/   -DTEST_SELECT=sw_cmplx_add   -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null
    cmake -S ./ -B ./rocc_complex_acc/build-sw-mul/   -DTEST_SELECT=sw_cmplx_mul   -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null
    cmake -S ./ -B ./rocc_complex_acc/build-sw-mac/   -DTEST_SELECT=sw_cmplx_mac   -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null
    
    cmake -S ./ -B ./rocc_complex_acc/build-rocc-add/ -DTEST_SELECT=rocc_cmplx_add -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null
    cmake -S ./ -B ./rocc_complex_acc/build-rocc-mul/ -DTEST_SELECT=rocc_cmplx_mul -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null
    cmake -S ./ -B ./rocc_complex_acc/build-rocc-mac/ -DTEST_SELECT=rocc_cmplx_mac -DMAX_VECT=${VEC_SIZE} -DDEBUG_PRINT=0 > /dev/null

    # Compile all 6binaries
    make -j$(nproc) -C ./rocc_complex_acc/build-sw-add/   > /dev/null
    make -j$(nproc) -C ./rocc_complex_acc/build-sw-mul/   > /dev/null
    make -j$(nproc) -C ./rocc_complex_acc/build-sw-mac/   > /dev/null
    
    make -j$(nproc) -C ./rocc_complex_acc/build-rocc-add/ > /dev/null
    make -j$(nproc) -C ./rocc_complex_acc/build-rocc-mul/ > /dev/null
    make -j$(nproc) -C ./rocc_complex_acc/build-rocc-mac/ > /dev/null

    # Run all 6simulations in parallel on your 32-core host
    stdbuf -oL ${SIM_SW} ./rocc_complex_acc/build-sw-add/sw_cmplx_add.riscv     > ./rocc_complex_acc/result/${VEC_SIZE}_add_sw.txt &
    stdbuf -oL ${SIM_HW} ./rocc_complex_acc/build-rocc-add/rocc_cmplx_add.riscv > ./rocc_complex_acc/result/${VEC_SIZE}_add_hw.txt &
    echo " Running ADD"
    wait
    stdbuf -oL ${SIM_SW} ./rocc_complex_acc/build-sw-mul/cmplx_sw_mul.riscv     > ./rocc_complex_acc/result/${VEC_SIZE}_mul_sw.txt &
    stdbuf -oL ${SIM_HW} ./rocc_complex_acc/build-rocc-mul/rocc_cmplx_mul.riscv > ./rocc_complex_acc/result/${VEC_SIZE}_mul_hw.txt &
    echo " Running MUL"
    wait
    stdbuf -oL ${SIM_SW} ./rocc_complex_acc/build-sw-mac/sw_cmplx_mac.riscv     > ./rocc_complex_acc/result/${VEC_SIZE}_mac_sw.txt &   
    stdbuf -oL ${SIM_HW} ./rocc_complex_acc/build-rocc-mac/rocc_cmplx_mac.riscv > ./rocc_complex_acc/result/${VEC_SIZE}_mac_hw.txt &
    echo " Running MAC"

    wait

    echo "Done MAX_VECT = ${VEC_SIZE}! Results saved in ./rocc_complex_acc/result/"
done
