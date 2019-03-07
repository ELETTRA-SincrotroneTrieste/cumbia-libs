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

#ifndef EDITSOURCEDIALOG_H
#define EDITSOURCEDIALOG_H

#include "ui_editsourcedialog.h"

class EditSourceDialog: public QDialog
{
    Q_OBJECT
public:
    EditSourceDialog(QWidget * = nullptr);
    virtual ~EditSourceDialog();

    Ui::EditSourceDialog ui;

    QString source() const;

    void setSource(const QString& s);

    void setSources(const QStringList& srcs);

    void accept();

    bool checkSource();

    QString message();

    QStringList sources() const;

    void setMultiSource(bool multisource);

    bool isMultiSource() const;

protected slots:
    void addSrcTab();

    void removeSrcTab();

protected:

private:
    QString m_message;
    bool m_multiSource;
};

#endif // LISTWIDGETEDITOR_H
