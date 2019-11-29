#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T14:42:39
#
#-------------------------------------------------
# The application will be installed under INSTALL_ROOT (i.e. prefix)
#
# To set the prefix at build time, call 
# qmake   "INSTALL_ROOT=/my/custom/path"
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

# avoid stripping executable files (cumbia bash script)
QMAKE_STRIP = echo


TARGET = bin/dummy

OBJ = obj

SOURCES = dummy.cpp

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share

BASH_COMPLETION_DIR=$$system(pkg-config --variable=completionsdir bash-completion)

#target.path = $${INSTALL_ROOT}/bin
#target.files = $${TARGET}

#templates.path = $${TEMPLATES_INSTALLDIR}
#templates.files =  qumbia_project_template/*

completion.path = $${BASH_COMPLETION_DIR}
completion.files = bash_completion.d/cumbia

script.path = $${INSTALL_ROOT}/bin
script.files = cumbia

message("bash completion dir $${BASH_COMPLETION_DIR}")


INSTALLS = completion script

DISTFILES += \
	bash_completion.d/cumbia
        cumbia

