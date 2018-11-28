#ifndef QULINEARGAUGEBASE_H
#define QULINEARGAUGEBASE_H

#include <QWidget>
#include "qugaugeconfig.h"

class QuLinearGaugeBasePrivate;

class QuLinearGaugeBase : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(ScaleMode scaleMode READ scaleMode WRITE setScaleMode DESIGNABLE true)
    Q_PROPERTY(IndicatorType indicatorType READ indicatorType WRITE setIndicatorType DESIGNABLE true)

    Q_PROPERTY(double value READ value WRITE setValue DESIGNABLE true)
    Q_PROPERTY(double value_anim READ value_anim WRITE setValue_anim DESIGNABLE false) // for animation
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue DESIGNABLE true)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue DESIGNABLE true)
    Q_PROPERTY(bool drawText READ drawText WRITE setDrawText DESIGNABLE true)
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled DESIGNABLE true)
    Q_PROPERTY(qint64 maxAnimationDuration READ maxAnimationDuration WRITE setMaxAnimationDuration DESIGNABLE true)

    Q_PROPERTY(double lowWarning READ lowWarning WRITE setLowWarning DESIGNABLE true)
    Q_PROPERTY(double highWarning READ highWarning WRITE setHighWarning DESIGNABLE true)

    Q_PROPERTY(double lowError READ lowError WRITE setLowError DESIGNABLE true)
    Q_PROPERTY(double highError READ highError WRITE setHighError DESIGNABLE true)

    Q_PROPERTY(int ticksCount READ ticksCount WRITE setTicksCount DESIGNABLE true)
    Q_PROPERTY(int minorTicksCount READ minorTicksCount WRITE setMinorTicksCount DESIGNABLE true)
    Q_PROPERTY(double  tickLen READ tickLen WRITE setTickLen DESIGNABLE true)
    Q_PROPERTY(double  pivotCircleRadius READ pivotCircleRadius WRITE setPivotCircleRadius DESIGNABLE true)
    Q_PROPERTY(double  gaugeWidth READ gaugeWidth WRITE setGaugeWidth DESIGNABLE true)
    Q_PROPERTY(bool drawBackground READ drawBackgroundEnabled WRITE setDrawBackgroundEnabled DESIGNABLE true)
    Q_PROPERTY(bool drawTickCore1 READ drawTickCore1 WRITE setDrawTickCore1 DESIGNABLE true)
    Q_PROPERTY(bool drawTickCore2 READ drawTickCore2 WRITE setDrawTickCore2 DESIGNABLE true)
    Q_PROPERTY(bool drawColoredTicks READ drawColoredTicks WRITE setDrawColoredTicks DESIGNABLE true)
    Q_PROPERTY(double arrowSize READ arrowSize WRITE setArrowSize DESIGNABLE true)
    Q_PROPERTY(double thermoWidth READ thermoWidth WRITE setThermoWidth DESIGNABLE true)
    Q_PROPERTY(QString format READ format WRITE setFormat DESIGNABLE true)
    Q_PROPERTY(bool formatFromPropertyEnabled READ formatFromPropertyEnabled WRITE setFormatFromPropertyEnabled DESIGNABLE true)

    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor DESIGNABLE true)
    Q_PROPERTY(QColor warningColor READ warningColor WRITE setWarningColor DESIGNABLE true)
    Q_PROPERTY(QColor tickCoreColor READ tickCoreColor WRITE setTickCoreColor DESIGNABLE true)
    Q_PROPERTY(QColor errorColor READ errorColor WRITE setErrorColor DESIGNABLE true)
    Q_PROPERTY(QColor normalColor READ normalColor WRITE setNormalColor DESIGNABLE true)
    Q_PROPERTY(QColor readErrorColor READ readErrorColor WRITE setReadErrorColor DESIGNABLE true)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor DESIGNABLE true)
    Q_PROPERTY(QColor needleColor READ needleColor WRITE setNeedleColor DESIGNABLE true)
    Q_PROPERTY(QColor needleGuideColor READ needleGuideColor WRITE setNeedleGuideColor DESIGNABLE (indicatorType == Needle))
    Q_PROPERTY(QColor arrowColor READ arrowColor WRITE setArrowColor DESIGNABLE true)
    Q_PROPERTY(QColor thermoColor1 READ thermoColor1 WRITE setThermoColor1 DESIGNABLE true)
    Q_PROPERTY(QColor thermoColor2 READ thermoColor2 WRITE setThermoColor2 DESIGNABLE true)
    Q_PROPERTY(QColor thermoPenColor READ thermoPenColor WRITE setThermoPenColor DESIGNABLE true)

    Q_PROPERTY(double arrowPenWidth READ arrowPenWidth WRITE setArrowPenWidth DESIGNABLE true)
    Q_PROPERTY(int backgroundColorAlpha READ backgroundColorAlpha WRITE setBackgroundColorAlpha DESIGNABLE true)
    Q_PROPERTY(double backgroundGradientSpread READ backgroundGradientSpread WRITE setBackgroundGradientSpread DESIGNABLE true)

    Q_PROPERTY(QString label READ label WRITE setLabel )
    Q_PROPERTY(LabelPosition labelPosition READ labelPosition WRITE setLabelPosition DESIGNABLE true)
    Q_PROPERTY(QString labelValueFormat READ labelValueFormat WRITE setLabelValueFormat )
    Q_PROPERTY(QString unit READ unit WRITE setUnit DESIGNABLE true)
    Q_PROPERTY(double labelFontScale READ labelFontScale WRITE setLabelFontScale DESIGNABLE true)

