#include "cu-module-component.h"
#include "term.h"
#include <string.h>
#include <cumacros.h>
#include <cumbia.h>

CuModuleComponent::CuModuleComponent(const std::string &cnfdir) : ConfigComponent_A(cnfdir) {
    m_load_file();
    m_upgrade_conf_file();
}

std::string CuModuleComponent::update(const char *value) {
    Mode mode; // detect mode from value
    std::string(value).find("://") != std::string::npos ? mode = Url : mode = Generic;
    if(mode == Url)
        m_url = std::string(value);
    if(!m_lines.size()) {
        m_lines.push_back(std::string(value));
        return std::string(value);
    }

    for(size_t i = 0; i < m_lines.size(); i++) {
        std::string& l = m_lines[i];
        if(l.size() > 0 && l[0] != '#') {
            for(int i = Generic; i < EndModes; i++ ) {
                const char *kw = keywords[i];
                if(l.find(kw) != std::string::npos) {
                    if(mode == Url) l = std::string(keywords[Url]);
                    else if(mode == Generic) l = std::string(value);
                    return std::string(value);
                }
            }
        }
    }
    return "";
}

std::string CuModuleComponent::process(int o) {
    std::string upd_line;
    if(o <= 0)
        return upd_line;

    m_url.clear();
    std::vector<const char*> prots = { "https://", "http://", "wss://", "ws://"   };
    std::vector<const char*> msgs = { "http secure server URL: \e[1;33;4mhttps://",
                                      "http server URL: \e[1;32mhttp://",
                                      "websocket secure server URL: \e[1;33;4mwss://",
                                      "websocket server URL: \e[1;36;4mws://" };
    if(o >= 1 && o <= 4) {
        char url[512];
        strncpy(url, prots[o-1], 512);
        printf("  %s", msgs[o-1]);
        scanf("%s", url + strlen(url));
        printf("\e[0m");
        upd_line = update(url);
        m_url = std::string(url);
    }
    else if(o == 5) {
        upd_line = update("native");
    }
    return upd_line;
}

bool CuModuleComponent::save()
{
    m_error.clear();
    std::ofstream ofile(filenam());
    if(ofile.is_open()) {
        for(const std::string& s : m_lines)
            ofile << s << std::endl;
        ofile.close();
        if(!m_url.empty()) {
            std::ofstream urlfile(confdir() + "url");
            if(urlfile.is_open()) {
                urlfile << m_url;
                urlfile.close();
            }
            else
                m_error = "cu-module-component: failed to open file \"" + confdir() + "/url\" in write mode";
        }
    }
    else {
        m_error = "cuenv: main.cpp failed to open file \"" + filenam() + "\" in write mode";
    }
    return m_error.length() == 0;
}

std::string CuModuleComponent::error() const {
    return m_error;
}

int CuModuleComponent::menu_get_option()
{
    printf("\n\e[1;37;4mcumbia apps module configuration\e[0m\n");
    printf("\n  -  let apps use either cumbia-http or cumbia-websocket modules by default\n");
    printf(" [1] set a \e[1;32mhttps\e[0m URL to use with the \e[1;36mhttp\e[0m module\n");
    printf(" [2] set a \e[0;32mhttp\e[0m URL to use with the \e[1;36mhttp\e[0m module\n");
    printf(" [3] set a \e[1;33msecure \e[0;32mwebsocket\e[0m URL to use with the \e[1;36mhttp\e[0m module\n");
    printf(" [4] set a \e[0;36mwebsocket\e[0m URL to use with the \e[1;36mwebsocket\e[0m module\n");
    printf("  -  let apps pick native engines first\n");
    printf(" [5] set \e[1;32mnative\e[0m. App command line options may be used to override this default\n");
    if(history_enabled())
        printf(" [6] pick from history");
    printf("\n[1|2|3|4|5|6|q]: ");

    Term t;
    t.set_unbuf();
    int o = getchar();
    t.reset();
    printf("\n");
    if(o < '1' || o > '6' ) return -1;
    else if(o == '6') return 0;
    return o - '0';
}

bool CuModuleComponent::history_enabled() const {
    return true;
}

