#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cumacros.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <string.h> // strerror
#include <vector>
#include <map>
#include <signal.h>
#include <algorithm>

#include "cu-module-component.h"
#include "history.h"
#include "term.h"

using namespace std;

struct termios m_default_tio;

void signal_handler(int ) {
    tcsetattr(STDIN_FILENO,TCSANOW, &m_default_tio);
    printf("\e[0m\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    tcgetattr(STDIN_FILENO, &m_default_tio);
    signal(SIGINT, signal_handler);
    enum Options { Read, Set, ListMod };
    struct passwd *pw = getpwuid(getuid());
    const char* homedir = pw->pw_dir;
    char path[256];
    char history_fnam[280];
    int o;
    std::vector<std::string> known_components = { "modules" };
    if(!homedir) {
        perr("cuenv.main.cpp: failed to get home dir");
        return EXIT_FAILURE;
    }
    snprintf(path, 255, "%s/.config/cumbia", homedir);
    snprintf(history_fnam, 280, "%s/history.dat", path);

    errno = 0;
    DIR *dir = opendir(path);
    if(!dir) { // failed to open
        printf("cuenv.main.cpp: creating directory \"%s\"...", path);
        if(mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            perr("cuenv.main.cpp: failed to create directory \"%s\": %s", path, strerror(errno));
            return EXIT_FAILURE;
        }
        else
            printf("\t[\e[1;32mOK\e[0m]\n");
    }

    Options opt = Read;
    std::string module;
    while ((o = getopt(argc, argv, "shlc:")) != -1) {
        if(o == 's')
            opt = Set;
        else if(o == 'c')
            module = std::string(optarg);
        else if(o == 'l')
            opt = ListMod;
        else if(o == 'h' || o == '?') {
            fprintf(stdout, "Usage: %s [-u] (edit url) -h (help)\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if(module.length() == 0) {
        perr("main.cpp: no configuration component specified: usage %s -c component", argv[0]);
        return EXIT_FAILURE;
    }
    else if(std::find(known_components.begin(), known_components.end(), module) == known_components.end()) {
        printf("main.cpp: \e[1;31mno known configuration component \"%s\"\e[0m: known components: { ", module.c_str());
        for(size_t i = 0; i < known_components.size(); i++) {
            const std::string& s = known_components[i];
            printf( i < known_components.size() - 1 ? "%s, " : "%s", s.c_str());
        }
        printf(" }\n\n");
        return EXIT_FAILURE;
    }


    ConfigComponent_A *component = new CuModuleComponent(path);

    if(opt == Set) {
        History  h(history_fnam);
        std::string conf_line;
        int o = component->menu_get_option();
        if(o > 0) {
            conf_line = component->process(o);
            h.add(conf_line);
        }
        else if(o == 0) {
            std::string entry = h.pick_entry();
            if(entry.length() > 0)
                conf_line = component->update(entry.c_str());
        }
        if(conf_line.length() > 0)
            component->save();
    }
    if(opt != ListMod)
        component->print();
    else
        static_cast<CuModuleComponent *>(component)->list_modules();

    if(component->error().length() > 0)
        perr("main.cpp \"%s\" component error: %s\n", component->name().c_str(), component->error().c_str());

    return EXIT_SUCCESS;
}
