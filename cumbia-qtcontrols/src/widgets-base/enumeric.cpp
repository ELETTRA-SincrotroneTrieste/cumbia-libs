#include "enumeric.h"
#include "leftclick_with_modifiers_eater.h"
#include <cumacros.h>

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QPainter>
#include <QTimer>
#include <QtDebug>
#include <QApplication>
#include <QRegularExpression>

#define MARGIN 0.1
#define MIN_MARGIN 1

ENumeric::ENumeric(QWidget *parent, int id, int dd) : QFrame(parent), FloatDelegate()
{
    intDig = id;
    decDig = dd;
    digits = id + dd;
    data = 0;
    minVal = (long long int) -pow(10.0, digits) + 1;
    maxVal = (long long int) pow(10.0, digits) - 1;
    d_minAsDouble = (double) roundl(minVal);
    d_maxAsDouble = (double) roundl(maxVal);
    bup = NULL;
    bdown = NULL;
    box = NULL;
    text = NULL;
    setFrameShape(QFrame::NoFrame);
    LeftClickWithModifiersEater *leftClickWithModifiersEater = new LeftClickWithModifiersEater(this);
    leftClickWithModifiersEater->setObjectName("leftClickWithModifiersEater");
    init();
}

QSize ENumeric::sizeHint() const
{
    return QSize(neededWidth() + 5, neededHeight() + 2);
}

QSize ENumeric::minimumSizeHint() const
{
    return QSize(neededWidth(), neededHeight());
}

int ENumeric::neededWidth() const
{
    QFontMetrics fm(font());
    return fm.horizontalAdvance('X') * digits + 4 + fm.horizontalAdvance("+") + fm.horizontalAdvance(".") + 4;
}

int ENumeric::neededHeight() const
{
    QFontMetrics fm(font());
    return ceil(fm.height() * 2.5);
}

void ENumeric::clearContainers()
{
    if (box)
    {
        labels.clear();
        foreach(QWidget *child, this->findChildren<QWidget *>(QRegularExpression("layoutmember*")))
            delete child;
        delete box;
    }
    if (bup)
        delete bup;
    if (bdown)
        delete bdown;
}

void ENumeric::init()
{
    LeftClickWithModifiersEater *lCWME = findChild<LeftClickWithModifiersEater *>("leftClickWithModifiersEater");
    setFocusPolicy(Qt::StrongFocus);

    box = new QGridLayout(this);
    box->setSpacing(0);
    box->setContentsMargins(1,1,1,1);
    bup = new QButtonGroup(this);
    bdown = new QButtonGroup(this);
    for (int i = 0; i < digits; i++)
    {
        QLabel *l;
        if (i == intDig)
        {
            pointLabel = new QLabel(".", this);
            pointLabel->setAlignment(Qt::AlignCenter);
            pointLabel->setObjectName("layoutmember.");
            box->addWidget(pointLabel, 1, intDig + 1);
        }

        QPushButton *temp = new QPushButton(this);
        temp->setObjectName(QString("layoutmember") + QString().setNum(i));
        temp->installEventFilter(lCWME);
        bup->addButton(temp);

        l = new QLabel(QString().setNum(i), this);
        l->setObjectName(QString("layoutmember") + QString().setNum(i));
        labels.push_back(l);

        QPushButton *temp2 = new QPushButton(this);
        temp2->setObjectName(QString("layoutmember") + QString().setNum(i));
        temp2->installEventFilter(lCWME);
        bdown->addButton(temp2);

        formatDigit(temp, l, temp2);
        if (i < intDig)
        {
            box->addWidget(temp, 0, i + 1);
            box->addWidget(l, 1, i + 1);
            box->addWidget(temp2, 2, i + 1);
        }
        else if (i >= intDig)
        {
            box->addWidget(temp, 0, i + 2);
            box->addWidget(l, 1, i + 2);
            box->addWidget(temp2, 2, i + 2);
        }

        if (i == 0)
        {
            /* messo qui per evitare casini col designer */
            signLabel = new QLabel("+", this);
            signLabel->setAlignment(Qt::AlignCenter);
            signLabel->setObjectName("layoutmember+");
            box->addWidget(signLabel, 1, 0);
        }
    }
//    for (int i = 0; i < box->rowCount(); i++)
//        box->setRowStretch(i, 10);
//    for (int i = 0; i < box->columnCount(); i++)
//        box->setColumnStretch(i, 10);
//    box->setColumnStretch(0, 3);
//    box->setColumnStretch(intDig+1, 1);

    showData();
    connect(bup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(upData(QAbstractButton*)));
    connect(bdown, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(downData(QAbstractButton*)));
}

