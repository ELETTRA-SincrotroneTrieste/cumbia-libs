#include "eimagewidget.h"
#include "colortablemap.h"
#include <QtDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>

/** @private
 * Class with properties specific to EImageWidget, as a QWidget.
 * Other properties are stored by EImageWidgetBasePrivate
 */
class EImageWidgetPrivate
{
public:

    bool scaleWithZoom;
};

/** \brief the class constructor
 *
 * @param parent a QWidget as a parent of this widget
 *
 */
EImageWidget::EImageWidget(QWidget *parent) :
    QWidget(parent),
    EImageWidgetBase(this, false)
{
    d = new EImageWidgetPrivate;
    d->scaleWithZoom = true;
}

EImageWidget::~EImageWidget()
{
    delete d;
}

bool EImageWidget::scaleWithZoom() const
{
    return d->scaleWithZoom;
}

/** \brief returns the QImage used as error image
 *
 * @return a QImage that is displayed when an error occurs
 *
 * The error image can be changed with setErrorImage
 */
QImage EImageWidget::errorImage() const
{
    return EImageWidgetBase::errorImage();
}

/** \brief set an error string
 *
 * @param msg an error string that will be displayed on the image
 * if an error occurs
 */
void EImageWidget::setErrorMessage(const QString &msg)
{
    EImageWidgetBase::setErrorMessage(msg);
}

/** \brief set the error flag
 *
 * @param error sets an error condition
 */
void EImageWidget::setError(bool error)
{
    EImageWidgetBase::setError(error);
}

/** \brief sets the ok flag
 *
 * calls setError with !ok
 */
void EImageWidget::setOk(bool ok)
{
    setError(!ok);
}

/** \brief sets the image img on the widget.
 *
 * @param img the image that will be displayed.
 *
 * If the scaleWithZoom property is true, the geometry of the widget is
 * scaled according to the image size multiplied by the zoom factor
 * (zoom / 100.0)
 */
void EImageWidget::setImage(const QImage& img)
{
    EImageWidgetBase::setImage(img);
    if(d->scaleWithZoom)
    {
        QImage& imgRef = image();
        setGeometry(x(), y(), imgRef.width() * (zoom() / 100.0), imgRef.height() * (zoom() / 100.0));
    }
    update();
}

/** \brief sets an image that is displayed if an error occurs
 *
 * @param img an image displayed when an error occurs
 */
void EImageWidget::setErrorImage(const QImage &img)
{
    EImageWidgetBase::setErrorImage(img);
}

void EImageWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePress(ev);
}

void EImageWidget::mouseMoveEvent(QMouseEvent *ev)
{
    mouseMove(ev);
}

void EImageWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    mouseRelease(ev);
}

void EImageWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    EImageWidgetBase::paint(e, this);
}

void EImageWidget::wheelEvent(QWheelEvent *we)
{
    EImageWidgetBase::wheelEvent(we);
    we->accept();
    setImage(image());
}

/** \brief the right click menu allows to execute a configuration dialog.
 *
 * @return a QMenu with a "Change color map" action that executes execConfigDialog
 *         when triggered.
 */
QMenu *EImageWidget::rightClickMenu()
{
    QMenu *menu = new QMenu(this);
    menu->addAction("Change color map...", this, SLOT(execConfigDialog()));
    return menu;
}

QSize EImageWidget::minimumSizeHint() const
{
    const int min = 32;
    const QImage& i = image();
    if(!i.isNull() && i.width()/4.0 * i.height()/4.0 > min * min)
        return i.size();
    return QSize(min, min);
}

QSize EImageWidget::sizeHint() const
{
    if(image().isNull())
        return QSize(256, 256);
    return image().size();
}

void EImageWidget::execConfigDialog()
{
    EImageWidgetBase::execConfigDialog();
    update();
}

void EImageWidget::setScaleWithZoom(bool scale)
{
    d->scaleWithZoom = scale;
    update();
}


