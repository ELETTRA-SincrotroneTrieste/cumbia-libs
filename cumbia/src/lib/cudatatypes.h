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
        Event,  ///< event type, was "E"
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
        case CuDType::Time_us: return "Time_us";
        case CuDType::Time_ms: return "Time_ms";
        case CuDType::Value: return "Value";
        case CuDType::Src: return "Src";
        case CuDType::Err: return "Err";
        case CuDType::State: return "State";
        case CuDType::SuccessColor: return "SuccessColor";
        case CuDType::StateColor: return "StateColor";
        case CuDType::Quality: return "Quality";
        case CuDType::QualityColor: return "QualityColor";
        case CuDType::QualityString: return "QualityString";
        case CuDType::Event: return "Event";
        case CuDType::Color: return "Color";
        case CuDType::WValue: return "WValue";
        case CuDType::Thread: return "Thread";
        case CuDType::Name: return "Name";
        case CuDType::Time_ns: return "Time_ns";
        case CuDType::Timestamp_Str: return "Timestamp_Str";
        case CuDType::Message: return "Message";
        case CuDType::Mode: return "Mode";
        case CuDType::Type: return "Type";
        case CuDType::Exit: return "Exit";
        case CuDType::Ptr: return "Ptr";
        case CuDType::InType: return "InType";
        case CuDType::OutType: return "OutType";
        case CuDType::InTypeStr: return "InTypeStr";
        case CuDType::OutTypeStr: return "OutTypeStr";
        case CuDType::WriteValue: return "WriteValue";
        case CuDType::InputValue: return "InputValue";
        case CuDType::Properties: return "Properties";
        case CuDType::Property: return "Property";
        case CuDType::IsCommand: return "IsCommand";
        case CuDType::Args: return "Args";
        case CuDType::RefreshMode: return "RefreshMode";
        case CuDType::RefreshModeStr: return "RefreshModeStr";
        case CuDType::Pv: return "Pv";
        case CuDType::DataType: return "DataType";
        case CuDType::Writable: return "Writable";
        case CuDType::DataFormat: return "DataFormat";
        case CuDType::DataTypeStr: return "DataTypeStr";
        case CuDType::DataFormatStr: return "DataFormatStr";
        case CuDType::Description: return "Description";
        case CuDType::InTypeDesc: return "InTypeDesc";
        case CuDType::OutTypeDesc: return "OutTypeDesc";
        case CuDType::Status: return "Status";
        case CuDType::Device: return "Device";
        case CuDType::Attribute: return "Attribute";
        case CuDType::Class: return "Class";
        case CuDType::Pattern: return "Pattern";
        case CuDType::Point: return "Point";
        case CuDType::Max: return "Max";
        case CuDType::Min: return "Min";
        case CuDType::Keys: return "Keys";
        case CuDType::Connected: return "Connected";
        case CuDType::Period: return "Period";
        case CuDType::Timeout: return "Timeout";
        case CuDType::NumberFormat: return "NumberFormat";
        case CuDType::Label: return "Label";
        case CuDType::DimX: return "DimX";
        case CuDType::DimY: return "DimY";
        case CuDType::MaxDimX: return "MaxDimX";
        case CuDType::MaxDimY: return "MaxDimY";
        case CuDType::CmdName: return "CmdName";
        case CuDType::Activity: return "Activity";
        case CuDType::MaxDataKey: return "MaxDataKey";
        default: return "InvalidKey";
        }
    }
};

#endif // CUDATATYPES_H
