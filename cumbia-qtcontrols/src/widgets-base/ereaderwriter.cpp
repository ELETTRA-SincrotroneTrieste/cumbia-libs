#include "ereaderwriter.h"
#include "eapplynumeric.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QtDebug>

class EReaderWriterPrivate
{
public:
    EReaderWriter::WriterType type;
    QWidget *writer;
    QPushButton *pbApply, *pbEdit;
    EReaderWriter::WriterPosition w_pos;
};

EReaderWriter::EReaderWriter(QWidget *parent) : ELabel(parent)
{
    d = new EReaderWriterPrivate;
    d->type = Numeric;
    d->writer = NULL;
    d->pbApply = NULL;
    d->pbEdit = NULL;
    d->w_pos = AutoPosition;
    m_init();
}

EReaderWriter::WriterType EReaderWriter::writerType() const
{
    return d->type;
}

QSize EReaderWriter::minimumSizeHint() const
{
    int width;
    if(d->writer)
        width = d->writer->minimumSizeHint().width();
    else
        width = ELabel::minimumSizeHint().width();
    return QSize(width, ELabel::minimumSizeHint().height());
}

QSize EReaderWriter::sizeHint() const
{
    int width;
    if(d->writer)
        width = d->writer->sizeHint().width();
    else
        width = ELabel::sizeHint().width();
    return QSize(width, ELabel::sizeHint().height());
}

void EReaderWriter::setWriterType(EReaderWriter::WriterType t)
{
    d->type = t;
    if(d->writer)
        delete d->writer;
    if(!d->pbApply)
    {
        d->pbApply = new QPushButton(this);
        d->pbApply->setText("OK");
    }
    if(d->type == Numeric)
    {
        d->writer = new ENumeric(this);
    }
    else if(d->type == DoubleSpinBox)
    {
        d->writer = new QDoubleSpinBox(this);
    }
    else if(d->type == SpinBox)
    {
        d->writer = new QSpinBox(this);
    }
    m_editToggled(false);
}

void EReaderWriter::setValue(double d)
{
    m_setData(d);
}

void EReaderWriter::setValue(int i)
{
    m_setData(i);
}

void EReaderWriter::setValue(const QString &s)
{
    m_setData(s);
}

void EReaderWriter::m_setData(const QVariant &d)
{
    setText(d.toString());
}

void EReaderWriter::m_editToggled(bool en)
{
    if(en && d->writer)
    {
        m_showWriter();
        m_repositionAll();
    }
    else if(d->writer)
    {
        if(d->pbApply)
            d->pbApply->hide();
        d->pbEdit->hide();
        d->writer->hide();
    }
}

void EReaderWriter::enterEvent(QEvent *e)
{

    m_showEditButton();
    QWidget::enterEvent(e);
}

void EReaderWriter::leaveEvent(QEvent *e)
{
    if(d->pbApply && !d->pbApply->isVisible())
        d->pbEdit->scheduleHide();
    QWidget::leaveEvent(e);
}

void EReaderWriter::mouseMoveEvent(QMouseEvent *e)
{

}

void EReaderWriter::mousePressEvent(QMouseEvent *)
{

}

void EReaderWriter::m_init()
{
    d->pbEdit = new QPushButton("Edit", this);
    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SLOT(m_editToggled(bool)));
    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SIGNAL(editButtonToggled(bool)));
    connect(d->pbEdit, SIGNAL(clicked(bool)), this, SIGNAL(editButtonClicked()));
    setCursor(QCursor(Qt::PointingHandCursor));
    setMouseTracking(true); /* for enter/leave events */
}

void EReaderWriter::m_showWriter()
{
    if(d->writer)
    {
        d->writer->show();
        if(d->pbApply)
        {
            d->pbApply->show();
            d->pbApply->raise();
        }
    }
}

void EReaderWriter::m_showEditButton()
{
    if(d->writer && !d->pbEdit->isVisible())
    {
        m_editToggled(d->writer->isVisible());
        d->pbEdit->show();
        d->pbEdit->raise();
        m_repositionAll();
    }
}

void EReaderWriter::m_repositionAll()
{
    int applyWidth = 16;
    if(!d->writer)
        return;

    int height = this->height();


    /* writer widget shorter by the pbApply width minus... */
    applyWidth = d->pbApply->width();

    /* position the edit push button according to the geometry of the reader writer and the width
         * of the apply button, if present, or the default applyWidth
         */
    d->pbEdit->move(mapToParent(rect().topRight() - QPoint(applyWidth, 0)));
    qDebug() << mapToParent(rect().topRight() - QPoint(applyWidth, 0)) << "TReaderWriter rect " << geometry() <<
                "pb edit geometry " << d->pbEdit->geometry() << "pbEdit visible" << d->pbEdit->isVisible();

    /* d->pbApply does not scale with rescale because spin box or label height is constant */
    /* 1. parent widget contains writer below reader */
    if(( parentWidget() && (d->w_pos == AutoPosition && parentWidget()->rect().contains(
                                mapToParent(QPoint(rect().bottomLeft().x(),
                                                   rect().bottomLeft().y() + d->writer->height())))) ) || d->w_pos == South)
    {
        d->writer->move(mapToParent(rect().bottomLeft()));
        /* 1a. parent widget contains edit and apply buttons at reader's right */
        d->pbApply->move(mapToParent(rect().bottomRight() - QPoint(applyWidth, 0)));
    }
    /* 2. parent widget does not contain writer below reader */
    else if(parentWidget() && (d->w_pos == AutoPosition || d->w_pos == North))
    {
        /* move the writer up */
        d->writer->move(mapToParent(rect().topLeft()) - QPoint(0, d->writer->height()));
        /* 2a. parent contains apply and edit buttons at readers'right */
        d->pbApply->move(mapToParent(rect().topRight() - QPoint(applyWidth, d->writer->height())));
    }
    else if(parentWidget() && d->w_pos == East) /* place on the reader's right */
    {
        d->writer->move(mapToParent(rect().topRight() /*+ QPoint(1, 1)*/));
        d->pbApply->move(mapToParent(rect().topRight() + QPoint(d->writer->width(), 0)));
    }
    else if(parentWidget() && d->w_pos == West) /* place on the reader's right */
    {
        d->writer->move(mapToParent(rect().topLeft() - QPoint(d->writer->width(), 0)));
        d->pbApply->move(mapToParent(rect().topLeft() - QPoint(d->writer->width() + d->pbApply->width(), 0)));
    }

}

