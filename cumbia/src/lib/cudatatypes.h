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
        ExtractDataFlags, ///< flags to tell the engine the desired level of details to extract data
        MaxDataKey = 64 };
};

#define TTT CuDType

class CuDTypeUtils
{
public:

    virtual ~CuDTypeUtils() {}

    virtual std::string keyName(int k) const {
        switch(k){
        case TTT::Time_us: return "timestamp_us";
        case TTT::Time_ms: return "timestamp_ms";
        case TTT::Value: return "value";
        case TTT::Src: return "src";
        case TTT::Err: return "err";
        case TTT::State: return "state";
        case TTT::StateColor: return "sc";
        case TTT::Quality: return "q";
        case TTT::QualityColor: return "qc";
        case TTT::QualityString: return "qs";
        case TTT::Event: return "event";
        case TTT::Color: return "color";
        case TTT::Thread: return "thread";
        case TTT::Name: return "name";
        case TTT::Message: return "msg";
        case TTT::Mode: return "mode";
        case TTT::Type: return "type";
        case TTT::Exit: return "exit";
        case TTT::Ptr: return "ptr";
        case TTT::InType: return "in_type";
        case TTT::OutType: return "out_type";
        case TTT::InTypeStr: return "in_type_str";
        case TTT::OutTypeStr: return "out_type_str";
        case TTT::WriteValue: return "w_value";
        case TTT::Properties: return "properties";
        case TTT::Property: return "property";
        case TTT::IsCommand: return "is_command";
        case TTT::Args: return "args";
        case TTT::RefreshMode: return "refresh_mode";
        case TTT::Pv: return "pv";
        case TTT::Time_ns: return "timestamp_ns";
        case TTT::DataType: return "dt";
        case TTT::Writable: return "writable";
        case TTT::DataFormat: return "df";
        case TTT::DataFormatStr: return "dfs";
        case TTT::Description: return "description";
        case TTT::InTypeDesc: return "in_type_desc";
        case TTT::OutTypeDesc: return "out_type_desc";
        case TTT::Status: return "status";
        case TTT::Device: return "device";
        case TTT::Attribute: return "attribute";
        case TTT::Class: return "class";
        case TTT::Pattern: return "pattern";
        case TTT::Point: return "point";
        case TTT::Max: return "max";
        case TTT::Min: return "min";
        case TTT::Keys: return "keys";
        case TTT::Connected: return "connected";
        case TTT::Period: return "period";
        case TTT::NumberFormat: return "format";
        case TTT::Label: return "label";
        case TTT::DimX: return "dim_x";
        case TTT::DimY: return "dim_y";
        case TTT::MaxDimX: return "max_dim_x";
        case TTT::MaxDimY: return "max_dim_y";
        case TTT::CmdName: return "cmd";
        case TTT::Activity: return "activity";
        case TTT::MaxDataKey: return "MaxDataKey";
        default: return "InvalidKey";
        }
    }

    virtual TTT::Key idx(const char *s) const {
        if(!strcmp(s, "timestamp_us")) return  TTT::Time_us;
        else if(!strcmp(s, "timestamp_ms")) return  TTT::Time_ms;
        else if(!strcmp(s, "value")) return  TTT::Value;
        else if(!strcmp(s, "src")) return  TTT::Src;
        else if(!strcmp(s, "err")) return  TTT::Err;
        else if(!strcmp(s, "state")) return  TTT::State;
        else if(!strcmp(s, "sc")) return  TTT::StateColor;
        else if(!strcmp(s, "q")) return  TTT::Quality;
        else if(!strcmp(s, "qc")) return  TTT::QualityColor;
        else if(!strcmp(s, "sc")) return  TTT::StateColor;
        else if(!strcmp(s, "qs")) return  TTT::QualityString;
        else if(!strcmp(s, "qc")) return  TTT::QualityColor;
        else if(!strcmp(s, "E")) return  TTT::Event;
        else if(!strcmp(s, "color")) return  TTT::Color;
        else if(!strcmp(s, "w_value")) return  TTT::WriteValue;
        else if(!strcmp(s, "thread")) return  TTT::Thread;
        else if(!strcmp(s, "name")) return  TTT::Name;
        else if(!strcmp(s, "msg")) return  TTT::Message;
        else if(!strcmp(s, "mode")) return  TTT::Mode;
        else if(!strcmp(s, "type")) return  TTT::Type;
        else if(!strcmp(s, "exit")) return  TTT::Exit;
        else if(!strcmp(s, "ptr")) return  TTT::Ptr;
        else if(!strcmp(s, "in_type")) return  TTT::InType;
        else if(!strcmp(s, "out_type")) return  TTT::OutType;
        else if(!strcmp(s, "in_type_str")) return  TTT::InTypeStr;
        else if(!strcmp(s, "out_type_str")) return  TTT::OutTypeStr;
        else if(!strcmp(s, "property")) return  TTT::Property;
        else if(!strcmp(s, "properties")) return  TTT::Properties;
        else if(!strcmp(s, "is_command")) return  TTT::IsCommand;
        else if(!strcmp(s, "args")) return  TTT::Args;
        else if(!strcmp(s, "refresh_mode")) return  TTT::RefreshMode;
        else if(!strcmp(s, "pv")) return  TTT::Pv;
        else if(!strcmp(s, "timestamp_ns")) return  TTT::Time_ns;
        else if(!strcmp(s, "dt")) return  TTT::DataType;
        else if(!strcmp(s, "writable")) return  TTT::Writable;
        else if(!strcmp(s, "df")) return  TTT::DataFormat;
        else if(!strcmp(s, "dfs")) return  TTT::DataFormatStr;
        else if(!strcmp(s, "writable")) return  TTT::Writable;
        else if(!strcmp(s, "description")) return  TTT::Description;
        else if(!strcmp(s, "in_type_desc")) return  TTT::InTypeDesc;
        else if(!strcmp(s, "out_type_desc")) return  TTT::OutTypeDesc;
        else if(!strcmp(s, "status")) return  TTT::Status;
        else if(!strcmp(s, "device")) return  TTT::Device;
        else if(!strcmp(s, "attribute")) return  TTT::Attribute;
        else if(!strcmp(s, "class")) return  TTT::Class;
        else if(!strcmp(s, "pattern")) return  TTT::Pattern;
        else if(!strcmp(s, "point")) return  TTT::Point;
        else if(!strcmp(s, "max")) return  TTT::Max;
        else if(!strcmp(s, "min")) return  TTT::Min;
        else if(!strcmp(s, "keys")) return  TTT::Keys;
        else if(!strcmp(s, "connected")) return  TTT::Connected;
        else if(!strcmp(s, "period")) return  TTT::Period;
        else if(!strcmp(s, "format")) return  TTT::NumberFormat;
        else if(!strcmp(s, "label")) return  TTT::Label;
        else if(!strcmp(s, "dim_x")) return  TTT::DimX;
        else if(!strcmp(s, "dim_y")) return  TTT::DimY;
        else if(!strcmp(s, "max_dim_x")) return  TTT::MaxDimX;
        else if(!strcmp(s, "max_dim_y")) return  TTT::MaxDimY;
        else if(!strcmp(s, "cmd")) return  TTT::CmdName;
        else if(!strcmp(s, "activity")) return  TTT::Activity;
        else return TTT::MaxDataKey;
    }
};

#endif // CUDATATYPES_H
