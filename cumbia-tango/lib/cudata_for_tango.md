# CuData bundle description for data exchange with the Tango world {#cudata_for_tango}


### CuData for Tango attribute configuration and command info

#### The CuData received upon *attribute configuration* will contain the following keys:

Key                 |Type                           |Value          |Description                                                                                                |Notes 
--------------------|-------------------------------|---------------|-----------------------------------------------------------------------------------------------------------|-----------------
type                |string                         |"property"     |Identifies a configuration content within CuData                                                           |Fixed value to identify the type of CuData
src                 |string                         |   -           |The source name as configured with setSource. Tango device / attribute                                     |*src* key always present in CuData
value               |CuVariant                      |    -          |The attribute value that is normally fetched during the configuration.                                     |   -
data                |bool                           |true           |A CuData with configuration data will have the "data" key always set to true                               |   -
msg                 |string                         |    -          |A message bearing information about the success of the operation.                                          |See the *err* key below
err                 |bool                           |    -          |true if an error occurred, false otherwise                                                                 |See the *msg* key above
df		            |Tango::AttrDataFormat (enum)   |    -          |Tango attribute format, Scalar,Spectrum,...                                                                |convert with toInt()
dfs			        |string                         |    -          |string representation of data_format                                                                       |   -
data_type           |int                            |    -          |Tango data type                                                                                            |   -
description         |string                         |    -          |Tango attribute description from AttributeInfo                                                             |   -
display_unit        |string                         |    -          |Tango display unit from AttributeInfo                                                                      |   -
standard_unit       |string                         |    -          |Tango standard unit from AttributeInfo                                                                     |   -
unit                |string                         |    -          |Tango "unit" from AttributeInfo                                                                            |   -
label               |string                         |    -          |Tango "label" from AttributeInfo                                                                           |   -
writable            |Tango::AttrWriteType           |    -          |Tango write type (enum) { READ, READ_WITH_WRITE, WRITE, READ_WRITE, WT_UNKNOWN  }                          |convert with toInt()
writable_attr_name  |string                         |    -          |writable_attr_name  from AttributeInfo                                                                     |   -
max_alarm           |string                         |    -          |Tango::AttributeAlarmInfo max_alarm                                                                        |   devapi.h
min_alarm           |string                         |    -          |Tango::AttributeAlarmInfo min_alarm                                                                        |   devapi.h
max_warning         |string                         |    -          |Tango::AttributeAlarmInfo max_warning                                                                      |   devapi.h
min_warning         |string                         |    -          |Tango::AttributeAlarmInfo min_warning                                                                      |   devapi.h
delta_t             |string                         |    -          |Tango::AttributeAlarmInfo delta_t                                                                          |   devapi.h
delta_val           |string                         |    -          |Tango::AttributeAlarmInfo delta_val                                                                        |   devapi.h
archive_abs_change  |string                         |    -          |Tango::AttributeEventInfo.arch_event.archive_abs_change                                                    |   devapi.h
archive_rel_change  |string                         |    -          |Tango::AttributeEventInfo.arch_event.archive_rel_change                                                    |   devapi.h
archive_period      |string                         |    -          |Tango::AttributeEventInfo.arch_event.archive_period                                                        |   devapi.h
abs_change          |string                         |    -          |Tango::AttributeEventInfo.ch_event.abs_change                                                              |   devapi.h
rel_change          |string                         |    -          |Tango::AttributeEventInfo.ch_event.rel_change                                                              |   devapi.h
periodic_period     |string                         |    -          |Tango::AttributeEventInfo.per_event.period                                                                 |   devapi.h
max                 |string                         |    -          |Tango AttributeConfig max_value                                                                            |Named "max" for consistency with Epics
min                 |string                         |    -          |Tango AttributeConfig min_value                                                                            |Named "min" for consistency with Epics
name                |string                         |    -          |Tango name string from AttributeInfo                                                                       |   -
max_dim_x           |int                            |    -          |Tango attribute max x dimension                                                                            |   -
max_dim_y           |int                            |    -          |Tango attribute max y dimension                                                                            |   -
disp_level          |Tango::DispLevel               |    -          |Tango::DispLevel from AttributeInfoEx (lib/cpp/server/idl/tango.h:  { OPERATOR, EXPERT, DL_UNKNOWN) }      |convert with toInt()
root_attr_name      |string                         |    -          |from AttributeInfoEx: Root attribute name (in case of forwarded attribute)                 |   -
description         |string                         |    -          |Tango attribute description from AttributeInfo                                                             |   -
display_unit        |string                         |    -          |Tango display unit from AttributeInfo                                                                      |   -
standard_unit       |string                         |    -          |Tango standard unit from AttributeInfo                                                                     |   -
unit                |string                         |    -          |Tango "unit" from AttributeInfo                                                                            |   -
success		        |string                         |    -          |A string with the name of the color provided the class CuTangoWorldConfig associated to an error/success   |Can be used to decorate widgets


