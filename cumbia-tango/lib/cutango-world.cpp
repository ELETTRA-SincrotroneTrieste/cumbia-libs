#include "cutango-world.h"
#include "cutango-world-config.h"
#include <unordered_map>
#include <cumacros.h>
#include <regex>

class CuTangoWorldPrivate
{
public:
    bool error;
    std::string message;
    CuTangoWorldConfig t_world_conf;

    std::vector <std::string> src_patterns;
};

CuTangoWorld::CuTangoWorld()
{
    d = new CuTangoWorldPrivate();
    d->src_patterns.push_back("[A-Za-z0-9_\\.\\$]+/.+");
    d->src_patterns.push_back("[A-Za-z0-9_\\.\\$]+->.+");
}

CuTangoWorld::~CuTangoWorld()
{
    delete d;
}

std::string CuTangoWorld::strerror(const Tango::DevFailed &e)
{
    std::string msg;
    if(e.errors.length() > 0)
        msg = strerror(e.errors);

    return msg;
}

std::string CuTangoWorld::strerror(const Tango::DevErrorList &errors)
{
    std::string msg;
    for(int i = errors.length() - 1; i >= 0; i--)
    {
        msg += errors[i].origin;
        msg += "\n";
        msg += errors[i].desc;
        msg += "\n";
        msg += errors[i].reason;
        msg += "\n\n";
    }
    return msg;
}

void CuTangoWorld::fillThreadInfo(CuData &dat, const CuActivity* a)
{
    char info[32];
    sprintf(info, "0x%lx", pthread_self());
    dat["worker_thread"] = std::string(info);
    sprintf(info, "%p", a);
    dat["worker_activity"] = std::string(info);
}

/*! \brief extracts data from Tango::DeviceData and puts it into the CuData bundle
 *
 * \par Note
 * "timestamp_ms" and "timestamp_us" *are not set* by extractData. It must be set
 * manually by the caller. There is no date/time information in Tango::DeviceData.
 *
 *
 */
