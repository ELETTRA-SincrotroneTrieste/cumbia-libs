#ifndef CUDATATYPES_H
#define CUDATATYPES_H

#include <string>

#define DATA2CHUNK 32 // before including cudatatypes

class CuDType {
public:

    enum Key {
        Time_us,  ///< timestamp microsecs ("timestamp_us")
        Time_ms,   ///< timestamp millis ("timestamp_ms")
        Value, ///< the value, (was "value")
        Src,  ///< source name (was "src")
        Err,  ///< error flag ("err")
        SuccessColor,   ///< a color used to represent the successful or not outcome of an operation
        StateColor, ///< a color associated to a state (was "sc")
        State, ///< a state (was "s")
        Quality,  ///< quality value (was "q")
        QualityColor,  ///< a color associated to a quality value (was "qc")
        QualityString,  ///< a string associated to a quality value (was "qs")
        Time,  ///< timestamp
        Color, ///< color, was "color"
        WValue, ///< 'write' value (Tango set point, was "w_value")
        Thread,  ///< thread (e.g. for thread token)
        Name, ///< some name
        Time_ns,  ///< timestamp nanoseconds
        Timestamp_Str, ///< timestamp as string
        Message, ///< a message
        Mode,  ///< a mode
        Type,  ///< some type
        Exit,  ///< exit flag
        Ptr,  ///< pointer to something (void* is supported by CuVariant)
        InType,  ///< input argument type  (int, float, double, bool)
        OutType,  ///< output argument type  (int, float, double, bool)
        InTypeStr,  ///< input argument type  (int, float, double, bool)
        OutTypeStr,  ///< output argument type  (int, float, double, bool)
        WriteValue, ///< a *set point* value
        InputValue, ///< the input value to write to a target
        Properties, ///< list of properties
        Property, ///< a property
        IsCommand,  ///< true if data represents a command (e.g. Tango, was "is_command")
        Args,  ///< some arguments
        RefreshMode,  ///< a refresh (aka *read*) mode (polling, monitor,...)
        RefreshModeStr, ///< refresh mode as string
        Pv,  ///< Epics process variable
        DataType, ///< data type (int, float, double, bool)  (was "dt")
        Writable,   ///< read only or read/write ?
        DataFormat, ///< data format (vector, scalar, matrix, was "df")
        DataTypeStr, ///< data type as string (was "dts")
        DataFormatStr, ///< data format as string ("double", "int", ... ) (was "dfs")
        Description,  ///< a description
        InTypeDesc,  ///< input argument type description
        OutTypeDesc,  ///< output argument type description
        Status,    ///<
        Device,    ///<
        Attribute,    ///<
        Class,  ///<
        Pattern, ///< a pattern to search something (was: "pattern")
        Point,    ///<
        Max, ///<
        Min, ///<
        Keys, ///<
        Connected, ///< connection flag
        Period,   ///< period or timeout of  a timer (polling)
        Timeout,  ///< a timeout for something (e.g. Epics ca_timeout)
        NumberFormat, ///< "%.2f", "%d", "%s"...
        Label, ///< a label associated to a source, was "label"
        DimX, ///<  vector x dimension, was "dim_x"
        DimY, ///<  vector y dimension, was "dim_y"
        MaxDimX,
        MaxDimY,
        CmdName,///<  was "cmd"
        Activity,
        MaxDataKey = 64 };
};

class CuDTypeUtils
{
public:

    virtual ~CuDTypeUtils() {}

    virtual std::string keyName(int k) const {
        switch(k){
        case CuDType::Activity:
            return "Activity";
        case CuDType::Thread:
            return "Thread";
        case CuDType::Value:
            return std::string("value");
        case CuDType::Src:
            return std::string("src");
        case CuDType::Err:
            return std::string("err");
        case CuDType::Ptr:
            return std::string("ptr");
        case CuDType::Time:
            return std::string("timestamp");
        case CuDType::Time_ms:
            return std::string("timestamp_ms");
        case CuDType::Time_us:
            return std::string("timestamp_us");
        case CuDType::Time_ns:
            return std::string("timestamp_ns");

        case CuDType::Message:
            return std::string("msg");
        case CuDType::Mode:
            return std::string("Mode");
        case CuDType::Type:
            return std::string("Type");
        case CuDType::Exit:
            return std::string("Exit");
        case CuDType::MaxDataKey:
            return std::string("MaxDataKey");
        default:
            return std::string("Unknown_key ") + std::to_string(k);
        }
    }
};

#endif // CUDATATYPES_H
