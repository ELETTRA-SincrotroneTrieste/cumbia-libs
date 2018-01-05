#AUTOMAKE_OPTIONS = foreign subdir-objects

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = $(DX_CONFIG)

doc: doxygen-doc

include aminclude.am

#
# install-html and uninstall-local:
# remember to put tabs at the beginning of
# each line below install-html: or uninstall-local:
# 
install-html:
	$(MAKE) doc
	@$(NORMAL_INSTALL)
	$(MKDIR_P) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) $(DX_DOCDIR)/html/* $(DESTDIR)$(docdir)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	echo "removing $(DESTDIR)$(docdir)"
	rm -fr $(DESTDIR)$(docdir)

lib_LTLIBRARIES = libcumbia-tango.la

libcumbia_tango_la_SOURCES = lib/cumbiatango.cpp \
lib/cutango-world.cpp \
lib/cutango-world-config.cpp \
lib/tsource.cpp \
lib/tdevice.cpp \
lib/cutreader.cpp \
lib/cutangoactionfactories.cpp \
lib/cuactionfactoryservice.cpp \
lib/cudevicefactoryservice.cpp \
lib/cueventactivity.cpp \
lib/cutangoactioni.cpp \
lib/cupollingactivity.cpp \
lib/cuwriteactivity.cpp \
lib/cutwriter.cpp \
lib/cutattconfiguration.cpp \
lib/cutangoopt_builder.cpp \
lib/cutattconfigactivity.cpp \
lib/cutdbpropertyreader.cpp \
lib/cugettdbpropactivity.cpp


# Header files that must be installed are specified by the HEADERS family of variables.
# Headers can be installed in
# includedir, oldincludedir, pkgincludedir or any other directory you may have defined
#
include_HEADERS =  lib/cumbiatango.h \
lib/cutango-world.h \
lib/cutango-world-config.h \
lib/tsource.h \
lib/tdevice.h \
lib/cutreader.h \
lib/cutangoactioni.h \
lib/cutangoactionfactoryi.h \
lib/cutangoactionfactories.h \
lib/cudevicefactoryservice.h \
lib/cuactionfactoryservice.h \
lib/cueventactivity.h \
lib/cupollingactivity.h \
lib/cuwriteactivity.h \
lib/cutwriter.h \
lib/cutattconfiguration.h \
lib/cutangoopt_builder.h \
lib/cutattconfigactivity.h \
lib/cutdbpropertyreader.h \
lib/cugettdbpropactivity.h



libcumbia_tango_la_CXXFLAGS = -g -Wall -std=c++11 -I$(top_srcdir)/src/lib -I$(top_srcdir)/src/lib/threads \
    -I$(top_srcdir)/src/lib/services \
    -I/usr/local/include -I/usr/include $(tango_CFLAGS)  $(cumbia_CFLAGS) -Ilib

# libcumbia_tango_la_LIBADD =  $(tango_LIBS)

# Use revision instead of version info
libcumbia_tango_la_LDFLAGS = $(tango_LIBS) -release $(CUMBIA_TANGO_VERSION)


pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = lib/cumbia-tango.pc

