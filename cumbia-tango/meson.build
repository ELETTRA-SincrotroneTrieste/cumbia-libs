project('cumbia-tango', 'cpp', version : '2.0.0',
    default_options : ['c_std=c17', 'cpp_std=c++17'])

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
'lib/cutthread.h',
'lib/cutthreaddevices.h',
'lib/cudevicefactory_i.h',
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
'lib/cuactionfactoryservice_impls.h',
'lib/cueventactivity.h',
'lib/cudataupdatepolicy_enum.h',
'lib/cupollingactivity.h',
'lib/cupoller.h',
'lib/cupollingservice.h',
'lib/cuwriteactivity.h',
'lib/cutwriter.h',
'lib/cutconfiguration.h',
'lib/cutadb.h',
'lib/cutadbactivity.h',
'lib/cutangoopt_builder.h',
'lib/cutconfigactivity.h',
'lib/cutconfigactivity_executor_i.h',
'lib/cutdbpropertyreader.h',
'lib/cugettdbpropactivity.h',
'lib/cutthreadtokengen.h'
]

install_headers(headers, subdir : 'cumbia-tango') # -> include/cumbia-tango/

sources = [
'lib/cumbiatango.cpp',
'lib/cutthread.cpp',
'lib/cutthreaddevices.cpp',
'lib/cutango-world.cpp',
'lib/cutango-world-config.cpp',
'lib/tsource.cpp',
'lib/tdevice.cpp',
'lib/cutreader.cpp',
'lib/cutangoactionfactories.cpp',
'lib/cuactionfactoryservice.cpp',
'lib/cuactionfactoryservice_impls.cpp',
'lib/cudevicefactoryservice.cpp',
'lib/cueventactivity.cpp',
'lib/cutangoactioni.cpp',
'lib/cupollingactivity.cpp',
'lib/cupoller.cpp',
'lib/cupollingservice.cpp',
'lib/cuwriteactivity.cpp',
'lib/cutwriter.cpp',
'lib/cutconfiguration.cpp',
'lib/cutadb.cpp',
'lib/cutadbactivity.cpp',
'lib/cutangoopt_builder.cpp',
'lib/cutconfigactivity.cpp',
'lib/cutdbpropertyreader.cpp',
'lib/cugettdbpropactivity.cpp',
'lib/cutthreadtokengen.cpp'
]

cpp_arguments = ['-DCUMBIA_DEBUG_OUTPUT=1 -DCU_TANGO_VERSION=' + cu_version, ]
link_arguments = [ ]

# profiling with gperftools
#
if(get_option('gperf_enable'))
    message('\033[1;33m*\033[0m compiling with \033[1;31m-lprofiler\033[0m for gperftools profiling \033[1;33m*\033[0m')
    link_arguments += [ '-lprofiler' ]
endif

# profiling with gprof
#  gprof -q -b executable  > analysis.txt
#  gprof -p -b executable  > analysis.txt
#
if(get_option('gprof_enable'))
    message('\n\n\033[1;33m*\033[0m compiling with \033[1;31m-pg\033[0m for profiling \033[1;33m*\033[0m\n\n')
    cpp_arguments += [ '-pg' ]
    link_arguments += [ '-pg' ]
endif

tgdep = dependency('tango', method: 'pkg-config')

# tango 9.4.0 requires #include <tango/tango.h>.
# older versions would not find includes. So let's define a tginc
# storing the include directories without trailing /tango:
#
tginc = tgdep.get_pkgconfig_variable('includedir')

libdir='lib'
if not tginc.endswith('tango')
    tginc += '/tango'
endif

# add tginc to include dirs
includedirs = [ include_directories('lib'), include_directories(tginc) ]

deps = [ dependency('threads'), dependency('cumbia'), tgdep ]

cumbiatangolib = shared_library('cumbia-tango', sources,
        version : meson.project_version(),
        include_directories : includedirs,
        cpp_args : cpp_arguments,
        dependencies : deps ,
        install : true,
        link_args : link_arguments )

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
    requires: [ 'cumbia', 'tango' ],
    install_dir : 'lib/pkgconfig',
    description : project_description )


### ====================================== documentation ===========================
doxygen = find_program('doxygen', required : false)
if not doxygen.found()
  message('doxygen not found. Documentation will not be generated')
else

  doxydata = configuration_data()
  doxydata.set('VERSION', meson.project_version())


  if find_program('dot', required : false).found()
    doxydata.set('HAVE_DOT', 'YES')
  else
    doxydata.set('HAVE_DOT', 'NO')
endif


  subdir('doc')
endif

