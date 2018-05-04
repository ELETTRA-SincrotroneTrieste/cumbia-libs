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
    INSTALL_ROOT = /usr/local
}


TARGET = bin/dummy

OBJ = obj

SOURCES = dummy.cpp

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share

#target.path = $${INSTALL_ROOT}/bin
#target.files = $${TARGET}

#templates.path = $${TEMPLATES_INSTALLDIR}
#templates.files =  qumbia_project_template/*

completion.path = /usr/share/bash-completion/completions
completion.files = bash_completion.d/cumbia

bashrc.path = /etc/profile.d
bashrc.files = bashrc.d/cumbia.sh

INSTALLS = completion bashrc

DISTFILES += \
	bash_completion.d/cumbia
        bashrc.d/cumbia.sh

