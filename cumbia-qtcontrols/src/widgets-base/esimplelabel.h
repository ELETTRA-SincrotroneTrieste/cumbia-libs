#ifndef ESIMPLELABEL_H
#define ESIMPLELABEL_H

#include <QLabel>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>

class ESimpleLabelPrivate;

/** \brief A QLabel that in addition decorates its border and background.
 *
 * \ingroup basew
 *
 */
class ESimpleLabel : public QLabel
{
  
  Q_OBJECT
    Q_PROPERTY(double borderWidth READ borderWidth WRITE setBorderWidth DESIGNABLE true)
  public:

	ESimpleLabel(QWidget *parent);
	
	ESimpleLabel(const QString& text, QWidget *parent);

    virtual ~ESimpleLabel();

    void setDecoration(const QColor &background, const QColor &border);

    double borderWidth() const;

public slots:
    void setBorderWidth(double w);

    QSize minimumSizeHint() const;

protected:
    void paintEvent(QPaintEvent *pe);

private:
    ESimpleLabelPrivate *d_ptr;
	
};


#endif
