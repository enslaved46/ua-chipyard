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
# Conditional Build
#################################

# ------------------------------------------------------------------------------
# 1. Set defaults if not passed via -D flags on the command line
# ------------------------------------------------------------------------------
if(NOT DEFINED MAX_VECT)
    set(MAX_VECT 10)
endif()

if(NOT DEFINED DEBUG_PRINT)
    set(DEBUG_PRINT 0)
endif()

# Print active settings during 'cmake' configuration so you can verify them
message(STATUS "Benchmark Settings -> MAX_VECT = ${MAX_VECT}, DEBUG_PRINT = ${DEBUG_PRINT}")

# Pass these as compiler preprocessor macros (-DMAX_VECT=... -DDEBUG_PRINT=...)
add_compile_definitions(
    MAX_VECT=${MAX_VECT}
    DEBUG_PRINT=${DEBUG_PRINT}
)



###########################################
# COMPLEX ARITHMETIC TARGET SOFTWARE ADD
###########################################
if(TEST_SELECT STREQUAL "sw_cmplx_add")
    message(STATUS "Configuring target: complex_add software baseline")
    add_executable(sw_cmplx_add rocc_complex_acc/software_imp/complex_sw_add_test.c)
    add_dump_target(sw_cmplx_add)

#################################
# MULTIPLICATION TARGET SOFTWARE MUL
#################################
elseif(TEST_SELECT STREQUAL "sw_cmplx_mul")
    message(STATUS "Configuring target: cmplx_sw_mul baseline")
    add_executable(cmplx_sw_mul rocc_complex_acc/software_imp/complex_sw_mul_test.c)
    add_dump_target(cmplx_sw_mul)


#################################
# SW  MAC
#################################
elseif(TEST_SELECT STREQUAL "sw_cmplx_mac")
    message(STATUS "Configuring target:  sw_cmplx_mac")
    add_executable(sw_cmplx_mac  rocc_complex_acc/software_imp/complex_sw_mac_test.c)
    add_dump_target(sw_cmplx_mac)
 

#################################
# ROCC ACCELERATOR ADD 
#################################
elseif(TEST_SELECT STREQUAL "rocc_cmplx_add")
    message(STATUS "Configuring target: rocc_cmplx_add")
    add_executable(rocc_cmplx_add  rocc_complex_acc/sw_on_acc/complex_add_test.c)
    add_dump_target(rocc_cmplx_add)

#################################
# ROCC ACCELERATOR MUL
#################################
elseif(TEST_SELECT STREQUAL "rocc_cmplx_mul")
    message(STATUS "Configuring target: rocc_cmplx_mul")
    add_executable(rocc_cmplx_mul  rocc_complex_acc/sw_on_acc/complex_mul_test.c)
    add_dump_target(rocc_cmplx_mul)

#################################
# ROCC ACCELERATOR MAC
#################################
elseif(TEST_SELECT STREQUAL "rocc_cmplx_mac")
    message(STATUS "Configuring target: rocc_cmplx_mac")
    add_executable(rocc_cmplx_mac  rocc_complex_acc/sw_on_acc/complex_mac_test.c)
    add_dump_target(rocc_cmplx_mac1)

  
#################################
# for FIR and other Test Type add here    
#################################

#################################
# ERROR CATCH: 
#################################
else()
    message(FATAL_ERROR "Unknown TEST_SELECT value: '${TEST_SELECT}'. Please use rocc_cmplx_add, rocc_cmplx_mul, sw_cmplx_mul, sw_cmplx_add or rocc_cmplx_mac")
endif()


# Add custom command to generate spiflash.img from spiflash.py