void ENumeric::setValue(double v)
{ 
    long long temp = (long long) round(v * pow(10.0, decDig));
    if ((temp >= minVal) && (temp <= maxVal))
    {
        bool valChanged = data != temp;
        data = temp;
        /* call this before emitting value changed to be sure that the value is up to date
         * in the labels of the TNumeric.
         */
        showData();
        if (valChanged)
            emit valueChanged(temp*pow(10.0, -decDig));
    }
    else
        perr("ENumeric \"%s\": value %f outta range [%lld, %lld]: please set minimum/maximum properly", qstoc(objectName()), v, minVal, maxVal);
}

void ENumeric::silentSetValue(double v)
{
    long long temp = (long long) round(v * pow(10.0, decDig));
    if ((temp >= minVal) && (temp <= maxVal))
    {
        data = temp;
        showData();
    }
}

void ENumeric::setMaximum(double v)
{ 
    if (v >= d_minAsDouble)
    {
        d_maxAsDouble = v;
        maxVal = (long long) round(v* (long long)pow(10.0, decDig));
    }
}

void ENumeric::setMinimum(double v)
{ 
    if (v <= d_maxAsDouble)
    {
        d_minAsDouble = v;
        minVal = (long long) round(v* (long long)pow(10.0, decDig));
    }
}

void ENumeric::setIntDigits(int i)
{ 
    if (i < 1)
        return;
    clearContainers();
    intDig = i;
    digits = intDig + decDig;
    init();
}

void ENumeric::setDecDigits(int d)
{
    if (d < 0)
        return;
    clearContainers();
    data = (long long) (data * (long long)pow(10.0, d - decDig));
    maxVal = (long long) (maxVal * (long long)pow(10.0, d - decDig));
    minVal = (long long) (minVal * (long long)pow(10.0, d - decDig));
    decDig = d;
    digits = intDig + decDig;
    /* when changing decimal digits, minimum and maximum need to be recalculated, to avoid
	 * round issues. So, recalculating maximum and minimum is required  to obtain precision
     */
    setMinimum(d_minAsDouble);
    setMaximum(d_maxAsDouble);
    init();
}

void ENumeric::upData(QAbstractButton* b)
{
    int id = b->objectName().remove("layoutmember").toInt();
    if ((data + (long long) pow(10.0, digits-id-1)) <= maxVal)
    {
        data += (long long) pow(10.0, digits-id-1);
        emit valueChanged(data * pow(10.0, -decDig));
        showData();
    }
    if (text != NULL)
        text->hide();
}

void ENumeric::downData(QAbstractButton* b)
{
    int id = b->objectName().remove("layoutmember").toInt();
    if ((data - pow(10.0, digits-id-1)) >= minVal)
    {
        data -= (long long) pow(10.0, digits-id-1);
        emit valueChanged(data * pow(10.0, -decDig));
        showData();
    }
    if (text != NULL)
        text->hide();
}

void ENumeric::showData()
{
    long long temp = data;
    int num = 0;
    if (data < 0)
        signLabel->setText(QString("-"));
    else
        signLabel->setText(QString("+"));

    for (int i = 0; i < digits; i++)
    {
        num = (long long) (temp / (long long) pow(10.0, digits-i-1));
        temp -= num * (long) pow(10.0, digits-i-1);
        labels[i]->setText(QString().setNum(abs(num)));
    }
    valueUpdated();
    QTimer::singleShot(200, this, SLOT(valueUpdated()));
}

void ENumeric::valueUpdated()
{
    QResizeEvent *re = new QResizeEvent(size(), size());
    resizeEvent(re);
}

void ENumeric::dataInput()
{
    setFocus();
    bool ok;
    double val;
    val = text->text().toDouble(&ok);
    if (ok)
        this->setValue(val);
    text->hide();
}

