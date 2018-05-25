project('cumbia-tango', 'cpp', version : '1.0.0',
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
'lib/cumbiatango.h',
'lib/cutango-world.h',
'lib/cutango-world-config.h',
'lib/tsource.h',
'lib/tdevice.h',
'lib/cutreader.h',
'lib/cutangoactioni.h',
'lib/cutangoactionfactoryi.h',
'lib/cutangoactionfactories.h',
'lib/cudevicefactoryservice.h',
'lib/cuactionfactoryservice.h',
'lib/cueventactivity.h',
'lib/cupollingactivity.h',
'lib/cuwriteactivity.h',
'lib/cutwriter.h',
'lib/cutattconfiguration.h',
'lib/cutangoopt_builder.h',
'lib/cutattconfigactivity.h',
'lib/cutdbpropertyreader.h',
'lib/cugettdbpropactivity.h'
]

install_headers(headers, subdir : 'cumbia-tango') # -> include/cumbia-tango/

sources = [
'lib/cumbiatango.cpp',
'lib/cutango-world.cpp',
'lib/cutango-world-config.cpp',
'lib/tsource.cpp',
'lib/tdevice.cpp',
'lib/cutreader.cpp',
'lib/cutangoactionfactories.cpp',
'lib/cuactionfactoryservice.cpp',
'lib/cudevicefactoryservice.cpp',
'lib/cueventactivity.cpp',
'lib/cutangoactioni.cpp',
'lib/cupollingactivity.cpp',
'lib/cuwriteactivity.cpp',
'lib/cutwriter.cpp',
'lib/cutattconfiguration.cpp',
'lib/cutangoopt_builder.cpp',
'lib/cutattconfigactivity.cpp',
'lib/cutdbpropertyreader.cpp',
'lib/cugettdbpropactivity.cpp'
]

includedirs = include_directories('lib')

cpp_arguments = '-DCUMBIA_DEBUG_OUTPUT=1'
# cpp_arguments = []

deps = [ dependency('cumbia'), dependency('tango') ]

cumbiatangolib = shared_library('cumbia-tango', sources,
        soversion : '0',
        version : meson.project_version(),
        include_directories : includedirs,
        cpp_args : cpp_arguments,
        dependencies : deps ,
        install : true)

### ====================================== pkg config   ============================
pkgconfig = find_program('pkg-config', required: false)
if not pkgconfig.found()
  error('MESON_SKIP_TEST: pkg-config not found')
endif

pkgg = import('pkgconfig')

h = ['cumbia-tango']  # subdirectories of ${prefix}/${includedir} to add to header path
pkgg.generate(
    libraries : cumbiatangolib,
    subdirs : h,
    version : meson.project_version(),
    name : 'libcumbia-tango',
    filebase : 'cumbia-tango',
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

