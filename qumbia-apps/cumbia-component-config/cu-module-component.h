#ifndef MODSETTER_H
#define MODSETTER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "configcomponent_a.h"

class CuModuleComponent : public ConfigComponent_A
{
public:
    CuModuleComponent(const std::string &cnfdir);

    enum Mode { Generic, Url, EndModes };
    const char *keywords[32] = { "native", "url:" };
    std::string update(const char* value);
    std::string process(int option);
    bool save();
    std::string error() const;
    int menu_get_option();
    void print();
    void list_modules() const;
    std::string name() const;
    virtual bool history_enabled() const;

private:
    char m_option;
    std::vector<std::string> m_lines;
    std::string m_error;

    bool m_load_file();
};

#endif // MODSETTER_H
