########################################################################################################################
# file:  CMakeLists.txt
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#   Build instructions:
#     cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug
#     cmake --build ./build/ --target all
#   Cleaning:
#     cmake --build ./build/ --target clean
########################################################################################################################
cmake_minimum_required(VERSION 3.10)

project(chipyard-tests LANGUAGES C CXX)


#################################
# RISCV Toolchain
#################################

set(CMAKE_SYSTEM_NAME         "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR    "riscv"   CACHE STRING "")

set(TOOLCHAIN_PREFIX    "riscv64-unknown-elf-")

set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_LINKER        "${TOOLCHAIN_PREFIX}ld")
set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP       "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size")

set(CMAKE_EXECUTABLE_SUFFIX            ".riscv")


#################################
# Flags
#################################

# CPU architecture
set(ARCH                "rv64imafd")
set(ABI                 "lp64d")
set(CMODEL              "medany")
set(ARCH_FLAGS          -march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL})

# spec
set(SPECS               "htif_nano.specs")
set(SPEC_FLAGS          -specs=${SPECS})

# linker script
set(LINKER_SCRIPT       "htif.ld")

add_compile_options(-std=gnu99)
add_compile_options(-O2 -Wall -Wextra)
add_compile_options(-fno-common -fno-builtin-printf)
add_compile_options(${ARCH_FLAGS})
add_compile_options(${SPEC_FLAGS})

add_link_options(-static)
add_link_options(${SPEC_FLAGS})
add_link_options(-T ${LINKER_SCRIPT})


#################################
# Build
#################################

#add_executable(complex_arithmetic complex_arithmetic.c)
#add_executable(cmplx_v_add model/software_imp/complex_vector_add_test.c)

#################################
# Disassembly
#################################

# Add a target to generate all disassemblies
add_custom_target(dump ALL)

# Function to add disassembly target for an executable
function(add_dump_target target_name)
    add_custom_target(${target_name}-dump
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${target_name}.dump
        COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${target_name}> > ${CMAKE_BINARY_DIR}/${target_name}.dump
        DEPENDS ${target_name}
        COMMENT "Generating disassembly for ${target_name}"
    )
    add_dependencies(${target_name}-dump ${target_name})
    add_dependencies(dump ${target_name}-dump)
endfunction()

#add_executable(complex_arithmetic complex_arithmetic.c)
#add_executable(cmplx_v_add model/software_imp/complex_vector_add_test.c)

#add_dump_target(complex_arithmetic)
#add_dump_target(cmplx_v_add)


#################################
# Conditional Build Execution
# we can use runtime build
#################################

#################################
# COMPLEX ARITHMETIC TARGET
#################################
if(TEST_SELECT STREQUAL "complex_arith")
    message(STATUS "Configuring target: complex_arithmetic for scalar arithemetic use")
    add_executable(complex_arithmetic models/software_imp/complex_vector_arithematic.c)
    add_dump_target(complex_arithmetic)

#################################
# VECTOR ADD TARGET
#################################
elseif(TEST_SELECT STREQUAL "vector_add")
    message(STATUS "Configuring target: cmplx_v_add")
    add_executable(cmplx_v_add models/software_imp/complex_vector_add_test.c)
    add_dump_target(cmplx_v_add)

#################################
# MULTIPLICATION TARGET
#################################
elseif(TEST_SELECT STREQUAL "vector_mul")
    message(STATUS "Configuring target: cmplx_v_mul")
    add_executable(cmplx_v_mul models/software_imp/complex_vector_mul_test.c)
    add_dump_target(cmplx_v_mul)

#################################
# COMPLEX VECTOR BENCHMARK (Kody): software vs accelerator, one timed pass
# usage: cmake -S ./ -B ./build/vec_add_sw_N50 -DTEST_SELECT=vec_bench -DVEC_OP=add -DVEC_MODE=sw -DVEC_N=50
#   VEC_OP = add | mul     VEC_MODE = sw | hw (hw uses ACMUL/AADD, ComplexAccelConfig only)
#################################
elseif(TEST_SELECT STREQUAL "vec_bench")
    if(NOT DEFINED VEC_N)
        set(VEC_N 50)
    endif()
    if(NOT DEFINED VEC_OP)
        set(VEC_OP add)
    endif()
    if(NOT DEFINED VEC_MODE)
        set(VEC_MODE sw)
    endif()
    set(VEC_TARGET vec_${VEC_OP}_${VEC_MODE}_N${VEC_N})
    message(STATUS "Configuring target: ${VEC_TARGET}")
    add_executable(${VEC_TARGET} accelerator/vec/vec_bench.c)
    target_compile_definitions(${VEC_TARGET} PRIVATE VEC_N=${VEC_N})
    if(VEC_OP STREQUAL "mul")
        target_compile_definitions(${VEC_TARGET} PRIVATE OP_MUL)
    endif()
    if(VEC_MODE STREQUAL "hw")
        target_compile_definitions(${VEC_TARGET} PRIVATE USE_ROCC)
    endif()
    add_dump_target(${VEC_TARGET})

#################################
# for FIR and other Test Type add here    
#################################

#################################
# ERROR CATCH: 
#################################
else()
    message(FATAL_ERROR "Unknown TEST_SELECT value: '${TEST_SELECT}'. Please use vector_add, complex_arith, or mul.")
endif()


# Add custom command to generate spiflash.img from spiflash.py
