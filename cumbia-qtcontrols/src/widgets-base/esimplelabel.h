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
  public:

	ESimpleLabel(QWidget *parent);
	
	ESimpleLabel(const QString& text, QWidget *parent);

    virtual ~ESimpleLabel();

    void decorate(const QColor &background, const QColor &border);

protected:

private:
    ESimpleLabelPrivate *d_ptr;
	
};


#endif
