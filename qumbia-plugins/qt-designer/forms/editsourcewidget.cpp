#include "editsourcewidget.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QJSEngine>
#include <QtDebug>
#include <cumacros.h>
#include <cuformulaplugininterface.h> // for FORMULA_RE regexp pattern
#include <cupluginloader.h>
#include <QPluginLoader>

EditSourceWidget::EditSourceWidget(int index, QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    m_formulaParserI = nullptr;
    CuPluginLoader plulo;
    QString plupath = plulo.getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, "cuformula-plugin.so");
    QPluginLoader pluginLoader(plupath);
    QObject *plugin = pluginLoader.instance();
    if (plugin){
        CuFormulaPluginI *fplu = qobject_cast<CuFormulaPluginI *>(plugin);
        if(!fplu) {
            ui.gbFormula->setTitle("Formula plugin is not available");
            ui.gbFormula->setToolTip(pluginLoader.errorString());
            ui.labelPluginErr->setText(ui.gbFormula->toolTip());
        }
        else {
            m_formulaParserI = fplu->getFormulaParserInstance();
            ui.labelPluginErr->setVisible(false);
        }
    }

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
                s = QString("formula://[%1]%2 %3").arg(ui.leAlias->text()).arg(ui.lineEdit->text()).arg(formula);
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
    if(m_formulaParserI) {
        m_formulaParserI->parse(s);
        if(!m_formulaParserI->error()) {
            QString srcs = QString::fromStdString(m_formulaParserI->joinedSources());
            ui.leAlias->setText(m_formulaParserI->name());
            ui.lineEdit->setText(QString("{%1}").arg(srcs));
            ui.teJsCode->setPlainText(m_formulaParserI->formula());
            ui.gbFormula->setChecked(true);
        }
    }
    if(ui.lineEdit->text().isEmpty()) {
        ui.lineEdit->setText(s);
        ui.gbFormula->setChecked(false);
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


