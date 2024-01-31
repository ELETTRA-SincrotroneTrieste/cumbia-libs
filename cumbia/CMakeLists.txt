cmake_minimum_required(VERSION 3.10)

project(cumbia VERSION 2.0.0
    DESCRIPTION "library that offers a carefree approach to multi thread application design and implementation"
    LANGUAGES CXX)

# to have CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

file(GLOB_RECURSE SOURCES src/lib/*.cpp)
file(GLOB_RECURSE HEADERS src/lib/*.h)

add_library(cumbia SHARED  ${SOURCES} ${HEADERS})

# in order to have lib and symlinks
set_target_properties(cumbia PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cumbia PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

target_compile_features(cumbia PUBLIC cxx_std_17)
target_include_directories(cumbia PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/lib/threads>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/lib/services>
    $<INSTALL_INTERFACE:include/cumbia>  # <prefix>/include/mylib
)
target_compile_definitions(cumbia PRIVATE
    CUMBIA_DEBUG_OUTPUT=1
    CU_VERSION="2.0.0"
)

configure_file(src/cumbia.pc.cmake.in cumbia.pc @ONLY)

install(TARGETS cumbia EXPORT cumbiaTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${HEADERS} DESTINATION include/cumbia)

# install pkgconfig file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

export(TARGETS cumbia FILE cumbiaTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE cumbia)
install(EXPORT cumbiaTargets
    FILE cumbiaTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# Generate Config.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cumbiaConfigVersion.cmake"
    VERSION   ${cumbia_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cumbiaConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cumbiaConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cumbiaConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