void ENumeric::mouseDoubleClickEvent(QMouseEvent*)
{
    if (text == NULL)
    {
        text = new QLineEdit(this);
        connect(text, SIGNAL(returnPressed()), this, SLOT(dataInput()));
        connect(text, SIGNAL(lostFocus()), text, SLOT(hide()));
    }
    text->setGeometry(QRect(box->cellRect(1, 0).topLeft(), box->cellRect(1, box->columnCount() - 1).bottomRight()));
    text->setFont(signLabel->font());
    text->setAlignment(Qt::AlignRight);
    text->setMaxLength(digits+2);
    text->setText("");
    text->setFocus();
    text->show();
}

void ENumeric::reconstructGeometry()
{

}

void ENumeric::resizeEvent(QResizeEvent *e)
{
    int hmargin, vmargin;
    QPushButton *temp;

    temp = qobject_cast<QPushButton *>(bup->buttons().front());
    if (temp)
    {
        QPixmap pix(temp->size());
        pix.fill(palette().color(QPalette::Window));
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        hmargin = (int) (pix.width() * MARGIN);
        vmargin = (int) (pix.height() * MARGIN);
        if (hmargin < MIN_MARGIN)
            hmargin = MIN_MARGIN;
        if (vmargin < MIN_MARGIN)
            vmargin = MIN_MARGIN;
        int h = pix.height(), w = pix.width();
        QPolygon poly(3);
        poly.setPoint(0, (int) (w * .5), vmargin);
        poly.setPoint(1, w - hmargin, h - vmargin);
        poly.setPoint(2, hmargin, h - vmargin);
        QPen	pen;
        pen.setColor(palette().color(QPalette::Text)); // was Foreground in Qt5
        p.setPen(pen);
        QLinearGradient linearGradient(0, 0, w, h);
        linearGradient.setColorAt(0.0, palette().color(QPalette::Light));
        linearGradient.setColorAt(1.0, palette().color(QPalette::Dark));
        p.setBrush(linearGradient);
        p.drawConvexPolygon(poly);
        p.end();

        foreach (QAbstractButton* but, bup->buttons())
        {
            temp = qobject_cast<QPushButton *>(but);
            if (temp)
            {
                temp->setIconSize(pix.size());
                temp->setIcon(pix);
            }
        }
        //QPixmap pix2 = pix.transformed(QMatrix().rotate(180)).transformed(QMatrix().translate(-20,-20));
        QPixmap pix2 = pix.transformed(QTransform().rotate(-180));
        foreach (QAbstractButton* but, bdown->buttons())
        {
            temp = qobject_cast<QPushButton *>(but);
            if (temp)
            {
                temp->setIconSize(pix2.size());
                temp->setIcon(pix2);
            }
        }

    }

    if (text != NULL)
        text->setGeometry(QRect(box->cellRect(1, 0).topLeft(), box->cellRect(1, box->columnCount() - 1).bottomRight()));

    QWidget::resizeEvent(e);
}


void ENumeric::formatDigit(QPushButton *up, QLabel *l, QPushButton *down)
{
    up->setText("");
    // 	up->setAutoRepeat(true); /* used to cause infinite loop */
   // up->setMinimumSize(QSize(MIN_SIZE,MIN_SIZE));
    up->setFlat(true);
    up->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    up->setFocusPolicy(Qt::NoFocus);

    l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    l->setAlignment(Qt::AlignCenter);
  //  l->setMinimumSize(QSize(MIN_SIZE,2*MIN_SIZE));

    down->setText("");
    // 	down->setAutoRepeat(true); /* used to cause infinite loop */
 //   down->setMinimumSize(QSize(MIN_SIZE, MIN_SIZE));
    down->setFlat(true);
    down->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    down->setFocusPolicy(Qt::NoFocus);
}

void ENumeric::setEnabled(bool b)
{
    QWidget::setEnabled(b);
    update();
}

void ENumeric::setDisabled(bool b)
{
    QWidget::setDisabled(b);
    update();
}

void ENumeric::showEvent(QShowEvent *) {
    QTimer::singleShot(200, this, SLOT(valueUpdated()));
}

void ENumeric::updateMaxMinAuto() {
    minVal = (long long int) -pow(10.0, digits) + 1;
    maxVal = (long long int) pow(10.0, digits) - 1;
    d_minAsDouble = (double) roundl(minVal);
    d_maxAsDouble = (double) roundl(maxVal);
}
