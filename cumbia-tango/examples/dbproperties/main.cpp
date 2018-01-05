#include <iostream>
#include "propertyreader.h"
#include <unistd.h>
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>

int main(int argc, char *argv[])
{
    std::vector<std::string> props;
    for(int i = 1; i < argc; i++)
        props.push_back( std::string(argv[i]));
    PropertyReader pr;
    pr.get("pr1", props);
}