void CuTangoWorld::extractData(Tango::DeviceData *data, CuData& da)
{
    d->error = false;
    d->message = "";
    int t = (Tango::CmdArgType) data->get_type();
    da["data_format_str"] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>(t));
    da["data_type"] = t;

    if (data->is_empty())
    {
        d->error = true;
        d->message = "empty device data";
    }
    else
    {
        d->error = false;
    }
    try {
        switch (t)
        {
        case Tango::DEV_STATE:
        {
            CuTangoWorldConfig wc;
            Tango::DevState temp;
            *data >> temp;
            da["value"] = wc.stateString(temp);
            da["state"] = static_cast<long int>(temp);
            da["state_color"] = wc.stateColorName(temp);
            break;
        }
        case Tango::DEV_BOOLEAN:
        {
            bool booltmp;
            *data >> booltmp;
            da["value"] = booltmp;
            break;
        }
        case Tango::DEV_SHORT:
        {
            short temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEV_USHORT:
        {
            unsigned short temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEV_LONG:
        {
            Tango::DevLong temp;
            *data >> temp;
            da["value"] = (long) temp;
            break;
        }
        case Tango::DEV_LONG64: {
            long int temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEV_ULONG:
        {
            Tango::DevULong temp;
            *data >> temp;
            da["value"] = static_cast<unsigned long> (temp);
            break;
        }
        case Tango::DEV_ULONG64: {
            long unsigned int temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEV_FLOAT:
        {
            Tango::DevFloat temp;
            *data >> temp;
            da["value"] = (float) temp;
            break;
        }
        case Tango::DEV_DOUBLE:
        {
            double temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEV_STRING:
        {
            std::string temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_BOOLEANARRAY:
        {
            perr("CuTangoWorld::extractData: boolean array is not implemented");
            break;
        }
        case Tango::DEVVAR_SHORTARRAY:
        {
            vector<short> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_USHORTARRAY:
        {
            vector<unsigned short> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_LONGARRAY:
        {
            vector<Tango::DevLong> temp;
            vector<long int> li; // !! Tango DevLong
            *data >> temp;
            li.reserve(temp.size());
            for(size_t i = 0; i < temp.size(); i++)
                li.push_back(static_cast<long int>(temp[i]));
            da["value"] = li;
            break;
        }
        case Tango::DEVVAR_ULONGARRAY:
        {
            vector<unsigned long> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_FLOATARRAY:
        {
            vector<float> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_DOUBLEARRAY:
        {
            vector<double> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        case Tango::DEVVAR_STRINGARRAY:
        {
            std::vector<std::string> temp;
            *data >> temp;
            da["value"] = temp;
            break;
        }
        default:
            d->error = true;
            d->message = "CuTangoWorld::extractData: type " + std::to_string(t) + " not supported";
            break;
        } // end switch(t)

        time_t tp;
        time(&tp);
        d->message = da["mode"].toString() + ": " + dateTimeToStr(&tp) + "[" + da["data_format_str"].toString() + "]";

    } catch (Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
    }
}

void CuTangoWorld::extractData(Tango::DeviceAttribute *p_da, CuData &dat)
{
    d->error = false;
    d->message = "";
    Tango::TimeVal tv = p_da->get_date();
    struct timeval tiv;
    const Tango::AttrQuality quality = p_da->get_quality();
    const Tango::AttrDataFormat f = p_da->get_data_format();
    const bool w = (p_da->get_nb_written() > 0);
    tiv.tv_sec = tv.tv_sec;
    tiv.tv_usec = tv.tv_usec;
    // set the message field: mode and timestamp, e.g.: "polled: mar 16 jun 2018..."
    d->message = dat["mode"].toString() + ": " + dateTimeToStr(&(tiv.tv_sec));
    putDateTime(tv, dat);

    CuDataQuality cuq = toCuQuality(quality);
    dat["quality"] = cuq.toInt();
    dat["quality_color"] = cuq.color();
    dat["quality_string"] = cuq.name();
    dat["data_format_str"] = formatToStr(f);

    try{
        if(quality == Tango::ATTR_INVALID)
        {
            d->message = "CuTangoWorld.extractData: attribute quality invalid";
            d->error = true;
        }
        else if(p_da->is_empty())
        {
            d->message = "CuTangoWorld.extractData: attribute " + p_da->get_name() + " is empty";
            d->error = true;
        }
        else if(f == Tango::IMAGE)
        {
            d->message = "CuTangoWorld.extractData: attribute " + p_da->get_name() + ": image format is unsupported";
            d->error = true;
        }
        else if(p_da->get_type() == Tango::DEV_DOUBLE)
        {
            std::vector<double> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_LONG)
        {
            std::vector<Tango::DevLong> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR && v.size())
                dat["value"] = static_cast<long int>(v.at(0));
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_LONG64)
        {
            std::vector<Tango::DevLong64> v;
            p_da->extract_read(v);

            if(f == Tango::SCALAR)
                dat["value"] = static_cast<long int>(v.at(0));
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_STATE)
        {
            CuTangoWorldConfig wc;
            if(f == Tango::SCALAR)
            {
                Tango::DevState state;
                *p_da >> state;
                dat["value"] = wc.stateString(state);
                dat["state"] = static_cast<long int>(state);
                dat["state_color"] = wc.stateColorName(state);
            }
            else
            {
                std::vector<Tango::DevState> v;
                std::vector<std::string> temp;
                std::vector<std::string> state_colors;
                std::vector<long int>tempi;
                p_da->extract_read(v);
                for(size_t i = 0; i < v.size(); i++)
                {
                    temp.push_back(wc.stateString(v.at(i)));
                    tempi.push_back(v[i]);
                    state_colors.push_back(wc.stateColorName(v.at(i)));
                }
                dat["value"] = temp;
                dat["state"] = tempi;
                dat["state_color"] = state_colors;
            }
            if(w)
            {
                if(f == Tango::SCALAR)
                {
                    Tango::DevState state;
                    *p_da >> state;
                    dat["w_value"] = wc.stateString(state);
                    dat["state_color"] = wc.stateColorName(state);
                    dat["w_state"] = static_cast<long int>(state);
                }
                else
                {
                    std::vector<Tango::DevState> v;
                    std::vector<long int>tempi;
                    std::vector<std::string> temp;
                    std::vector<std::string> state_colors;
                    p_da->extract_set(v);
                    for(size_t i = 0; i < v.size(); i++)
                    {
                        temp.push_back(wc.stateString(v.at(i)));
                        tempi.push_back(static_cast<long int>(v.at(i)));
                        state_colors.push_back(wc.stateColorName(v.at(i)));
                    }
                    dat["w_value"] = temp;
                    dat["w_state"] = temp;
                    dat["state_color"] = state_colors;
                }
            }
        }
        else if(p_da->get_type() == Tango::DEV_ULONG)
        {
            std::vector<Tango::DevULong> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = static_cast<unsigned long>(v.at(0));
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_ULONG64) {
            std::vector<Tango::DevULong64> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = static_cast<unsigned long>(v.at(0));
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_SHORT || p_da->get_type() == 100) /* 100! bug */
        {
            std::vector<short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_USHORT)
        {
            std::vector<unsigned short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_INT)
        {
            std::vector<int>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_FLOAT)
        {
            std::vector<float>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_BOOLEAN)
        {
            std::vector<bool> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
            {
                bool b = v.at(0);
                dat["value"] = b;
            }
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                {
                    bool bset = v.at(0);
                    dat["w_value"] = bset;
                }
                else
                    dat["w_value"] = v;
            }
        }
        else if(p_da->get_type() == Tango::DEV_STRING)
        {
            std::vector<std::string> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else
                dat["value"] = v;
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else
                    dat["w_value"] = v;
            }
        }
        else
        {
            d->message = "CuTangoWorld.extractData: unsupported data type " + std::to_string(p_da->get_type());
            d->error = true;
        }
    }
    catch(const Tango::DevFailed &e) { // read_attributes sets exception
        d->message = strerror(e);
        d->error = true;
    }
}

void CuTangoWorld::fillFromAttributeConfig(const Tango::AttributeInfoEx &ai, CuData &dat)
{
    d->error = false;
    d->message = "";
    dat["type"] = "property";
    dat["data_format"] = ai.data_format;
    dat["data_format_str"] = formatToStr(ai.data_format); /* as string */
    dat["data_type"] = ai.data_type;
    dat["description"] = ai.description;
    ai.display_unit != std::string("No display unit") ? dat["display_unit"] = ai.display_unit : dat["display_unit"] = "";
    dat["format"] = ai.format;
    dat["label"] = ai.label;
    dat["max_alarm"] = ai.max_alarm;
    dat["max_dim_x"] = ai.max_dim_x;
    dat["max_dim_y"] = ai.max_dim_y;
    dat["max"] = ai.max_value;
    dat["min"] = ai.min_value;
    dat["min_alarm"] = ai.min_alarm;
    dat["name"] = ai.name;
    dat["standard_unit"] = ai.standard_unit;
    dat["unit"] = ai.unit;
    dat["writable"] = ai.writable;
    dat["writable_attr_name"] = ai.writable_attr_name;
    dat["disp_level"] = ai.disp_level;
    dat["root_attr_name"] = ai.root_attr_name; // Root attribute name (in case of forwarded attribute)

    Tango::AttributeAlarmInfo aai = ai.alarms;
    dat["delta_t"] = aai.delta_t;
    dat["delta_val"] = aai.delta_val;
    dat["max_alarm"] = aai.max_alarm;
    dat["min_alarm"] = aai.min_alarm;
    dat["max_warning"] = aai.max_warning;
    dat["min_warning"] = aai.min_warning;

    Tango::AttributeEventInfo ei = ai.events;
    dat["archive_abs_change"] = ei.arch_event.archive_abs_change;
    dat["archive_period"] = ei.arch_event.archive_period;
    dat["archive_rel_change"] = ei.arch_event.archive_rel_change;

    dat["abs_change"] = ei.ch_event.abs_change;
    dat["rel_change"] = ei.ch_event.rel_change;

    dat["periodic_period"] = ei.per_event.period;

    // dim_x property contains the actual number of x elements
    long int dimx = dat["value"].getSize(); // if !contains value, empty variant, 0 dimx
    if(dimx > 0)
        dat["dim_x"] = dimx;
}

void CuTangoWorld::fillFromCommandInfo(const Tango::CommandInfo &ci, CuData &d)
{
    d["type"] = "property";
    d["cmd_name"] = ci.cmd_name;
    d["in_type"] = ci.in_type;
    d["out_type"] = ci.out_type;
    d["in_type_desc"] = ci.in_type_desc;
    d["out_type_desc"] = ci.out_type_desc;
    d["display_level"] = ci.disp_level;
    d["data_type"] = ci.out_type;

    /* fake data_format property for commands */
    switch(ci.out_type)
    {
    case Tango::DEV_BOOLEAN:
    case Tango::DEV_SHORT: case Tango::DEV_LONG: case Tango::DEV_FLOAT: case Tango::DEV_DOUBLE:
    case Tango::DEV_USHORT: case Tango::DEV_ULONG: case Tango::DEV_STRING: case Tango::DEV_STATE:
    case Tango::CONST_DEV_STRING: case Tango::DEV_UCHAR: case Tango::DEV_LONG64: case Tango::DEV_ULONG64:
    case Tango::DEV_INT:
        d["data_format"] = Tango::SCALAR;
        d["data_format_str"] = formatToStr(Tango::SCALAR); /* as string */
        break;
    case Tango::DEVVAR_STATEARRAY:
    case Tango::DEVVAR_LONG64ARRAY: case Tango::DEVVAR_ULONG64ARRAY:
    case Tango::DEVVAR_BOOLEANARRAY: case Tango::DEVVAR_CHARARRAY:
    case Tango::DEVVAR_SHORTARRAY: case Tango::DEVVAR_LONGARRAY:
    case Tango::DEVVAR_FLOATARRAY: case Tango::DEVVAR_DOUBLEARRAY:
    case Tango::DEVVAR_USHORTARRAY: case Tango::DEVVAR_ULONGARRAY:
    case Tango::DEVVAR_STRINGARRAY: case Tango::DEVVAR_LONGSTRINGARRAY:
    case Tango::DEVVAR_DOUBLESTRINGARRAY:
        d["data_format"] = Tango::SPECTRUM;
        d["data_format_str"] = formatToStr(Tango::SPECTRUM); /* as string */
        break;
    default:
        d["data_format"] = Tango::FMT_UNKNOWN;
        d["data_format_str"] = formatToStr(Tango::FMT_UNKNOWN); /* as string */
    };
}

bool CuTangoWorld::read_att(Tango::DeviceProxy *dev, const string &attribute, CuData &res)
{
    d->error = false;
    d->message = "";
    try
    {
        std::string att(attribute);
        Tango::DeviceAttribute da = dev->read_attribute(att);
        extractData(&da, res);
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
        res.putTimestamp();
        cuprintf("\e[1;31;31mCuTangoWorld.read_att: attribute ERRROR %s : %s\e[0m\n", attribute.c_str(),
                 d->message.c_str());
    }
    res["err"] = d->error;
    res["msg"] = d->message;
    res["success_color"] = d->t_world_conf.successColor(!d->error);
    return !d->error;
}

bool CuTangoWorld::read_atts(Tango::DeviceProxy *dev,
                             std::vector<CuData>& att_datalist,
                             std::vector<CuData> *reslist,
                             int results_offset)
{
    d->error = false;
    d->message = "";
    Tango::DeviceAttribute *p_da;
    std::vector<Tango::DeviceAttribute> *devattr = NULL;
    std::vector<std::string> attrs;
    for(size_t i = 0; i < att_datalist.size(); i++) // fill the list of attributes as string vector
        attrs.push_back(att_datalist[i]["point"].toString());
    try
    {
        // read_attributes
        // case 1. test/device/1 is running
        //         reading test/device/1/double_scalar and test/device/1/throw_exception
        //         returns two results, one is valid the other is invalid, as expected
        // case 2. device is down/goes down while reading
        //         we enter the catch clause, where the results have to be manually populated
        //         with data reporting the error.
        //         In that case, the poller must be slowed down
        devattr = dev->read_attributes(attrs);
        for(size_t i = 0; i < devattr->size(); i++) {
            (*reslist)[results_offset] = std::move(att_datalist[i]);
            p_da = &(*devattr)[i];
            p_da->set_exceptions(Tango::DeviceAttribute::failed_flag);
            extractData(p_da, (*reslist)[results_offset]);
            (*reslist)[results_offset]["err"] = d->error;
            (*reslist)[results_offset]["msg"] = d->message;
            (*reslist)[results_offset]["success_color"] = d->t_world_conf.successColor(!d->error);
            results_offset++;
        }
        delete devattr;
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
        for(size_t i = 0; i < attrs.size(); i++) {
            (*reslist)[results_offset] = std::move(att_datalist[i]);
            (*reslist)[results_offset]["err"] = d->error;
            (*reslist)[results_offset]["msg"] = d->message;
            (*reslist)[results_offset]["success_color"] = d->t_world_conf.successColor(!d->error);
            results_offset++;
        }
    }
    return !d->error;
}

// cmd_inout version 2: from the data argument, guess input DeviceData
// data must contain the output from a previous call to get_command_info
bool CuTangoWorld::cmd_inout(Tango::DeviceProxy *dev,
                             const std::string& cmd,
                             CuData& data)
{
    bool has_argout = data["out_type"].toLongInt() != Tango::DEV_VOID;
    Tango::DeviceData din = toDeviceData(data["argins"], data);
    return cmd_inout(dev, cmd, din, has_argout, data);
}

bool CuTangoWorld::cmd_inout(Tango::DeviceProxy *dev,
                             const std::string& cmd,
                             Tango::DeviceData& din,
                             bool has_argout,
                             CuData& data)
{
    d->error = false;
    d->message = "";
    try
    {
        std::string cmdnam(cmd);
        Tango::DeviceData dout;
        // prevent DeviceData::is_empty from throwing exception!
        din.reset_exceptions(Tango::DeviceData::isempty_flag);
        if(din.is_empty()) {
            dout = dev->command_inout(cmdnam);
        }
        else  {
            dout = dev->command_inout(cmdnam, din);
        }
        // if(point_info["out_type"].toLongInt() != Tango::DEV_VOID)
        if(has_argout)
            extractData(&dout, data);
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
    }
    // extractData does not set date/time information on data
    data.putTimestamp();
    data["err"] = d->error;
    data["msg"] = d->message;
    //  data["success_color"] = d->t_world_conf.successColor(!d->error);
    return !d->error;
}

bool CuTangoWorld::write_att(Tango::DeviceProxy *dev,
                             const string &attnam,
                             const CuVariant& argins,
                             const CuData& point_info,
                             CuData &data)
{
    assert(dev != NULL);
    try
    {
        Tango::DeviceAttribute da = toDeviceAttribute(attnam, argins, point_info);
        if(!d->error)
        {
            dev->write_attribute(da);
            d->message = "successfully written \"" + attnam + "\" on dev \"" + data["device"].toString() + "\"";
        }
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
    }
    return !d->error;
}

/** \brief Get Tango CommandInfo data and save it into a CuData.
 *
 * @param dev a pre allocated DeviceProxy
 * @param cmd the name of the command
 * @param cmd_info a non const reference to a CuData that will contain the Tango::CommandInfo fields.
 *         If an error occurs, the exception message is stored into the error parameter and
 *         cmd_info is left untouched.
 *
 * @return true if successful, false otherwise.
 */
bool CuTangoWorld::get_command_info(Tango::DeviceProxy *dev, const std::string& cmd, CuData &cmd_info)
{
    d->error = false;
    d->message = "";
    try
    {
        Tango::CommandInfo ci = dev->command_query(cmd);
        fillFromCommandInfo(ci, cmd_info);
    }
    catch(Tango::DevFailed &e)
    {
        d->message = strerror(e);
        d->error = true;
    }
    return !d->error;
}

bool CuTangoWorld::get_att_config(Tango::DeviceProxy *dev, const string &attribute, CuData &dres, bool skip_read_att)
{
    d->error = false;
    d->message = "";
    /* first read attribute to get the value */
    bool attr_read_ok = true;
    if(!skip_read_att)
        attr_read_ok = read_att(dev, attribute, dres);

    Tango::AttributeInfoEx aiex;
    if(attr_read_ok)
    {
        try
        {
            aiex = dev->get_attribute_config(attribute);
            fillFromAttributeConfig(aiex, dres);
            d->message = "successfully got configuration for " + dres["src"].toString();
            return true;
        }
        catch(Tango::DevFailed &e)
        {
            d->error = true;
            d->message = strerror(e);
        }
    }
    dres["success_color"] = d->t_world_conf.successColor(!d->error);
    return false;
}

bool CuTangoWorld::get_att_props(Tango::DeviceProxy *dev,
                                 const string &attribute_name,
                                 CuData &dres,
                                 const std::vector<string> props)
{
    d->error = false;
    d->message = "";
    Tango::Database db(dev->get_db_host(), dev->get_db_port_num());
    Tango::DbData db_data;
    db_data.push_back(Tango::DbDatum(attribute_name.c_str()));
    for(size_t j = 0; j < props.size(); j++)
    {
        std::vector<std::string> res;
        std::string prop = props.at(j);
        try
        {
            std::string device_name = dev->name();
            db.get_device_attribute_property(device_name, db_data);
            std::vector<std::string> vs;

            for(size_t i = 0; i < db_data.size(); i++)
            {
                long nb_prop;
                std::string &attname = db_data[i].name;
                db_data[i] >> nb_prop;
                i++;
                for(int k = 0; k < nb_prop; k++)
                {
                    std::string &prop_name = db_data[i].name;
                    if(strcasecmp(attribute_name.c_str(), attname.c_str()) == 0 &&
                            strcasecmp(prop_name.c_str(), prop.c_str()) == 0 && !db_data[i].is_empty())
                    {
                        db_data[i] >> vs;
                    }
                    i++;
                }
            }

            if(vs.size() > 0) {
                for(size_t i = 0; i < vs.size(); i++)
                    res.push_back(vs[i]);
                dres[prop] = res;
                std::vector<std::string> pp = dres["properties"].toStringVector();
                pp.push_back(prop);
                dres["properties"] = pp;
            }
        }
        catch (Tango::DevFailed &e)
        {
            d->error = true;
            d->message = strerror(e);
        }
    }
    dres["success_color"] = d->t_world_conf.successColor(!d->error);
    return !d->error;
}

Tango::Database *CuTangoWorld::getTangoDb(const std::string& dbhost) const
{
    std::string db = dbhost;
    if(dbhost.size() == 0)
        return new Tango::Database();
    else
        return new Tango::Database(db);
}

/**
 * @brief CuTangoWorld::get_properties
 * @param in_list list of properties to fetch, described with a CuData bundle
 * @param res the result of the database query
 * @return true if no errors occurred.
 *
 * \par Results
 * Results are stored into the res CuData passed as reference.
 *
 * \li device attribute properties:  res["the/device/name/attname/property_name"] = property as std vector of strings
 * \li device properties: res["the/device/name/property_name"] = property as std vector of strings
 * \li class properties: res["classname/property_name"] = property as std vector of strings
 *
 */
bool CuTangoWorld::get_properties(const std::vector<CuData> &in_list, CuData &res, const std::string& dbhost)
{
    // maps tango host name to device attribute props, device props, class attribute props
    // and class props
    std::unordered_map<std::string, std::list<CuData> > daprops, dprops, cprops;
    std::vector<std::string> names;
    d->error = false;

    // divide in_list by type and by device name
    for(std::vector<CuData>::const_iterator it = in_list.begin(); it != in_list.end(); ++it)
    {
        const CuData& in = *it;

        if(in.containsKey("device") && in.containsKey("attribute"))
            daprops[in["device"].toString()].push_back(in);
        else if(in.containsKey("device") && !in.containsKey("attribute")) // device property
            dprops[in["device"].toString()].push_back(in);
        else if(in.containsKey("class")) // class property
            cprops[in["class"].toString()].push_back(in);
    }
    Tango::Database *db = getTangoDb(dbhost);
    d->error = false;
    d->message = "";
    std::string attnam;
    std::vector<std::string> vs;

    try {
        // 1. device attribute properties
        // scan the per-device map
        for(std::unordered_map<std::string, std::list< CuData> >::const_iterator it = daprops.begin(); it != daprops.end(); ++it)
        {
            Tango::DbData db_data;
            std::vector<std::string> req_a, req_p; // requested attributes, requested properties
            for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit) {
                const std::string &attname = (*dit)["attribute"].toString();
                if(find(req_a.begin(), req_a.end(), attname) == req_a.end()) {
                    db_data.push_back(Tango::DbDatum(attname.c_str()));
                    req_a.push_back(attname);
                    if((*dit).containsKey("name"))
                        req_p.push_back((*dit)["name"].toString());
                }
            }

            db->get_device_attribute_property(it->first, db_data);
            for(size_t i = 0; i < db_data.size(); i++) {
                if(i == 0)
                    attnam = db_data[i].name;
                else if(!db_data[i].is_empty() && (req_p.size() == 0 || find(req_p.begin(), req_p.end(), db_data[i].name) != req_p.end()))
                {
                    db_data[i] >> vs;
                    names.push_back(it->first + "/" + attnam + ":" + db_data[i].name);
                    res[names.at(names.size() - 1)] = vs;
                }
            }
        }
        // 2. device properties
        // scan the per-device map
        for(std::unordered_map<std::string, std::list< CuData> >::const_iterator it = dprops.begin(); it != dprops.end(); ++it)
        {
            Tango::DbData db_data;
            for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit)
                db_data.push_back(Tango::DbDatum((*dit)["name"].toString().c_str()));

            db->get_device_property(it->first, db_data);
            for(size_t i = 0; i < db_data.size(); i++) {
                if(!db_data[i].is_empty()) {
                    db_data[i] >> vs;
                    names.push_back(it->first + ":" + db_data[i].name);
                    res[names.at(names.size() - 1)] = vs;
                }
            }
        }
        // 3. class properties
        // scan the per-class map
        for(std::unordered_map<std::string, std::list< CuData> >::const_iterator it = cprops.begin(); it != cprops.end(); ++it)
        {
            Tango::DbData db_data;
            for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit)
                db_data.push_back(Tango::DbDatum((*dit)["name"].toString().c_str()));

            db->get_class_property(it->first, db_data); // it->first is class name
            for(size_t i = 0; i < db_data.size(); i++) {
                if(!db_data[i].is_empty()) {
                    db_data[i] >> vs;
                    names.push_back(it->first + ":" + db_data[i].name);
                    res[names.at(names.size() -1)] = vs;
                }
            }
        }
        res["list"] = names;
    }
    catch(Tango::DevFailed& e)
    {
        d->error = true;
        d->message = strerror(e);
    }

    res.putTimestamp();
    res["err"] = d->error;
    if(!d->error) {
        d->message = std::string("CuTangoWorld.get_properties successfully completed on " +  dateTimeToStr(NULL));
    }
    res["msg"] = d->message;

    return !d->error;
}

bool CuTangoWorld::source_valid(const string &src)
{
    //  (tango://){0,1}([A-Za-z_0-9\._\-]*[:]{1}[0-9]+[/]){0,1}(([A-Za-z_0-9_\-\.]+/[A-Za-z_0-9_\-\.]+/[A-Za-z_0-9_\-\.]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9_\-\.]+)([\(]{1}[&A-Za-z_0-9_\-\\.,]+[\)]){0,1})    const char* SOURCE_REGEXP = "(tango://){0,1}"
    std::regex re = std::regex("(tango://){0,1}([A-Za-z_0-9_\\-\\.]*[:]{1}[0-9]+[/]){0,1}(([A-Za-z_0-9_\\-\\.]+/[A-Za-z_0-9_\\-\\.]+/[A-Za-z_0-9_\\-\\.]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9_\\-\\.]+)([\\(]{1}[&A-Za-z_0-9_\\-\\\\.,]+[\\)]){0,1})");
    std::smatch m;
    return std::regex_match(src, m, re);
}

string CuTangoWorld::getLastMessage() const
{
    return d->message;
}

/** \brief returns a string representation of the attribute data format to be used in the
 *         CuData "data_format_str" property.
 *
 * Valid return values are scalar, vector and matrix
 */
string CuTangoWorld::formatToStr(Tango::AttrDataFormat f) const
{
    switch(f)
    {
    case Tango::SCALAR:
        return "scalar";
    case Tango::SPECTRUM:
        return "vector";
    case Tango::IMAGE:
        return "matrix";
    default:
        return "data format unknown";
    }
}

/** \brief Returns a string representation of the Tango::CmdArgType
 *
 * valid values for compatibility: scalar, vector
 *
 * CuData "data_format_str" property
 */
string CuTangoWorld::cmdArgTypeToDataFormat(Tango::CmdArgType t) const
{
    switch (t)
    {
    case Tango::DEV_STATE: case Tango::DEV_BOOLEAN: case Tango::DEV_SHORT: case Tango::DEV_USHORT:
    case Tango::DEV_LONG: case Tango::DEV_ULONG: case Tango::DEV_FLOAT:case Tango::DEV_DOUBLE:
    case Tango::DEV_STRING:
        return "scalar";

    case Tango::DEVVAR_BOOLEANARRAY: case Tango::DEVVAR_SHORTARRAY: case Tango::DEVVAR_USHORTARRAY:
    case Tango::DEVVAR_LONGARRAY: case Tango::DEVVAR_ULONGARRAY: case Tango::DEVVAR_FLOATARRAY:
    case Tango::DEVVAR_DOUBLEARRAY: case Tango::DEVVAR_STRINGARRAY:
        return "vector";

    case Tango::DEV_VOID:
        return "void";

    default:
        return "data format type unknown";
    }
}

bool CuTangoWorld::error() const
{
    return d->error;
}

void CuTangoWorld::setSrcPatterns(const std::vector<string> &pat_regex)
{
    d->src_patterns = pat_regex;
}

std::vector<string> CuTangoWorld::srcPatterns() const
{
    return d->src_patterns;
}

Tango::DeviceData CuTangoWorld::toDeviceData(const CuVariant &arg,
                                             const CuData &info)
{
    bool type_match = false;
    d->error = false;
    d->message = "";
    long in_type = info["in_type"].toLongInt();
    Tango::DeviceData dd;
    if((arg.isNull() || arg.getFormat() < 0) && in_type == static_cast<Tango::CmdArgType>(Tango::DEV_VOID))
    {
        type_match = true;
    }
    else if(arg.getFormat() == CuVariant::Scalar)
    {
        if(in_type == Tango::DEV_BOOLEAN && arg.getType() == CuVariant::Boolean)
            dd << (bool) arg.toBool();
        else if(in_type == Tango::DEV_SHORT  && arg.getType() == CuVariant::Short)
            dd << (Tango::DevShort) arg.toShortInt();
        else if(in_type == Tango::DEV_USHORT  && arg.getType() == CuVariant::UShort)
            dd << arg.toUShortInt();
        else if(in_type == Tango::DEV_LONG && arg.getType() == CuVariant::LongInt)
            dd << (Tango::DevLong) arg.toLongInt();
        else if(in_type == Tango::DEV_LONG64 && arg.getType() == CuVariant::LongInt)
            dd << (Tango::DevLong64) arg.toLongInt();
        else if(in_type == Tango::DEV_ULONG64 && arg.getType() == CuVariant::LongUInt)
            dd << (Tango::DevULong64) arg.toULongInt();
        else if(in_type == Tango::DEV_ULONG && arg.getType() == CuVariant::LongUInt)
            dd << (Tango::DevULong) arg.toULongInt();
        else if(in_type == Tango::DEV_FLOAT  && arg.getType() == CuVariant::Float)
            dd << arg.toFloat();
        else if(in_type == Tango::DEV_DOUBLE  && arg.getType() == CuVariant::Double)
            dd << arg.toDouble();
        else if(in_type == Tango::DEV_STATE  && arg.getType() == CuVariant::LongInt)
            dd << static_cast<Tango::DevState>( arg.toLongInt() );
        else if(in_type == Tango::DEV_STRING  && arg.getType() == CuVariant::String)
        {
            std::string s = arg.toString();
            dd << s;
        }
    }
    else if(arg.getFormat() == CuVariant::Vector)
    {
        /*
         * Tango::DEVVAR_SHORTARRAY Tango::DEVVAR_USHORTARRAY)
         Tango::DEVVAR_LONGARRAY  Tango::DEVVAR_ULONGARRAY) ango::DEVVAR_FLOATARRAY)
          Tango::DEVVAR_DOUBLEARRAY) Tango::DEVVAR_STRINGARRAY)
         */
        if(in_type == Tango::DEVVAR_BOOLEANARRAY  && arg.getType() == CuVariant::Boolean)
        {
            perr("CuTangoWorld::toDeviceData boolean array conversion not supported in device data");
        }
        else if(in_type == Tango::DEVVAR_SHORTARRAY  && arg.getType() == CuVariant::Short)
        {
            std::vector<short> v = arg.toShortVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_USHORTARRAY  && arg.getType() == CuVariant::UShort)
        {
            std::vector<unsigned short> v = arg.toUShortVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_LONGARRAY  && arg.getType() == CuVariant::LongInt)
        {
            std::vector<long int> v = arg.toLongIntVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_ULONGARRAY  && arg.getType() == CuVariant::LongUInt)
        {
            std::vector<unsigned long> v = arg.toULongIntVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_FLOATARRAY  && arg.getType() == CuVariant::Float)
        {
            std::vector<float> v = arg.toFloatVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_DOUBLEARRAY  && arg.getType() == CuVariant::Double)
        {
            std::vector<double> v = arg.toDoubleVector();
            dd << v;
        }
        else if(in_type == Tango::DEVVAR_STRINGARRAY  && arg.getType() == CuVariant::String)
        {
            std::vector<std::string> v = arg.toStringVector();
            dd << v;
        }
        else
            type_match = false;
    }
    if(!type_match)
    {
        /* no match between CommandInfo argin type and CuVariant type: try to get CuVariant
         * data as string and convert it according to CommandInfo type
         */
        return toDeviceData(arg.toStringVector(), info);
    }
    return dd;
}

Tango::DeviceData CuTangoWorld::toDeviceData(const std::vector<std::string> &argins,
                                             const CuData &cmdinfo)
{
    d->error = false;
    d->message = "";
    long in_type = cmdinfo["in_type"].toLongInt();
    Tango::DeviceData dd;
    if(argins.size() == 0) {
        return dd;
    }

    std::string v = argins[0];
    try
    {
        if(argins.size() == 1)
        {
            switch (in_type)
            {
            case Tango::DEV_BOOLEAN:
                dd << (bool) std::stoi(v);
                break;
            case Tango::DEV_SHORT:
                dd << (short) std::stoi(v);
                break;
            case Tango::DEV_USHORT:
                dd << (unsigned short) std::stoul(v);
                break;
            case Tango::DEV_LONG:
                dd << (Tango::DevLong) std::stol(v);
                break;
            case Tango::DEV_STATE:
                dd << static_cast<Tango::DevState> (std::stol(v));
                break;
            case Tango::DEV_ULONG:
                dd << (Tango::DevULong) std::stoul(v);
                break;
            case Tango::DEV_LONG64:
                dd << (Tango::DevLong64) std::stol(v);
                break;
            case Tango::DEV_ULONG64:
                dd << (Tango::DevULong64) std::stoul(v);
                break;
            case Tango::DEV_FLOAT:
                dd << (Tango::DevFloat) std::stof(v);
                break;
            case Tango::DEV_DOUBLE:
                dd << (Tango::DevDouble) std::stod(v);
                break;
            case Tango::DEV_STRING:
            case Tango::CONST_DEV_STRING:
                dd << v;
                break;
            case Tango::DEV_VOID:
                break;
            default:
                perr("CuTangoWorld::toDeviceData: in_type %ld not supported by the library", in_type);
                break;
            }
        }
        else
        {

            if(in_type == Tango::DEVVAR_BOOLEANARRAY)
            {
                // std::vector<bool> bv = v.toBoolVector();
                // dd << bv;
            }
            else if (in_type == Tango::DEVVAR_SHORTARRAY)
            {
                std::vector<short> sv;
                for(size_t i = 0; i < argins.size(); i++)
                    sv.push_back((Tango::DevShort) std::stoi(argins[i]));
                dd << sv;
            }
            else if (in_type ==  Tango::DEVVAR_USHORTARRAY)
            {
                std::vector<unsigned short> usv;
                for(size_t i = 0; i < argins.size(); i++)
                    usv.push_back((Tango::DevUShort) std::stoi(argins[i]));
                dd << usv;
            }
            else if (in_type ==  Tango::DEVVAR_LONGARRAY)
            {
                std::vector<Tango::DevLong> lv;
                for(size_t i = 0; i < argins.size(); i++)
                    lv.push_back((Tango::DevLong) std::stol(argins[i]));
                dd << lv;

            }
            else if (in_type ==  Tango::DEVVAR_ULONGARRAY)
            {
                std::vector<Tango::DevLong> ulv;
                for(size_t i = 0; i < argins.size(); i++)
                    ulv.push_back((Tango::DevULong) std::stoul(argins[i]));
                dd << ulv;
            }
            else if (in_type ==  Tango::DEVVAR_FLOATARRAY)
            {
                std::vector<Tango::DevFloat> fv;
                for(size_t i = 0; i < argins.size(); i++)
                    fv.push_back((Tango::DevFloat) std::stof(argins[i]));
                dd << fv;
            }
            else if (in_type ==  Tango::DEVVAR_DOUBLEARRAY)
            {
                std::vector<double> dv;
                for(size_t i = 0; i < argins.size(); i++)
                    dv.push_back((Tango::DevDouble) std::stod(argins[i]));
                dd << dv;
            }
            else if (in_type ==  Tango::DEVVAR_STRINGARRAY)
            {
                std::vector<std::string> nonconstv = argins;
                dd << nonconstv;
            }
            else
            {
                perr("CuTangoWorld::toDeviceData: in_type %ld not supported by the library", in_type);
            }


        } /* if */
    }
    catch(const std::invalid_argument& ia)
    {
        d->error = true;
        d->message = "CuTangoWorld.toDeviceData: cannot convert argins \"" + v + "\" to type " +
                std::to_string(in_type) + ": invalid argument: " + ia.what();
        perr("%s", d->message.c_str());
    }
    catch(const std::out_of_range& ore)
    {
        d->error = true;
        d->message = "CuTangoWorld.toDeviceData: cannot convert argins \"" + v + "\" to type " +
                std::to_string(in_type) + ": invalid argument: " + ore.what();
        perr("%s", d->message.c_str());
    }
    return dd;
}

Tango::DeviceAttribute CuTangoWorld::toDeviceAttribute(const string &name,
                                                       const CuVariant& arg,
                                                       const CuData &attinfo)
{
    d->error = false;
    d->message = "";
    bool ok;
    std::string attname = name;
    Tango::DeviceAttribute da;
    CuVariant::DataType t = arg.getType();
    int tango_type = attinfo["data_type"].toInt();
    Tango::AttrDataFormat tango_format = static_cast<Tango::AttrDataFormat>(attinfo["data_format"].toInt());
    if(tango_format == Tango::SCALAR && arg.getFormat() == CuVariant::Scalar)
    {
        if(t == CuVariant::Double && tango_type == Tango::DEV_DOUBLE)
        {
            double dou = arg.toDouble(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, dou);
        }
        else if(t == CuVariant::Float && tango_type == Tango::DEV_FLOAT)
        {
            Tango::DevFloat l = arg.toFloat(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::Short && tango_type == Tango::DEV_SHORT)
        {
            Tango::DevShort l = arg.toShortInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::UShort && tango_type == Tango::DEV_USHORT)
        {
            Tango::DevUShort l = arg.toUShortInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::LongInt && tango_type == Tango::DEV_LONG)
        {
            Tango::DevLong l = arg.toLongInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::LongInt && tango_type == Tango::DEV_LONG64)
        {
            Tango::DevLong64 l = arg.toLongInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::LongUInt && tango_type == Tango::DEV_ULONG)
        {
            Tango::DevULong l = arg.toULongInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::LongUInt && tango_type == Tango::DEV_ULONG64)
        {
            Tango::DevULong64 l = arg.toULongInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, l);
        }
        else if(t == CuVariant::Boolean && tango_type == Tango::DEV_BOOLEAN)
        {
            Tango::DevBoolean b = (bool) arg.toBool(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, b);
        }
        else if(t == CuVariant::String && tango_type == Tango::DEV_STRING)
        {
            std::string s = arg.toString(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, s);
        }
        else if(t == CuVariant::LongInt && tango_type == Tango::DEV_STATE)
        {
            long l = arg.toLongInt(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, static_cast<Tango::DevState>(l));
        }
        else
        {
            std::vector<std::string> vs1el;
            vs1el.push_back(arg.toString());
            return toDeviceAttribute(attname, vs1el, attinfo);
        }
    }
    else if(tango_format == Tango::SPECTRUM && arg.getFormat() == CuVariant::Vector)
    {
        ok = true; // force for now
        if(t == CuVariant::Double && tango_type == Tango::DEV_DOUBLE)
        {
            std::vector<double> dou = arg.toDoubleVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, dou);
        }
        else if(t == CuVariant::Float && tango_type == Tango::DEV_FLOAT)
        {
            std::vector<float> vf = arg.toFloatVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, vf);
        }
        else if(t == CuVariant::Short && tango_type == Tango::DEV_SHORT)
        {
            std::vector<short> sv = arg.toShortVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, sv);
        }
        else if(t == CuVariant::UShort && tango_type == Tango::DEV_USHORT)
        {
            std::vector<unsigned short> usv = arg.toUShortVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, usv);
        }
        else if(t == CuVariant::LongInt && tango_type == Tango::DEV_LONG)
        {
            std::vector<long> lov = arg.toLongIntVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, lov);
        }
        else if(t == CuVariant::LongInt && tango_type == Tango::DEV_LONG64)
        {
            std::vector<long> lov = arg.toLongIntVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, lov);
        }
        else if(t == CuVariant::LongUInt && tango_type == Tango::DEV_ULONG)
        {
            std::vector<unsigned long> luv = arg.toULongIntVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, luv);
        }
        else if(t == CuVariant::LongUInt && tango_type == Tango::DEV_ULONG64)
        {
            std::vector<unsigned long> luv = arg.toULongIntVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, luv);
        }
        else if(t == CuVariant::Boolean && tango_type == Tango::DEV_BOOLEAN)
        {
            std::vector<bool> bv = arg.toBoolVector();
            if(ok)
                da = Tango::DeviceAttribute(attname, bv);
        }
        else if(t == CuVariant::String && tango_type == Tango::DEV_STRING)
        {
            std::vector<std::string> vs = arg.toStringVector(&ok);
            if(ok)
                da = Tango::DeviceAttribute(attname, vs);
        }
        else // try conversion to string vector
        {
            /* no match between AttributeInfo data type and CuVariant type: try to get CuVariant
             * data as string and convert it according to AttributeInfo data type
             */
            return toDeviceAttribute(attname, arg.toStringVector(), attinfo);
        }
    }
    else if(tango_format == Tango::SPECTRUM && arg.getFormat() == CuVariant::Scalar)
    {
        CuVariant vvar(arg);
        vvar.toVector();
        return toDeviceAttribute(name, vvar, attinfo);
    }

    d->error = !ok;
    if(d->error)
        d->message = "CuTangoWorld::fillDeviceAttribute: data type " + std::to_string(t) + " cannot be converted";
    return da;
}

