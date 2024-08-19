project('timers-activities', 'cpp', version : '1.0.0', default_options : ['cpp_std=c++17'])

sources = [ 'main.cpp', 'myactivity.cpp', 'myactivitylistener.cpp', 'stats.cpp'
]

headers = [
    'timers-activities.h', 'myactivity.h', 'myactivitylistener.h', 'stats.h'
]

cpp_arguments = ['-DCUMBIA_DEBUG_OUTPUT=1 -DVERSION=' + meson.project_version()  ]
link_arguments = [ ]

# profiling with gprof
#  gprof -q -b puma-torrent  > analysis.txt
#  gprof -p -b puma-torrent  > analysis.txt
#
if(get_option('gprof_enable'))
    message('\n\n\033[1;33m*\033[0m compiling with \033[1;31m-pg\033[0m for profiling \033[1;33m*\033[0m\n\n')
    cpp_arguments += [ '-pg' ]
    link_arguments += [ '-pg' ]
endif


deps = [   dependency('cumbia', version : '>=2.1.3') ]

executable('timers-activities',
    sources,
    dependencies : deps,
    cpp_args : cpp_arguments,
    link_args : link_arguments )
