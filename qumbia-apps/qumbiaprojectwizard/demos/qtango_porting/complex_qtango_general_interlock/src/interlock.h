/***************************************************************************
 *   Copyright (C) 2007 by Vincenzo Forchi`,,,   *
 *   vincenzo.forchi@elettra.trieste.it   *
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


#ifndef interlock_H
#define interlock_H

#include "ui_interlock.h"
#include "queryconfiguration.h"
#include <QString>
#include <QSqlDatabase>

//class QGraphicsScene;
class MySqlModel;

class Interlock: public QWidget
{
    Q_OBJECT

public:

    enum Tabs { Mod, ModHistory, PsVacLinac, PsVacUH, PsVacPadres, PsVacHistory };

    Interlock(QWidget * =NULL);
    ~Interlock();

//     QStringList getProperty(string, string);

protected slots:
    void queryConfiguration();
    void readFromDb();

private:
    Ui::Interlock ui;

    QSqlDatabase db;
    MySqlModel  *modulatorModel, *psvacModel;

    QueryConfiguration::QueryType       queryType;
    QDateTime   begin, end;
    QTimer      *dbTimer;
	
	void setupDb();
	void configureUi();
};


#endif
