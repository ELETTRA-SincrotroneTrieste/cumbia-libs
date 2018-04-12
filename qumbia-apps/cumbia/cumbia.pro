#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T14:42:39
#
#-------------------------------------------------
INSTALL_ROOT = /usr/local

TARGET = bin/dummy

OBJ = obj

SOURCES = dummy.cpp

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share

#target.path = $${INSTALL_ROOT}/bin
#target.files = $${TARGET}

#templates.path = $${TEMPLATES_INSTALLDIR}
#templates.files =  qumbia_project_template/*

completion.path = /etc/bash_completion.d
completion.files = bash_completion.d/cumbia

bashrc.path = /etc/bash/bashrc.d
bashrc.files = bashrc.d/cumbia.sh

INSTALLS = completion bashrc

DISTFILES += \
	bash_completion.d/cumbia
        bashrc.d/cumbia.sh

