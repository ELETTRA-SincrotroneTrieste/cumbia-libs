#include "einputoutputwidget.h"
#include <cumacros.h>
#include <QPushButton>
#include <QGroupBox>
#include <QtDebug>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QMetaProperty>

/*! @private */
Container::Container(QWidget *parent) : QFrame(parent) {
    setFrameShape(QFrame::StyledPanel);
    setWindowFlag(Qt::Popup, true);
}

/*! @private */
void Container::hideEvent(QHideEvent *e) {
    QFrame::hideEvent(e);
    emit visibilityChanged(false);
}

/*! @private */
class EReaderWriterPrivate
{
public:
    EReaderWriterPrivate() : on_apply_hide_writer(true) {}

    QWidget *input_w;
    Container *w_container;
    QPushButton *pbApply, *pbEdit;
    QWidget *output_w;
    bool on_apply_hide_writer;
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

/** \brief single parameter constructor.
 *
 * setOutputWidget must be called manually later.
 *
 * @see EInputOutputWidget::EInputOutputWidget(QWidget* outputw, QWidget *parent)
 */
EInputOutputWidget::EInputOutputWidget(QWidget *parent) : QFrame(parent)
{
    d = new EReaderWriterPrivate;
    m_createContainer();
}

/*! \brief sets the *output widget* that's used to display a value
 *
 * @param outputw an output widget, with either the *value* or *text*
 */
void EInputOutputWidget::setOutputWidget(QWidget *outputw)
{
    m_init(outputw);
}

/*! \brief sets a QWidget as input widget
 *
 * @param inputw a QWidget that will be used as input widget
 */
void EInputOutputWidget::setInputWidget(QWidget *inputw)
{
    if(d->input_w == nullptr) {
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

        d->pbApply->setMinimumWidth(fm.horizontalAdvance(d->pbApply->text()) * 1.4);
        connect(d->pbApply, SIGNAL(clicked()), this, SLOT(m_applyClicked()));
    }
    else
        perr("EInputOutputWidget.setInputWidget: this method can be called only once in \"%s\"", qstoc(objectName()));
}

/*! calculates the minimum size hint for the widget
 *
 * @return the minimum size hint for the widget
 *
 * @see sizeHint
 */
QSize EInputOutputWidget::minimumSizeHint() const
{
    int inwidth = 0, outwidth, width, height, lo_leftm, lo_topm, lo_rightm, lo_botm;
    layout()->getContentsMargins(&lo_leftm, &lo_topm, &lo_rightm, &lo_botm);
    d->input_w && d->pbApply ? // suppose that d->pbApply width is the same as d->pbEdit's
        inwidth = d->input_w->minimumSizeHint().width() + d->pbApply->minimumSizeHint().width() : 0;
    QFontMetrics fm(font());
    // QLabel minimum size hint  + space for button text
    outwidth = d->output_w->minimumSizeHint().width() +
               (fm.horizontalAdvance(d->pbEdit->text()) * 1.8);
    width = qMax(inwidth, outwidth);
    height = qMax(d->output_w ? d->output_w->minimumSizeHint().height() : 0, d->pbEdit ? d->pbEdit->minimumSizeHint().height() + 12 : 0 );
    width += (lineWidth() + midLineWidth() + frameWidth() + lo_leftm + lo_rightm + layout()->spacing() );
    height += contentsMargins().top() + contentsMargins().bottom() +
              (lineWidth() + midLineWidth() + frameWidth() + lo_topm + lo_botm + layout()->spacing() );
    return QSize(width, height);
}

/*! calculates the size hint for the widget
 *
 * @return the size hint for the widget, calculated according to the input widget
 *         and apply button size hint widths and the output widget's height
 *
 * @see minimumSizeHint
 */
QSize EInputOutputWidget::sizeHint() const
{
    int width;
    if(d->input_w && d->pbApply)
        width = d->input_w->sizeHint().width() + d->pbApply->sizeHint().width();
    else
        width = d->output_w->sizeHint().width();
    return QSize(width, d->output_w->height());
}

/*! \brief returns the input widget
 * @return the input widget
 */
QWidget *EInputOutputWidget::inputWidget() const
{
    return d->input_w;
}

/*! \brief Returns the QFrame containing the writer
 *
 * @return QFrame containing the writer
 */
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
QPushButton *EInputOutputWidget::getApplyButton() {
    if(!d->pbApply)
        d->pbApply = new QPushButton("OK", this);
    return d->pbApply;
}

/*!
 * \brief EInputOutputWidget::onApplyHideWriter
 * \return true if the writer is hidden after Apply button is clicked, false otherwise
 */
bool EInputOutputWidget::onApplyHideWriter() const {
    return d->on_apply_hide_writer;
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

/** \brief sets the "value" property on the output widget.
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

/** \brief sets the "value" property on the output widget.
 *
 * @param s the value to display on the output widget
 *
 * \par Note
 * The output widget must declare the value Q_PROPERTY
 */
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

/*!
 * \brief EInputOutputWidget::setOnApplyHideWriter
 * \param h true hide writer when apply is clicked (default).
 * \param h false keep the writer visible after apply is clicked.
 */
void EInputOutputWidget::setOnApplyHideWriter(bool h) {
    d->on_apply_hide_writer = h;
}

/*! \brief returns the output widget
 *
 * @return the output widget as QWidget
 */
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

/*! @private */
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

/*! @private */
void EInputOutputWidget::m_editToggled(bool en)
{
    if(en)
        m_show();
    d->w_container->setVisible(en);
}

/** @private
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
    if(d->on_apply_hide_writer)
        d->w_container->setVisible(false);
}

/*! @private */
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
    lo->setSpacing(1);
    lo->setMargin(2);
    new QHBoxLayout(d->w_container);
    connect(d->w_container, SIGNAL(visibilityChanged(bool)), d->pbEdit, SLOT(setChecked(bool)));
    // Adjust pbEdit geometry
    QFontMetrics fm(font());
    d->pbEdit->setMinimumWidth(fm.horizontalAdvance(d->pbEdit->text()) * 1.8);
    d->pbEdit->setDisabled(true);

    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SLOT(m_editToggled(bool)));
    connect(d->pbEdit, SIGNAL(toggled(bool)), this, SIGNAL(editButtonToggled(bool)));
    connect(d->pbEdit, SIGNAL(clicked(bool)), this, SIGNAL(editButtonClicked()));

    outputw->setObjectName(objectName() + "_outputWidget");
}

/*! @private */
void EInputOutputWidget::m_createContainer()
{
    d->w_container = new Container(this);
    d->w_container->setVisible(false);
}

/*! @private */
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

