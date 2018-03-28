#include "Sound.h"
#include <QString>

Sound::Sound(QString sound_file, QObject *parent) : QObject(parent)
{

    //create phonon objects
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    source =  Phonon::MediaSource(sound_file);
    connect(mediaObject, SIGNAL(finished()), this, SLOT(finished()));
    connect(&loop_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    audiograph = Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setCurrentSource(source);
    set_loop_delay(500);
    loop_timer.setSingleShot(true);
    _loop = false;
}

void Sound::play()
{
    if (_loop) return;
    mediaObject->play();
}

void Sound::loop()
{
    if(!_loop){
        _loop = true;
        mediaObject->play();
    }
}

void Sound::stop()
{
    _loop = false;
    loop_timer.stop();
    mediaObject->stop();

}

void Sound::finished()
{
    mediaObject->stop();
    loop_timer.setSingleShot(true);
    loop_timer.start();
}

void Sound::set_loop_delay(int delay)
{
    loop_timer.setInterval(delay);
    _delay=delay;
}

int  Sound::get_loop_delay()
{
    return loop_timer.interval();
}

void Sound::timer_timeout(){
    if (_loop){
         mediaObject->play();
    }
}

