# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# ********** ATIVE as duas linhas abaixo para fazer um debug consistente **********
# set(CMAKE_BUILD_TYPE "Debug")
# set(PICO_DEOPTIMIZED_DEBUG 1)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(estacao_meteorologica_lora C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(estacao_meteorologica_lora
        main.c
        src/aq_data_ad.c
        src/aq_data_bmep280.c
        src/aq_data_gps.c
        src/aq_data_lux.c
        src/aq_data.c
        src/buttons_and_leds.c
        src/fcnt.c
        src/est_config.c
        src/hw_sleep.c
        src/menu_conf.c
        src/wrap_watchdog.c
        src/wcm.c
        src/storage.c
        )

pico_set_program_name(estacao_meteorologica_lora "estacao_meteorologica_lora")
pico_set_program_version(estacao_meteorologica_lora "0.1")

# no_flash means the target is to run from RAM
# pico_set_binary_type(estacao_meteorologica_lora no_flash)

# ********** Configuração default da estação **********
pico_enable_stdio_uart(estacao_meteorologica_lora 0)
pico_enable_stdio_usb(estacao_meteorologica_lora 1)

# ********** Configuração alternativa da estação para utilizar a UART **********
# Obs.: quando a UART está ativa deve-se obrigatoriamente habilitar o flag: PICO_WITH_DEBUG_PROBE
# pico_enable_stdio_uart(estacao_meteorologica_lora 1)
# pico_enable_stdio_usb(estacao_meteorologica_lora  1)

# Add the standard library to the build
target_link_libraries(estacao_meteorologica_lora
        pico_stdlib
        hardware_adc
        hardware_i2c
        )

# Add the standard include files to the build
target_include_directories(estacao_meteorologica_lora PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(estacao_meteorologica_lora)



# === GENERATE AUTOMATIC DOCUMENTATION (OPTIONAL) ===

# Creates an option for the user to decide whether to generate documentation
# option(GENERATE_DOCUMENTATION "Generate project documentation using Doxygen" OFF)
option(GENERATE_DOCUMENTATION "Generate project documentation using Doxygen" ON)


# Try to find Doxygen installed
find_package(Doxygen)

if (DOXYGEN_FOUND AND GENERATE_DOCUMENTATION)
    message(STATUS "Doxygen found. Configuring documentation generation...")

    # Defines the configuration files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Creates the documentation target
    add_custom_target(doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen..."
        VERBATIM
    )
    
    # Makes the documentation be generated only after compiling the project
    add_dependencies(estacao_meteorologica_lora doc_doxygen)
else()
    if(GENERATE_DOCUMENTATION)
        message(WARNING "Doxygen not found! Documentation will not be generated")
    endif()
endif()

