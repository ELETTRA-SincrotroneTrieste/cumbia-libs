#include "erw.h"
#include "enumeric.h"
#include "elabel.h"
#include "egauge.h"
#include "einputoutputwidget.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>

ERW::ERW(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vlo = new QVBoxLayout(this);
    QGroupBox *gb = new QGroupBox("Box1", this);
    vlo->addWidget(gb);
    ECircularGauge *cirg = new ECircularGauge(this);
    EInputOutputWidget *iow = new EInputOutputWidget(cirg, this);
    QComboBox *combo = new QComboBox(iow);
    combo->insertItems(0, QStringList() << "this" << "that" << "these" << "those");
    iow->setInputWidget(combo);
    vlo->addWidget(iow);
    QVBoxLayout *vlo2 = new QVBoxLayout(gb);
    QGroupBox *gb2 = new QGroupBox("Nested", gb);
    vlo2->addWidget(gb2);
    QGridLayout *lo = new QGridLayout(gb2);
    ELabel *el = new ELabel(gb2);
    ENumeric *en = new ENumeric(gb2);
    EInputOutputWidget *rw1 = new EInputOutputWidget(el, gb2);
    rw1->setInputWidget(en);
    en->setIntDigits(10);
    en->setMinimum(-1000);
    en->setMaximum(1000000);
    en->setValue(20000);
    rw1->setOutputText("100.120");
    lo->addWidget(rw1, 0, 0, 1, 1);

    QDoubleSpinBox *sb = new QDoubleSpinBox(this);
    sb->setMinimum(en->minimum());
    sb->setMaximum(en->maximum());
    sb->setReadOnly(true);
    QLineEdit *le = new QLineEdit(this);
    le->setText("Here's the input value when OK is clicked. Type to change the displayed output");
    lo->addWidget(sb, 0, 1, 1,1);
    lo->addWidget(le, 1, 0, 1, 2);

    // set the value on the spin box when OK is clicked
    connect(rw1, SIGNAL(applyClicked(double)), sb, SLOT(setValue(double)));
    // set the text on the display widget (it has the "text" property) when OK is clicked
    connect(rw1, SIGNAL(applyClicked(QString)), le, SLOT(setText(QString)));
    // change the output displayed on the output widget when we type on the line edit
    connect(le, SIGNAL(textChanged(QString)), rw1, SLOT(setOutputText(QString)));
}

ERW::~ERW()
{

}
