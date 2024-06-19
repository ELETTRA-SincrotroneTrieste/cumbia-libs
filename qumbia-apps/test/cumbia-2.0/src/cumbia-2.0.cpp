#include "cumbia-2.0.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

#include <chrono>
#include <cucontrolsutils.h>
#include <QDateTime>
#include <sys/time.h>

Cumbia2::Cumbia2(CumbiaPool *cumbia_pool, QWidget *parent) : QObject(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;

    new CuEngineAccessor(this, &cu_pool, &m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
    // cumbia

    CuControlsUtils cu;
    char s[MSGLEN];
    struct timeval tday;

    // test microseconds flavour
    {
        printf("------------------- \e[1,35m microseconds version \e[0m-------------------\n");

        gettimeofday(&tday, nullptr);
        double us = tday.tv_sec + tday.tv_usec * 1e-6;

        CuData da(TTT::Time_us, us);
        da[TTT::Src] = "test/CuControlsUtils/msg/micro";
        printf("current time in microseconds: %f\n", da.d(TTT::Time_us) );
        auto start = std::chrono::high_resolution_clock::now();
        cu.msg_short(da, s);
        auto end = std::chrono::high_resolution_clock::now();

        auto  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "3. Elapsed time for short timestamp (microseconds,double) duration: " << duration.count() << " microseconds" << std::endl;
        printf("version 1 '\e[1;32m%s\e[0m'\n", s);

        start = std::chrono::high_resolution_clock::now();
        const QString& qs2 = cu.msg(da);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "4. Elapsed time for Qt timestamp (microseconds,double) to date time: " << duration.count() << " microseconds" << std::endl;
        printf("version 2 '\e[1;32m%s\e[0m'\n", qstoc(qs2));
    }



    {
        printf("------------------- \e[1,34m milliseconds version \e[0m-------------------\n");

        CuData d(TTT::Time_ms, QDateTime::currentMSecsSinceEpoch());
        d[TTT::Src] = "test/CuControlsUtils/msg/millis";
        auto start = std::chrono::high_resolution_clock::now();
        cu.msg_short(d, s);
        auto end = std::chrono::high_resolution_clock::now();

        printf("current time in milliseconds: %lld\n", d[TTT::Time_ms].toLongLongInt() );

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "1. Elapsed time for short timestamp (milliseconds ,long)  duration: "
                  << duration.count() << " microseconds" << std::endl;
        printf("version 1 '\e[1;36m%s\e[0m'\n", s);

        start = std::chrono::high_resolution_clock::now();
        const QString &qs = cu.msg(d);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "2. Elapsed time for Qt timestamp (milliseconds,long)  to date time: " << duration.count() << " microseconds" << std::endl;
        printf("version 2 '\e[1;36m%s\e[0m'\n", qstoc(qs));
    }

    printf("test cudata copy\n");
    CuData d1(TTT::Src, "d1");
    printf("calling CuData d2 = d1\n");
    CuData d2 = d1;
    printf("called d2 = d1.\n Now cloning calling CuData d3 = d1.clone()\n");
    CuData d3 = d1.clone();
    printf("cloneth\n");

}

Cumbia2::~Cumbia2()
{
    //    delete ui;
}
