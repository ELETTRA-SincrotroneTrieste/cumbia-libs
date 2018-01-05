#SUBDIRS = src/lib  src/lib/services src/lib/threads
#AUTOMAKE_OPTIONS = foreign subdir-objects

ACLOCAL_AMFLAGS = -I m4

include aminclude.am

EXTRA_DIST = $(DX_CONFIG)

doc: doxygen-doc

install-html:
	$(MAKE) doc
	@$(NORMAL_INSTALL)
	$(MKDIR_P) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) $(DX_DOCDIR)/html/* $(DESTDIR)$(docdir)



uninstall-local:
	@$(NORMAL_UNINSTALL)
	echo "removing $(DESTDIR)$(docdir)"
	rm -fr $(DESTDIR)$(docdir)

lib_LTLIBRARIES = libcumbia.la

libcumbia_la_SOURCES = src/lib/cumbia.cpp \
src/lib/cumbiapool.cpp \
src/lib/cucontinuousactivity.cpp \
src/lib/cuisolatedactivity.cpp \
src/lib/cumacros.h \
src/lib/cuactivity.cpp \
src/lib/cuactivityevent.cpp \
src/lib/threads/cutimer.cpp \
src/lib/threads/cuthreadlistener.cpp \
src/lib/cuvariant.cpp \
src/lib/cuvariantprivate.cpp \
src/lib/cudata.cpp \
src/lib/cudatalistener.cpp \
src/lib/services/cuthreadservice.cpp \
src/lib/services/cuserviceprovider.cpp \
src/lib/threads/cuthreadevents.cpp \
src/lib/threads/cuthreadfactory.cpp \
src/lib/threads/cuthreadfactoryimpl.cpp \
src/lib/threads/cuthread.cpp \
src/lib/threads/cueventloop.cpp \
src/lib/services/cuactivitymanager.cpp \
src/lib/threads/cuevent.cpp \
src/lib/cumbia.h \
src/lib/cumbiapool.h \
src/lib/cucontinuousactivity.h \
src/lib/cuisolatedactivity.h \
src/lib/cuactivity.h \
src/lib/threads/cuthreadinterface.h \
src/lib/threads/cuthreadlistener.h \
src/lib/cuvariant.h \
src/lib/cuvariant_t.h \
src/lib/cuvariantprivate.h \
src/lib/cudata.h \
src/lib/cudatalistener.h \
src/lib/services/cuservices.h \
src/lib/services/cuserviceprovider.h \
src/lib/services/cuthreadservice.h \
src/lib/services/cuservicei.h \
src/lib/services/culog.cpp \
src/lib/threads/cuthreadfactory_i.h \
src/lib/threads/cuthreadfactory.h \
src/lib/threads/cuthreadevents.h \
src/lib/threads/cuthreadfactoryimpl_i.h \
src/lib/threads/cuthreadfactoryimpl.h \
src/lib/threads/cuthread.h \
src/lib/cuactivityfactoryimpli.h \
src/lib/threads/cueventloop.h \
src/lib/services/cuactivitymanager.h \
src/lib/threads/cuevent.h \
src/lib/threads/mainloop_event_deliverer.h \
src/lib/threads/cuthreadseventbridge_i.h \
src/lib/threads/cuthreadseventbridge.h \
src/lib/threads/cuthreadseventbridgefactory_i.h


# Header files that must be installed are specified by the HEADERS family of variables.
# Headers can be installed in
# includedir, oldincludedir, pkgincludedir or any other directory you may have defined
#
include_HEADERS = \
src/lib/cumbia.h \
src/lib/cumacros.h \
src/lib/cucontinuousactivity.h \
src/lib/cuisolatedactivity.h \
src/lib/cuactivity.h \
src/lib/cuactivityevent.h \
src/lib/threads/cuthreadinterface.h \
src/lib/threads/cuthreadlistener.h \
src/lib/threads/cutimer.h \
src/lib/threads/cutimerlistener.h \
src/lib/cuvariant.h \
src/lib/cuvariant_t.h \
src/lib/cuvariantprivate.h \
src/lib/cudata.h \
src/lib/cumbiapool.h \
src/lib/cudatalistener.h \
src/lib/services/cuservices.h \
src/lib/services/cuserviceprovider.h \
src/lib/services/cuthreadservice.h \
src/lib/services/cuservicei.h \
src/lib/services/culog.h \
src/lib/threads/cuthreadfactory_i.h \
src/lib/threads/cuthreadfactory.h \
src/lib/threads/cuthreadevents.h \
src/lib/threads/cuthreadfactoryimpl_i.h \
src/lib/threads/cuthreadfactoryimpl.h \
src/lib/threads/cuthread.h \
src/lib/cuactivityfactoryimpli.h \
src/lib/threads/cueventloop.h \
src/lib/services/cuactivitymanager.h \
src/lib/threads/cuevent.h \
src/lib/threads/mainloop_event_deliverer.h \
src/lib/threads/cuthreadseventbridge_i.h \
src/lib/threads/cuthreadseventbridge.h \
src/lib/threads/cuthreadseventbridgefactory_i.h


libcumbia_la_CXXFLAGS = -Wall -std=c++11 -I$(top_srcdir)/src/lib -I$(top_srcdir)/src/lib/threads \
    -I$(top_srcdir)/src/lib/services \
    -I/usr/local/include -I/usr/include

# libcumbia___la_LDFLAGS = -version-info $(HDBX_VERSION)

# Use revision instead of version info
libcumbia_la_LDFLAGS = -release $(CUMBIA_VERSION)

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = src/cumbia.pc
