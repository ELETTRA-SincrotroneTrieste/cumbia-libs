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


#include "queryconfiguration.h"

#include <QDateTime>
#include <QMessageBox>

#include <QtDebug>

QueryConfiguration::QueryConfiguration(QDialog *parent) : QDialog(parent)
{
	ui.setupUi(this);
	ui.dateStart->setDate(QDate::currentDate());
	ui.dateStop->setDate(QDate::currentDate());
	ui.timeStart->setTime(QTime::currentTime());
	ui.timeStop->setTime(QTime::currentTime());
	connect(ui.comboPeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(enableFrame(int)));
	connect((QObject*) ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
}

QueryConfiguration::~QueryConfiguration()
{
}

void QueryConfiguration::enableFrame(int index)
{
	if (index == 3)
		ui.frame->setEnabled(true);
	else
		ui.frame->setEnabled(false);
}

void QueryConfiguration::ok()
{
	if (ui.comboPeriod->currentIndex() == 3)
	{
		QDateTime begin, end;
		begin.setDate(ui.dateStart->date());
		begin.setTime(ui.timeStart->time());
		end.setDate(ui.dateStop->date());
		end.setTime(ui.timeStop->time());
		if (begin >= end)
			QMessageBox::warning(0, "Warning", "Invalid interval: check starting and ending dates");
		else
			accept();
	}
	else
		accept();
}

void QueryConfiguration::setQueryType(QueryType qt)
{
	ui.comboPeriod->setCurrentIndex((int) qt);
}

QueryConfiguration::QueryType QueryConfiguration::queryType()
{
	return (QueryType) ui.comboPeriod->currentIndex();
}

void QueryConfiguration::setStartDateTime(QDateTime dt)
{
	ui.dateStart->setDate(dt.date());
	ui.timeStart->setTime(dt.time());
}

void QueryConfiguration::setStopDateTime(QDateTime dt)
{
	ui.dateStop->setDate(dt.date());
	ui.timeStop->setTime(dt.time());
}

QDateTime QueryConfiguration::startDateTime()
{
	QDateTime dt;
	dt.setDate(ui.dateStart->date());
	dt.setTime(ui.timeStart->time());
	return dt;
}

QDateTime QueryConfiguration::stopDateTime()
{
	QDateTime dt;
	dt.setDate(ui.dateStop->date());
	dt.setTime(ui.timeStop->time());
	return dt;
}

