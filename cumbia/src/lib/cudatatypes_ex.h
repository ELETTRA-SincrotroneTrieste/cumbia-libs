#ifndef CUDATATYPES_EX_H
#define CUDATATYPES_EX_H

// extensions for CuData data types

#include <cudatatypes.h>

/*! \brief extended data types
 *
 * The XKeys enum can hold up to XKeysMax - TTT::MaxBaseDataKey values
 * XKeysMax is 128 if DATA2CHUNK is defined to be 32 in cudata.h
 */
class CuXDType : public CuDType
{
public:
    enum XKeys {


                 ///
                 /// the following one defines the size of the data array
                 /// reserved for fast access to data
                 ArrayDataEnd = TTT::MaxDataKey + 1, /// < reserved 64
                 ///

                 ///
                 /// data2: dynamically allocated data chunk 1 starts
                 ///
                 // 65
                 UpperAlarmLimit = ArrayDataEnd + 1,    ///<
                 LowerAlarmLimit,    ///<
                 LowerWarningLimit,    ///<
                 UpperWarningLimit,    ///<
                 UpperCtrlLimit,    ///<
                 LowerCtrlLimit,    ///<
                 MaxDimX,    ///<
                 MaxDimY,    ///<
                 DisplayUnit,    ///<
                 Unit,    ///<
                 StandardUnit,    ///<
                 WritableAttrName,    ///<
                 Delta_t,    ///<
                 ArchiveAbsChange,    ///<
                 ArchiveRelChange,    ///<
                 ArchivePeriod,    ///<
                 AbsChange,    ///<
                 RelChange,    ///<
                 PeriodicPeriod,
                 Delta_val,    ///<
                 DisplayLevel,    ///<
                 RootAttrName,   ///<

                 FetchProperties, ///< fetch properties option (i.e. Tango attribute properties)
                 FetchHistory,      ///< fetch history (i.e. Tango attribute history)

                State,

                XKeysUser = 128,
                XKeysMax  = 256      // 192
               };
};

class CuXDTypeUtils : public CuDTypeUtils
{
public:

    virtual ~CuXDTypeUtils() {}

    virtual std::string keyName(int k) const {

        if(k <= TTT::MaxDataKey)
            return CuDTypeUtils::keyName(k);
        else {
            switch(k){
            case CuXDType::DataType:
                return std::string("DataType");
            case CuXDType::InType:
                return std::string("InType");
            case CuXDType::OutType:
                return std::string("OutType");
            case CuXDType::WriteValue:
                return std::string("WriteValue");
            case CuXDType::Properties:
                return std::string("Properties");
            case CuXDType::Args:
                return std::string("Args");
            case CuXDType::RefreshMode:
                return std::string("RefreshMode");
            case CuXDType::Pv:
                return std::string("Pv");
            case CuXDType::Writable:
                return std::string("Writable");
            case CuXDType::QualityColor:
                return std::string("QualityColor");
            case CuXDType::Quality:
                return std::string("Quality");


            case CuXDType::Description:
                return std::string("Description");
            case CuXDType::InTypeDesc:
                return std::string("InTypeDesc");
            case CuXDType::OutTypeDesc:
                return std::string("OutTypeDesc");

            case CuXDType::Status:
                return std::string("Status");
            case CuXDType::State:
                return std::string("State");
            case CuXDType::Device:
                return std::string("Device");
            case CuXDType::Point:
                return std::string("Point");

            case CuXDType::Max:
                return std::string("Max");
            case CuXDType::Min:
                return std::string("Min");
            case CuXDType::Connected:
                return std::string("Connected");
            case CuXDType::Period:
                return std::string("Period");
            case CuXDType::Timeout:
                return std::string("Timeout");
            case CuXDType::DataFormat:
                return std::string("DataFormat");
            case CuXDType::DataFormatStr:
                return std::string("DataFormatStr");
            case CuXDType::NumberFormat:
                return std::string("NumberFormat");
            case CuXDType::Label:
                return std::string("Label");
            case CuXDType::DimX:
                return std::string("DimX");
            case CuXDType::DimY:
                return std::string("DimY");
            case CuXDType::CmdName:
                return std::string("CmdName");

            case CuXDType::UpperAlarmLimit:
                return std::string("UpperAlarmLimit");
            case CuXDType::LowerAlarmLimit:
                return std::string("LowerAlarmLimit");
            case CuXDType::UpperWarningLimit:
                return std::string("UpperWarningLimit");
            case CuXDType::LowerWarningLimit:
                return std::string("LowerWarningLimit");
            case CuXDType::LowerCtrlLimit:
                return std::string("LowerCtrlLimit");
            case CuXDType::UpperCtrlLimit:
                return std::string("UpperCtrlLimit");


            case CuXDType::MaxDimX:
                return std::string("MaxDimX");
            case CuXDType::MaxDimY:
                return std::string("MaxDimY");
            case CuXDType::DisplayUnit:
                return std::string("DisplayUnit");
            case CuXDType::Unit:
                return std::string("Unit");
            case CuXDType::StandardUnit:
                return std::string("StandardUnit");
            case CuXDType::WritableAttrName:
                return std::string("WritableAttrName");
            case CuXDType::Delta_t:
                return std::string("Delta_t");
            case CuXDType::Delta_val:
                return std::string("Delta_val");
            case  CuXDType::ArchiveAbsChange:
                return std::string("ArchiveAbsChange");
            case  CuXDType::ArchiveRelChange:
                return std::string("ArchiveRelChange");
            case  CuXDType::ArchivePeriod:
                return std::string("ArchivePeriod");
            case  CuXDType::AbsChange:
                return std::string("AbsChange");
            case  CuXDType::RelChange:
                return std::string("RelChange");
            case  CuXDType::PeriodicPeriod:
                return std::string("PeriodicPeriod");
            case  CuXDType::DisplayLevel:
                return std::string("DisplayLevel");
            }
        }
        return "Unknown_key[" + std::to_string(k) + "]";
    }
};

#endif

