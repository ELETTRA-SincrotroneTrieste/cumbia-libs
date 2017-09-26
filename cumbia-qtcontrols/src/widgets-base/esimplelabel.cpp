#include "esimplelabel.h"
#include <cumacros.h>
#include <QtDebug>

class ESimpleLabelPrivate
{
public:
    QColor borderColor, backgroundColor;
};

ESimpleLabel::ESimpleLabel(QWidget *parent) : QLabel(parent)
{
    d_ptr = new ESimpleLabelPrivate;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

ESimpleLabel::ESimpleLabel(const QString& text, QWidget *parent) : QLabel(text, parent)
{
    d_ptr = new ESimpleLabelPrivate;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

ESimpleLabel::~ESimpleLabel()
{
    delete d_ptr;
}

void ESimpleLabel::decorate(const QColor & background, const QColor &border)
{
    if(background == d_ptr->backgroundColor && border == d_ptr->borderColor)
        return;

    if(border.isValid())
        d_ptr->borderColor = border;
    if(background.isValid())
        d_ptr->backgroundColor = background;

    QString bgStyle, borStyle;
    QColor bor;
    if(d_ptr->backgroundColor.isValid())
        bgStyle = QString("background-color: rgb(%1,%2,%3);").
                arg(d_ptr->backgroundColor.red())
                .arg(d_ptr->backgroundColor.green())
                .arg(d_ptr->backgroundColor.blue());

    if(d_ptr->borderColor.isValid())
        bor = d_ptr->borderColor;
    else
        bor = QColor(Qt::lightGray);

    borStyle = QString("border-width: 0.02em; "
                       "border-style: solid; border-radius: 6px; "
                       "border-color: rgb(%1,%2,%3); ")
            .arg(bor.red()).arg(bor.green()).arg(bor.blue());

    QString style = QString("QLabel { %1 %2 }").arg(bgStyle).arg(borStyle);
    setStyleSheet(style);
}

