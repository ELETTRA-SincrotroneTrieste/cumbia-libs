#include "history.h"
#include "term.h"
#include <fstream>
#include <cumacros.h>
#include <algorithm>

History::History(const std::string& fname) : m_fnam(fname),  history_max_entries(10) {}

void History::m_load() {
    // read history content
    std::ifstream hisfile(m_fnam);
    std::string line;
    if(hisfile.is_open()) {
        // load contents
        while ( getline (hisfile,line) ) {
            entries.push_back(line);
        }
        hisfile.close();
    }
    else { // try create file
        std::ofstream out(m_fnam);
        if(!out.is_open()) perr("History.m_load: failed to create file %s", m_fnam.c_str());
        else out.close();
    }
}

std::string History::pick_entry()
{
    m_load();

    int o = -1;
    int i = 0;
    int idx = -1;

    for(const std::string& he : entries)
        printf("\t[%d] %s\n", ++i, he.c_str());
    if(entries.size() > 0) {
        printf("\t[c] cancel\n[1-%d|c]: ", i);
        Term t;
        t.set_unbuf();
        o = getchar();
        t.reset();
        idx = o - '0' - 1;
    }
    else
        printf("\n\e[1;33m* \e[0mhistory is still empty\n\n");

    if(idx >= 0 && idx < (int) entries.size()) {
        return entries[idx];
    }
    return "";
}

void History::add(const std::string &line)
{
    m_load();
    std::string entry;
    const char* kw = keywords[0];

    if(std::find(entries.begin(), entries.end(), line) == entries.end())
        line.find(kw) != std::string::npos ? entry = line.substr(line.find(kw)) : entry = line;
    if(entry.size() && std::find(entries.begin(), entries.end(), entry) == entries.end()) {
        entries.push_back(entry);
        if(entries.size() > history_max_entries)
            entries.erase(entries.begin());
        std::ofstream out(m_fnam);
        if(out.is_open()) {
            for(const std::string &e : entries) {
                out << e << std::endl;
            }
            out.close();
        }
        else
            perr("History.update: failed to open file %s for writing", m_fnam.c_str());
    }
}
