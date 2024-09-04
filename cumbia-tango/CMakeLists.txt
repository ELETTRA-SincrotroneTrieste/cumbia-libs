cmake_minimum_required(VERSION 3.10)

project(cumbia-tango VERSION 2.2.1
    DESCRIPTION "C++ library based on cumbia and the Tango control system"
    LANGUAGES CXX)

find_package(cumbia 2.0 REQUIRED)

include(FindPkgConfig)
#set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
pkg_check_modules(TANGO_DEPENDENCY REQUIRED tango)

# Include directories and compile flags from the dependency
include_directories(${TANGO_DEPENDENCY_INCLUDE_DIRS})
link_directories(${TANGO_DEPENDENCY_LIBRARY_DIRS})
add_definitions(${TANGO_DEPENDENCY_CFLAGS_OTHER})

# to have CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

file(GLOB_RECURSE SOURCES lib/*.cpp)
file(GLOB_RECURSE HEADERS lib/*.h)

add_library(cumbia-tango SHARED  ${SOURCES} ${HEADERS})

target_link_libraries(cumbia-tango PUBLIC cumbia)
target_link_libraries(cumbia-tango PUBLIC tango)

# in order to have lib and symlinks
set_target_properties(cumbia-tango PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cumbia-tango PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

set_target_properties(cumbia-tango
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

target_compile_features(cumbia-tango PUBLIC cxx_std_17)

target_include_directories(cumbia-tango PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
    $<INSTALL_INTERFACE:include/cumbia-tango>  # <prefix>/include/cumbia-tango
)

target_compile_definitions(cumbia-tango PRIVATE
    CUMBIA_DEBUG_OUTPUT=1
    CU_TANGO_VERSION="2.0.0"
)

configure_file(lib/cumbia-tango.pc.cmake.in cumbia-tango.pc @ONLY)

install(TARGETS cumbia-tango EXPORT cumbia-tangoTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cumbia-tango)

# install pkgconfig file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

export(TARGETS cumbia-tango FILE cumbia-tangoTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE cumbia-tango)
install(EXPORT cumbia-tangoTargets
    FILE cumbia-tangoTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# Generate Config.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-tangoConfigVersion.cmake"
    VERSION  ${cumbia-tango_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-tangoConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cumbia-tangoConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cumbia-tangoConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
