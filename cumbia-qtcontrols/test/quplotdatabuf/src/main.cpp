#include <quapplication.h>
#include <cuengineaccessor.h>
#include <quplotdatabuf.h>


// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <quplotdatabuf.h>
// cumbia

#define VERSION "1.0"

// print raw x and y buffers
void pbuf_raw(const char* bufnam, const QuPlotDataBuf& b) {
    printf("'\e[1;37;3m%s\e[0m' <\e[1;34;4mRAW\e[0m?> idx: [x, y] { ", bufnam);
    for(size_t i = 0; i < b.size(); i++) {
        if(i == b.first())
            printf(" \e[1;31m>>\e[0m");
        printf("\e[0;35m%ld\e[0m: [\e[0;36m%.1f\e[0m,\e[0;32m%.1f\e[0m], ", i, b.x[i], b.y[i]);
    }
    for(size_t i = b.size(); i < b.bufsize(); i++)
        printf(" \e[0;34m%ld\e[0m: (%.1f, %.1f)", i, b.x[i], b.y[i]);
    printf("\e[0m } [data size: %ld, buf size %ld]\n", b.size(), b.bufsize());
}

// print
void pbuf(const char* bufnam, const QuPlotDataBuf& b) {
    printf("'\e[1;37;3m%s\e[0m' <using \e[1;33;4msamples\e[0m> { ", bufnam);
    for(size_t i = 0; i < b.size(); i++) {
        printf("\e[1;35m%ld\e[0m: [\e[1;36m%.1f\e[0m,\e[1;32m%.1f\e[0m], ", i, b.sample(i).x(), b.sample(i).y());
    }
    printf("\e[0m } [data size: %ld, buf size %ld, \e[0;33mx_auto\e[0m: %s\e[0m]\n",
           b.size(), b.bufsize(), b.x_auto() ? "\e[1;32mYES" : "\e[1;35mNO");
}



int main(int argc, char *argv[])
{
    QuPlotDataBuf b1(20), b2(100);
    std::vector<double> v1;
    for(size_t i = 0; i < 20; i++)
        v1.push_back(i);

    printf("\n*** b1 contains numbers from 0 to 19\n");
    b1.set(v1);
    pbuf_raw("b1", b1);
    pbuf("b1",b1);

    printf("\n*** resize to 15 elements:\n");
    b1.resize(15);
    pbuf_raw("b1",b1);
    pbuf("b1",b1);

    double x = 100, y = 100;
    printf("\n*** appending 5 elements [100, ... 104] to circular buf using append (y only):\n");
    for(size_t i = 0; i < 5; i++) {
        b1.append(&x, &y, 1);
        pbuf_raw("b1",b1);
        pbuf("b1", b1);
        x++, y++;
    }

    printf("\n----------------- buffer 2 ---------------\n");

    printf("*** initialize buffer 2 with v1[0, 1, 2, ... 19] and xv [0, 10, 20, ... 190]\n");
    printf("*** buffer size before the operation: %ld data siz %ld\n", b2.bufsize(), b2.size());
    std::vector<double> xv;
    for(size_t i = 0; i < v1.size(); i++)
        xv.push_back(i * 10);
    b2.set(xv, v1);
    printf("    note that bufsize after b2.set(xv, v1) is %ld, data size %ld\n", b2.bufsize(), b2.size());
    pbuf_raw("b2",b2);
    pbuf("b2", b2);
    printf("*** \e[1;32mresize\e[0m b2 from its size %ld to double its size\n", b2.size());
    b2.resize(2 * b2.size());
    pbuf_raw("b2",b2);
    pbuf("b2", b2);

    printf("*** call \e[1;32mappend(xx, yy, 1) 10 times: [(200,500), (210,501), ... (290,509)]\n");
    double xx = 200, yy = 500;
    for(size_t i = 0; i < 10; i++) {
        b2.append(&xx, &yy, 1);
        xx += 10;
        yy += 1;
    }
    pbuf_raw("b2",b2);
    pbuf("b2", b2);

    printf("*** call \e[1;32mappend(xx, yy, 1) 10 times: [(300,700), (310,701), ... (390,709)]\n");
    xx = 300, yy = 700;
    for(size_t i = 0; i < 10; i++) {
        b2.append(&xx, &yy, 1);
        xx += 10;
        yy += 1;
    }
    pbuf_raw("b2",b2);
    pbuf("b2", b2);


    printf("*** call \e[1;32mappend(xx, yy, 1) 10 times: [(400,800), (410,801), ... (490,809)]\n");
    xx = 400, yy = 800;
    for(size_t i = 0; i < 10; i++) {
        b2.append(&xx, &yy, 1);
        xx += 10;
        yy += 1;
    }
    pbuf_raw("b2",b2);
    pbuf("b2", b2);
}