bool CuModuleComponent::m_load_file()
{
    std::ifstream cfile(filenam());
    if(cfile.is_open()) {
        std::string line;
        // load contents
        while ( getline (cfile, line) ) {
            m_lines.push_back(line);
        }
        cfile.close();
        return true;
    }
    return false;
}

std::string CuModuleComponent::m_url_from_file() const {
    std::string url;
    std::ifstream urlsf(confdir() + "url");
    if(urlsf.is_open()) {
        std::string line;
        // load contents
        while (getline (urlsf, line) ) {
            if(line.find("http") == 0)
                url = line;
        }
        urlsf.close();
    }
    return url;
}

void CuModuleComponent::m_upgrade_conf_file() {
    bool migrate = false;
    for(size_t i = 0; i < m_lines.size(); i++) {
        std::string& l = m_lines[i];
        if(l.find("url:") != std::string::npos) {
            m_url = l.substr(l.find("url:") + 4);
            l = "http";
            printf("\n--\ncumbia apps module default: \033[1;35mmigrating configuration file from cumbia version < 1.5.0\033[0m:\n");
            printf("1. url --> http in %s\n", filenam().c_str());
            printf("2. %s in %s\n", m_url.c_str(), (confdir() + "url").c_str());
            printf("--\n\n");
            migrate = true;
        }
    }
    if(migrate)
        save();

}

void CuModuleComponent::print()
{
    printf("\n%-30s\e[1;37;4m%s\e[0m\n", "configuration component:", name().c_str());
    std::string url = m_url_from_file();
    for(std::string l : m_lines) {
        if(l.length() > 0 && l[0] != '#') {
            printf("printing for line '%s'\n", l.c_str());
            if(l.find("http") == 0) {
                std::string prot = url.substr(0, url.find("://") + 3);
                if(prot.find("http") == 0)
                    printf("%-30s\e[1;37;4m%s\e[0m%-6s\e[0m", "cumbia apps default module:", "http", "");
                else if(prot.find("ws") == 0)
                    printf("%-30s\e[1;37;4m%s\e[0m%-6s", "cumbia apps default module:", "websocket", "");
                prot.find("wss") == 0 || prot.find("https") == 0 ? printf("[ \e[1;33mencrypted\e[0m ]\n") : printf("\n");
                printf("%-30s%s\n", "server URL:", url.c_str());
            }
            else {
                printf("module: \e[1;37;4mnative\e[0m\n");
                if(!url.empty()) {
                    printf("\nURL: '%s' [\e[1;31m¹\e[0m]\n", url.c_str());
                    printf("[\033[1;31m¹\033[0m] used by apps when URL not specified in the command line\n");
                }
            }
        }
    }
    if(m_lines.size() == 0) {
        printf("\e[1;33mi\e[0m: no module configuration found: applications will use \e[1;37;4mnative\e[0m mode\n");
        printf("   and load available cumbia modules according to command line arguments\n");
    }
    printf("\n");
}

void CuModuleComponent::list_modules() const {
    printf("\n%-30s\e[1;37;4m%s\e[0m\n", "configuration component:", name().c_str());
    printf("modules available in the current cumbia installation:\n");
    std::vector<std::string> m;
    int i = 0;

#ifdef CUMBIA_VERSION
    printf("%d. cumbia base library\n", ++i);
#endif

#ifdef CUMBIA_HTTP_VERSION
    printf("%d. cumbia-http v%s\n", ++i, CUMBIA_HTTP_VERSION_STR);
#endif

#ifdef CUMBIA_WEBSOCKET_VERSION
    printf("%d. cumbia-websocket v%s\n", ++i, CUMBIA_WEBSOCKET_VERSION_STR);
#endif

#ifdef CUMBIA_RANDOM_VERSION
    printf("%d. cumbia-random v%s\n", ++i, CUMBIA_RANDOM_VERSION_STR);
#endif

#ifdef QUMBIA_EPICS_CONTROLS
    printf("%d. cumbia-epics\n", ++i);
    printf("%d. qumbia-tango-controls v%s\n", ++i, QUMBIA_EPICS_CONTROLS_VERSION_STR);
#endif

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    printf("%d. cumbia-tango\n", ++i);
    printf("%d. qumbia-tango-controls v%s\n", ++i, QUMBIA_TANGO_CONTROLS_VERSION_STR);
#endif

}

std::string CuModuleComponent::name() const {
    return "modules";
}

