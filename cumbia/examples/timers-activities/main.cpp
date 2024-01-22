#include <iostream>
#include <string>
#include <cumbia.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>
#include <cuserviceprovider.h>
#include <sstream>
#include <string.h>
#include <cutimerservice.h>
#include <unistd.h>
#include <math.h> // rand, test
#include <chrono>
#include <cuhltimer.h>
#include "myactivity.h"
#include "myactivitylistener.h"
#include "stats.h"

using namespace std;


int main(int argc, char *argv[]) {
    // parse options
    int opt;
    int period = 0;
    int stats_period = 5000;
    int sleep_ms = 0;
    bool verbose = false;
    int len = 0, count = 0;
    bool err = false;
    while ((opt = getopt(argc, argv, "s:t:p:l:c:v")) != -1) {
        switch (opt) {
        case 's':
            sleep_ms = atoi(optarg);
            break;
        case 'p':
            period = atoi(optarg);
            break;
        case 't':
            stats_period = atoi(optarg);
            break;
        case 'l':
            len = atoi(optarg);
            break;
        case 'c':
            count = atoi(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        default: /* '?' */
            err = true;
            break;
        }
    }
    if(len > 0) {
        std::vector<int> ri1, ri2;
        auto t0 = std::chrono::high_resolution_clock::now();
        srand(t0.time_since_epoch().count());
        for(int i = 0; i < len; i++) {
            ri1.push_back(rand());
        }
        ri2.reserve(len);
        auto t1 = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < len; i++)
            ri2.push_back(ri1[i]);
        auto t2 = std::chrono::high_resolution_clock::now();
        printf("main.cpp for cycle (push_back + reserve): copy of %d elements took %ldus \n",
               len, std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
        ri1.clear();
        ri2.clear();
        {
            for(int i = 0; i < len; i++) {
                ri1.push_back(rand());
            }
            auto t1 = std::chrono::high_resolution_clock::now();
            std::copy(ri1.begin(), ri1.end(), ri2.begin());
            auto t2 = std::chrono::high_resolution_clock::now();
            printf("main.cpp std::copy: copy of %d elements took %ldus \n",
                   len, std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
        }

        int *v_int  = new int[len];
        ri1.clear();
        {
            for(int i = 0; i < len; i++) {
                ri1.push_back(rand());
            }
            auto t1 = std::chrono::high_resolution_clock::now();
            for(int i = 0; i < len; i++)
                v_int[i] = (ri1[i]);
            auto t2 = std::chrono::high_resolution_clock::now();
            printf("main.cpp for cycle to C pointer: copy of %d elements took %ldus \n",
                   len, std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
        }

        ri1.clear();
        {
            for(int i = 0; i < len; i++) {
                ri1.push_back(rand());
            }
            auto t1 = std::chrono::high_resolution_clock::now();
            memcpy(v_int, ri1.data(), len);
            auto t2 = std::chrono::high_resolution_clock::now();
            printf("main.cpp memcpy to C pointer: copy of %d elements took %ldus \n",
                   len, std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
        }

        return 0;
    }

    if(period == 0 || count == 0) {
        fprintf(stderr, "Usage: %s -p period [ms] -c count [number of activities] -t stats_period [ms] "
            " -s millis [activity execution sleeps for millis] -v [verbose]\n", argv[0]);
        fprintf(stderr, "Usage: %s -l count [bytes] for vector copy performance comparisons\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    Cumbia *c = new Cumbia();
    CuEventLoopService *loos = new CuEventLoopService();
    c->getServiceProvider()->registerService(CuServices::EventLoop, loos);
    CuHLTimer *stats_timer = new CuHLTimer(c);
    Stats stats(count, period);
    stats_timer->addTimerListener(&stats, stats_period);
    stats_timer->start();
    MyActivityListener alistener(&stats);
    for(int i = 0; i < count; i++) {
        MyActivity *a = new MyActivity(i, verbose, sleep_ms);
        a->setInterval(period);
        c->registerActivity(a, &alistener, "a_" + std::to_string(i), CuThreadFactoryImpl(), CuThreadsEventBridgeFactory());
    }
    loos->exec();

    return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
