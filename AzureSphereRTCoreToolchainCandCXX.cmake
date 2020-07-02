SET(CMAKE_SYSTEM_NAME Generic)

MESSAGE("Version C++ 3.1")

# Get sdk and cmake dir from environment set from toolchain location
IF(DEFINED CMAKE_TOOLCHAIN_FILE)
    STRING(REPLACE "\\" "/" CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
    STRING(FIND ${CMAKE_TOOLCHAIN_FILE} "/" AZURE_SPHERE_CMAKE_PATH_END REVERSE)
    STRING(SUBSTRING ${CMAKE_TOOLCHAIN_FILE} 0 ${AZURE_SPHERE_CMAKE_PATH_END} AZURE_SPHERE_CMAKE_PATH)
    STRING(FIND ${AZURE_SPHERE_CMAKE_PATH} "/" AZURE_SPHERE_SDK_PATH_END REVERSE)
    STRING(SUBSTRING ${AZURE_SPHERE_CMAKE_PATH} 0 ${AZURE_SPHERE_SDK_PATH_END} AZURE_SPHERE_SDK_PATH)
    SET(ENV{AzureSphereCMakePath} ${AZURE_SPHERE_CMAKE_PATH})
    SET(ENV{AzureSphereSDKPath} ${AZURE_SPHERE_SDK_PATH})
ENDIF()
SET(AZURE_SPHERE_CMAKE_PATH $ENV{AzureSphereCMakePath})
SET(AZURE_SPHERE_SDK_PATH $ENV{AzureSphereSDKPath} CACHE INTERNAL "Path to the Azure Sphere SDK")

INCLUDE("${AZURE_SPHERE_CMAKE_PATH}/AzureSphereToolchainBase.cmake")

IF(DEFINED ARM_GNU_PATH)
    STRING(REPLACE "\\" "/" ARM_GNU_PATH ${ARM_GNU_PATH})
    STRING(REGEX REPLACE "/$" "" ARM_GNU_PATH ${ARM_GNU_PATH})
    STRING(REGEX MATCH "bin$" ARM_GNU_PATH_IS_BIN ${ARM_GNU_PATH})
    IF("${ARM_GNU_PATH_IS_BIN}" STREQUAL "")
        SET(ENV{ArmGnuBasePath} ${ARM_GNU_PATH})
        SET(ENV{ArmGnuBinPath} "${ARM_GNU_PATH}/bin")
    ELSE()
        STRING(FIND ${ARM_GNU_PATH} "/" ARM_GNU_PATH_END REVERSE)
        STRING(SUBSTRING ${ARM_GNU_PATH} 0 ${ARM_GNU_PATH_END} ARM_GNU_BASE_PATH)
        SET(ENV{ArmGnuBasePath} ${ARM_GNU_BASE_PATH})
        SET(ENV{ArmGnuBinPath} ${ARM_GNU_PATH})
    ENDIF()
ENDIF()
SET(ARM_GNU_BIN_PATH $ENV{ArmGnuBinPath})
SET(ARM_GNU_BASE_PATH $ENV{ArmGnuBasePath} CACHE INTERNAL "Path to the ARM embedded toolset")

SET(ENV{PATH} "${AZURE_SPHERE_SDK_PATH}/Tools;${ARM_GNU_BIN_PATH};$ENV{PATH}")
SET(CMAKE_FIND_ROOT_PATH "${ARM_GNU_BASE_PATH}")

# Set up compiler and flags
IF(${CMAKE_HOST_WIN32})
    SET(CMAKE_CXX_COMPILER "${ARM_GNU_BIN_PATH}/arm-none-eabi-g++.exe" CACHE INTERNAL "Path to the C++ compiler in the ARM embedded toolset targeting Real-Time Core")
    SET(CMAKE_C_COMPILER "${ARM_GNU_BIN_PATH}/arm-none-eabi-gcc.exe" CACHE INTERNAL "Path to the C compiler in the ARM embedded toolset targeting Real-Time Core")
    SET(CMAKE_AR "${ARM_GNU_BIN_PATH}/arm-none-eabi-ar.exe" CACHE INTERNAL "Path to the AR compiler in the ARM embedded toolset targeting Real-Time Core")
ELSE()
    MESSAGE(FATAL_ERROR "Building on non-Windows is not yet supported")
ENDIF()

SET(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mfloat-abi=soft -march=armv7e-m -mthumb")
SET(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m4 -mfloat-abi=soft -march=armv7e-m -mthumb")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs --specs=nano.specs -nostartfiles -Wl,--no-undefined -Wl,-n -Wl,--allow-multiple-definition -Wl,--verbose -T \"${CMAKE_SOURCE_DIR}/linker.ld\"")

FILE(GLOB ARM_GNU_INCLUDE_PATH "${ARM_GNU_BASE_PATH}/lib/gcc/arm-none-eabi/*/include")
SET(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES "${ARM_GNU_INCLUDE_PATH}" "${ARM_GNU_BASE_PATH}/arm-none-eabi/include")
SET(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES "${ARM_GNU_INCLUDE_PATH}" "${ARM_GNU_BASE_PATH}/arm-none-eabi/include")
SET(COMPILE_DEBUG_FLAGS $<$<CONFIG:Debug>:-g2> $<$<CONFIG:Debug>:-gdwarf-2> $<$<CONFIG:Debug>:-Og>)
SET(COMPILE_RELEASE_FLAGS $<$<CONFIG:Release>:-g1> $<$<CONFIG:Release>:-O3>)

#ADD_COMPILE_OPTIONS(-ffunction-sections)
ADD_COMPILE_OPTIONS(-Wall)
ADD_COMPILE_OPTIONS(-flto)
ADD_COMPILE_OPTIONS(-ffat-lto-objects)

ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:C>:-std=c11>)
 
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-xc++>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-lstdc++>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-shared-libgcc>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
#ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables>)
#ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-g>)

ADD_COMPILE_OPTIONS(${COMPILE_DEBUG_FLAGS})
ADD_COMPILE_OPTIONS(${COMPILE_RELEASE_FLAGS})

