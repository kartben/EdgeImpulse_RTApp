#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(EdgeImpulse_RTApp)

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5+Beta2004") 

file(GLOB edge_impulse_sdk_SRC
	edge-impulse-sdk/classifier/*.cpp
	edge-impulse-sdk/dsp/kissfft/*.cpp
	edge-impulse-sdk/dsp/dct/*.cpp
	edge-impulse-sdk/tensorflow/lite/kernels/*.cc
	edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc
	edge-impulse-sdk/tensorflow/lite/kernels/internal/reference/*.cc
	edge-impulse-sdk/tensorflow/lite/micro/*.cc
	edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc
	edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc
	edge-impulse-sdk/tensorflow/lite/core/api/*.cc
	edge-impulse-sdk/dsp/memory.cpp
)

file(GLOB_RECURSE cmsis_SRC
	edge-impulse-sdk/CMSIS/DSP/Source/*.c
	edge-impulse-sdk/CMSIS/NN/Source/*.c
)

add_library(edgeimpulse OBJECT
	${edge_impulse_sdk_SRC}
	${cmsis_SRC}
)

add_subdirectory(lib/mt3620-m4-drivers)

target_include_directories(
	edgeimpulse
    PUBLIC
		${CMAKE_SOURCE_DIR}/.
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk 
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/dsp
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/classifier
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/CMSIS/Core/Include
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/CMSIS/DSP/Include
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/CMSIS/NN/Include
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/porting
		${CMAKE_SOURCE_DIR}/model-parameters
		${CMAKE_SOURCE_DIR}/tflite-model
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/third_party/flatbuffers/include/ 
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/third_party/gemmlowp/ 
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/third_party/ruy/
	PRIVATE
		${CMAKE_SOURCE_DIR}/edge-impulse-sdk/CMSIS/DSP/PrivateInclude
) 

target_compile_definitions(edgeimpulse 	PUBLIC	EIDSP_USE_CMSIS_DSP=1)
target_compile_definitions(edgeimpulse 	PUBLIC	EIDSP_SIGNAL_C_FN_POINTER=1)
target_compile_definitions(edgeimpulse 	PUBLIC	EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1)
target_compile_definitions(edgeimpulse 	PUBLIC	TF_LITE_STATIC_MEMORY>)

add_library(edgeimpulse_azsphere
	${CMAKE_SOURCE_DIR}/edge-impulse-sdk/porting/azure_sphere/debug_log.cc
	${CMAKE_SOURCE_DIR}/edge-impulse-sdk/porting/azure_sphere/ei_classifier_porting.cpp
)
target_link_libraries(edgeimpulse_azsphere PUBLIC edgeimpulse PRIVATE mt3620)

# Create executable
add_executable(${PROJECT_NAME} 
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/edge-impulse-sdk/tensorflow/lite/c/common.c
)

target_link_libraries(${PROJECT_NAME} mt3620 edgeimpulse_azsphere nosys stdc++ supc++ m c gcc)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
include("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
