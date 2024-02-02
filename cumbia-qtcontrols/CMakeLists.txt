# Generated from cumbia-qtcontrols.pro.

cmake_minimum_required(VERSION 3.10)
project(cumbia-qtcontrols VERSION 2.0.0 LANGUAGES CXX)

# to have CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS OpenGLWidgets PrintSupport Svg)

file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)

qt_add_library(cumbia-qtcontrols SHARED
    ${HEADERS} ${SOURCES}
)

target_compile_definitions(cumbia-qtcontrols PUBLIC
    CUMBIAQTCONTROLS_LIBRARY
    CUMBIA_DEBUG_OUTPUT=1
# NOTE: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    CUMBIA_QTCONTROLS_PLUGIN_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/qumbia-plugins"
    CUMBIA_QTCONTROLS_VERSION=0x020000
    CUMBIA_QTCONTROLS_VERSION_STR="2.0.0"
    QT_DEPRECATED_WARNINGS
    QT_NO_DEBUG_OUTPUT
)

target_link_libraries(cumbia-qtcontrols PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
find_package(cumbia 2.0 REQUIRED)
find_package(cumbia-tango 2.0 REQUIRED)

include(FindPkgConfig)
#set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
pkg_check_modules(QWT_DEPENDENCY QUIET Qt6Qwt6)

if(NOT QWT_DEPENDENCY_FOUND)
    pkg_check_modules(QWT_DEPENDENCY REQUIRED Qt5Qwt6)
endif()

# Include directories and compile flags from the dependency
include_directories(${QWT_DEPENDENCY_INCLUDE_DIRS})
link_directories(${QWT_DEPENDENCY_LIBRARY_DIRS})
add_definitions(${QWT_DEPENDENCY_CFLAGS_OTHER})

target_link_libraries(cumbia-qtcontrols PRIVATE cumbia)
target_link_libraries(cumbia-qtcontrols PRIVATE cumbia-tango)
# target_link_libraries(cumbia-qtcontrols PRIVATE ${QWT_DEPENDENCY}) <- not needed

target_compile_options(cumbia-qtcontrols PUBLIC
    -Wall
)

# Resources:
set(confdialog_resource_files
    "src/widgets-base/eimage/artwork/bwnoise.png"
    "src/widgets-base/eimage/artwork/okguy.png"
    "src/widgets-base/eimage/artwork/paletteBW.png"
    "src/widgets-base/eimage/artwork/paletteC.png"
    "src/widgets-base/eimage/artwork/paletteHot.png"
    "src/widgets-base/eimage/artwork/paletteJet.png"
    "src/widgets-base/eimage/artwork/trollface-problem.png"
)

qt6_add_resources(cumbia-qtcontrols "confdialog"
    PREFIX
    "/"
    BASE
    "src/widgets-base/eimage"
    FILES
    ${confdialog_resource_files}
)

target_include_directories(cumbia-qtcontrols PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin_ifaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base/eimage>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets-base/eplotlight>
    $<INSTALL_INTERFACE:include/cumbia-qtcontrols>  # <prefix>/include/cumbia-tango
)

target_compile_definitions(cumbia-qtcontrols PUBLIC
    CUMBIAQTCONTROLS_HAS_QWT=1
)

# in order to have lib and symlinks
set_target_properties(cumbia-qtcontrols PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cumbia-qtcontrols PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
# Set public headers for the library
set_target_properties(cumbia-qtcontrols
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

if((QT_VERSION VERSION_GREATER 6))
    target_link_libraries(cumbia-qtcontrols PUBLIC
        Qt::OpenGLWidgets
    )
endif()

install(TARGETS cumbia-qtcontrols EXPORT cumbia-qtcontrolsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #  BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cumbia-qtcontrols
)

# install cumbia-qtcontrols.pri
install(
    FILES ${CMAKE_CURRENT_LIST_DIR}/cumbia-qtcontrols.pri
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cumbia-qtcontrols
)

# generate pkgconfig file
configure_file(src/cumbia-qtcontrols.pc.cmake.in cumbia-qtcontrols.pc @ONLY)
# install pkgconfig file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

export(TARGETS cumbia-qtcontrols FILE cumbia-qtcontrolsTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE cumbia-qtcontrols)
install(EXPORT cumbia-qtcontrolsTargets
    FILE cumbia-qtcontrolsTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# Generate Config.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-qtcontrolsConfigVersion.cmake"
    VERSION  ${cumbia-tango_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-qtcontrolsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-qtcontrolsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cumbia-qtcontrolsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
