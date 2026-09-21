cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (PamplejuceVersion)
include (CPM)
include (PamplejuceMacOS)
include (JUCEDefaults)

set(PROJECT_NAME "DJMong5ProDlayOne")
set(PRODUCT_NAME "DJ Mong5 Pro DlayOne")
set(COMPANY_NAME "DJ Mong5")
set(BUNDLE_ID "com.djmong5.prodlayone")

set(FORMATS Standalone AU VST3 AUv3)

set(CMAKE_POLICY_DEFAULT_CMP0141 NEW CACHE STRING "" FORCE)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# Desativa erros por avisos (evita que warnings de ponto flutuante quebrem o CI em qualquer SO)
if(MSVC)
    add_compile_options(/Zc:char8_t- /wd4244 /wd4305 /wd4756)
    string(REGEX REPLACE "/WX" " /WX-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/WX" " /WX-" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    foreach(lang C CXX)
        set(CMAKE_${lang}_FLAGS "${CMAKE_${lang}_FLAGS} /WX-")
    endforeach()
else()
    add_compile_options(-Wno-error -Wno-nan-infinity-disabled)
endif()

include(PamplejuceLog)
include(PamplejuceFunctions)

add_subdirectory(JUCE)
add_subdirectory(modules/clap-juce-extensions EXCLUDE_FROM_ALL)
add_subdirectory(modules/melatonin_inspector)

# 1. Criação oficial dos dados binários do JUCE para gerar o namespace BinaryData
set(BINARY_DATA_TARGET "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/images")
    file(GLOB_RECURSE ImageFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/images/*.*")
    if(ImageFiles)
        juce_add_binary_data(PluginBinaryData SOURCES ${ImageFiles})
        set(BINARY_DATA_TARGET PluginBinaryData)
    endif()
endif()

juce_add_plugin("${PROJECT_NAME}"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icon.png"
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Mong
    PLUGIN_CODE Dm01
    FORMATS "${FORMATS}"
    PRODUCT_NAME "${PRODUCT_NAME}"
)

add_library(SharedCode INTERFACE)

clap_juce_extensions_plugin(TARGET "${PROJECT_NAME}"
    CLAP_ID "${BUNDLE_ID}"
    CLAP_FEATURES audio-effect
)

pamplejuce_shared_code_defaults()

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(SharedCode INTERFACE ${SourceFiles})

if(APPLE)
    pamplejuce_xcode_prettify()
endif()

target_compile_definitions(SharedCode
    INTERFACE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"
    PRODUCT_NAME_WITHOUT_VERSION="DJ Mong5 Pro DlayOne"
)

# 2. Vinculação segura do binário para todas as plataformas (Linux, macOS, Windows)
if(NOT "${BINARY_DATA_TARGET}" STREQUAL "")
    target_link_libraries(SharedCode INTERFACE ${BINARY_DATA_TARGET})
    target_link_libraries("${PROJECT_NAME}" PRIVATE ${BINARY_DATA_TARGET})
endif()

target_link_libraries(SharedCode
    INTERFACE
    melatonin_inspector
    juce_audio_utils
    juce_audio_processors
    juce_dsp
    juce_gui_basics
    juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_link_libraries("${PROJECT_NAME}" PRIVATE SharedCode)

include(PamplejuceIPP)
include(GitHubENV)
