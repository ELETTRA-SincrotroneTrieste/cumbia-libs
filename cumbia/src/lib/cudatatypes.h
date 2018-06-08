#ifndef CUDATATYPES_H
#define CUDATATYPES_H

#include <string>

#define DATA2CHUNK 32 // before including cudatatypes

class CuDType {
public:

    enum Key { Activity = 1,
               Thread,  ///< thread (e.g. for thread token)
               Class,  ///< class
               Value, ///<
               Src,  ///<
               Name, ///< some name
               Err,  ///<
               Time,  ///< timestamp
               Time_ms,   ///< timestamp millis
               Time_ns,  ///< timestamp nanoseconds
               Time_us,  ///< timestamp microsecs
               Timestamp_Str, ///< timestamp as string
               Message, ///< a message
               Mode,  ///< a mode
               Type,  ///< some type
               Exit,  ///< exit flag
               Ptr,  ///< pointer to something (void* is supported by CuVariant)
               MaxBaseDataKey = 24,
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
        case CuDType::Class:
            return "Class";
        case CuDType::Value:
            return std::string("Value");
        case CuDType::Src:
            return std::string("Src");
        case CuDType::Err:
            return std::string("Err");
        case CuDType::Ptr:
            return std::string("Ptr");
        case CuDType::Time:
            return std::string("Time");
        case CuDType::Time_ms:
            return std::string("Time_ms");
        case CuDType::Time_us:
            return std::string("Time_us");
        case CuDType::Time_ns:
            return std::string("Time_ns");

        case CuDType::Message:
            return std::string("Message");
        case CuDType::Mode:
            return std::string("Mode");
        case CuDType::Type:
            return std::string("Type");
        case CuDType::Exit:
            return std::string("Exit");

        case CuDType::MaxBaseDataKey:
            return std::string("MaxBaseDataKey");
        case CuDType::MaxDataKey:
            return std::string("MaxDataKey");
        default:
            return std::string("Unknown_key ") + std::to_string(k);
        }
    }
};

#include <cudatatypes_ex.h>
#endif // CUDATATYPES_H