Tango::DeviceAttribute CuTangoWorld::toDeviceAttribute(const string &aname,
                                                       const std::vector<std::string> &argins,
                                                       const CuData &attinfo)
{
    d->error = false;
    d->message = "";
    int tango_type = attinfo["data_type"].toLongInt();
    Tango::AttrDataFormat tango_fmt = static_cast<Tango::AttrDataFormat>(attinfo["data_format"].toInt());
    Tango::DeviceAttribute da;
    std::string name(aname);
    if(argins.size() == 0)
    {
        d->error = true;
        d->message = "CuTangoWorld.toDeviceAttribute: empty argument";
        return da;
    }

    std::string v = argins[0];
    try{
        if(argins.size() == 1 && tango_fmt == Tango::SCALAR)
        {
            switch (tango_type)
            {
            case Tango::DEV_BOOLEAN:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevBoolean>(std::stoi(v)));
                break;
            case Tango::DEV_SHORT:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevShort>(std::stoi(v)));
                break;
            case Tango::DEV_USHORT:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevUShort>(std::stoul(v)));
                break;
            case Tango::DEV_LONG:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevLong>(std::stol(v)));
                break;
            case Tango::DEV_ULONG:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevULong>(std::stoul(v)));
                break;
            case Tango::DEV_LONG64:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevLong64>(std::stol(v)));
                break;
            case Tango::DEV_ULONG64:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevULong64>(std::stoul(v)));
                break;
            case Tango::DEV_FLOAT:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevFloat>(std::stof(v)));
                break;
            case Tango::DEV_DOUBLE:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevDouble>(std::stod(v)));
                break;
            case Tango::DEV_STATE:
                da = Tango::DeviceAttribute(name, static_cast<Tango::DevState>(std::stol(v)));
                break;
            case Tango::DEV_STRING:
            case Tango::CONST_DEV_STRING:
                da = Tango::DeviceAttribute(name, v);
                break;
            default:
                perr("CuTangoWorld::toDeviceAttribute: in_type %d not supported by the library", tango_type);
                break;
            }
        }
        else if(tango_fmt == Tango::SPECTRUM)
        {

            if(tango_type == Tango::DEV_BOOLEAN)
            {
                std::vector<bool> bv;
                for(size_t i = 0; i < argins.size(); i++)
                    bv.push_back(static_cast<bool> (std::stoi(argins[i])));
                da = Tango::DeviceAttribute(name, bv);
            }
            else if (tango_type == Tango::DEV_SHORT)
            {
                std::vector<short> sv;
                for(size_t i = 0; i < argins.size(); i++)
                    sv.push_back(static_cast<short>(std::stoi(argins[i])));
                da = Tango::DeviceAttribute(name, sv);
            }
            else if (tango_type ==  Tango::DEV_USHORT)
            {
                std::vector<unsigned short> usv;
                for(size_t i = 0; i < argins.size(); i++)
                    usv.push_back(static_cast<unsigned short>(std::stoul(argins[i])));
                da = Tango::DeviceAttribute(name, usv);
            }
            else if (tango_type ==  Tango::DEV_LONG)
            {
                std::vector<Tango::DevLong> lv;
                for(size_t i = 0; i < argins.size(); i++)
                    lv.push_back(static_cast<Tango::DevLong>(std::stol(argins[i])));
                da = Tango::DeviceAttribute(name, lv);
            }
            else if (tango_type ==  Tango::DEV_LONG64)
            {
                std::vector<Tango::DevLong64> lv;
                for(size_t i = 0; i < argins.size(); i++)
                    lv.push_back(static_cast<Tango::DevLong64>(std::stol(argins[i])));
                da = Tango::DeviceAttribute(name, lv);
            }
            else if (tango_type ==  Tango::DEV_ULONG)
            {
                std::vector<Tango::DevULong> ulv;
                for(size_t i = 0; i < argins.size(); i++)
                    ulv.push_back(static_cast<Tango::DevULong>(stoul(argins[i])));
                da = Tango::DeviceAttribute(name, ulv);
            }
            else if (tango_type ==  Tango::DEV_ULONG64)
            {
                std::vector<Tango::DevULong64> ulv;
                for(size_t i = 0; i < argins.size(); i++)
                    ulv.push_back(static_cast<Tango::DevULong64>(stoul(argins[i])));
                da = Tango::DeviceAttribute(name, ulv);
            }
            else if (tango_type ==  Tango::DEV_FLOAT)
            {
                std::vector<Tango::DevFloat> fv;
                for(size_t i = 0; i < argins.size(); i++)
                    fv.push_back(static_cast<Tango::DevFloat> (std::stof(argins[i])));
                da = Tango::DeviceAttribute(name, fv);
            }
            else if (tango_type ==  Tango::DEV_DOUBLE)
            {
                std::vector<double> dv;
                for(size_t i = 0; i < argins.size(); i++)
                    dv.push_back(static_cast<double> (std::stod(argins[i])));
                da = Tango::DeviceAttribute(name, dv);
            }
            else if (tango_type ==  Tango::DEV_STRING)
            {
                std::vector<std::string> nonconstv = argins;
                da = Tango::DeviceAttribute(name, nonconstv);
            }
            else
            {
                perr("CuTangoWorld::toDeviceAttribute: in_type %d not supported by the library", tango_type);
                d->error = true;
                d->message = std::string("CuTangoWorld::toDeviceData: in_type "
                                         + std::to_string(tango_type) + " not supported by the library");
            }

        } /* if */
    }
    catch(const std::invalid_argument& ia)
    {
        d->error = true;
        d->message = "CuTangoWorld.toDeviceAttribute: cannot convert argins to type " +
                std::to_string(tango_type) + ": invalid argument: " + ia.what();
        perr("%s", d->message.c_str());
    }
    catch(const std::out_of_range& ore)
    {
        d->error = true;
        d->message = "CuTangoWorld.toDeviceAttribute: cannot convert argins to type " +
                std::to_string(tango_type) + ": invalid argument: " + ore.what();
        perr("%s", d->message.c_str());
    }

    return da;
}

