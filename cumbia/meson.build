project('cumbia', 'cpp', version : '1.0.2',
    default_options : ['c_std=c17', 'cpp_std=c++17'])

project_description = 'library that offers a carefree approach to multi thread application design and implementation'

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
	'src/lib/cumbia.h',
	'src/lib/cumacros.h',
'src/lib/cucontinuousactivity.h',
'src/lib/cuisolatedactivity.h',
'src/lib/cuactivity.h',
'src/lib/cuactivityevent.h',
'src/lib/threads/cuthreadinterface.h',
'src/lib/threads/cuthreadlistener.h',
'src/lib/threads/cutimer.h',
'src/lib/threads/cutimerlistener.h',
'src/lib/cuvariant.h',
'src/lib/cuvariant_t.h',
'src/lib/cuvariantprivate.h',
'src/lib/cudata.h',
'src/lib/cudataquality.h',
'src/lib/cumbiapool.h',
'src/lib/cudatalistener.h',
'src/lib/services/cuservices.h',
'src/lib/services/cuserviceprovider.h',
'src/lib/services/cuthreadservice.h',
'src/lib/services/cuservicei.h',
'src/lib/services/cutimerservice.h',
'src/lib/services/culog.h',
'src/lib/threads/cuthreadevents.h',
'src/lib/threads/cuthreadfactoryimpl_i.h',
'src/lib/threads/cuthreadfactoryimpl.h',
'src/lib/threads/cuthread.h',
'src/lib/threads/cueventloop.h',
'src/lib/services/cuactivitymanager.h',
'src/lib/threads/cuevent.h',
'src/lib/threads/cuthreadseventbridge_i.h',
'src/lib/threads/cuthreadseventbridge.h',
'src/lib/threads/cuthreadseventbridgefactory_i.h' ,
'src/lib/threads/cuthreadtokengeni.h'
]

install_headers(headers, subdir : 'cumbia') # -> include/cumbia/

sources = [
'src/lib/cumbia.cpp',
'src/lib/cumbiapool.cpp',
'src/lib/cucontinuousactivity.cpp',
'src/lib/cuisolatedactivity.cpp',
'src/lib/cumacros.h',
'src/lib/cuactivity.cpp',
'src/lib/cuactivityevent.cpp',
'src/lib/threads/cutimer.cpp',
'src/lib/cuvariant.cpp',
'src/lib/cuvariantprivate.cpp',
'src/lib/cudata.cpp',
'src/lib/cudataquality.cpp',
'src/lib/cudatalistener.cpp',
'src/lib/services/cuthreadservice.cpp',
'src/lib/services/cutimerservice.cpp',
'src/lib/services/cuserviceprovider.cpp',
'src/lib/services/culog.cpp',
'src/lib/threads/cuthreadevents.cpp',
'src/lib/threads/cuthreadfactoryimpl.cpp',
'src/lib/threads/cuthread.cpp',
'src/lib/threads/cueventloop.cpp',
'src/lib/services/cuactivitymanager.cpp',
'src/lib/threads/cuevent.cpp',
'src/lib/threads/cuthreadseventbridge.cpp'

]

includedirs = include_directories('src/lib', 'src/lib/threads', 'src/lib/services')

cpp_arguments = '-DCUMBIA_DEBUG_OUTPUT=1'

# cpp_arguments = []

deps = [ dependency('threads') ]

cumbialib = shared_library('cumbia', sources,
        version : meson.project_version(),
        include_directories : includedirs,
        cpp_args : cpp_arguments,
        dependencies : deps ,
        install : true)

pkg = import('pkgconfig')
pkg.generate(version : meson.project_version(),
    name : 'libcumbia',
    filebase : 'cumbia',
    description : project_description)

### ====================================== pkg config   ============================
pkgconfig = find_program('pkg-config', required: false)
if not pkgconfig.found()
  error('MESON_SKIP_TEST: pkg-config not found')
endif

pkgg = import('pkgconfig')

h = ['cumbia']  # subdirectories of ${prefix}/${includedir} to add to header path
pkgg.generate(
    libraries : cumbialib,
    subdirs : h,
    version : meson.project_version(),
    name : 'libcumbia',
    filebase : 'cumbia',
    description : project_description )


### ====================================== documentation ===========================
doxygen = find_program('doxygen', required : false)
if not doxygen.found()
  error('MESON_SKIP_TEST doxygen not found.')
endif
  doxydata = configuration_data()
  doxydata.set('VERSION', meson.project_version())


if find_program('dot', required : false).found()
  # In the real world this would set the variable
  # to YES. However we set it to NO so that the
  # list of generated files is always the same
  # so tests always pass.
  doxydata.set('HAVE_DOT', 'YES')
else
  doxydata.set('HAVE_DOT', 'NO')
endif

subdir('doc')

