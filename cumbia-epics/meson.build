project('cumbia-epics', 'cpp', version : '1.0.0',
    default_options : ['c_std=c11', 'cpp_std=c++11'])

project_description = 'C++ library based on cumbia and the Tango control system'

cu_version = meson.project_version() # set in project() below
ver_arr = cu_version.split('.')

cu_major_version = ver_arr[0]
cu_minor_version = ver_arr[1]
cu_micro_version = ver_arr[2]

conf = configuration_data()
# Surround the version in quotes to make it a C string
conf.set_quoted('VERSION', cu_version)
configure_file(output : 'config.h',
               configuration : conf)


headers = [
'lib/cumbiaepics.h',
'lib/cuepics-world.h',
'lib/cuepics-world-config.h',
'lib/epsource.h',
'lib/cumonitor.h',
'lib/cuepactioni.h',
'lib/cuepcaservice.h',
'lib/cuepactionfactoryi.h',
'lib/cuepactionfactories.h',
'lib/cuepactionfactoryservice.h',
'lib/cumonitoractivity.h',
'lib/cuputactivity.h',
'lib/cuput.h',
'lib/cuepconfiguration.h',
'lib/cuepreadoptions.h',
'lib/cuepconfigactivity.h'
]

install_headers(headers, subdir : 'cumbia-epics') # -> include/cumbia-epics/

sources = [
'lib/cumbiaepics.cpp',
'lib/cuepics-world.cpp',
'lib/cuepics-world-config.cpp',
'lib/epsource.cpp',
'lib/cumonitor.cpp',
'lib/cuepactionfactories.cpp',
'lib/cuepcaservice.cpp',
'lib/cuepactionfactoryservice.cpp',
'lib/cuepactioni.cpp',
'lib/cumonitoractivity.cpp',
'lib/cuputactivity.cpp',
'lib/cuput.cpp',
'lib/cuepconfiguration.cpp',
'lib/cuepreadoptions.cpp',
'lib/cuepconfigactivity.cpp'
]

includedirs = include_directories('lib')

cpp_arguments = '-DCUMBIA_PRINT=1'

deps = [dependency('epics-base-linux-x86_64'), dependency('epics-base'), dependency('cumbia')]

## Y are they missing in pkgconfig file?
linker_args = [ '-lca',  '-ldbCore', '-lCom' ]

cumbiaepicslib = shared_library('cumbia-epics', sources,
        soversion : '0',
        version : meson.project_version(),
        include_directories : includedirs,
        cpp_args : cpp_arguments,
        dependencies : deps ,
        link_args : linker_args,
        install : true)

### ====================================== pkg config   ============================
pkgconfig = find_program('pkg-config', required: false)
if not pkgconfig.found()
  error('MESON_SKIP_TEST: pkg-config not found')
endif

pkgg = import('pkgconfig')

h = ['cumbia-epics']  # subdirectories of ${prefix}/${includedir} to add to header path
pkgg.generate(
    libraries : cumbiaepicslib,
    subdirs : h,
    version : meson.project_version(),
    name : 'libcumbia-epics',
    filebase : 'cumbia-epics',
    description : project_description )


### ====================================== documentation ===========================
doxygen = find_program('doxygen', required : false)
if not doxygen.found()
  error('MESON_SKIP_TEST doxygen not found.')
endif
  doxydata = configuration_data()
  doxydata.set('VERSION', meson.project_version())


if find_program('dot', required : false).found()
  doxydata.set('HAVE_DOT', 'YES')
else
  doxydata.set('HAVE_DOT', 'NO')
endif

subdir('doc')

