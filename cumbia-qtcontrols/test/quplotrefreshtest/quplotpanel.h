/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "quplotsettings.h"
#include <QTabWidget>

class QComboBox;
class SpinBox;
class CheckBox;

class QuPlotPanel : public QTabWidget
{
    Q_OBJECT

  public:
    QuPlotPanel( QWidget* = NULL );

    QuPlotSettings settings() const;
    void setSettings( const QuPlotSettings& );

  Q_SIGNALS:
    void settingsChanged( const QuPlotSettings& );

  private Q_SLOTS:
    void edited();

  private:
    QWidget* createPlotTab( QWidget* );
    QWidget* createCanvasTab( QWidget* );
    QWidget* createCurveTab( QWidget* );

    SpinBox* m_numPoints;
    SpinBox* m_updateInterval;
    QComboBox* m_updateType;
    QComboBox* m_setDataMode;

    QComboBox* m_gridStyle;
    CheckBox* m_paintCache;
    CheckBox* m_paintOnScreen;
    CheckBox* m_immediatePaint;
#ifndef QWT_NO_OPENGL
    CheckBox* m_openGL;
#endif

    QComboBox* m_curveType;
    CheckBox* m_curveAntialiasing;
    CheckBox* m_curveClipping;
    QComboBox* m_curveWeeding;
    CheckBox* m_lineSplitting;
    SpinBox* m_curveWidth;
    QComboBox* m_curvePen;
    CheckBox* m_curveFilled;
};