public:

    enum LabelPosition { NoLabel, East, Center, West };


    enum ScaleMode { Normal, Inverted };
    enum IndicatorType { Needle, Thermo };

    Q_ENUM(LabelPosition)
    Q_ENUM(IndicatorType)
    Q_ENUM (ScaleMode)

    explicit QuLinearGaugeBase(QWidget *parent = nullptr);

    virtual ~QuLinearGaugeBase();

    LabelPosition labelPosition() const;
    IndicatorType indicatorType() const;
    ScaleMode scaleMode() const;

    double value() const;
    bool readError() const;
    double value_anim() const;
    double maxValue() const;
    double minValue() const;
    int ticksCount() const;
    int minorTicksCount() const;
    double tickLen() const;
    double gaugeWidth() const;
    double arrowSize() const;
    double thermoWidth() const;
    double pivotCircleRadius() const;
    bool drawBackgroundEnabled() const;
    bool drawText() const;
    bool drawTickCore1() const;
    bool drawTickCore2() const;
    bool drawColoredTicks() const;
    bool animationEnabled() const;
    qint64 maxAnimationDuration() const;
    double labelFontScale() const;
    double lowWarning();
    double highWarning();
    double lowError();
    double highError();
    QColor textColor() const;
    QColor warningColor() const;
    QColor errorColor() const;
    QColor normalColor() const;
    QColor readErrorColor() const;
    QColor backgroundColor() const;
    QColor needleColor() const;
    QColor needleGuideColor() const;
    QColor tickCoreColor() const;
    QColor arrowColor() const;
    QColor thermoColor1() const;
    QColor thermoColor2() const;
    QColor thermoPenColor() const;
    double arrowPenWidth() const;
    double backgroundGradientSpread() const;
    int backgroundColorAlpha() const;

    Qt::Orientation orientation() const;

    QString label() const;
    QString format() const;
    bool formatFromPropertyEnabled() const;
    QString getAppliedFormat() const;
    QString labelValueFormat() const;
    QString unit() const;

    double mapTo(double val, const QRectF &paint_area = QRectF());

    QRectF textRect(const QRectF &paintArea, int tick);

    double labelFontSize();

    virtual QString formatLabel(double val, const char *format) const;

    QString label(int i) const;

    QRectF paintArea();

    QSize sizeHint() const;

    QSize minimumSizeHint() const;

    bool inWarningRange(double val) const;

    bool inErrorRange(double val) const;

    QColor valueColor(double val) const;


    QRectF labelRect() const;

    double getGaugeWidth(const QRectF &paint_area) const;

    double getThermoWidth(const QRectF &paint_area) const;

    double getTickLen(const QRectF &paint_area) const;

    QRectF freeSpace(const QRectF &paint_area) const;

    QRectF gaugeRect(const QRectF &paint_area) const;

signals:

public slots:
    void setLabelPosition(LabelPosition p);
    void setIndicatorType(IndicatorType t);
    void setScaleMode(ScaleMode sm);

    void setValue(double v);
    void setReadError(bool re);
    void setValue_anim(double v);
    void setMinValue(double min);
    void setMaxValue(double max);
    void setLabel(const QString& l);
    void setTicksCount(int t);
    void setMinorTicksCount(int t);
    void setTickLen(double tl);
    void setGaugeWidth(double w);
    void setArrowSize(double s);
    void setThermoWidth(double tw);
    void setPivotCircleRadius(double percent);
    void setDrawBackgroundEnabled(bool en);
    void setHighWarning(double w);
    void setLowWarning(double w);
    void setHighError(double e);
    void setLowError(double e);
    void setTextColor(const QColor& c);
    void setErrorColor(const QColor& ec);
    void setWarningColor(const QColor& wc);
    void setNormalColor(const QColor& nc);
    void setReadErrorColor(const QColor& rec);
    void setBackgroundColor(const QColor &bc);
    void setNeedleColor(const QColor& c);
    void setNeedleGuideColor(const QColor& c);
    void setArrowColor(const QColor & c);
    void setThermoColor1(const QColor& c);
    void setThermoColor2(const QColor& c);
    void setThermoPenColor(const QColor& c);
    void setArrowPenWidth(double pw);
    void setTickCoreColor(const QColor& c);
    void setBackgroundGradientSpread(double s);
    void setBackgroundColorAlpha(int a);
    void setDrawTickCore1(bool t);
    void setDrawTickCore2(bool t);
    void setDrawColoredTicks(bool dr);
    void setDrawText(bool dt);
    void setAnimationEnabled(bool ae);
    void setMaxAnimationDuration(qint64 millis);
    void setLabelValueFormat(const QString& f);
    void setUnit(const QString& u);
    void setLabelFontScale(double factor);
    void setFormat(const QString& f);
    void setFormatFromPropertyEnabled(bool ffp);
    void setFormatProperty(const QString& fp);

    void updateLabelsCache();

    void regenerateCache();

    void updatePaintArea();

    void updateLabelsFontSize();

protected:
    void paintEvent(QPaintEvent *pe);

    virtual void drawText(const QRectF &re, int idx, QPainter& p);

    virtual void drawNeedle(double pos, const QRectF &rect, QPainter &p);

    virtual void drawGauge(const QRectF &rect, QPainter& p);

    virtual void drawTicks(const QRectF &rect, QPainter &p);

    virtual void drawBackground(const QRectF &rect, QPainter& p);

    virtual void drawLabel(const QRectF& rect, QPainter &p);

    void resizeEvent(QResizeEvent *re);

    void changeEvent(QEvent *e);
private:

    double m_getMarginW(double radius);
    double m_getMarginH(double radius);

    QuGaugeConfig *g_config;

    QuLinearGaugeBasePrivate *d;

};

#endif // QULinearGAUGEBASE_H
