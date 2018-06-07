#include "qutable.h"
#include <eflag.h>
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include <cudatatypes_ex.h>
#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontext.h"
#include "culinkstats.h"
#include <QVector>
#include <QtDebug>

/** @private */
class QuTablePrivate
{
public:
    bool auto_configure, read_ok;
    std::vector<std::string> desired_att_props;
    QuPalette palette;
    CuContext *context;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuTable::QuTable(QWidget *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) :
    EFlag(parent)
{
    m_init();
    d->context = new CuContext(cumbia, r_fac);
    m_initCtx();
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuTable::QuTable(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    EFlag(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
    m_initCtx();
}

QuTable::~QuTable()
{
    delete d->context;
    delete d;
}

void QuTable::m_init()
{
    d = new QuTablePrivate;
    d->auto_configure = true;
    d->read_ok = false;

    QColor background = d->palette["white"];
    QColor border = d->palette["gray"];

    /* some free properties */
    setProperty("trueString", "TRUE");
    setProperty("falseString", "FALSE");
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));

    foreach(ELabel *l, cells)
    {
        l->setValue(QString("No Link"));
        l->decorate(background, border);
    }
}

void QuTable::m_initCtx()
{
    d->desired_att_props.push_back("numRows");
    d->desired_att_props.push_back("numColumns");
    d->desired_att_props.push_back("displayMask");
    d->desired_att_props.push_back("trueStrings");
    d->desired_att_props.push_back("trueColours");
    d->desired_att_props.push_back("falseColours");
    d->desired_att_props.push_back("falseStrings");
    d->context->setOptions(CuData(CuXDType::FetchProperties, d->desired_att_props));
}

QString QuTable::source() const
{
    if(d->context->getReader())
        return d->context->getReader()->source();
    return "";
}

CuContext *QuTable::getContext() const
{
    return d->context;
}

void QuTable::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuTable::unsetSource()
{
    d->context->disposeReader();
}

void QuTable::onUpdate(const CuData& da)
{
    QColor background, border;
    d->read_ok = !da[CuDType::Err].toBool();
    setEnabled(d->read_ok);

    if(d->read_ok && d->auto_configure && da[CuDType::Type].toString() == "property")
        configure(da);

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[CuDType::Message].toString());

    if(da.containsKey(CuXDType::QualityColor))
        background = d->palette[QString::fromStdString(da[CuXDType::QualityColor].toString())];
    if(da.containsKey(CuXDType::SuccessColor))
        border = d->palette[QString::fromStdString(da[CuXDType::SuccessColor].toString())];

    setToolTip(da[CuDType::Message].toString().c_str());

    if(da[CuDType::Err].toBool() )
    {
        foreach(ELabel *l, cells)
            l->setText("####");
    }
    else if(da.containsKey(CuDType::Value))
    {
        CuVariant val = da[CuDType::Value];
        cuprintf("QuTable value is %s type %d\n\n", val.toString().c_str(), val.getType());
        if(val.getType() == CuVariant::UInt && val.getFormat() == CuVariant::Scalar)
            EFlag::setValue(QVariant(val.toUInt()));
        else if(val.getType() == CuVariant::UShort && val.getFormat() == CuVariant::Scalar)
            EFlag::setValue(QVariant(val.toUShortInt()));
        else if(val.getType() == CuVariant::LongInt && val.getFormat() == CuVariant::Scalar)
            EFlag::setValue(QVariant(static_cast<long long int>(val.toLongInt())));
        else if(val.getType() == CuVariant::LongUInt && val.getFormat() == CuVariant::Scalar)
            EFlag::setValue(QVariant(static_cast<unsigned long long int>(val.toULongInt())));
        else if(val.getType() == CuVariant::Boolean && val.getFormat() == CuVariant::Vector)
        {
            std::vector<bool> bv = val.toBoolVector();
            std::vector<bool>::const_iterator it;
            QList<QVariant> l;
            for (it = bv.begin(); it < bv.end(); it++)
                l << QVariant(*it);
            EFlag::setValue(QVariant(l));
        }
        else if(val.getType() == CuVariant::String && val.getFormat() == CuVariant::Vector)
        {
            std::vector<std::string> data = val.toStringVector();
            int size = qMin(cells.size(),(int)data.size());
            for (int i = 0; i < size; i++)
            {
                cells[i]->setValue(QVariant(QString::fromStdString(data[i])), false);
                cells[i]->display();
            }
        }
        else
        {
            border = d->palette["gray"];
            foreach(ELabel *l, cells)
                l->setText("####");
            setToolTip(QString("Wrong data type %1 format %2").arg(val.getType()).arg(val.getFormat()));
        }
    }

