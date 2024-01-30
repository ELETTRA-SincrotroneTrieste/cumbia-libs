cmake_minimum_required(VERSION 3.10)


project(cumbia-tango VERSION 2.0.0
    DESCRIPTION "C++ library based on cumbia and the Tango control system"
    LANGUAGES CXX)

find_package(cumbia)

# to have CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

file(GLOB_RECURSE SOURCES lib/*.cpp)
file(GLOB_RECURSE HEADERS lib/*.h)

add_library(cumbia-tango SHARED  ${SOURCES} ${HEADERS})

# in order to have lib and symlinks
set_target_properties(cumbia-tango PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cumbia-tango PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

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
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${HEADERS} DESTINATION include/cumbia-tango)

# install pkgconfig file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION lib/pkgconfig
)

export(TARGETS cumbia-tango NAMESPACE cumbia-tango:: FILE cumbia-tangoTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE cumbia-tango)
install(EXPORT cumbia-tangoTargets  FILE cumbia-tangoTargets.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)
