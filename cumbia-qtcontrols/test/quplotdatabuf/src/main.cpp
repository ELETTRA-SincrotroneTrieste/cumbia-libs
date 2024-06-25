#include <quapplication.h>
#include <cuengineaccessor.h>
#include <quplotdatabuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <quplotdatabuf.h>
// cumbia

#define VERSION "1.0"

#define MAXLINELEN 65536

// print raw x and y buffers
void pbuf_raw(const char* bufnam, const QuPlotDataBuf& b) {
    printf("'\e[1;37;3m%s\e[0m' <\e[1;34;4mRAW\e[0m?> ", bufnam);
    b.x.size() != b.y.size() ? printf("idx: \e[1;32my\e[0m {")
        :  printf("idx: [\e[0;36mx\e[0m,\e[1;32my\e[0m] { ");
    for(size_t i = 0; i < b.size(); i++) {
        if(i == b.first())
            printf(" \e[1;31m>>\e[0m");
        b.x.size() == b.y.size() ?
            printf("\e[0;35m%ld\e[0m: [\e[0;36m%.1f\e[0m,\e[1;32m%.1f\e[0m], ", i, b.x[i], b.y[i])
            : printf("\e[0;35m%ld\e[0m: \e[1;32m%.1f\e[0m, ", i, b.y[i]);


    }
    if(b.size() < b.bufsize())
        printf("\e[1;34m [ \e[0m");
    for(size_t i = b.size(); i < b.bufsize(); i++)
        b.x.size() == b.y.size() ?
            printf(" \e[0;34m%ld\e[0m: (%.1f, %.1f)", i, b.x[i], b.y[i])
                : printf(" \e[0;34m%ld\e[0m: %.1f", i, b.y[i]);
    if(b.size() < b.bufsize())
        printf("\e[1;34m ] \e[0m");
    printf("\e[0m } [data size: %ld, buf size %ld]\n", b.size(), b.bufsize());
}

// print
void pbuf(const char* bufnam, const QuPlotDataBuf& b) {
    printf("'\e[1;37;3m%s\e[0m' <using \e[1;33;4msamples\e[0m> { ", bufnam);
    for(size_t i = 0; i < b.size(); i++) {
        b.x_auto() ? printf("[\e[1;36m%.1f\e[0m,\e[1;32m%.1f\e[0m], ", b.sample(i).x(), b.sample(i).y())
            : printf("\e[1;35m%ld\e[0m: [\e[1;36m%.1f\e[0m,\e[1;32m%.1f\e[0m], ", i, b.sample(i).x(), b.sample(i).y());
    }
    printf("\e[0m } [data size: %ld, buf size %ld, \e[0;33mx_auto\e[0m: %s\e[0m]\n",
           b.size(), b.bufsize(), b.x_auto() ? "\e[1;32mYES" : "\e[1;35mNO");
}

void print_menu(const QuPlotDataBuf &b) {
    printf("current buffer size \e[1;35m%ld\e[0m. data size \e[1;32m%ld\e[0m. X auto mode %s\e[0m\n",
           b.bufsize(), b.size(), b.x_auto() ? "\e[1;32mYES" : "\e[1;36mNO\e[0m");
    printf("n. new buffer\n");
    printf("m. new buffer (x,y)\n");
    printf("r. resize buffer\n");
    printf("a. append (y)\n");
    printf("b. append(x,y)\n");
    printf("i. insert y at pos\n");
    printf("j. insert(x,y) at pos\n");
    printf("p. print buffer (RAW)\n");
    printf("s. print samples\n");
    printf("q. quit\n");
}

struct termios originalTermios;

void unbufio() {
    struct termios newTermios;
    tcgetattr(STDIN_FILENO, &originalTermios);
    newTermios = originalTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
}

void restoreio() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

std::vector<double> parse(const char* cstr) {
    size_t pos = 0, idx;
    std::string token, s(cstr);
    std::vector<double> out;
    while ((idx = s.find(',', pos)) != std::string::npos) {
        token = s.substr(pos, idx);
        out.push_back(atof(token.c_str()));
        pos = idx + 1;
    }
    if(pos < s.length())
        out.push_back(atof(s.substr(pos).c_str()));
    return out;
}

