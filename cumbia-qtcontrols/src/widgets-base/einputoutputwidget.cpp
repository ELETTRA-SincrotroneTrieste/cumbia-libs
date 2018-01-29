#include "einputoutputwidget.h"
#include <cumacros.h>
#include <QPushButton>
#include <QGroupBox>
#include <QtDebug>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QMetaProperty>


Container::Container(QWidget *parent) : QFrame(parent) {
    setFrameShape(QFrame::StyledPanel);
    setWindowFlag(Qt::Popup, true);
}

void Container::hideEvent(QHideEvent *e) {
    QFrame::hideEvent(e);
    emit visibilityChanged(false);
}

class EReaderWriterPrivate
{
public:
    QWidget *input_w;
    Container *w_container;
    QPushButton *pbApply, *pbEdit;
    QWidget *output_w;
};

/** \brief A container to organize an output (display) widget and an input (writer) widget.
 *
 * EInputOutputWidget is initialised with an <em>output widget</em> and a parent. The <em>output
 * widget</em> is used to display data (read value).
 *
 * setInputWidget must be called with an input widget as parameter. The <em>input widget</em> is normally
 * hidden until the "<em>...</em>" button on the right of the output widget is clicked.
 * The input widget is shown through a popup below the output widget. Clicking anywhere out of the input
 * widget makes it disappear, as in a combo box.
 *
 * \par Apply the input value
 * When the "<em>OK</em>" button is clicked on the input (popup) container, one to three signals can be emitted:
 *
 * \li an empty applyClicked signal is always emitted before attempting to read the <em>value</em> and <em>text</em>
 *     properties.
 * \li If the input widget has the "value" property, the applyClicked(double) signal is emitted with the value converted to double
 * \li If the input widget has the "text" property, the applyClicked(QString) signal is emitted with the text
 * \li If the input widget has the "value" property, the applyClicked(QString) signal is emitted with the value
 *      converted to string with the QString::number() method.
 *
 * \par Note:
 * If no "value" nor "text" properties are defined for the input widget, then only the empty applyClicked()
 * signal is emitted, at the beginning of the method.
 *
 * \par Note:
 * The minimumSizeHint.width of the input widget is used to provide the minimumSizeHint.width of the EInputOutputWidget.
 *
 * \par Object names and relationships
 * \li Input widget object name is set to objectName() + "_inputWidget"
 * \li Output widget object name is set to objectName() + "_outputWidget"
 * \li To find the apply button, use getContainer()->findChild<QPushButton *>() or the getApplyButton method
 * \li To find the edit button, use findChild<QPushButton *>("pbEdit")
 *
*/
EInputOutputWidget::EInputOutputWidget(QWidget* outputw, QWidget *parent) : QFrame(parent)
{
    d = new EReaderWriterPrivate;
    m_createContainer();
    setOutputWidget(outputw);
}

EInputOutputWidget::EInputOutputWidget(QWidget *parent) : QFrame(parent)
{
    d = new EReaderWriterPrivate;
    m_createContainer();
}

void EInputOutputWidget::setOutputWidget(QWidget *outputw)
{
    m_init(outputw);
}

void EInputOutputWidget::setInputWidget(QWidget *inputw)
{
    if(d->input_w == NULL)
    {
        d->input_w = inputw;
        d->input_w->setObjectName(objectName() + "_inputWidget");
        inputw->setParent(d->w_container);
        m_editToggled(false);
        QHBoxLayout *hlo = static_cast<QHBoxLayout *>(d->w_container->layout());
        hlo->insertWidget(0, d->input_w, 5);
        //    updateGeometry();
        d->pbEdit->setEnabled(true);
        d->pbApply = getApplyButton();
        d->pbApply->setParent(d->w_container);
        hlo->addWidget(d->pbApply, 1);
        // Adjust pbApply geometry
        QFontMetrics fm(font());
        d->pbApply->setMinimumWidth(fm.width(d->pbApply->text()) * 1.4);
        connect(d->pbApply, SIGNAL(clicked()), this, SLOT(m_applyClicked()));
    }
    else
        perr("EInputOutputWidget.setInputWidget: this method can be called only once in \"%s\"", qstoc(objectName()));
}

