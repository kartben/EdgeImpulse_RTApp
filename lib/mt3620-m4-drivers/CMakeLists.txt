#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required(VERSION 3.11)

set(TARGET_NAME "mt3620")
project(${TARGET_NAME})

add_library (
  ${TARGET_NAME} OBJECT
  ${CMAKE_CURRENT_LIST_DIR}/VectorTable.c
  ${CMAKE_CURRENT_LIST_DIR}/GPT.c
  ${CMAKE_CURRENT_LIST_DIR}/GPIO.c
  ${CMAKE_CURRENT_LIST_DIR}/ADC.c
  ${CMAKE_CURRENT_LIST_DIR}/UART.c
  ${CMAKE_CURRENT_LIST_DIR}/Print.c
  ${CMAKE_CURRENT_LIST_DIR}/I2S.c
  ${CMAKE_CURRENT_LIST_DIR}/I2CMaster.c
  ${CMAKE_CURRENT_LIST_DIR}/SPIMaster.c
)

target_include_directories (${TARGET_NAME}
  PUBLIC  ./
  PRIVATE	./mt3620
)

#target_link_libraries (${TARGET_NAME})