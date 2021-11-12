#include "cuepaletteprocess.h"
#include <QWidget>
#include <QFile>
#include <QRadioButton>
#include <QRegularExpression>

CuEPaletteProcess::CuEPaletteProcess()
{

}

bool CuEPaletteProcess::process(const QString &fname, QString &contents, QWidget* rbcontainer) {
    m_msg.clear();
    QString paname;
    foreach(QRadioButton *rb, rbcontainer->findChildren<QRadioButton *>())
        if(rb->isChecked())
            paname = rb->text();
    if(paname != "default") {
        QFile palettesnip(fname);
        if(palettesnip.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString s = palettesnip.readAll();
            s.replace("\"default\"", "\"" + paname + "\"");
            // find where include <quapplication.h> is and add #include <cuepalette>
            QString inc_in_re = "#include\\s+<quapplication.h>";
            QString inc_out = "#include <quapplication.h>\n#include <cuepalette.h>";
            contents.replace(QRegularExpression("//\\s+\\$palette\\$"), s);
            contents.replace(QRegularExpression(inc_in_re), inc_out);
            palettesnip.close();
        }
        else
            m_msg = palettesnip.errorString();
    }
    return m_msg.isEmpty();
}

QString CuEPaletteProcess::message() const {
    return m_msg;
}
