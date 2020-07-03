#ifndef TERM_H
#define TERM_H

#ifdef __FreeBSD__
#include <termios.h>
#else
#include <termio.h>
#endif
#include <unistd.h>

class Term
{
public:
    struct termios m_default_tio;

    void set_unbuf() {
        struct termios  new_tio;
        /* get the terminal settings for stdin */
        tcgetattr(STDIN_FILENO,& m_default_tio);

        /* we want to keep the old setting to restore them a the end */
        new_tio=m_default_tio;

        /* disable canonical mode (buffered i/o) and local echo */
        new_tio.c_lflag &=(~ICANON & ~ECHO);

        /* set the new settings immediately */
        tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    }

    void reset() {
        /* restore the former settings */
        tcsetattr(STDIN_FILENO,TCSANOW,&m_default_tio);
    }
};

#endif // TERM_H
