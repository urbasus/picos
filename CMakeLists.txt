# Pico stuff, from examples root
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

if (NOT DEFINED PICO_BOARD)
    set(PICO_BOARD "pico_w")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_executable(picow_blink
        picow_blink.c
)

target_link_libraries(picow_blink
        pico_stdlib              # for core functionality
        pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
)

pico_add_extra_outputs(picow_blink)

# Set wifi credentials file
set_source_files_properties(wifi_connect.c PROPERTIES COMPILE_FLAGS -DWIFI_CREDENTIALS_FILE=\\\"wifi_credentials.h\\\")

add_executable(wifi_connect
        wifi_connect.c
)

target_link_libraries(wifi_connect
        pico_stdlib               # for core functionality
        pico_cyw43_arch_lwip_poll # we need Wifi to access the GPIO and... Wifi
)

target_include_directories(wifi_connect PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
)

pico_add_extra_outputs(wifi_connect)
