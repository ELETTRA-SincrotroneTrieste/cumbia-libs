/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "cuwatcher.h"
#include "quwatcher.h"
#include "quwriter.h"
#include <QSettings>
#include <QMessageBox>
#include <QtDebug>
#include <cucontextactionbridge.h>

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quinputoutput.h>
#include <cucontext.h>
#include <cudata.h>

Watcher::Watcher(CumbiaTango *cut, QWidget *parent) : QWidget(parent)
{
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);


    ui.setupUi(this);

    QuWatcher *lcdWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    lcdWatcher->attach(ui.lcdNumber, SLOT(display(double)));
    lcdWatcher->setAutoDestroy(true);
    lcdWatcher->setObjectName("lcdWatcher");
    lcdWatcher->setSource("$1/double_scalar");

    QuWatcher *lineEditWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    lineEditWatcher->attach(ui.le,   SLOT(setText(const QString&)));
    lineEditWatcher->setSource("$1/string_scalar");
    /* write string scalar
     * NOTE: textChanged would perform a write at startup! Use textEdited
     */
    QuWriter *lineEditWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    lineEditWriter->attach(ui.leW, SIGNAL(textEdited(const QString&)), SLOT(setText(const QString&)));
    lineEditWriter->setTargets(lineEditWatcher->source());

    QuWatcher *labelWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    labelWatcher->attach(ui.l1,  SLOT(setText(const QString&)));
    labelWatcher->setSource("$1/short_scalar");
    connect(labelWatcher, SIGNAL(newData(CuData)), this, SLOT(updateShortSetPoint(CuData)));

    /* use the dial to change  short scalar */
    QuWriter *dualWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    /* adding SLOT(setValue(int)) would not be useful because the QSlider has not
     * got the slots to set minimum and maximum values. So
     */
    dualWriter->attach(ui.writeDial, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));

    /* connect auto configuration signal to dialConfigured slot to set max and min
     * on the dial. Sliders are all connected to the dialConfigured slot because they
     * do not have special slots to set minimum and maximum.
     * NOTE: dialConfigured SLOT sets minimum and maximum of all sliders!
     * Current initialization values are then set by the auto configuration performed by
     * QuWriter by invoking the auto configuration slot provided in the attach() method
     * for the writers.
     */
    connect(dualWriter, SIGNAL(configured(CuData)), this, SLOT(dialConfigured(CuData)));
    dualWriter->setTargets(labelWatcher->source());

    QuWatcher *dialWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    dialWatcher->attach(ui.dial, SLOT(setValue(int)));
    dialWatcher->setSource("$1/short_scalar");

    QuWatcher *sliderWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    sliderWatcher->attach(ui.horizontalSlider, SLOT(setValue(int)));
    sliderWatcher->setSource("$1/short_scalar");
    QuWriter *sliderWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    /* NOTE: sliderMoved not valueChanged! */
    sliderWriter->attach(ui.writeSlider, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
    sliderWriter->setTargets(sliderWatcher->source());

    /* combo box. Let's try with it too! */
    QuWatcher *comboWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    comboWatcher->attach(ui.comboBox, SLOT(setCurrentIndex(int)));
    comboWatcher->setSource("$1/short_scalar");
    for(int i = 0; i < 512; i++)
        ui.comboBox->insertItem(i, QString("index %1").arg(i));
    QuWriter *comboWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    comboWriter->attach(ui.comboBox, SIGNAL(activated(int)), SLOT(setCurrentIndex(int)));
    comboWriter->setTargets(sliderWatcher->source());


    QuWatcher *progressBarWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    progressBarWatcher->attach(ui.progressBar, SLOT(setValue(int)));
    /* maximum and minimum */
    progressBarWatcher->setAutoConfSlot(QuWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QuWatcher::Max, SLOT(setMaximum(int)));

    progressBarWatcher->setSource("$1/short_scalar_ro");

    /* use watcher to watch simple variables */
    connect(ui.pbUpdateInternal, SIGNAL(clicked()), this, SLOT(displayInternalVariables()));

    QuWatcher *intWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    QuWatcher *doubleWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    QuWatcher *shortWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    QuWatcher *stringWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);

    intWatcher->attach(&intVar);
    intWatcher->setSource("$1/long_scalar");
    doubleWatcher->attach(&doubleVar);
    doubleWatcher->setSource("$1/double_scalar");
    shortWatcher->attach(&shortVar);
    shortWatcher->setSource("$1/short_scalar_ro");
    stringWatcher->attach(&stringVar);
    stringWatcher->setSource("$1/string_scalar");

    QuWatcher *boolWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    boolWatcher->attach(ui.cb, SLOT(setChecked(bool)));
    boolWatcher->setSource("$1/boolean_scalar");
    QuWriter *boolWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    boolWriter->attach(ui.cb, SIGNAL(clicked(bool)), SLOT(setChecked(bool)));
    boolWriter->setTargets(boolWatcher->source());

    /* writer for state */
    QuWriter *stateWriter = new QuWriter(this, cu_t, cu_tango_w_fac);
    stateWriter->attach(ui.pushButton,  SIGNAL(clicked()));
    stateWriter->setTargets("$1->SwitchStates");
    /* switch states changes the state of the device. Let's connect the state to
     * the label aimed at indicating it.
     */
    QuWatcher *stateWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);
    stateWatcher->attach(ui.labelState,  SLOT(setText(const QString&)));
    stateWatcher->setSource("$1->State");

    new CuContextActionBridge(this, cu_t, cu_tango_r_fac);
}

Watcher::~Watcher()
{

}

void Watcher::displayInternalVariables()
{
    QString text;
    text += "<h3>Internal variables values</h3>\n";
    text += "<ul>\n";
    text += QString("<li><strong>intVar</strong>(long_scalar): %1;</li>\n").arg(intVar);
    text += QString("<li><strong>doubleVar</strong>(double_scalar): %1;</li>\n").arg(doubleVar);
    text += QString("<li><strong>shortVar</strong>(short_scalar_ro): %1;</li>\n").arg(shortVar);
    text += QString("<li><strong>stringVar</strong>(string_scalar): %1;</li>\n").arg(stringVar);
    text += "</ul>\n";

    ui.textEdit->setHtml(text);
}

void Watcher::updateShortSetPoint(const CuData &d)
{
    ui.l1Set->setDisabled(d["err"].toBool());
    if(ui.l1Set->isEnabled() && d.containsKey("w_value"))
        ui.l1Set->setText(d["w_value"].toString().c_str());
    else
        ui.l1Set->setText("####");
    ui.l1Set->setToolTip(d["msg"].toString().c_str());
}

void Watcher::dialConfigured(const CuData &cd)
{
    if(cd.containsKey("min") && cd.containsKey("max"))
    {
        double m, M, v = 0.0;
        cd["min"].to<double>(m);
        cd["max"].to<double>(M);
        if(cd.containsKey("w_value"))
            cd["w_value"].to<double>(v);
        ui.dial->setMinimum(m);
        ui.dial->setMaximum(M);
     //   ui.dial->setValue(v);
        ui.horizontalSlider->setMinimum(m);
        ui.horizontalSlider->setMaximum(M);
      //  ui.horizontalSlider->setValue(v);
     //   printf("\e[0;33msetting min and max on the writeDial %f %f\e[0m\n", m, M);
    //    ui.writeDial->setMinimum(m);
    //    ui.writeDial->setMaximum(M);
     //   ui.writeDial->setValue(v);
    //    ui.writeSlider->setMinimum(qRound(m));
    //    ui.writeSlider->setMaximum(qRound(M));
      //  ui.writeSlider->setValue(v);
    }
}

