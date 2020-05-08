#include "cuwstangohelper.h"
#include <cumacros.h>

CuWsTangoHelper::CuWsTangoHelper()
{

}

CuWsTangoHelper::~CuWsTangoHelper()
{

}

// from idl/tango.h
//   enum AttrDataFormat { SCALAR, SPECTRUM, IMAGE, FMT_UNKNOWN /*, __max_AttrDataFormat=0xffffffff */ };

int CuWsTangoHelper::dataFormatStrToInt(const std::string &fmt) const
{
    if(fmt == "scalar") return 0;
    else if (fmt == "vector") return 1;
    else if (fmt == "matrix") return 2;
    else return 3;
}

std::string CuWsTangoHelper::dataFormatToStr(int fmt) const
{
    // from idl/tango.h
    switch(fmt) {
    case 0:
        return "scalar";
    case 1:
        return "vector";
    case 2:
        return "matrix";
    default:
        return "unknown_format";
    }
}

// from tango_const.h

/*
 * enum CmdArgType {
        DEV_VOID = 0,
        DEV_BOOLEAN, 1
        DEV_SHORT, 2
        DEV_LONG,3
        DEV_FLOAT, 4
        DEV_DOUBLE, 5
        DEV_USHORT, 6
        DEV_ULONG, 7
        DEV_STRING, 8
        DEVVAR_CHARARRAY, 9
        DEVVAR_SHORTARRAY, 10
        DEVVAR_LONGARRAY, 11
        DEVVAR_FLOATARRAY, 12
        DEVVAR_DOUBLEARRAY, 13
        DEVVAR_USHORTARRAY, 14
        DEVVAR_ULONGARRAY, 15
        DEVVAR_STRINGARRAY, 16
        DEVVAR_LONGSTRINGARRAY, 17
        DEVVAR_DOUBLESTRINGARRAY, 18
        DEV_STATE, 19
        CONST_DEV_STRING, 20
        DEVVAR_BOOLEANARRAY, 21
        DEV_UCHAR, 22
        DEV_LONG64,  23
        DEV_ULONG64,  24
        DEVVAR_LONG64ARRAY, 25
        DEVVAR_ULONG64ARRAY, 26
        DEV_INT, 27
        DEV_ENCODED ,  28
        DEV_ENUM,  29
        DEV_PIPE_BLOB ,  30
        DEVVAR_STATEARRAY,  31
        DATA_TYPE_UNKNOWN = 100
};

 */
std::string CuWsTangoHelper::dataTypeToStr(int dt) const
{
    if(dt == 0) return "void";
    else if(dt == 1) return "bool";
    else if(dt == 2) return "short";
    else if(dt == 3) return "long";
    else if(dt == 4) return "float";
    else if(dt == 5) return "double";
    else if(dt == 6) return "ushort";
    else if(dt == 7) return "ulong";
    else if(dt == 8) return "string";
    else if(dt == 9) return "char";
    else if(dt == 10) return "short";
    else if(dt == 11) return "long";
    else if(dt == 12) return "float";
    else if(dt == 13) return "double";
    else if(dt == 14) return "ushort";
    else if(dt == 15) return "ulong";
    else if(dt == 16) return "string";
    else if(dt == 19) return "state";
    else if(dt == 20) return "const string";
    else if(dt == 22) return "uchar";
    else if(dt == 23) return "long64";
    else if(dt == 24) return "ulong64";
    else if(dt == 27) return "int";

    else return "unknown_type";
}

int CuWsTangoHelper::dataTypeStrToInt(const std::string &) const
{
    pred("TangoHelper.dataTypeToInt: unimplemented\n");
    return -1;
}

std::vector<std::string> CuWsTangoHelper::srcPatterns() const {
    std::vector<std::string> src_patterns;
    src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+/.+");
    src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+->.+");

    // support tango://host:PORT/a/b/c/d and tango://host:PORT/a/b/c->e
    // when CumbiaPool.guessBySrc needs to be used
    // with the two above only, sources like "tango://hokuto:20000/test/device/1/double_scalar"
    // are not identified unless they are preceded by an additional ws://, like
    // "ws://tango://hokuto:20000/test/device/1/double_scalar"
    src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+/.+");
    src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+->.+");
    return src_patterns;
}