#### The CuData received upon *command info* configuration type will contain the following keys:

Key                 |Type                           |Value          |Description                                                                                                |Notes 
--------------------|-------------------------------|---------------|-----------------------------------------------------------------------------------------------------------|-----------------
type                |string                         |"property"     |Identifies a configuration content within CuData                                                           |Fixed value to identify the type of CuData
src                 |string                         |   -           |The source name as configured with setSource. Tango device / attribute                                     |*src* 
data                |bool                           |true           |A CuData with configuration data will have the "data" key always set to true                               |   -
msg                 |string                         |    -          |A message bearing information about the success of the operation.                                          |See the *err* key below
err                 |bool                           |    -          |true if an error occurred, false otherwise                                                                 |See the *msg* key abovekey always present in CuData
value               |CuVariant                      |    -          |The attribute value that is normally fetched during the configuration.                                     |   -
cmd_name            |string                         |   -           |The command name, from Tango::CommandInfo                                                                  |from Tango::CommandInfo, devapi.h
in_type             |long                           |   -           |Input parameter data type                                                                                  |from Tango::CommandInfo, devapi.h
out_type            |long                           |   -           |Output parameter data type                                                                                 |from Tango::CommandInfo, devapi.h
dt		            |long                           |   -           |Output parameter type, same as "out_type", in order to be consistent with *attribute configuration* CuData |see CuData "data_type" from attribute configuration above
in_type_desc        |string                         |   -           |Input parameter description                                                                                |from Tango::CommandInfo, devapi.h
out_type_desc       |string                         |   -           |Output parameter description                                                                               |from Tango::CommandInfo, devapi.h
df		            |Tango::AttrDataFormat (enum)   |   -           |Provided for consistency with *attribute configuration* CuData, determined from CommandInfo out_type       |see CuData "df" from attribute configuration above
color               |string                         |    -          |A string with the name of the color provided the class CuTangoWorldConfig associated to an error/success   |Can be used to decorate widgets


### Example

In this example, we discuss how a plot is configured using the CuData with "type" set to "property".

\code
void QuSpectrumPlot::update(const CuData &da)
{
    if(!da["err"].toBool() && da["type"].toString() == "property")
    {
        CuVariant m, M;
        m = da["min"];  // min value
        M = da["max"];  // max value
        bool okl, oku;  // toDouble ok for lower and upper bound
        double lb, ub;  // double for lower and upper bound
        QString min = QString::fromStdString(m.toString()); // min is of type string
        QString max = QString::fromStdString(M.toString()); // max is of type string
        QString plotTitle = QString::fromStdString(da["label"].toString());
        lb = min.toDouble(&okl);  // string to double, see if ok
        ub = max.toDouble(&oku);  // string to double, see if ok

        if(okl && oku)
        {
            // get current default lower and upper bounds
            double current_def_lb = defaultLowerBound(QwtPlot::yLeft);
            double current_def_ub = defaultUpperBound(QwtPlot::yLeft);
            // if the minimum saved into lb is smaller than the current lower bound
            if(current_def_lb > lb)
                current_def_lb = lb;
            if(current_def_ub < ub)
                current_def_ub = ub;
            setDefaultBounds(current_def_lb, current_def_ub, QwtPlot::yLeft);
        }
    }
}
\endcode

### CuData contents from Tango Device attributes

When reading attributes, the key/values you should expect to find within the CuData are shown in the following table:



