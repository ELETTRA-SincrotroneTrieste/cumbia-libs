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

# comment this line if you want to install bash completion scripts
# system wide
isEmpty(BASH_COMPLETION_DIR) {
    BASH_COMPLETION_DIR=$${INSTALL_ROOT}/share/bash-completion.d
}

isEmpty(BASH_COMPLETION_DIR) {
    message("cumbia-build: BASH_COMPLETION_DIR is empty: using pkg-config to find system wide bash completion dir")
    BASH_COMPLETION_DIR=$$system(pkg-config --variable=completionsdir bash-completion)
}

# avoid stripping executable files (cumbia bash script)
QMAKE_STRIP = echo


TARGET = bin/dummy

OBJ = obj

SOURCES = dummy.cpp

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cumbia-run


#target.path = $${INSTALL_ROOT}/bin
#target.files = $${TARGET}

#templates.path = $${TEMPLATES_INSTALLDIR}
#templates.files =  qumbia_project_template/*

# completion.path = $${BASH_COMPLETION_DIR}
# completion.files = bash_completion.d/cumbia

script.path = $${INSTALL_ROOT}/bin
script.files = cumbia-build cumbia-run curun_db.py

sqlscript.path = $${SHAREDIR}/cumbia-run
sqlscript.files = curun-create-db.sql

doc.files = doc/*
doc.path = $${DOCDIR}
doc.commands = doxygen Doxyfile;

#message("bash completion dir $${BASH_COMPLETION_DIR}")


QMAKE_EXTRA_TARGETS += doc
INSTALLS = script sqlscript doc

DISTFILES +=