QSize EInputOutputWidget::minimumSizeHint() const
{
    int width, height;
    if(d->input_w) // suppose that d->pbApply width is the same as d->pbEdit's
        width = d->input_w->minimumSizeHint().width() + d->pbEdit->minimumSizeHint().width();
    else
        width = d->output_w->minimumSizeHint().width();
    height = qMax(d->output_w->minimumSizeHint().height(), d->pbEdit->minimumSize().height() + 12);
    width += contentsMargins().left() + contentsMargins().right() +
            (lineWidth() + midLineWidth() + frameWidth() + layout()->margin() + layout()->spacing() );
    height += contentsMargins().top() + contentsMargins().bottom() +
            (lineWidth() + midLineWidth() + frameWidth() + layout()->margin() + layout()->spacing() );
    return QSize(width, height);
}

QSize EInputOutputWidget::sizeHint() const
{
    int width;
    if(d->input_w && d->pbApply)
        width = d->input_w->sizeHint().width() + d->pbApply->sizeHint().width();
    else
        width = d->output_w->sizeHint().width();
    return QSize(width, d->output_w->height());
}

QWidget *EInputOutputWidget::inputWidget() const
{
    return d->input_w;
}

QFrame *EInputOutputWidget::getContainer() const
{
    return d->w_container;
}

/**
 * \brief Provide an apply push button.
 *
 * The base implementation makes a QPushButton.
 * The apply button, when clicked, emits the applyClicked signals.
 *
 * \note The apply button must be created only once.
 */
QPushButton *EInputOutputWidget::getApplyButton()
{
    if(!d->pbApply)
        d->pbApply = new QPushButton("OK", this);
    return d->pbApply;
}

/** \brief Tries to write the "value" property on the input widget.
 *
 * @param s the value to display on the input widget
 *
 * \par Note
 * The input widget must declare the value Q_PROPERTY
 */
void EInputOutputWidget::setInputValue(double val)
{
    m_setValue(val, d->input_w);
}

void EInputOutputWidget::setInputValue(int val)
{
    m_setValue(val, d->input_w);
}

/** \brief Tries to write the "text" property on the input widget.
 *
 * @param s the value to display on the input widget
 *
 * \par Note
 * The input widget must declare the text Q_PROPERTY
 */
void EInputOutputWidget::setInputText(const QString &text)
{
    m_setText(text, d->input_w);
}

/** \brief Tries to write the "value" property on the output widget.
 *
 * @param s the value to display on the output widget
 *
 * \par Note
 * The output widget must declare the value Q_PROPERTY
 */
void EInputOutputWidget::setOutputValue(int val)
{
    m_setValue(val, d->output_w);
}

void EInputOutputWidget::setOutputValue(double val)
{
    m_setValue(val, d->output_w);
}

/** \brief Tries to write the "text" property on the output widget.
 *
 * @param s the value to display on the output widget
 *
 * \par Note
 * The output widget must declare the text Q_PROPERTY
 */
void EInputOutputWidget::setOutputText(const QString &text)
{
    m_setText(text, d->output_w);
}

QWidget *EInputOutputWidget::outputWidget() const
{
    return d->output_w;
}

void EInputOutputWidget::m_setValue(const QVariant& dv, QWidget *w)
{
    bool success;
    int idx = w->metaObject()->indexOfProperty("value");
    success = idx >= 0;
    if(success)
    {
        QMetaProperty mp = w->metaObject()->property(idx);
        success = mp.isValid();
        if(success)
            success = mp.write(w, dv);
    }
    if(!success)
        perr("EInputOutputWidget::setValue: failed to write \"value\" property on \"%s\"", qstoc(objectName()));
}

