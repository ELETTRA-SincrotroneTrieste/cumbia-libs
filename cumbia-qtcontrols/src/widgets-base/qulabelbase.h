#ifndef QULABELBASE_H
#define QULABELBASE_H

#include <QLabel>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>

class QuLabelBasePrivate;

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

    void setDecoration(const QColor &background, const QColor &border);

    double borderWidth() const;

public slots:
    void setBorderWidth(double w);

    QSize minimumSizeHint() const;

protected:
    void paintEvent(QPaintEvent *pe);

private:
    QuLabelBasePrivate *d_ptr;
	
};


#endif
