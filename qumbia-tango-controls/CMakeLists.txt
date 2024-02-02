# Generated from qumbia-tango-controls.pro.

cmake_minimum_required(VERSION 3.10)
project(qumbia-tango-controls VERSION 2.0.0 LANGUAGES CXX)

# to have CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(cumbia 2.0 REQUIRED)
find_package(cumbia-tango 2.0 REQUIRED)
find_package(cumbia-qtcontrols 2.0 REQUIRED)
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS OpenGLWidgets PrintSupport Svg)

include(FindPkgConfig)
pkg_check_modules(TANGO_DEPENDENCY REQUIRED tango)

# Include directories and compile flags from the dependency
include_directories(${TANGO_DEPENDENCY_INCLUDE_DIRS})
link_directories(${TANGO_DEPENDENCY_LIBRARY_DIRS})
add_definitions(${TANGO_DEPENDENCY_CFLAGS_OTHER})

file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)

qt_add_library(qumbia-tango-controls SHARED
    ${HEADERS} ${SOURCES}
)

target_compile_definitions(qumbia-tango-controls PUBLIC
    QUMBIATANGOCONTROLS_LIBRARY
    CUMBIA_DEBUG_OUTPUT=1
    QUMBIA_TANGO_CONTROLS_VERSION=0x020000
    QUMBIA_TANGO_CONTROLS_VERSION_STR="2.0.0"
    QT_DEPRECATED_WARNINGS
    QT_NO_DEBUG_OUTPUT=1
    CUMBIA_PRINTINFO=1
)

# target_link_libraries(qumbia-tango-controls PUBLIC
#     Qt::Core
#     Qt::Gui
#     Qt::Widgets
# )

#set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
pkg_check_modules(QWT_DEPENDENCY QUIET Qt6Qwt6)

if(NOT QWT_DEPENDENCY_FOUND)
    pkg_check_modules(QWT_DEPENDENCY REQUIRED Qt5Qwt6)
endif()

# Include directories and compile flags from the dependency
include_directories(${QWT_DEPENDENCY_INCLUDE_DIRS})
link_directories(${QWT_DEPENDENCY_LIBRARY_DIRS})
add_definitions(${QWT_DEPENDENCY_CFLAGS_OTHER})

target_link_libraries(qumbia-tango-controls PRIVATE cumbia)
target_link_libraries(qumbia-tango-controls PRIVATE cumbia-tango)
target_link_libraries(qumbia-tango-controls PRIVATE cumbia-qtcontrols)
# target_link_libraries(cumbia-qtcontrols PRIVATE ${QWT_DEPENDENCY}) <- not needed

target_compile_options(qumbia-tango-controls PUBLIC
    -Wall
)

# Resources:
# set(confdialog_resource_files
#     "src/widgets-base/eimage/artwork/bwnoise.png"
# )

# qt6_add_resources(qumbia-tango-controls "confdialog"
#     PREFIX
#     "/"
#     BASE
#     "src/widgets-base/eimage"
#     FILES
#     ${confdialog_resource_files}
# )

target_include_directories(qumbia-tango-controls PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin_ifaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base/eimage>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base/eplotlight>
    $<INSTALL_INTERFACE:include/qumbia-tango-controls>  # <prefix>/include/cumbia-tango
)

target_compile_definitions(qumbia-tango-controls PUBLIC
    CUMBIAQTCONTROLS_HAS_QWT=1
)

set_target_properties(qumbia-tango-controls PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(qumbia-tango-controls PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
# Set public headers for the library
set_target_properties(qumbia-tango-controls
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

if((QT_VERSION VERSION_GREATER 6))
    target_link_libraries(qumbia-tango-controls PUBLIC
        Qt::OpenGLWidgets
    )
endif()

install(TARGETS qumbia-tango-controls EXPORT qumbia-tango-controlsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #  BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qumbia-tango-controls
)

# install qumbia-tango-controls.pri
install(
    FILES ${CMAKE_CURRENT_LIST_DIR}/qumbia-tango-controls.pri
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qumbia-tango-controls
)

configure_file(src/qumbia-tango-controls.pc.cmake.in qumbia-tango-controls.pc @ONLY)

# install pkgconfig file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

export(TARGETS qumbia-tango-controls FILE qumbia-tango-controlsTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE qumbia-tango-controls)
install(EXPORT qumbia-tango-controlsTargets
    FILE qumbia-tango-controlsTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# Generate Config.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/qumbia-tango-controlsConfigVersion.cmake"
    VERSION  ${cumbia-tango_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/qumbia-tango-controlsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/qumbia-tango-controlsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/qumbia-tango-controlsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
