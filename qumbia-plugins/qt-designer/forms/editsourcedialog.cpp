/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "editsourcedialog.h"
#include "editsourcewidget.h"
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QJSEngine>
#include <QtDebug>
#include <cumacros.h>

EditSourceDialog::EditSourceDialog(QWidget *parent) : QDialog(parent)
{
    m_multiSource = false;
    ui.setupUi(this);
    // Tab widget is cleared
    ui.tabWidget->clear();
    // and addSrcTab is used immediately to populate first source editor
    addSrcTab();
    connect(ui.pbSrcAdd, SIGNAL(clicked()), this, SLOT(addSrcTab()));
    connect(ui.pbSrcRemove, SIGNAL(clicked()), this, SLOT(removeSrcTab()));
    ui.pbSrcAdd->setVisible(false);
    ui.pbSrcRemove->setVisible(false);
    // disable remove button with one source only
    // check is performed in removeSrcTab also
    ui.pbSrcRemove->setDisabled(true);
}

EditSourceDialog::~EditSourceDialog()
{
}

QString EditSourceDialog::source() const
{
    QString s;
    s = findChild<EditSourceWidget *>("editSrcW_0")->source();
    printf("\e[1;32mreturning source %s\e[0m\n", (s.toStdString().c_str()));
    return s;
}

void EditSourceDialog::setSource(const QString &s)
{
    findChild<EditSourceWidget *>("editSrcW_0")->setSource(s);
}

void EditSourceDialog::setSources(const QStringList &srcs)
{
    printf("EditSourceDialog::setSources: \e[1;32m sources are %s\e[0m\n", srcs.join(" -- ").toStdString().c_str());
    setMultiSource(true);
    for(int i = 0; i < srcs.size(); i++) {
        EditSourceWidget *esw = findChild<EditSourceWidget *>(QString("editSrcW_%1").arg(i));
        if(!esw) {
            addSrcTab();
            esw = findChild<EditSourceWidget *>(QString("editSrcW_%1").arg(i));
        }
        esw->setSource(srcs[i]);
    }
}

void EditSourceDialog::accept()
{
    bool error = !checkSource();
    if(!error)
        QDialog::accept();
}

bool EditSourceDialog::checkSource()
{
    bool ok = true;
    foreach(EditSourceWidget *w, findChildren<EditSourceWidget *>())
        ok = ok & w->checkSource();
    return ok;
}

QStringList EditSourceDialog::sources() const
{
    QStringList srcs;
    foreach(EditSourceWidget *w, findChildren<EditSourceWidget *>())
        if(!w->source().isEmpty())
            srcs << w->source();
    qDebug() << __PRETTY_FUNCTION__ << srcs;
    return srcs;
}

void EditSourceDialog::setMultiSource(bool multisource)
{
    m_multiSource = multisource;
    foreach(QPushButton *pb, findChildren<QPushButton *>(QRegularExpression("pbSrc.+")))
        pb->setVisible(multisource);
    if(multisource) {
        ui.tabWidget->setTabText(0, "Source 1");
        ui.tabWidget->setCurrentIndex(0);
    }
}

bool EditSourceDialog::isMultiSource() const
{
    return m_multiSource;
}

void EditSourceDialog::addSrcTab()
{
    QString tabText;
    int i = ui.tabWidget->count();
    i == 0 ? tabText = "Set connection source" : QString("Source %1").arg(i + 1 );
    EditSourceWidget* esw = new EditSourceWidget(i, this);
    ui.tabWidget->addTab(esw, tabText);
    esw->setObjectName(QString("editSrcW_%1").arg(i));
    ui.tabWidget->setTabText(i, QString("Source %1").arg(i+1));
    ui.pbSrcRemove->setDisabled(ui.tabWidget->count() == 1);
    ui.tabWidget->setCurrentIndex(i);
}

void EditSourceDialog::removeSrcTab()
{
    int delidx = ui.tabWidget->currentIndex();
    QWidget *delw = ui.tabWidget->widget(delidx);
    ui.tabWidget->removeTab(delidx);
    delete delw;
    // rename remaining EditSourceWidget
    for(int i = 0; i < ui.tabWidget->count(); i++) {
        qDebug() << __PRETTY_FUNCTION__ << "widget tab at" << i << "is " << ui.tabWidget->widget(i);
        ui.tabWidget->widget(i)->setObjectName(QString("editSrcW_%1").arg(i));
        if(ui.tabWidget->count() > 0)
            ui.tabWidget->setTabText(i, QString("Source %1").arg(i+1));
        else {
            ui.tabWidget->setTabText(0, "Set connection source");
        }
    }
    ui.pbSrcRemove->setDisabled(ui.tabWidget->count() == 1);
}


