/***************************************************************************
 *   Copyright (C) 2006 by Vincenzo Forchi`,,,   *
 *   tus@ken   *
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


#ifndef QUERYCONFIGURATION_H
#define QUERYCONFIGURATION_H

#include "ui_queryconfiguration.h"

class QueryConfiguration : public QDialog
{
    Q_OBJECT

public:
    QueryConfiguration(QDialog * =NULL);
    ~QueryConfiguration();

    enum QueryType
    {
	LastDay,
	LastWeek,
	LastMonth,
	Interval,
	All
    };

    void 	setQueryType(QueryType);
    QueryType	queryType();

    void	setStartDateTime(QDateTime);
    void	setStopDateTime(QDateTime);
    QDateTime	startDateTime();
    QDateTime	stopDateTime();

protected slots:
    void enableFrame(int);
    void ok();

private:
    Ui::QueryConfiguration ui;
};


#endif
