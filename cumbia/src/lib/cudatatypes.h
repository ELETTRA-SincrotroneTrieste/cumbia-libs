#ifndef CUDATATYPES_H
#define CUDATATYPES_H

#include <string>
#include <string.h>

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
        StateColor, ///< a color associated to a state (was "sc")
        State, ///< a state (was "s")
        Quality,  ///< quality value (was "q")
        QualityColor,  ///< a color associated to a quality value (was "qc")
        QualityString,  ///< a string associated to a quality value (was "qs")
        Event,  ///< event type, was "E"
        Color, ///< color, was "color"
        Thread,  ///< thread (e.g. for thread token, was "thread")
        Name, ///< some name ("name")
        Timestamp_Str, ///< timestamp as string ("timestamp_str")
        Message, ///< a message (was "msg")
        Mode,  ///< a mode (was "mode")
        Type,  ///< type (was "type"), for example 'property'
        Exit,  ///< exit flag (was "exit")
        Ptr,  ///< pointer to something (void* is supported by CuVariant). Was "ptr"
        InType,  ///< input argument type  (int, float, double, bool). Was "in_type"
        OutType,  ///< output argument type  (int, float, double, bool). Was "out_type"
        InTypeStr,  ///< input argument type  (int, float, double, bool). Was "in_type_str"
        OutTypeStr,  ///< output argument type  (int, float, double, bool). Was "out_type_str"
        WriteValue, ///< a *set point* value (Tango write value, was "w_value")
        Properties, ///< list of properties, was "properties"
        Property, ///< a property, was "property"
        IsCommand,  ///< true if data represents a command (e.g. Tango, was "is_command")
        Args,  ///< some arguments, was "args"
        RefreshMode,  ///< a refresh (aka *read*) mode (polling, monitor,..., was "refresh_mode")
        RefreshModeStr, ///< refresh mode as string, was "refresh_mode_str"
        Pv,  ///< Epics process variable, was "pv"
        Time_ns,   ///< timestamp millis ("timestamp_ns")
        DataType, ///< data type (int, float, double, bool)  (was "dt")
        Writable,   ///< read only or read/write,
        DataFormat, ///< data format (vector, scalar, matrix, was "df")
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
        case CuDType::Time_us: return "timestamp_us";
        case CuDType::Time_ms: return "timestamp_ms";
        case CuDType::Value: return "value";
        case CuDType::Src: return "src";
        case CuDType::Err: return "err";
        case CuDType::State: return "state";
        case CuDType::StateColor: return "sc";
        case CuDType::Quality: return "q";
        case CuDType::QualityColor: return "qc";
        case CuDType::QualityString: return "qs";
        case CuDType::Event: return "event";
        case CuDType::Color: return "color";
        case CuDType::Thread: return "thread";
        case CuDType::Name: return "name";
        case CuDType::Message: return "msg";
        case CuDType::Mode: return "mode";
        case CuDType::Type: return "type";
        case CuDType::Exit: return "exit";
        case CuDType::Ptr: return "ptr";
        case CuDType::InType: return "in_type";
        case CuDType::OutType: return "out_type";
        case CuDType::InTypeStr: return "in_type_str";
        case CuDType::OutTypeStr: return "out_type_str";
        case CuDType::WriteValue: return "w_value";
        case CuDType::Properties: return "properties";
        case CuDType::Property: return "property";
        case CuDType::IsCommand: return "is_command";
        case CuDType::Args: return "args";
        case CuDType::RefreshMode: return "refresh_mode";
        case CuDType::Pv: return "pv";
        case CuDType::Time_ns: return "timestamp_ns";
        case CuDType::DataType: return "dt";
        case CuDType::Writable: return "writable";
        case CuDType::DataFormat: return "df";
        case CuDType::DataFormatStr: return "dfs";
        case CuDType::Description: return "description";
        case CuDType::InTypeDesc: return "in_type_desc";
        case CuDType::OutTypeDesc: return "out_type_desc";
        case CuDType::Status: return "status";
        case CuDType::Device: return "device";
        case CuDType::Attribute: return "attribute";
        case CuDType::Class: return "class";
        case CuDType::Pattern: return "pattern";
        case CuDType::Point: return "point";
        case CuDType::Max: return "max";
        case CuDType::Min: return "min";
        case CuDType::Keys: return "keys";
        case CuDType::Connected: return "connected";
        case CuDType::Period: return "period";
        case CuDType::NumberFormat: return "format";
        case CuDType::Label: return "label";
        case CuDType::DimX: return "dim_x";
        case CuDType::DimY: return "dim_y";
        case CuDType::MaxDimX: return "max_dim_x";
        case CuDType::MaxDimY: return "max_dim_y";
        case CuDType::CmdName: return "cmd";
        case CuDType::Activity: return "activity";
        case CuDType::MaxDataKey: return "MaxDataKey";
        default: return "InvalidKey";
        }
    }

    virtual CuDType::Key idx(const char *s) const {
        if(!strcmp(s, "timestamp_us")) return  CuDType::Time_us;
        else if(!strcmp(s, "timestamp_ms")) return  CuDType::Time_ms;
        else if(!strcmp(s, "value")) return  CuDType::Value;
        else if(!strcmp(s, "src")) return  CuDType::Src;
        else if(!strcmp(s, "err")) return  CuDType::Err;
        else if(!strcmp(s, "state")) return  CuDType::State;
        else if(!strcmp(s, "sc")) return  CuDType::StateColor;
        else if(!strcmp(s, "q")) return  CuDType::Quality;
        else if(!strcmp(s, "qc")) return  CuDType::QualityColor;
        else if(!strcmp(s, "sc")) return  CuDType::StateColor;
        else if(!strcmp(s, "qs")) return  CuDType::QualityString;
        else if(!strcmp(s, "qc")) return  CuDType::QualityColor;
        else if(!strcmp(s, "E")) return  CuDType::Event;
        else if(!strcmp(s, "color")) return  CuDType::Color;
        else if(!strcmp(s, "w_value")) return  CuDType::WriteValue;
        else if(!strcmp(s, "thread")) return  CuDType::Thread;
        else if(!strcmp(s, "name")) return  CuDType::Name;
        else if(!strcmp(s, "msg")) return  CuDType::Message;
        else if(!strcmp(s, "mode")) return  CuDType::Mode;
        else if(!strcmp(s, "type")) return  CuDType::Type;
        else if(!strcmp(s, "exit")) return  CuDType::Exit;
        else if(!strcmp(s, "ptr")) return  CuDType::Ptr;
        else if(!strcmp(s, "in_type")) return  CuDType::InType;
        else if(!strcmp(s, "out_type")) return  CuDType::OutType;
        else if(!strcmp(s, "in_type_str")) return  CuDType::InTypeStr;
        else if(!strcmp(s, "out_type_str")) return  CuDType::OutTypeStr;
        else if(!strcmp(s, "property")) return  CuDType::Property;
        else if(!strcmp(s, "properties")) return  CuDType::Properties;
        else if(!strcmp(s, "is_command")) return  CuDType::IsCommand;
        else if(!strcmp(s, "args")) return  CuDType::Args;
        else if(!strcmp(s, "refresh_mode")) return  CuDType::RefreshMode;
        else if(!strcmp(s, "pv")) return  CuDType::Pv;
        else if(!strcmp(s, "timestamp_ns")) return  CuDType::Time_ns;
        else if(!strcmp(s, "dt")) return  CuDType::DataType;
        else if(!strcmp(s, "writable")) return  CuDType::Writable;
        else if(!strcmp(s, "df")) return  CuDType::DataFormat;
        else if(!strcmp(s, "dfs")) return  CuDType::DataFormatStr;
        else if(!strcmp(s, "writable")) return  CuDType::Writable;
        else if(!strcmp(s, "description")) return  CuDType::Description;
        else if(!strcmp(s, "in_type_desc")) return  CuDType::InTypeDesc;
        else if(!strcmp(s, "out_type_desc")) return  CuDType::OutTypeDesc;
        else if(!strcmp(s, "status")) return  CuDType::Status;
        else if(!strcmp(s, "device")) return  CuDType::Device;
        else if(!strcmp(s, "attribute")) return  CuDType::Attribute;
        else if(!strcmp(s, "class")) return  CuDType::Class;
        else if(!strcmp(s, "pattern")) return  CuDType::Pattern;
        else if(!strcmp(s, "point")) return  CuDType::Point;
        else if(!strcmp(s, "max")) return  CuDType::Max;
        else if(!strcmp(s, "min")) return  CuDType::Min;
        else if(!strcmp(s, "keys")) return  CuDType::Keys;
        else if(!strcmp(s, "connected")) return  CuDType::Connected;
        else if(!strcmp(s, "period")) return  CuDType::Period;
        else if(!strcmp(s, "format")) return  CuDType::NumberFormat;
        else if(!strcmp(s, "label")) return  CuDType::Label;
        else if(!strcmp(s, "dim_x")) return  CuDType::DimX;
        else if(!strcmp(s, "dim_y")) return  CuDType::DimY;
        else if(!strcmp(s, "max_dim_x")) return  CuDType::MaxDimX;
        else if(!strcmp(s, "max_dim_y")) return  CuDType::MaxDimY;
        else if(!strcmp(s, "cmd")) return  CuDType::CmdName;
        else if(!strcmp(s, "activity")) return  CuDType::Activity;
        else return CuDType::MaxDataKey;
    }
};

#endif // CUDATATYPES_H