    if(da.containsKey(CuXDType::StateColor))
    {
        CuVariant v = da[CuXDType::StateColor];
        background = d->palette[QString::fromStdString(v.toString())];
    }

//    foreach(ELabel *l, cells)
//        l->decorate(background, border);

    emit newData(da);
}

void QuTable::configure (const CuData& da)
{
    cuprintf("\e[1;34mQuTable:configure: got configuration data as follows:\n\n%s\n\n", da.toString().c_str());

    if(da.containsKey(CuXDType::Description))
        setWhatsThis(da[CuXDType::Description].toString().c_str());

    try
    {
        if(da.containsStrKey("numRows"))
        {
            int numRows = std::stoi(da["numRows"].toString());
            if(numRows > 0)
                EFlag::setNumRows(numRows);
        }
        if(da.containsStrKey("numColumns"))
        {
            int numColumns = std::stoi(da["numColumns"].toString());
            if(numColumns > 0)
                EFlag::setNumColumns(numColumns);
        }
        if(da.containsStrKey("displayMask"))
        {
            /* EFlag display mask requires comma separated values */
            QString dmask = QString::fromStdString(da["displayMask"].toString());
            EFlag::setDisplayMask(dmask); /* display mask comma separated */
        }
        if(da.containsStrKey("trueStrings") && da.containsStrKey("falseStrings"))
        {
            QString ts = QString::fromStdString(da["trueStrings"].toString());
            QString fs = QString::fromStdString(da["falseStrings"].toString());
            if(ts.count(",") == fs.count(","))
            {
                EFlag::setTrueStrings(ts.replace(",", ";"));
                EFlag::setFalseStrings(fs.replace(",", ";"));
            }
            else
            {
                perr("QuTable::configure: table \"%s\" connected to \"%s\" has inconsistent true and false strings size: %d/%d",
                     qstoc(objectName()), qstoc(source()), ts.count(","), fs.count(",") );
            }
        }
        if(da.containsStrKey("trueColours") && da.containsStrKey("falseColours"))
        {
            QString tc = QString::fromStdString(da["trueColours"].toString());
            QString fc = QString::fromStdString(da["falseColours"].toString());
            if(tc.count(",") == tc.count(","))
            {
                QList<QColor> trueColors;
                QList<QColor> falseColors;
                foreach(QString s, tc.split(",", QString::SkipEmptyParts))
                    trueColors << QColor(s);
                foreach(QString s, fc.split(",", QString::SkipEmptyParts))
                    falseColors << QColor(s);
                EFlag::setTrueColorList(trueColors);
                EFlag::setFalseColorList(falseColors);
            }
            else
            {
                perr("QuTable::configure: table \"%s\" connected to \"%s\" has inconsistent true and false color size: %d/%d",
                     qstoc(objectName()), qstoc(source()), tc.count(","), fc.count(",") );
            }
        }


        /* additional info displaying mask (since: 4.3.4) */
        /* first remove old information about display mask, if present */
        QString displayMask;
        QList< QList<unsigned int> > dMask = EFlag::displayMask();
        if(dMask.size())
        {
            displayMask += "<!-- displayMask info -->";
            displayMask += "<h3>Display Mask</h3><ul>\n";
            for(int i = 0; i < dMask.size(); i++)
            {
                QList<unsigned int> nested = dMask.at(i);
                displayMask += "<li>";
                for(int j = 0; j < nested.size(); j++)
                    displayMask += QString("%1").arg(nested.at(j)) + " ";
                displayMask += "</li>";
            }
            displayMask += "</ul><!-- end displayMask info -->";
        }
        setProperty("display_mask_html", displayMask);
    }
    catch(const std::invalid_argument &ia)
    {
        perr("QuTable::configure: error converting string to number: %s", ia.what());
    }
    catch(const std::out_of_range &outr)
    {
        perr("QuTable::configure: error converting string to number: %s", outr.what());
    }
}