void EInputOutputWidget::m_setText(const QString &s, QWidget *w)
{
    bool success;
    int idx = w->metaObject()->indexOfProperty("text");
    success = idx >= 0;
    if(success)
    {
        QMetaProperty mp = w->metaObject()->property(idx);
        success = mp.isValid();
        if(success)
            success = mp.write(w, s);
    }
    if(!success)
        perr("EInputOutputWidget::setValue: failed to write \"text\" property on \"%s\"", qstoc(objectName()));
}

void EInputOutputWidget::m_editToggled(bool en)
{
    if(en)
        m_show();
    d->w_container->setVisible(en);
}

/*
 * emits applyClicked
 * If the input widget has the "value" property, the applyClicked(double) signal is emitted with the value
 * If the input widget has the "text" property, the applyClicked(QString) signal is emitted with the text
 * If the input widget has the "value" property, the applyClicked(QString) signal is emitted with the value
 * converted to string with the QString::number() method.
 *
 * If no "value" nor "text" properties are defined for the input widget, then only the empty applyClicked()
 * signal is emitted, at the beginning of the method.
 */
void EInputOutputWidget::m_applyClicked()
{
    emit applyClicked();

    double v = 0.0;
    QString text;
    bool ok = false;
    int pi = d->input_w->metaObject()->indexOfProperty("value");
    if(pi >= 0)
        v = d->input_w->metaObject()->property(pi).read(d->input_w).toDouble(&ok);
    if(ok)
        emit applyClicked(v);

    // try with text
    pi = d->input_w->metaObject()->indexOfProperty("text");
    if(pi >= 0)
        text = d->input_w->metaObject()->property(pi).read(d->input_w).toString();
    else if(ok) // we have a double in v
        text = QString::number(v);

    if(pi >= 0 && !ok) // text but no valid value: convert it to double
    {
        v = text.toDouble(&ok);
        if(ok)
            emit applyClicked(v);
    }

    if(pi >= 0 || ok)
        emit applyClicked(text);
}

void EInputOutputWidget::m_init(QWidget *outputw)
{
    setFrameStyle(QFrame::Sunken);
    setFrameShape(QFrame::StyledPanel);
    d->input_w = NULL;
    d->pbApply = NULL;
    d->output_w = outputw;
    if(outputw->parent() != this)
        d->output_w->setParent(this);
    d->pbEdit = new QPushButton("...", this);
    d->pbEdit->setObjectName("pbEdit");
    QHBoxLayout *lo  = new QHBoxLayout(this);
    d->pbEdit->setCheckable(true);
    lo->addWidget(d->output_w);
    lo->addWidget(d->pbEdit);
    lo->setStretch(0, 10);
    lo->setStretch(1, 1);
    new QHBoxLayout(d->w_container);
    connect(d->w_container, SIGNAL(visibilityChanged(bool)), d->pbEdit, SLOT(setChecked(bool)));
    // Adjust pbEdit geometry
    QFontMetrics fm(font());
    d->pbEdit->setMinimumWidth(fm.width(d->pbEdit->text()) * 1.8);
    d->pbEdit->setDisabled(true);

    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SLOT(m_editToggled(bool)));
    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SIGNAL(editButtonToggled(bool)));
    connect(d->pbEdit, SIGNAL(clicked(bool)), this, SIGNAL(editButtonClicked()));

    outputw->setObjectName(objectName() + "_outputWidget");
}

void EInputOutputWidget::m_createContainer()
{
    d->w_container = new Container(this);
    d->w_container->setVisible(false);
}

void EInputOutputWidget::m_show()
{
    QPoint wbl = frameGeometry().bottomLeft(); // writer top left
    QPoint wtl = frameGeometry().topLeft();
    QPoint gp = mapToGlobal(wbl);
    gp.setX(gp.x() - wtl.x());
    gp.setY(gp.y() - wtl.y() + 1);
    d->w_container->resize(frameSize().width(), d->w_container->minimumSizeHint().height());
    d->w_container->move(gp);
    // qDebug() << __FUNCTION__ << "GEOM: from " << wtl << "TO" << d->w_container->geometry() << "TOP LEFT WAS " << frameGeometry().topLeft();
}

