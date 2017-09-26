#ifndef IMAGEREADEVENT_H
#define IMAGEREADEVENT_H

#include <QEvent>
#include <QString>
#include <vector>

class ImageReadEvent : public QEvent
{
public:
    ImageReadEvent();

    virtual ~ImageReadEvent();

    bool error;
    bool save;
    QString message;

    int imageDepth;

    int dimX, dimY;
    std::vector<unsigned char> imageArray8;
    std::vector<unsigned short> imageArray16;
};

#endif // IMAGEREADEVENT_H
