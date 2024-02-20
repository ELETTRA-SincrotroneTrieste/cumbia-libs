# Writing a tango "torrent" application {#torrent_app}

In this page we will write an application that reads from the native Tango engine
and posts the results on a web server using cURL.
The application accepts multiple *recipes* listing Tango attributes (or commands).
Recipes are organized in folders whose name is an integer indicating the desired
polling period.
One single timer in the application periodically checks the set of results 
gathered from Tango and sends them grouped by recipe through cURL. One cURL
*post* operation for each recipe, according to the associated timeout.
If the desired periods are not multiple one another, they will be approximated.

The *meson* build system will be adopted.

### meson.build

```
project('puma-torrent', 'cpp', version : '1.0.0', default_options : ['cpp_std=c++17'])

tgdep = dependency('cumbia-tango', method: 'pkg-config')

sources = [ 'main.cpp', 'puma-torrent.cpp'
]

headers = [
    'puma-torrent.h'
]

cpp_arguments = ['-DCUMBIA_DEBUG_OUTPUT=1 -DVERSION=' + meson.project_version()  ]
link_arguments = [ ]

deps = [ tgdep, dependency('libcurl') ]

executable('puma-torrent', sources, dependencies : deps, cpp_args : cpp_arguments )
```

