#ifndef QULABELBASE_H
#define QULABELBASE_H

#include <QLabel>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>

class QuLabelBasePrivate;
class CuVariant;

/** \brief A QLabel that in addition decorates its border and background.
 *
 * \ingroup basew
 *
 */
class QuLabelBase : public QLabel
{
  
  Q_OBJECT
    Q_PROPERTY(double borderWidth READ borderWidth WRITE setBorderWidth DESIGNABLE true)
  public:

    QuLabelBase(QWidget *parent);
	
    QuLabelBase(const QString& text, QWidget *parent);

    virtual ~QuLabelBase();

    void setBackground(const QColor& background);

    void setBorderColor(const QColor& border);

    void setDecoration(const QColor &background, const QColor &border);

    double borderWidth() const;

    void setEnumDisplay(int val, const QString& text, const QColor& c);

    void setFormat(const QString& fmt);

    int maximumLength() const;

    QSize minimumSizeHint() const override;

    QSize sizeHint() const override;

    int heightForWidth(int w ) const override;

    bool hasHeightForWidth() const override;

public slots:
    void setBorderWidth(double w);

    void setValue(const CuVariant& d, bool *background_modified = NULL);


    void setMaximumLength(int len);

protected:
    void paintEvent(QPaintEvent *pe);

private:
    QuLabelBasePrivate *d_ptr;
	
};


#endif
