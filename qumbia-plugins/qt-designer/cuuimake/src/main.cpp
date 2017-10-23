#include <QCoreApplication>
#include "cuuimake.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("cuuimake");
    a.setApplicationVersion(CUUIMAKE_VERSION_STR);
    CuUiMake cuuim;

    bool success = cuuim.make();

    if(success)
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
