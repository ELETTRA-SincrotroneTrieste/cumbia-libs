#include "editsourcewidget.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QJSEngine>
#include <QtDebug>
#include <cumacros.h>

EditSourceWidget::EditSourceWidget(int index, QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    m_index = index;
    connect(ui.pbFromJs, SIGNAL(clicked()), this, SLOT(loadFromJs()));
    ui.leScriptPath->setVisible(false);
    ui.gbFormula->setChecked(false);
    connect(ui.gbFormula, SIGNAL(toggled(bool)), this, SLOT(onFormulaEnabled(bool)));
    connect(ui.teJsCode, SIGNAL(textChanged()),this, SLOT(expressionValid()));
}

QString EditSourceWidget::source() const
{
    QString s;
    if(ui.gbFormula->isChecked()) {
        QString src = ui.lineEdit->text();
        // \{(.*)\}
        QRegularExpression re("\\{(.*)\\}");
        QRegularExpressionMatch match = re.match(src);
        if(match.hasMatch()) {
            QString formula = ui.teJsCode->toPlainText() ;
            QJSEngine engine;
            QJSValue v = engine.evaluate("(" + formula+ ")");
            if(!v.isCallable()) {
                QString message = "function \"" + formula + "\" is not callable";
                perr("%s", qstoc(message));
            }
            else
                s = "formula://" + ui.lineEdit->text() + " " + formula;
        }
    }
    else {
        s = ui.lineEdit->text();
    }
    printf("\e[1;32mreturning source %s\e[0m\n", (s.toStdString().c_str()));
    return s;
}

void EditSourceWidget::setSource(const QString &s)
{
    QRegularExpression re("formula://(\\{.+\\})(\\s*function.*)",
                          QRegularExpression::DotMatchesEverythingOption); // capture newlines
    QRegularExpressionMatch match = re.match(s);
    if(match.hasMatch() && match.capturedTexts().size() == 3) {
        ui.lineEdit->setText(match.captured(1));
        ui.teJsCode->setPlainText(match.captured(2));
        ui.gbFormula->setChecked(true);
    }
    else {
        re.setPattern("formula://(\\{.+\\})");
        match = re.match(s);
        if(match.hasMatch() && match.captured().size() == 1) {
            ui.lineEdit->setText(match.captured(1));
            ui.gbFormula->setChecked(true);
        }
        else {
            ui.lineEdit->setText(s);
            ui.gbFormula->setChecked(false);
        }
    }
}


bool EditSourceWidget::checkSource()
{
    bool error = false;
    if(ui.gbFormula->isChecked()) {
        QString src = ui.lineEdit->text();
        // \{(.*)\}
        // {test/device/1/double_scalar,test/device/2/double_scalar}
        QRegularExpression re("\\{(.*)\\}");
        QRegularExpressionMatch match = re.match(src);
        error = !match.hasMatch();
        if(error) {
            QMessageBox::critical(nullptr,
                                  "Source syntax error",
                                  QString("Source \"%1\" is not valid:\n").arg(src) +
                                  "formula sources must be listed within \"{ }\" brackets.\n" +
                                  "For example \"{test/device/1/double_scalar,test/device/2/double_scalar}");
        }
        else {
            QString formula = ui.teJsCode->toPlainText();
            QJSEngine engine;
            QJSValue v = engine.evaluate("(" + formula + ")");
            error = !v.isCallable();
            if(error) {
                QMessageBox::critical(nullptr, "Error: the provided script is not callable",
                                      "The script must be a function, such as:\n"
                                      "function(a,b) { return a + b; }:\n\n"
                                      + formula);
            }
        }
    }
    return !error;
}

void EditSourceWidget::expressionValid()
{
    QString e = ui.teJsCode->toPlainText();
    QJSEngine engine;
    QJSValue v = engine.evaluate("(" + e + ")");
    (!v.isCallable() || v.isError()) && !e.isEmpty() ? ui.teJsCode->setStyleSheet("border: 0.1em solid red;") :
                      ui.teJsCode->setStyleSheet("border: 0.07em solid green;");
}

void EditSourceWidget::loadFromJs()
{
    QSettings s;
    QString prevPath = s.value("QTDESIGNER_PLUGIN_FILEPATH", QDir::homePath()).toString();
    QString file = QFileDialog::getOpenFileName(this, "Pick a JS file", prevPath, "*.js");
    if(!file.isEmpty()) {
        QFile f(file);
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream in(&f);
            ui.teJsCode->disconnect();
            ui.teJsCode->setText(in.readAll());
            ui.leScriptPath->setVisible(true);
            ui.leScriptPath->setText(file);
            connect(ui.teJsCode, SIGNAL(textChanged()), ui.leScriptPath, SLOT(hide()));
            connect(ui.teJsCode, SIGNAL(textChanged()),this, SLOT(expressionValid()));
            QFileInfo fi(f);
            s.setValue("QTDESIGNER_PLUGIN_FILEPATH", fi.dir().path());
            f.close();
        }
        else {
            QMessageBox::critical(this, "Unable to read file", "Failed to open file \"" +
                                  file + "\" in read only mode: " + f.errorString());
        }
    }
}

void EditSourceWidget::onFormulaEnabled(bool en)
{
    QString src = ui.lineEdit->text();
    // \{(.*)\}
    QRegularExpression re("\\{(.*)\\}");
    QRegularExpressionMatch match = re.match(src);
    if(!en) {
        if(match.hasMatch())
            src = match.captured(1);
    }
    else if(!match.hasMatch()) {
        // enclose src within {}
        src = "{" + src + "}";
    }
    ui.lineEdit->setText(src);
}


