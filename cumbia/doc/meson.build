doxydata.set('TOP_SRCDIR', meson.source_root())
doxydata.set('TOP_BUILDDIR', meson.build_root())

doxyfile = configure_file(input: 'Doxyfile.in',
                          output: 'Doxyfile.out',
                          configuration: doxydata,
                          install: false)

datadir = join_paths(get_option('datadir'), 'doc', 'cumbia')

html_target = custom_target('cumbia-tango-docs',
                            input: doxyfile,
                            output: 'html',
                            command: [doxygen, doxyfile],
                            install: true,
                            install_dir: datadir)

