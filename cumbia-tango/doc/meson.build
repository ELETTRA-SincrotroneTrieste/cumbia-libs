doxydata.set('TOP_SRCDIR', meson.source_root())
doxydata.set('TOP_BUILDDIR', meson.build_root())

doxyfile = configure_file(input: 'Doxyfile.in',
                          output: 'Doxyfile.out',
                          configuration: doxydata,
                          install: false)

datadir = join_paths(get_option('datadir'), 'doc', 'cumbia-tango')

html_target = custom_target('cumbia-tango-docs',
                            input: doxyfile,
                            output: 'html',
                            command: [doxygen, doxyfile],
                            install: true,
                            install_dir: datadir)

htmlindex = join_paths(get_option('prefix'),  datadir, 'html') + '/index.html'

run_target('show-docs', command : ['xdg-open', htmlindex] )


