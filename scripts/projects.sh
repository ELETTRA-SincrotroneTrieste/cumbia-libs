#!/bin/bash

gitrepo="https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git"

# this file contains the list of the projects in the cumbia-libs distribution
# the pure cpp projects are built with meson, the qt projects use qmake

meson_p=(cumbia cumbia-tango cumbia-epics)

qmake_p=(cumbia-qtcontrols qumbia-epics-controls qumbia-tango-controls)

# these projects are grouped together under a common directory
# they are qt projects with the TEMPLATE=subdir directive in the .pro file
# for example, qumbia-apps and qumbia-plugins

qmake_subdir_p=(qumbia-apps  qumbia-plugins)
