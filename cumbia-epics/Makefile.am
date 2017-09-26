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

lib_LTLIBRARIES = libcumbia-epics.la

libcumbia_epics_la_SOURCES = lib/cumbiaepics.cpp \
lib/cuepics-world.cpp \
lib/cuepics-world-config.cpp \
lib/epsource.cpp \
lib/cumonitor.cpp \
lib/cuepactionfactories.cpp \
lib/cuepcaservice.cpp \
lib/cuepactionfactoryservice.cpp \
lib/cuepactioni.cpp \
lib/cumonitoractivity.cpp \
lib/cuputactivity.cpp \
lib/cuput.cpp \
lib/cuepconfiguration.cpp \
lib/cuepreadoptions.cpp \
lib/cuepconfigactivity.cpp


# Header files that must be installed are specified by the HEADERS family of variables.
# Headers can be installed in
# includedir, oldincludedir, pkgincludedir or any other directory you may have defined
#
include_HEADERS =  lib/cumbiaepics.h \
lib/cuepics-world.h \
lib/cuepics-world-config.h \
lib/epsource.h \
lib/cumonitor.h \
lib/cuepactioni.h \
lib/cuepcaservice.h \
lib/cuepactionfactoryi.h \
lib/cuepactionfactories.h \
lib/cuepactionfactoryservice.h \
lib/cumonitoractivity.h \
lib/cuputactivity.h \
lib/cuput.h \
lib/cuepconfiguration.h \
lib/cuepreadoptions.h \
lib/cuepconfigactivity.h



libcumbia_epics_la_CXXFLAGS = -g -Wall -std=c++11 -I$(top_srcdir)/src/lib -I$(top_srcdir)/src/lib/threads \
    -I$(top_srcdir)/src/lib/services \
    -I/usr/local/include -I/usr/include $(epics_base_CFLAGS)  $(cumbia_CFLAGS) -Ilib

# libcumbia_epics_la_LIBADD =  $(epics_LIBS)

# Use revision instead of version info
libcumbia_epics_la_LDFLAGS = $(epics_base_LIBS) -release $(CUMBIA_EPICS_VERSION)


pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = lib/cumbia-epics.pc

