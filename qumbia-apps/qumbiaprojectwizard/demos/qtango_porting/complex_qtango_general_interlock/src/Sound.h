#ifndef SOUND_H
#define SOUND_H

//#include <tango.h>
#include <QString>
#include <QObject>
#include <phonon/audiooutput.h>
//#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
//#include <phonon/volumeslider.h>
//#include <phonon/backendcapabilities.h>
#include <QTimer>

class Sound:public QObject
{
   Q_OBJECT
public:
   Sound(QString sound_file, QObject* parent);
   void set_loop_delay(int);
   int get_loop_delay();
public slots:
   void play();
   void stop();
   void loop();
private slots:
   void finished();
   void timer_timeout();
private:
   Phonon::AudioOutput *audioOutput;
   Phonon::MediaObject *mediaObject;
   Phonon::Path audiograph;
   Phonon::MediaSource source;
   Phonon::State oldstate;
   Phonon::State newstate;
   bool _loop;
   int _delay;
   QTimer loop_timer;


};




#endif



