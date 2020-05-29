#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>

class History
{
public:
    History(const std::string &fname);

    std::string pick_entry();
    void add(const std::string& line);

    std::vector<std::string> entries;
    std::string m_fnam;

    const char *keywords[16] = { "url:" };
    size_t history_max_entries;

private:
    void m_load();
};

#endif // HISTORY_H