Key                 |Type                           |Value          |Description                                                                                                |Notes 
--------------------|-------------------------------|---------------|-----------------------------------------------------------------------------------------------------------|-----------------
data                |bool                           |true           |A CuData with configuration data will have the "data" key always set to true                               |-
src                 |string                         |   -           |The source name as configured with setSource. Tango device / attribute                                     |*src* 
msg                 |string                         |   -           |A message bearing information about the success of the operation.                                          |See the *err* key below
err                 |bool                           |   -           |true if an error occurred, false otherwise                                                                 |See the *msg* key abovekey always present in CuData
color	            |string                         |    -          |A string with the name of the color provided the class CuTangoWorldConfig associated to an error/success   |Can be used to decorate widgets
timestamp_ms        |time_t + suseconds_t           |   -           |Timestamp as integer: a struct timeval is used to store the timestamp. Formula: tiv.tv_sec * 1000 + tiv.tv_usec / 1000;          |can convert with toLongInt()
timestamp_us        |double                         |   -           |timestamp in a double: seconds.microseconds: static_cast<double>(tiv.tv_sec) + static_cast<double>(tiv.tv_usec) * 1e-6 | convert with toDouble
q		            |const Tango::AttrQuality       |   -           |Tango attribute quality { ATTR_VALID, ATTR_INVALID, ATTR_ALARM, ATTR_CHANGING, ATTR_WARNING }              |from server/idl/tango.h. Convert with toInt
qc			        |string                         |   -           |A string with the name of the color provided the class CuTangoWorldConfig                                  |can be used to decorate widgets. Use in combination with QuPalette. Convert with toString
dfs			        |string                         |   -           |Data format as string ("scalar", "vector", matrix", "data format unknown")                                 |consistent naming across tango and epics
value               |CuVariant                      |   -           |The attribute value. (read)                                                                                |The tango read value
value               |string                         |   -           |For Tango::DEV_STATE attribute types, the state is mapped into its string representation.                  |Convenience conversion applied only for DEV_STATE attribute data type.
w_value             |CuVariant                      |   -           |The attribute write value. Available if the attribute is read/write, key missing otherwise.                |Tango write value in read/write attributes
sc		            |string                         |   -           |state color: a string describing a color for a Tango::DEV_STATE, configuration possible through CuTangoWorldConfig.     |Can be used to decorate a widget. Available only if attribute.get_type() == Tango::DEV_STATE
s	                |long int                       |   -           |A Tango::DEV_STATE casted to long int. Available for Tango::DEV_STATE attributes                           |Available only if attribute.get_type() == Tango::DEV_STATE


### CuData contents from Tango commands

When getting output arguments from commands, the key/values you should expect to find within the CuData are shown in the following table:



Key                 |Type                           |Value          |Description                                                                                                |Notes 
--------------------|-------------------------------|---------------|-----------------------------------------------------------------------------------------------------------|-----------------
data                |bool                           |true           |A CuData with configuration data will have the "data" key always set to true                               |-
src                 |string                         |   -           |The source name as configured with setSource. Tango device / attribute                                     |*src* 
msg                 |string                         |   -           |A message bearing information about the success of the operation.                                          |See the *err* key below
err                 |bool                           |   -           |true if an error occurred, false otherwise                                                                 |See the *msg* key abovekey always present in CuData
color		        |string                         |    -          |A string with the name of the color provided the class CuTangoWorldConfig associated to an error/success   |Can be used to decorate widgets
timestamp_ms        |time_t + suseconds_t           |   -           |Timestamp as integer: a struct timeval is used to store the timestamp. Formula: tiv.tv_sec * 1000 + tiv.tv_usec / 1000;          |can convert with toLongInt()
timestamp_us        |double                         |   -           |timestamp in a double: seconds.microseconds: static_cast<double>(tiv.tv_sec) + static_cast<double>(tiv.tv_usec) * 1e-6 | convert with toDouble
dfs			        |string                         |   -           |Data format as string ("scalar", "vector", matrix", "data format unknown")                                 |consistent naming across tango and epics
dt		            |int                            |   -           |Tango::CmdArgType casted to an int: enum CmdArgType {DEV_VOID = 0,DEV_BOOLEAN,DEV_SHORT,DEV_LONG,... }     |From /lib/cpp/server/tango_const.h
value               |CuVariant                      |   -           |The attribute value. (read)                                                                                |The tango read value
value               |string                         |   -           |For Tango::DEV_STATE attribute types, the state is mapped into its string representation.                  |Convenience conversion applied only for DEV_STATE attribute data type.
sc		            |string                         |   -           |A string describing a color for a Tango::DEV_STATE, configuration possible through CuTangoWorldConfig.     |Can be used to decorate a widget. Available only if attribute.get_type() == Tango::DEV_STATE
s	                |long int                       |   -           |A Tango::DEV_STATE casted to long int. Available for Tango::DEV_STATE attributes                           |Available only if attribute.get_type() == Tango::DEV_STATE


### CuData contents for attribute properties.

The contents of CuData when used to fetch properties from the database are more dynamic and depend on the type of property fetched: attribute, class or device.
Please refer to the  \ref tutorial_activity for more information and for an example of application reading from the tango database.

To fetch  properties from the Tango database, a list of *input CuData* must be provided.

\li To get *attribute properties*, each CuData element must be compiled as follows:

Key                 |Type                           |Description                                |Example               
--------------------|-------------------------------|-------------------------------------------|-------------------
device              |string                         |The tango device name                      |"test/device/1"   
name                |string                         |The attribute property name                |"description"     


\li To get *device properties*, each CuData element must be compiled as follows:


Key                 |Type                           |Description                                |Example                  
--------------------|-------------------------------|-------------------------------------------|-----------------------
device              |string                         |The tango device name                      |"test/device/1"    
name                |string                         |The *device* property name                 |"description"       

\li To get *class properties*, each CuData bundle must be compiled as follows:

Key                 |Type                           |Description                                |Example                      
--------------------|-------------------------------|-------------------------------------------|-----------------------------
class               |string                         |The class name of the device               |"TangoTest"              
name                |string                         |The property name                 |"description"             

### Example of code to get properties from the Tango database

The following code shows how to set up a list of CuData where every element describes the type and name of the property 
to be fetched.
The PropertyReader class implements CuDataListener, so that when properties are available, the *onUpdate* method is invoked.

\code

// Suppose each element of "props" is a std::string formed as follows:
// - test/device/1:description device property: two '/' and ':'
// - test/device/1/double_scalar:values attribute properties: three '/' and one ':'
// - TangoTest:Description class property: one '/'
void PropertyReader::get(const char *id, const std::vector<std::string> &props)
{
    /* start the event loop in a separate thread, where data from activities will be posted */
    std::list<CuData> in_data;
    for(size_t i = 0; i < props.size(); i++) {
        size_t cnt = count(props[i].begin(), props[i].end(), '/');
        size_t cpos = props[i].find(':');
        if(cnt == 2 && cpos < std::string::npos)
        {
            CuData devpd("device", props[i].substr(0, cpos));
            devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            in_data.push_back(devpd);
        }
        else if(cnt == 3) {
            CuData devpd("device", props[i].substr(0, props[i].rfind('/')));
            if(cpos < std::string::npos) {
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos - props[i].rfind('/') -1);
                devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            }
            else
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos); // cpos == npos

            in_data.push_back(devpd);
        }
        else if(cnt == 0 && cpos < std::string::npos) { // class
            CuData cld("class", props[i].substr(0, cpos));
            cld["name"] = props[i].substr(cpos + 1);
            in_data.push_back(cld);
        }
    }
    
    // now fetch the properties
    CuTDbPropertyReader *pr = new CuTDbPropertyReader(id, m_ct);
    // PropertyReader in this example implements the CuDataListener interface
    pr->addListener(this);
    // get the properties listed within in_data
    pr->get(in_data);

\endcode

The *result* is again a CuData compiled as follows:

Key                 |Type                           |Description                                                        |Example                                                                                                |Notes
--------------------|-------------------------------|-------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------|-----------------------------------
err                 |bool                           |true if an error occurred, false otherwise                         |if(data["err"].toBool() { ... error ... }                              |See the *msg* key below
msg                 |string                         |A message bearing information about the success of the operation.  |   -                                                                       |See the *err* key above
list                |vector of strings              |The list of the properties contained in the result            |"test/device/1/long_scalar:abs_change", "TangoTest:Description", "test/device/1:helperApplication"  |use this to get the list of fetched properties
property1           |CuVariant                      |The property value corresponding to the "property1" property  |double abs_ch = std::stod(data["test/device/1/long_scalar:abs_change"].toString());                 |always use toString, properties are fetched as strings
property2           |CuVariant                      |The property value corresponding to the "property2" property  |std::string helperApp = data["test/device/1:helperApplication"].toString();                         |always use toString, properties are fetched as strings
...                 |CuVariant                      |The i-th property value...                                    |    -                                                                                                |always use toString, properties are fetched as strings
propertyN           |CuVariant                      |The N-th property value corresponding to the "propertyN" property  |std::string nthpropval = data["propertyN"].toString(); |always use toString, properties are fetched as strings

and is delivered as argument of the *onUpdate* method.


