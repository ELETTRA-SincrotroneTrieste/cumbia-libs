#ifndef QUCIRCULARGAUGEBASE_H
#define QUCIRCULARGAUGEBASE_H

#include <QWidget>
#include "qugaugeconfig.h"

class QuCircularGaugeBasePrivate;

class QuCircularGaugeBase : public QWidget
{
    Q_OBJECT
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

    Q_PROPERTY(int angleStart READ angleStart WRITE setAngleStart DESIGNABLE true)
    Q_PROPERTY(int angleSpan READ angleSpan WRITE setAngleSpan DESIGNABLE true)
    Q_PROPERTY(int ticksCount READ ticksCount WRITE setTicksCount DESIGNABLE true)
    Q_PROPERTY(int minorTicksCount READ minorTicksCount WRITE setMinorTicksCount DESIGNABLE true)
    Q_PROPERTY(double  tickLen READ tickLen WRITE setTickLen DESIGNABLE true)
    Q_PROPERTY(double  pivotCircleRadius READ pivotCircleRadius WRITE setPivotCircleRadius DESIGNABLE true)
    Q_PROPERTY(double  gaugeWidth READ gaugeWidth WRITE setGaugeWidth DESIGNABLE true)
    Q_PROPERTY(bool drawBackground READ drawBackgroundEnabled WRITE setDrawBackgroundEnabled DESIGNABLE true)
    Q_PROPERTY(bool drawTickCore1 READ drawTickCore1 WRITE setDrawTickCore1 DESIGNABLE true)
    Q_PROPERTY(bool drawTickCore2 READ drawTickCore2 WRITE setDrawTickCore2 DESIGNABLE true)
    Q_PROPERTY(bool drawColoredTicks READ drawColoredTicks WRITE setDrawColoredTicks DESIGNABLE true)
    Q_PROPERTY(QString format READ format WRITE setFormat DESIGNABLE true)

    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor DESIGNABLE true)
    Q_PROPERTY(QColor warningColor READ warningColor WRITE setWarningColor DESIGNABLE true)
    Q_PROPERTY(QColor tickCoreColor READ tickCoreColor WRITE setTickCoreColor DESIGNABLE true)
    Q_PROPERTY(QColor errorColor READ errorColor WRITE setErrorColor DESIGNABLE true)
    Q_PROPERTY(QColor normalColor READ normalColor WRITE setNormalColor DESIGNABLE true)
    Q_PROPERTY(QColor readErrorColor READ readErrorColor WRITE setReadErrorColor DESIGNABLE true)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor DESIGNABLE true)
    Q_PROPERTY(double backgroundGradientSpread READ backgroundGradientSpread WRITE setBackgroundGradientSpread DESIGNABLE true)
    Q_PROPERTY(QColor needleColor READ needleColor WRITE setNeedleColor DESIGNABLE true)
    Q_PROPERTY(int backgroundColorAlpha READ backgroundColorAlpha WRITE setBackgroundColorAlpha DESIGNABLE true)

    Q_PROPERTY(QString label READ label WRITE setLabel )
    Q_PROPERTY(LabelPosition labelPosition READ labelPosition WRITE setLabelPosition DESIGNABLE true)
    Q_PROPERTY(double labelDistFromCenter READ labelDistFromCenter WRITE setLabelDistFromCenter DESIGNABLE true)
    Q_PROPERTY(QString labelValueFormat READ labelValueFormat WRITE setLabelValueFormat )
    Q_PROPERTY(QString unit READ unit WRITE setUnit DESIGNABLE true)
    Q_PROPERTY(double labelFontScale READ labelFontScale WRITE setLabelFontScale DESIGNABLE true)

public:
    enum Quads { q1, q2, q3, q4,
                 q21, q43, q32, q14,
                 q432, q321, q214, q143,
                 q4321, q3214, q2143, q1432 };

    enum LabelPosition { NoLabel, North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest };

    Q_ENUM(LabelPosition)

    explicit QuCircularGaugeBase(QWidget *parent = nullptr);

    virtual ~QuCircularGaugeBase();

    double value() const;
    double value_anim() const;
    double maxValue() const;
    double minValue() const;
    int angleSpan() const;
    int angleStart() const;
    int ticksCount() const;
    int minorTicksCount() const;
    double tickLen() const;
    double gaugeWidth() const;
    double pivotCircleRadius() const;
    bool drawBackgroundEnabled() const;
    bool drawText() const;
    bool drawTickCore1() const;
    bool drawTickCore2() const;
    bool drawColoredTicks() const;
    bool animationEnabled() const;
    bool readError() const;
    qint64 maxAnimationDuration() const;
    LabelPosition labelPosition() const;
    double labelDistFromCenter() const;
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
    QColor tickCoreColor() const;
    double backgroundGradientSpread() const;
    int backgroundColorAlpha() const;


    double getPivotRadius();

    QString label() const;
    QString format() const;
    QString labelValueFormat() const;
    QString unit() const;

    QPointF mapTo(double val, int *angleSpan, double radius = -1.0, const QRectF &paint_area = QRectF());

    QRectF textRect(int tick, double radius);

    double labelFontSize();

    virtual QString formatLabel(double val, const QString &format) const;

    QString label(int i) const;

    QRectF paintArea();

    QSize sizeHint() const;

    QSize minimumSizeHint() const;

    bool inWarningRange(double val) const;

    bool inErrorRange(double val) const;

    QColor valueColor(double val) const;

signals:

public slots:
    void setValue(double v);
    void setReadError(bool err);
    void setValue_anim(double v);
    void setAngleStart(int as);
    void setAngleSpan(int a);
    void setMinValue(double min);
    void setMaxValue(double max);
    void setLabel(const QString& l);
    void setTicksCount(int t);
    void setMinorTicksCount(int t);
    void setTickLen(double tl);
    void setGaugeWidth(double w);
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
    void setTickCoreColor(const QColor& c);
    void setBackgroundGradientSpread(double s);
    void setBackgroundColorAlpha(int a);
    void setDrawTickCore1(bool t);
    void setDrawTickCore2(bool t);
    void setDrawColoredTicks(bool dr);
    void setDrawText(bool dt);
    void setAnimationEnabled(bool ae);
    void setMaxAnimationDuration(qint64 millis);
    void setLabelPosition(LabelPosition p);
    void setLabelDistFromCenter(double radius_percent);
    void setLabelValueFormat(const QString& f);
    void setUnit(const QString& u);
    void setLabelFontScale(double factor);

    void setFormat(const QString& f);

    QList<int> updateLabelsDistrib() const;

    void updateLabelsCache();

    void regenerateCache();

    void updatePaintArea();

    void updateLabelsFontSize();

protected:
    void paintEvent(QPaintEvent *pe);

    virtual void drawText(const QRectF &rect, int idx, QPainter& p);

    virtual void drawNeedle(const QPointF& end, int angleSpan, const QRectF &rect, QPainter &p);

    virtual void drawGauge(const QRectF &rect, int startAngle, int span, QPainter& p);

    virtual void drawPivot(const QRectF &rect, QPainter &p);

    virtual void drawTicks(const QRectF &rect, QPainter &p);

    virtual void drawBackground(const QRectF &rect, QPainter& p);

    virtual void drawLabel(const QRectF& rect, QPainter &p);

    void changeEvent(QEvent *e);

    void resizeEvent(QResizeEvent *re);

private:

    void m_anglesUpdate();

    double m_getMarginW(double radius);
    double m_getMarginH(double radius);

    QuGaugeConfig *g_config;

    QuCircularGaugeBasePrivate *d;

};

#endif // QUCIRCULARGAUGEBASE_H
