/* $Id: elabel.h,v 1.4 2013-04-30 07:34:04 giacomo Exp $ */
#ifndef ELABEL_H
#define ELABEL_H

#include <QVariant>
#include <QString>
#include <QColor>
#include <QtDebug>
#include "elettracolors.h"
#include "esimplelabel.h"

class ELabelPrivate;

/**
 * \brief A label that can be used to display numeric, boolean or enum values
 *
 * \ingroup corew
 */
class ELabel : public ESimpleLabel
{
Q_OBJECT
Q_PROPERTY(QColor trueColor    READ trueColor   WRITE setTrueColor   DESIGNABLE false)
Q_PROPERTY(QColor falseColor   READ falseColor  WRITE setFalseColor  DESIGNABLE false)
Q_PROPERTY(QString trueString  READ trueString  WRITE setTrueString  DESIGNABLE false)
Q_PROPERTY(QString falseString READ falseString  WRITE setFalseString DESIGNABLE false)

public:
	ELabel(QWidget *parent);
	
    virtual ~ELabel();

	void setValue(QVariant v, bool ref=true);
    QVariant value() const;

	void setBooleanDisplay(QString, QString, QColor, QColor);
	
	/** \brief Assigns a label and a color to a particular value.
	 *
	 * The tango attribute must not have a display unit for this to work correctly.
	 */
	void setEnumDisplay(unsigned int value, QString label, QColor color);
	
	void setTrueString(QString s);
    QString trueString() const;

	void setFalseString(QString s);
    QString falseString() const;

	void setTrueColor(QColor c);
    QColor trueColor() const;
	
	void setFalseColor(QColor c);
    QColor falseColor() const;

	bool enumDisplayConfigured();

	void clearEnumDisplay();

	void display();


protected:
 
    QVariant val;

	/* to display enum values */
	QVector<unsigned int> v_values;
	QVector<QColor> v_colors;
	QVector<QString> v_strings;

private:
    ELabelPrivate *d_ptr;
};

#endif  /* ELABEL_H */
