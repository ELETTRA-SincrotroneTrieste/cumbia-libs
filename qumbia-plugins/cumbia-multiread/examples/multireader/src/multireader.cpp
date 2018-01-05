#include "multireader.h"

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QApplication>
#include <QProgressBar>

#include <qumultireaderplugininterface.h>

#include <QDir>
#include <QPluginLoader>
#include <cumacros.h>
#include <cutreader.h>
#include <QtDebug>

Multireader::Multireader(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)
{
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    m_sequential =  qApp->arguments().contains("--sequential");
    QStringList srcs;
    QGridLayout *lo = new QGridLayout(this);
    int nr = qApp->arguments().count() - 1;

    if(nr == 0) {
        printf("Usage: %s test/device/1/double_scalar test/device/1/short_scalar test/device/2/double_scalar test/device/2/short_scalar"
               " [--sequential to perform sequential readings ]", qstoc(qApp->arguments().first()));
        exit(EXIT_SUCCESS);
    }

    for(int i = 0; i < nr; i++)
    {
        if(qApp->arguments().at(i+1).count("/") < 2) // skip other command line arguments
            continue;
        QLabel *l = new QLabel(qApp->arguments().at(i + 1), this);
        l->setToolTip("This value is updated as soon as it's available");
        lo->addWidget(l, i, 0, 1, 1);
        QLineEdit *le = new QLineEdit(this);
        le->setObjectName(l->text());
        lo->addWidget(le, i, 1, 1, 1);
        srcs << le->objectName();
    }

    // sum
    QLabel *lsum = new QLabel("Sum", this);
    lo->addWidget(lsum, ++nr, 0, 1, 1);
    lsum->setEnabled(m_sequential);
    QLineEdit *lesum = new QLineEdit(this);
    lesum->setObjectName("sum");
    lo->addWidget(lesum, nr, 1, 1, 1);
    lesum->setEnabled(m_sequential);
    lesum->setToolTip("This value is available when the n values above have been read in sequence");

    // avg
    QLabel *lavg = new QLabel("Average", this);
    lo->addWidget(lavg, ++nr, 0, 1, 1);
    lavg->setEnabled(m_sequential);
    QLineEdit *leavg = new QLineEdit(this);
    leavg->setObjectName("avg");
    lo->addWidget(leavg, nr, 1, 1, 1);
    leavg->setEnabled(m_sequential);
    leavg->setToolTip("This value is available when the n values above have been read in sequence");

    QLabel *lTip = new QLabel("Sum and average available if program is launched with \"--sequential\"");
    lo->addWidget(lTip, ++nr, 0, 1, 2);
    lTip->setHidden(m_sequential);

    foreach(QLineEdit *le, findChildren<QLineEdit *>() )
        le->setReadOnly(true);

    m_loadMultiReaderPlugin();

    if(!m_multir) {
        perr("MultiReader.MultiReader: failed to load multi reader plugin");
        exit(EXIT_FAILURE);
    }
    int manual_code;
    m_sequential ? manual_code = CuTReader::Manual : manual_code = -1;
    m_sequential ? setWindowTitle("Multi reader example: sequential mode") : setWindowTitle("Multi reader example: parallel readings");

    // configure multi reader
    m_multir->init(cu_t, cu_tango_r_fac, manual_code);
    // get multi reader as qobject in order to connect signals to slots
    connect(m_multir->get_qobject(), SIGNAL(onNewData(const CuData&)), this, SLOT(newData(const CuData&)));
    connect(m_multir->get_qobject(), SIGNAL(onSeqReadComplete(const QList<CuData >&)), this, SLOT(seqReadComplete(const QList<CuData >&)));
    // set the sources
    m_multir->setSources(srcs);
}

Multireader::~Multireader()
{
    if(m_multir) delete m_multir;
}

// single read complete
//
void Multireader::newData(const CuData &da)
{
    QString src = QString::fromStdString(da["src"].toString());
    QLineEdit *le = findChild<QLineEdit *>(src); // find the line edit by source
    le->setDisabled(da["err"].toBool());
    if(le->isEnabled())
        le->setText(QString::fromStdString(da["value"].toString()));
    else
        le->setText("####");

    le->setToolTip(QString::fromStdString(da["msg"].toString()));
}

// read  cycle complete
//
void Multireader::seqReadComplete(const QList<CuData> &da)
{
    double sum = 0;
    double val;
    double cnt = 0;
    foreach(const CuData&d, da)
    {
        if(!d["err"].toBool()) {
            d["value"].to<double>(val);
            sum += val;
            cnt++;
        }
    }
    QLineEdit *lie = findChild<QLineEdit *>("sum");
    lie->setText(QString::number(sum));
    lie = findChild<QLineEdit *>("avg");
    (cnt > 0) ? lie->setText(QString::number(sum / cnt)) : lie->setText("0");
}

void Multireader::m_loadMultiReaderPlugin()
{
    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            m_multir = qobject_cast<QuMultiReaderPluginInterface *>(plugin);
        }
        else
            perr("Multireader.m_loadMultiReaderPlugin: error loading plugin: %s", qstoc(pluginLoader.errorString()));
    }
}