void parse(const char *cstr, std::vector<double>& x, std::vector<double>& y) {
    x.clear();
    y.clear();
    size_t pos = 0, idx;
    size_t i = 0;
    std::string token, s(cstr);
    while ((idx = s.find(',', pos)) != std::string::npos) {
        token = s.substr(pos, idx);
        if(i % 2 != 0)
            y.push_back(atof(token.c_str()));
        else
            x.push_back(atof(token.c_str()));
        pos = idx + 1;
        i += 1;
    }
}

int main(int argc, char *argv[])
{
    QuPlotDataBuf b;
    char o;
    char line[MAXLINELEN];
    std::vector<double> y{1,2,3,4,5,6,7,8,9,10}, x;
    b.set(y);
    printf("\e[1;32m*\e[0m buffer initialized with default data {0,1,2, ..., 10}\n");
    do {
        unbufio();
        print_menu(b);
        o = getchar();
        restoreio();
        switch(o) {
        case 'n':
            printf("print the comma-separated sequence of numbers\n");
            fgets(line, MAXLINELEN, stdin);
            b.set(parse(line));
            printf("used set(y): data size %ld buf size %ld\n", b.size(), b.bufsize());
            break;
        case 'm':
            printf("print the comma-separated sequence of numbers\n");
            printf("x0 y0 x1 y1 ... xN yN\n");
            fgets(line, MAXLINELEN, stdin);
            parse(line, x, y);
            if(x.size() == y.size()) {
                b.set(x, y);
                printf("called set(x, y): data size %ld buf size %ld\n", b.size(), b.bufsize());
            }
            else
                printf("different size for x and y\n");
            break;
        case 'r': {
            printf("type the new size for the buffer: ");
            size_t siz = 0, oldsiz = b.bufsize();
            scanf("%ld", &siz);
            if(siz > 0) {
                b.resize(siz);
                printf("used resize(%ld): size changed from %ld to %ld\n", siz, oldsiz, b.bufsize());
            } else
                printf("invalid size\n");
        }
            break;
        case 'a':
            printf("print the comma-separated sequence of numbers\n");
            fgets(line, MAXLINELEN, stdin);
            y = parse(line);
            b.append(y.data(), y.size());
            printf("called append(y, %ld): data size %ld buf size %ld\n", y.size(), b.size(), b.bufsize());
            break;
        case 'b':
            printf("print the comma-separated sequence of numbers\n");
            printf("x0 y0 x1 y1 ... xN yN\n");
            fgets(line, MAXLINELEN, stdin);
            parse(line, x, y);
            if(x.size() == y.size()) {
                b.append(x.data(), y.data(), x.size());
                printf("called append(x, y, %ld): data size %ld buf size %ld\n", y.size(), b.size(), b.bufsize());
            }
            else
                printf("different size for x and y\n");
            break;
        case 'i':
            printf("print the comma-separated sequence of numbers\n");
            fgets(line, MAXLINELEN, stdin);
            y = parse(line);
            printf("insert at position [0,%ld]: ", b.size());
            size_t idx;
            scanf("%ld", &idx);
            b.insert(idx, y.data(), y.size());
            printf("called insert(y, %ld): buf size %ld new data size %ld\n", y.size(), b.bufsize(), b.size());
            break;
        case 'j':
            printf("print the comma-separated sequence of numbers\n");
            printf("x0 y0 x1 y1 ... xN yN\n");
            fgets(line, MAXLINELEN, stdin);
            parse(line, x, y);
            if(x.size() == y.size()) {
                printf("insert at position [0,%ld]: ", b.size());
                size_t idx;
                scanf("%ld", &idx);
                b.insert(idx, x.data(), y.data(), x.size());
                printf("called append(x, y, %ld): data size %ld buf size %ld\n", y.size(), b.size(), b.bufsize());
            }
            else
                printf("different size for x and y\n");
            break;
        case 'p':
            pbuf_raw("buffer", b);
            break;
        case 's':
            pbuf("buffer", b);
            break;
        default:
            break;
        }
    }while(o != 'q');

    restoreio();
}
