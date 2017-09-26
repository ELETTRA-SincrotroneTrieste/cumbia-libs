/* $Id: elabel.cpp,v 1.8 2011-03-07 10:40:10 giacomo Exp $ */
#include <QResizeEvent>
#include <QPainter>
#include <cumacros.h>
#include "elabel.h"

class ELabelPrivate
{
public:
    QColor falseColor, trueColor;
    QString falseString, trueString;
    QVariant val;
};

ELabel::ELabel(QWidget *parent) : ESimpleLabel(parent)
{

    //	setFrameShape(QFrame::WinPanel);
    //	setFrameShadow(QFrame::Sunken);
    //	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d_ptr = new ELabelPrivate();
    d_ptr->falseColor = EColor(Elettra::red);
    d_ptr->trueColor = EColor(Elettra::green);
    d_ptr->falseString = QString("Zero");
    d_ptr->trueString = QString("One");
    /* initialize the vectors of the colours and the strings */
    d_ptr->val = QVariant("No Data");
}

ELabel::~ELabel()
{
    delete d_ptr;
}

void ELabel::setValue(QVariant v, bool ref)
{
    d_ptr->val = v;
    if (ref)
        display();
}

QVariant ELabel::value() const
{
    return d_ptr->val;
}

void ELabel::setTrueString(QString s) 
{
    d_ptr->trueString = s;
    display();
}

QString ELabel::trueString() const
{
    return d_ptr->trueString;
}

void ELabel::setFalseString(QString s) 
{
    d_ptr->falseString = s;
    display();
}

QString ELabel::falseString() const
{
    return d_ptr->falseString;
}

void ELabel::setTrueColor(QColor c) 
{
    d_ptr->trueColor = c;
    display();
}

QColor ELabel::trueColor() const
{
    return d_ptr->trueColor;
}

void ELabel::setFalseColor(QColor c) 
{
    d_ptr->falseColor = c;
    display();
}

QColor ELabel::falseColor() const
{
    return d_ptr->falseColor;
}

void ELabel::setBooleanDisplay(QString fS, QString tS, QColor fC, QColor tC)
{
    d_ptr->falseString = fS;
    d_ptr->trueString = tS;
    d_ptr->falseColor = fC;
    d_ptr->trueColor = tC;
    display();
}

/* The extended version has got the `SetEnumDisplay()' to configure
 * a string and a color for each desired value
 */
void ELabel::setEnumDisplay(unsigned int value, QString label, QColor color)
{
    /* If a value has been already set, update it */
    for (int i = 0; i < v_values.size(); i++)
    {
        if (v_values[i] == value)
        {
            v_colors[i] = color;
            v_strings[i] = label;
            display();
            return;
        }
    }
    /* Put the string and the color into their vectors */
    v_values.push_back(value);
    v_colors.push_back(color);
    v_strings.push_back(label);
    /* refresh the label */
    display();
    return;
}

void ELabel::clearEnumDisplay()
{
    v_values.clear();
    v_colors.clear();
    v_strings.clear();
}

bool ELabel::enumDisplayConfigured()
{
    return (v_values.size() > 0);
}

void ELabel::display()
{
    QColor background;
    if (d_ptr->val.type() == QVariant::Bool)
    {
        if (d_ptr->val.toBool())
        {
            if (background != d_ptr->trueColor || text() != d_ptr->trueString)
            {
                background = d_ptr->trueColor;
                setText(d_ptr->trueString);
            }
        }
        else
        {
            if (background != d_ptr->falseColor || d_ptr->falseString != text())
            {
                background = d_ptr->falseColor;
                setText(d_ptr->falseString);
            }
        }
        decorate(background, QColor());
    }
    //else if (d_ptr->val.type() == QVariant::UInt)
    else if (d_ptr->val.canConvert(QVariant::UInt) && (v_colors.size()) && (!d_ptr->val.toString().contains("###")))
    {
        /* Look for the value `d_ptr->val' inside the v_values
         * vector, to see if a string and a color were
         * configured for that value.
         */
        int index = v_values.indexOf(d_ptr->val.toUInt());
        if (index != -1)
        {
            if(background !=  v_colors[index])
            {
                background == v_colors[index];
                decorate(background, QColor());
            }
            setText(v_strings[index]);
        }
        else /* No string nor a colour for that value! */
        {
            if(background !=  QColor(Qt::white))
            {
                background = QColor(Qt::white);
                decorate(background, QColor());
            }
            setText(QString("No match for value %1!").arg(d_ptr->val.toUInt() ) );
        }
    }
    else
    {
        QString s = d_ptr->val.toString();
        if (s.contains("###"))
        {
            if(background !=   QColor(Qt::white))
            {
                background = QColor(Qt::white);
                decorate(background, QColor());
            }
        }
        setText(d_ptr->val.toString());
    }
}



