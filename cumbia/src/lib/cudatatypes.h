#ifndef CUDATATYPES_H
#define CUDATATYPES_H

#include <string>

#define DATA2CHUNK 32 // before including cudatatypes

class CuDType {
public:

    // each key defined here shall have a comment in the form ///<
    // followed by any text and the name of the corresponding string
    // key used in cumbia 1.x between "quotation marks", so that
    // cumbia 1.x - 2.x conversion utilities can parse the line
    // and map string keys to the new enum values
    enum Key {
        Time_us,  ///< timestamp microsecs ("timestamp_us")
        Time_ms,   ///< timestamp millis ("timestamp_ms")
        Value, ///< the value, (was "value")
        Src,  ///< source name (was "src")
        Err,  ///< error flag ("err")
        SuccessColor,   ///< a color used to represent the successful or not outcome of an operation (was "sc")
        StateColor, ///< a color associated to a state (was "sc")
        State, ///< a state (was "s")
        Quality,  ///< quality value (was "q")
        QualityColor,  ///< a color associated to a quality value (was "qc")
        QualityString,  ///< a string associated to a quality value (was "qs")
        Time,  ///< timestamp
        Color, ///< color, was "color"
        WValue, ///< 'write' value (Tango set point, was "w_value")
        Thread,  ///< thread (e.g. for thread token, was "thread")
        Name, ///< some name ("name")
        Time_ns,  ///< timestamp nanoseconds ("timestamp_ns")
        Timestamp_Str, ///< timestamp as string ("timestamp_str")
        Message, ///< a message (was "msg")
        Mode,  ///< a mode (was "mode")
        Type,  ///< some type (was "type")
        Exit,  ///< exit flag (was "exit")
        Ptr,  ///< pointer to something (void* is supported by CuVariant). Was "ptr"
        InType,  ///< input argument type  (int, float, double, bool). Was "in_type"
        OutType,  ///< output argument type  (int, float, double, bool). Was "out_type"
        InTypeStr,  ///< input argument type  (int, float, double, bool). Was "in_type_str"
        OutTypeStr,  ///< output argument type  (int, float, double, bool). Was "out_type_str"
        WriteValue, ///< a *set point* value (Tango write value, was "w_value")
        InputValue, ///< the input value to write to a target
        Properties, ///< list of properties, was "properties"
        Property, ///< a property, was "property"
        IsCommand,  ///< true if data represents a command (e.g. Tango, was "is_command")
        Args,  ///< some arguments, was "args"
        RefreshMode,  ///< a refresh (aka *read*) mode (polling, monitor,..., was "refresh_mode")
        RefreshModeStr, ///< refresh mode as string, was "refresh_mode_str"
        Pv,  ///< Epics process variable, was "pv"
        DataType, ///< data type (int, float, double, bool)  (was "dt")
        Writable,   ///< read only or read/write,
        DataFormat, ///< data format (vector, scalar, matrix, was "df")
        DataTypeStr, ///< data type as string (was "dts")
        DataFormatStr, ///< data format as string ("double", "int", ... ) (was "dfs")
        Description,  ///< a description, was "description"
        InTypeDesc,  ///< input argument type description, was "in_type_desc"
        OutTypeDesc,  ///< output argument type description, was "out_type_desc"
        Status,    ///< was "status"
        Device,    ///< was "device"
        Attribute,    ///< was "attribute"
        Class,  ///< was "class"
        Pattern, ///< a pattern to search something (was: "pattern")
        Point,    ///< typically, a Tango attribute or command name, such as double_scalar, was "point"
        Max, ///< max value, was "max"
        Min, ///< min value, was "min"
        Keys, ///< list of keys, was "keys"
        Connected, ///< connection flag, was "conn"
        Period,   ///< period or timeout of  a timer (polling), was "period"
        Timeout,  ///< a timeout for something (e.g. Epics ca_timeout)
        NumberFormat, ///< %.2f, %d, %s... (was "format")
        Label, ///< a label associated to a source, was "label"
        DimX, ///<  vector x dimension, was "dim_x"
        DimY, ///<  vector y dimension, was "dim_y"
        MaxDimX, ///< maximum dimension of X, was "max_dim_x" (used in Tango spectrum)
        MaxDimY, ///< maximum dimension of Y, was "max_dim_y" (used in Tango image)
        CmdName,///<  was "cmd"
        Activity, ///<  cumbia activity, was "activity"
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
