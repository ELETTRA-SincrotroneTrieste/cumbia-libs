/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <TApplication>
#include "danfisik9100.h"
#include <tutil.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <elettracolors.h>

#define CVSVERSION "$Name:  $"

int main( int argc, char ** argv ) {

    TApplication a( argc, argv );
    a.setPalette(EPalette("fermi"));
    /* uncomment to set application color */
    a.setOrganizationName("Elettra");
    a.setApplicationName("Danfisik9100");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "Giacomo");
    a.setProperty("mail", "giacomo.strangolino@elettra.trieste.it");
    a.setProperty("phone", "375-8073");
    a.setProperty("office", "T2PT025");
    a.setProperty("hwReferent", "Stefano Cleva"); /* name of the referent that provides the device server */
    TUtil::instance()->setLoggingTarget(argv[0]);
    Danfisik9100 mw;
    QString title("Ps Danfisik9100");
    if (a.arguments().size() > 1 )
        title=a.arguments()[ 1].split('/').last(); //solo il member
    mw.setWindowTitle(title);
    mw.show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) mw.winId();
    XSetCommand(disp, root_win, argv, argc);

    return a.exec();
}