void CuTangoWorld::putDateTime(const Tango::TimeVal &ttv, CuData &data)
{
    struct timeval tv;
    tv.tv_sec = ttv.tv_sec;    // _must_ copy from Tango::TimeVal to struct timeval!
    tv.tv_usec = ttv.tv_usec;
    data["timestamp_ms"] = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    data["timestamp_us"] = static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6;
}

/** \brief if the time_t parameter is not null, use it to format the date/time into a std:string,
 *         otherwise get system time and use the current date/time
 *
 * @param tp if not null, it is used to provide a string representation of the local time, otherwise
 *        get the system date and time
 *
 * @return a string with the date and time. strftime with "%c" format is used. From *man strftime*:
 *         %c     The preferred date and time representation for the current locale
 *
 */
std::string CuTangoWorld::dateTimeToStr(time_t *tp) const
{
    char t[128];
    struct tm* tmp;
    time_t tit;
    tp != NULL ? tit = *tp : tit  = time(NULL);
    tmp = localtime(&tit);
    if(tmp != NULL)
        strftime(t, sizeof(t), "%c", tmp);
    return std::string(t);
}

//   enum AttrQuality { ATTR_VALID, ATTR_INVALID, ATTR_ALARM,
//   ATTR_CHANGING, ATTR_WARNING /*, __max_AttrQuality=0xffffffff */ };
/**
 * @brief CuTangoWorld::toCuQuality
 * @param q the input Tango::AttrQuality enum value
 * @return The *engine independent* data quality
 *
 * @see CuDataQuality
 */
CuDataQuality CuTangoWorld::toCuQuality(Tango::AttrQuality q) const
{
    switch(q) {
    case Tango::ATTR_VALID:
        return CuDataQuality(CuDataQuality::Valid);
    case Tango::ATTR_ALARM:
        return CuDataQuality(CuDataQuality::Alarm);
    case Tango::ATTR_CHANGING:
        return CuDataQuality(CuDataQuality::Changing);
    case Tango::ATTR_WARNING:
        return CuDataQuality(CuDataQuality::Warning);
    case Tango::ATTR_INVALID:
    default:
        return CuDataQuality(CuDataQuality::Invalid);
    }
}


