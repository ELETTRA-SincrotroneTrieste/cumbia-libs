# ../cumbia-qt.prf defines INSTALL_ROOT
#
exists(../cumbia-qt.prf) {
    include(../cumbia-qt.prf)
	!isEmpty(cu_exclude_engines){
	    message("-")
		message("excluding engines: $${cu_exclude_engines}")
		message("from file cumbia-qt.prf under $${INSTALL_ROOT}/include.")
		message("To include any of them, override \"cu_exclude_engines\"")
		message("when calling qmake")
		message("-")
	}
}

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

isEmpty(CU_USER_CONFIG_DIR) {
    CU_USER_CONFIG_DIR=.config/cumbia
}

!isEmpty(cu_exclude_engines){
    message("-")
	message("excluding engines: $${cu_exclude_engines}")
	message("-")
}

# exclude modules?
ep_x=$$find(cu_exclude_engines,epics)
tg_x=$$find(cu_exclude_engines,tango)
ws_x=$$find(cu_exclude_engines,websocket)
http_x=$$find(cu_exclude_engines,http)
rnd_x=$$find(cu_exclude_engines,random)


# include cumbia-qtcontrols for necessary qt engine-unaware dependency (widgets, qwt, ...)
# and for CUMBIA_QTCONTROLS_VERSION definition
#
include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

linux-g++|linux-clang|freebsd-clang|freebsd-g++|openbsd {
    exists ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri):isEmpty(ep_x) {
	   message("+ adding EPICS module under $${INSTALL_ROOT}")
	   include ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
	}

    exists  ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri):isEmpty(tg_x) {
	    message("+ adding Tango module under $${INSTALL_ROOT}")
		include ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
		}
		greaterThan(QT_MAJOR_VERSION, 4): QT += 
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri):isEmpty(rnd_x) {
    message("+ adding cumbia-random module under $${INSTALL_ROOT}")
	include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri):isEmpty(ws_x)  {
    message("+ adding cumbia-websocket module under $${INSTALL_ROOT}")
	include($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-http/cumbia-http.pri):isEmpty(http_x)  {
    message("+ adding cumbia-http module under $${INSTALL_ROOT}")
	include($${INSTALL_ROOT}/include/cumbia-http/cumbia-http.pri)
}

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# silent compilation
# CONFIG += silent


TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_NO_DEBUG_OUTPUT

message("-")
message("quapps.pri: user configuration dir is set to $${CU_USER_CONFIG_DIR} under the home dir")
message("            it can be changed by calling qmake CU_USER_CONFIG_DIR=someotherdir")
message("quapps.pri: the 'cumbia-apps module-default' utility can help configure the default engine")
message("            for your applications, for example 'http' or 'native'")
message("            Type `cumbia apps module-default` or `cumbia apps module-default set` at the terminal")
message("-")

DEFINES += CUMBIA_USER_CONFIG_DIR=\"\\\"$${CU_USER_CONFIG_DIR}\\\"\"

unix:INCLUDEPATH += $${INSTALL_ROOT}/include/quapps

