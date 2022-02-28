#include "cutango-world.h"
#include "cutango-world-config.h"
#include "tsource.h"
#include <unordered_map>
#include <cumacros.h>
#include <chrono>
#include <regex>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


class CuTangoWorldPrivate
{
public:
    bool error;
    std::string message;
    CuTangoWorldConfig t_world_conf;
    // this is used in source_valid
    //  [A-Za-z_0-9_\-\.\,\*/\+\:\(\)>#{}]+
    char src_valid_pattern[128] = "[A-Za-z_0-9_\\-\\.\\,\\*/\\+\\:&\\(\\)>#{}]+";
    std::vector <std::string> src_patterns;
};

CuTangoWorld::CuTangoWorld()
{
    d = new CuTangoWorldPrivate();
    d->src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+/.+");
    d->src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+->.+");

    // support class property syntax, such as: hokuto:20000/TangoTest(ProjectTitle,Description),
    // hokuto:20000/TangoTest(*)
    d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\]+(?:[\\(A-Za-z0-9_\\-,\\)\\s*]+)");

    // free properties and device exported
    // tango://ken:20000/#Sequencer#TestList
    // ken:20000/#test/de*/*(*)
    d->src_patterns.push_back("(?:tango://){1,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\-\\.\\$#\\*/]+(?:\\(\\*\\)){0,1}");
    d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){1,1}[A-Za-z0-9_\\-\\.\\$#\\*/]+(?:\\(\\*\\)){0,1}");
    // tango domain search [tango://]hokuto:20000/ or [tango://]hokuto:20000/*
    d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){1}[*]{0,1}");

    // support tango://host:PORT/a/b/c/d and tango://host:PORT/a/b/c->e
    // when CumbiaPool.guessBySrc needs to be used
    // with the two above only, sources like "tango://hokuto:20000/test/device/1/double_scalar"
    // or "hokuto:20000/test/device/1/double_scalar" are not identified
    d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+/.+");
    d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+->.+");
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
    da["dfs"] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>(t));
    da["dt"] = t;

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
            da["s"] = static_cast<long int>(temp);
            da["sc"] = wc.stateColorName(temp);
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

    } catch (Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
    }
}

// extract data from DeviceAttribute to CuData
// NOTE: d->message shall be empty if d->error remains false
//
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
    putDateTime(tv, dat);

    CuDataQuality cuq = toCuQuality(quality);
    dat["q"] = cuq.toInt();
    dat["qc"] = cuq.color();
    dat["qs"] = cuq.name();
    dat["dfs"] = formatToStr(f);

    try{
        if(quality == Tango::ATTR_INVALID)
        {
            d->message = "CuTangoWorld.extractData: attribute quality invalid - ";
            d->message += strerror(p_da->get_err_stack());
            d->error = true;
        }
        else if(p_da->is_empty())
        {
            d->message = "CuTangoWorld.extractData: attribute " + p_da->get_name() + " is empty";
            d->error = true;
        }
        else if(p_da->get_type() == Tango::DEV_DOUBLE)
        {
            std::vector<double> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if(f == Tango::IMAGE)
                dat.set("value", CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y()));
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if(f == Tango::IMAGE)
                    dat.set("w_value", CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y()));
            }
        }
        else if(p_da->get_type() == Tango::DEV_LONG)
        {
            std::vector<Tango::DevLong> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR && v.size())
                dat["value"] = static_cast<long int>(v.at(0));
            else if(f == Tango::SPECTRUM || f == Tango::IMAGE)  {
                std::vector<long int> vlo;
                for(size_t i = 0; i < v.size(); i++)
                    vlo.push_back(static_cast<long int>(v.at(i)));
                if(f == Tango::SPECTRUM)
                    dat["value"] = vlo;
                else if(f == Tango::IMAGE)
                    dat["value"] = CuVariant(vlo, p_da->get_dim_x(), p_da->get_dim_y());
            }
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else {
                    std::vector<long int> vlo;
                    for(size_t i = 0; i < v.size(); i++)
                        vlo.push_back(static_cast<long int>(v.at(i)));
                    if(f == Tango::SPECTRUM)
                        dat["w_value"] = vlo;
                    else if(f == Tango::IMAGE)
                        dat["w_value"] = CuVariant(vlo, p_da->get_written_dim_x(), p_da->get_written_dim_y());
                }
            }
        }
        else if(p_da->get_type() == Tango::DEV_LONG64)
        {
            std::vector<Tango::DevLong64> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = static_cast<long int>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if(f == Tango::IMAGE)
                dat.set("value", CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y()));
            if(w) {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if(f == Tango::IMAGE)
                    dat.set("w_value", CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y()));
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
                dat["s"] = static_cast<long int>(state);
                dat["sc"] = wc.stateColorName(state);
            }
            else if(f == Tango::SPECTRUM || f == Tango::IMAGE)
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
                if(f == Tango::SPECTRUM) {
                    dat["value"] = temp;
                    dat["s"] = tempi;
                    dat["sc"] = state_colors;
                } else {
                    dat["value"] = CuVariant(temp, p_da->get_dim_x(), p_da->get_dim_y());
                    dat["s"] = CuVariant(tempi, p_da->get_dim_x(), p_da->get_dim_y());
                    dat["sc"] = CuVariant(state_colors, p_da->get_dim_x(), p_da->get_dim_y());
                }
            }
            if(w)
            {
                if(f == Tango::SCALAR)
                {
                    Tango::DevState state;
                    *p_da >> state;
                    dat["w_value"] = wc.stateString(state);
                    dat["sc"] = wc.stateColorName(state);
                    dat["w_state"] = static_cast<long int>(state);
                }
                else if(f == Tango::SPECTRUM || f == Tango::IMAGE)
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
                    if(f == Tango::SPECTRUM) {
                        dat["w_value"] = temp;
                        dat["w_state"] = temp;
                        dat["w_state_color"] = state_colors;
                    } else if (f == Tango::IMAGE) {
                        dat["w_value"] = CuVariant(temp, p_da->get_written_dim_x(), p_da->get_written_dim_y());
                        dat["w_state"] = CuVariant(tempi, p_da->get_written_dim_x(), p_da->get_written_dim_y());
                        dat["w_state_color"] = CuVariant(state_colors, p_da->get_written_dim_x(), p_da->get_written_dim_y());
                    }
                }
            }
        }
        else if(p_da->get_type() == Tango::DEV_ULONG)
        {
            std::vector<Tango::DevULong> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = static_cast<unsigned long>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_ULONG64) {
            std::vector<Tango::DevULong64> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = static_cast<unsigned long>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_SHORT || p_da->get_type() == 100) /* 100! bug */
        {
            std::vector<short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_USHORT)
        {
            std::vector<unsigned short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_UCHAR)
        {
            std::vector<unsigned char> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE) {
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            }
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_INT)
        {
            std::vector<int>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_FLOAT)
        {
            std::vector<float>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat["value"] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = v;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(v, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat["w_value"] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = v;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(v, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_BOOLEAN)
        {
            std::vector<bool> vboo;
            p_da->extract_read(vboo);
            if(f == Tango::SCALAR)
            {
                bool b = vboo.at(0);
                dat["value"] = b;
            }
            else if(f == Tango::SPECTRUM)
                dat["value"] = vboo;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(vboo, p_da->get_dim_x(), p_da->get_dim_y());

            if(w)
            {
                p_da->extract_set(vboo);
                if(f == Tango::SCALAR)
                {
                    bool bset = vboo.at(0);
                    dat["w_value"] = bset;
                }
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = vboo;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(vboo, p_da->get_written_dim_x(), p_da->get_written_dim_y());
            }
        }
        else if(p_da->get_type() == Tango::DEV_STRING)
        {
            std::vector<std::string> vs;
            p_da->extract_read(vs);
            if(f == Tango::SCALAR)
                dat["value"] = vs.at(0);
            else if(f == Tango::SPECTRUM)
                dat["value"] = vs;
            else if (f == Tango::IMAGE)
                dat["value"] = CuVariant(vs, p_da->get_dim_x(), p_da->get_dim_y());
            if(w)
            {
                p_da->extract_set(vs);
                if(f == Tango::SCALAR)
                    dat["w_value"] = vs.at(0);
                else if(f == Tango::SPECTRUM)
                    dat["w_value"] = vs;
                else if (f == Tango::IMAGE)
                    dat["w_value"] = CuVariant(vs, p_da->get_written_dim_x(), p_da->get_written_dim_y());
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
    dat["df"] = ai.data_format;
    dat["dfs"] = formatToStr(ai.data_format); /* as string */
    dat["dt"] = ai.data_type;
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
    d["dt"] = ci.out_type;

    /* fake data_format property for commands */
    switch(ci.out_type)
    {
    case Tango::DEV_BOOLEAN:
    case Tango::DEV_SHORT: case Tango::DEV_LONG: case Tango::DEV_FLOAT: case Tango::DEV_DOUBLE:
    case Tango::DEV_USHORT: case Tango::DEV_ULONG: case Tango::DEV_STRING: case Tango::DEV_STATE:
    case Tango::CONST_DEV_STRING: case Tango::DEV_UCHAR: case Tango::DEV_LONG64: case Tango::DEV_ULONG64:
    case Tango::DEV_INT:
        d["df"] = Tango::SCALAR;
        d["dfs"] = formatToStr(Tango::SCALAR); /* as string */
        break;
    case Tango::DEVVAR_STATEARRAY:
    case Tango::DEVVAR_LONG64ARRAY: case Tango::DEVVAR_ULONG64ARRAY:
    case Tango::DEVVAR_BOOLEANARRAY: case Tango::DEVVAR_CHARARRAY:
    case Tango::DEVVAR_SHORTARRAY: case Tango::DEVVAR_LONGARRAY:
    case Tango::DEVVAR_FLOATARRAY: case Tango::DEVVAR_DOUBLEARRAY:
    case Tango::DEVVAR_USHORTARRAY: case Tango::DEVVAR_ULONGARRAY:
    case Tango::DEVVAR_STRINGARRAY: case Tango::DEVVAR_LONGSTRINGARRAY:
    case Tango::DEVVAR_DOUBLESTRINGARRAY:
        d["df"] = Tango::SPECTRUM;
        d["dfs"] = formatToStr(Tango::SPECTRUM); /* as string */
        break;
    default:
        d["df"] = Tango::FMT_UNKNOWN;
        d["dfs"] = formatToStr(Tango::FMT_UNKNOWN); /* as string */
    };
}

bool CuTangoWorld::read_att(Tango::DeviceProxy *dev, const string &attribute, CuData &res)
{
    /// TEST
    auto t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t2;
    ///
    ///
    d->error = false;
    d->message = "";
    try {
        std::string att(attribute);
        Tango::DeviceAttribute da = dev->read_attribute(att);

        /// TEST
        t2 = std::chrono::steady_clock::now();
        ///
        extractData(&da, res);
    }
    catch(Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
        res.putTimestamp();
    }
    res["err"] = d->error;
    if(d->message.length() > 0)
        res["msg"] = d->message;
    res["color"] = d->t_world_conf.successColor(!d->error);

    /// TEST
    auto t3 = std::chrono::steady_clock::now();
    printf("CuTangoWorld::\e[1;36mread_att\e[0m took %ldus, extract data took %ldus (%s)\n",
           std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count(),
           std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count(), res.s("src").c_str());
    ///
    return !d->error;
}

// dev: pointer to device proxy
// p_v_p: pointer to vector with data about the attribute name and an optional cached value
//        from an earlier reading (depending on update policy updpo)
// reslist: vector of results where data from read_attributes shall be appended
// updpo: update policy: always update, update timestamp only or do nothing when data doesn't change
//
bool CuTangoWorld::read_atts(Tango::DeviceProxy *dev,
                             std::vector<std::string>* p_v_an, // att names
                             std::vector<CuData>* p_v_a,  // att cache, ordered same as att names
                             std::vector<CuData> *reslist,
                             CuPollDataUpdatePolicy updpo)
{
    d->error = false;
    d->message = "";

    /// TEST
    auto t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t2;
    std::vector<CuData> &va = *p_v_a;
    size_t offset = reslist->size();
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
        std::vector<Tango::DeviceAttribute> *devattr = dev->read_attributes(*p_v_an);

        /// TEST
        t2 = std::chrono::steady_clock::now();
        ///

        for(size_t i = 0; i < devattr->size(); i++) {
            Tango::DeviceAttribute *p_da = &(*devattr)[i];
            p_da->set_exceptions(Tango::DeviceAttribute::failed_flag);

            if(updpo == CuPollDataUpdatePolicy::UpdateAlways) {
                reslist->push_back(va[i]);
                extractData(p_da,  (*reslist)[offset]);
                (*reslist)[offset]["err"] = d->error;
                if(d->message.length() > 0)
                    (*reslist)[offset]["msg"] = d->message;
                (*reslist)[offset]["color"] = d->t_world_conf.successColor(!d->error);
                offset++;
            }
            else {
                CuData rv;
                extractData(p_da, rv);
                // note that if !d->error, d->message is empty
                // m_cache_upd compares new value rv with cached (or first time empty)
                // va[i] (value, w_value, err, quality). If changed, returns true and
                // updateds va[i], that will cache the new data for the next time
                bool changed = m_cache_upd(va[i], rv) && !d->error;
                if(changed) { // update exactly as above
                    (*reslist).push_back(va[i]);
                    (*reslist)[offset]["err"] = d->error;
                    if(d->message.length() > 0)
                        (*reslist)[offset]["msg"] = d->message;
                    (*reslist)[offset]["color"] = d->t_world_conf.successColor(!d->error);
                    offset++;
                }
                else if(updpo == CuPollDataUpdatePolicy::OnUnchangedTimestampOnly) {
                    (*reslist).push_back(CuData("timestamp_ms", rv["timestamp_ms"]));
                    (*reslist)[offset]["timestamp_us"] = rv["timestamp_us"];
                    (*reslist)[offset]["src"] = va[i]["src"];
                    offset++;
                }
                else if(updpo == CuPollDataUpdatePolicy::OnUnchangedNothing) {
                    // do nothing
                }
            }

            /// TEST
            auto t3 = std::chrono::steady_clock::now();
            printf("CuTangoWorld::\e[0;33mread_attributes\e[0m took %ldus, extract data took %ldus fpr %ld atts\n",
                   std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count(),
                   std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count(), devattr->size());
            ///
            ///
        }
        delete devattr;
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
        for(size_t i = 0; i < p_v_an->size(); i++) {
            reslist->push_back(std::move(va[i]));
            (*reslist)[offset]["err"] = d->error;
            if(d->message.length() > 0)
                (*reslist)[offset]["msg"] = d->message;
            (*reslist)[offset]["color"] = d->t_world_conf.successColor(!d->error);
            (*reslist)[offset].putTimestamp();
            offset++;
        }
    }
    return !d->error;
}

bool CuTangoWorld::m_cache_upd(CuData &cache_d, const CuData &nd) const {
    const char keys[5][8] = { "value", "err", "msg", "q", "w_value" };
    short i, changed = 0;
    const char *key;
    for(i = 0; i < 5; i++) {
        key = keys[i];
        if(cache_d[key] != nd[key]) {  // changed: update cache_d
            cache_d[key] = nd[key];
            changed++;
        }
    }
    if(!changed) {
        printf("CuTangoWorld::m_cache_upd: cached value \e[1;33mUNCHANGED\e[0m:\t");
        for(const std::string& s : std::vector<std::string>{"value", "err", "msg", "q", "w_value"} )
            printf("%s %s=%s | ", s.c_str(), cache_d[s].toString().c_str(), nd[s].toString().c_str());
        printf("\n");
    } else {
        printf("CuTangoWorld::m_cache_upd: cached value \e[1;32mCHANGED\e[0m:\n");
    }
    return changed > 0;
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
    if(d->message.length() > 0) data["msg"] = d->message;
    //  data["color"] = d->t_world_conf.successColor(!d->error);
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
            d->message = "WRITE OK: \"" + data["device"].toString() + "/" + attnam + "\"";
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
    if(!skip_read_att) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        attr_read_ok = read_att(dev, attribute, dres);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        printf("CuTangoWorld::get_att_config: \e[1;34mread_attribute\e[0m took \e[1;34m%ld us\e[0m\n",
               std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }
    //
    // read attribute may fail, (example: device server is online but read
    // generates an internal exception). get_attribute_config may be successful
    // nevertheless. An unsuccessful read_attribute here is not an error.
    //
    Tango::AttributeInfoEx aiex;
    try {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        aiex = dev->get_attribute_config(attribute);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        printf("CuTangoWorld::get_att_config: dev->get_attribute_config: \e[1;32mget_attribute_config\e[0m took \e[1;32m%ld us\e[0m\n",
               std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
        fillFromAttributeConfig(aiex, dres);
    }
    catch(Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
    }
    dres["err"] = d->error;
    dres["color"] = d->t_world_conf.successColor(!d->error);
    return !d->error;
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
    dres["color"] = d->t_world_conf.successColor(!d->error);
    return !d->error;
}

Tango::Database *CuTangoWorld::getTangoDb(const std::string& dbhost)
{
    try {
        std::string db = dbhost;
        if(dbhost.size() == 0)
            return new Tango::Database();
        else {
            std::string h = dbhost.substr(0, dbhost.find(":"));
            std::string p = dbhost.substr(dbhost.find(":") + 1);
            try {
                int port = std::stoi(p);
                return new Tango::Database(h, port);
            }  catch (const std::invalid_argument& ia) {
                d->error = true;
                d->message = "CuTangoWorld::getTangoDb: invalid hostname:port \"" + dbhost + "\"";
                perr("%s", d->message.c_str());
            }
        }
    }
    catch(const Tango::DevFailed& e) {
        d->message =  "CuTangoWorld::getTangoDb: failed to connect to database \"" + dbhost + "\": " + strerror(e);
        d->error = true;
    }

    return nullptr;
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
    d->error = (db == nullptr);
    if(!d->error) {
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
                std::map<std::string, std::string> dev_prop_wildcards;
                for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit) {
                    std::string name;
                    if((*dit).containsKey("name") )
                        name = (*dit)["name"].toString();

                    if(name.length() > 0 && name.find("*") == std::string::npos)
                        db_data.push_back(Tango::DbDatum((*dit)["name"].toString().c_str()));
                    else if(name.length() > 0)
                        dev_prop_wildcards[(*dit)["device"].toString()] = name; // wildcard specified
                    else  {
                        dev_prop_wildcards[(*dit)["device"].toString()] = "*";
                    }
                }
                if(db_data.size() > 0) {
                    db->get_device_property(it->first, db_data);
                    for(size_t i = 0; i < db_data.size(); i++) {
                        if(!db_data[i].is_empty()) {
                            db_data[i] >> vs;
                            names.push_back(it->first + ":" + db_data[i].name);
                            res[names.at(names.size() - 1)] = vs;
                        }
                    }
                }
                for(std::map<std::string, std::string>::const_iterator it = dev_prop_wildcards.begin(); it != dev_prop_wildcards.end(); ++it) {
                    std::string devna = it->first, wildcard = it->second;
                    Tango::DbDatum dplist = db->get_device_property_list(devna, wildcard);
                    std::vector<std::string> s_dplist;
                    dplist >> s_dplist;
                    names.push_back(it->first + ":" + it->second);
                    res[names.at(names.size() - 1)] = s_dplist;
                }
            }
            // 3. class properties
            // scan the per-class map
            // if no "name" key is found, then get the list of class properties for the given class "name"
            for(std::unordered_map<std::string, std::list< CuData> >::const_iterator it = cprops.begin(); it != cprops.end(); ++it)
            {
                std::vector<std::string> cl_names; // list of class names to use with get_class_property_list
                Tango::DbData db_data;
                for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit) {
                    if((*dit).containsKey("name") && (*dit)["name"].toString().length() > 0)
                        db_data.push_back(Tango::DbDatum((*dit)["name"].toString().c_str()));
                    else if(!(*dit).containsKey("name")) {
                        cl_names.push_back((*dit)["class"].toString());
                    }
                }

                if(db_data.size() > 0) {
                    db->get_class_property(it->first, db_data); // it->first is class name
                    for(size_t i = 0; i < db_data.size(); i++) {
                        if(!db_data[i].is_empty()) {
                            db_data[i] >> vs;
                            names.push_back(it->first + ":" + db_data[i].name);
                            res[names.at(names.size() -1)] = vs;
                        }
                    }
                }
                if(cl_names.size() > 0) {
                    for(size_t i = 0; i < cl_names.size(); i++) {
                        Tango::DbDatum plist = db->get_class_property_list(cl_names[i]);
                        std::vector<std::string> s_plist;
                        plist >> s_plist;
                        names.push_back(it->first + ":*");
                        res[names.at(names.size() - 1)] = s_plist;
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
        if(d->message.length() > 0)
            res["msg"] = d->message;
        delete db;
    } // if !d->error (i.e. db != nullptr)
    return !d->error;
}

bool CuTangoWorld::db_get(const TSource &tsrc, CuData &res) {
    d->error = false;
    d->message.clear();
    bool res_ismap = false;
    const TSource::Type t = tsrc.getType();
    Tango::DbData db_data;
    Tango::DbDatum dbd;
    Tango::DeviceProxy *dev = nullptr;
    std::string p = tsrc.getSearchPattern();
    std::string ps(p); // p + '*' if p ends with '/'
    if(p.length() > 0 && p.at(p.length() - 1) == '/')
        ps = p + "*";
    std::string tgh = tsrc.getTangoHost();
    std::string dnam = tsrc.getDeviceName();
    std::string dnam_nhnp = tsrc.getDeviceNameOnly(); // dnam no host no proto
    Tango::Database *db = getTangoDb(tgh);
    if(db != nullptr) {
        res["pattern"] = p;
        if(dnam.length() > 0)
            res["device"] = dnam;
        if(tgh.length() > 0)
            res["tango_host"] = tgh;
        if(tsrc.getPoint().length() > 0)
            res["point"] = tsrc.getPoint();
        if(tsrc.getFreePropNam().length() > 0)
            res["property"] = tsrc.getFreePropNam();
        if(tsrc.getPropClassNam().length() > 0)
            res["class"] = tsrc.getPropClassNam();

        std::vector<std::string> r;
        try {

            switch(t) {
            case TSource::SrcInvalid:
            case TSource::SrcCmd:
            case TSource::SrcAttr:
            case TSource::SrcEndTypes:
                break;
            case TSource::SrcDbDoma:
                // SrcDbDomain with an empty pattern corresponds to a wildcard domain search
                if(p.size() == 0) p = "*";
                dbd = db->get_device_domain(p);
                dbd >> r;
                break;
            case  TSource::SrcDbFam:
                dbd = db->get_device_family(ps != p ? ps : p);
                dbd >> r;
                break;
            case  TSource::SrcDbMem:
                dbd = db->get_device_member(ps != p ? ps : p);
                dbd >> r;
                break;
            case TSource::SrcDbFreeProp:
                // cumbia read tango://ken:20000/#Sequencer#TestList
                db_data.push_back(Tango::DbDatum(tsrc.getFreePropNam()));
                db->get_property(tsrc.getFreePropObj(), db_data);
                if(db_data.size() > 0)
                    db_data[0] >> r;
                break;
            case TSource::SrcExportedDevs: {
                std::string pattern = tsrc.getExportedDevSearchPattern();
                Tango::DbDatum devs_dbd = db->get_device_exported(pattern);
                devs_dbd >> r;
            }
                break;
            case TSource::SrcDbAtts: {
                dev = new Tango::DeviceProxy(dnam);
                std::vector<std::string> *al = dev->get_attribute_list();
                if(al) {
                    r = *al;
                    delete al;
                }
            }
                break;
            case TSource::SrcDbCmds: {
                dev = new Tango::DeviceProxy(dnam);
                Tango::CommandInfoList *cil = dev->command_list_query();
                if(cil) {
                    for(size_t i = 0; i < cil->size(); i++)
                        r.push_back(cil->at(i).cmd_name);
                    delete cil;
                }
            }
                break;
            case TSource::SrcDbAttInfo: {
                Tango::DbData db_data;
                std::vector<std::string> p_values;
                db_data.push_back(Tango::DbDatum(tsrc.getPoint()));
                db->get_device_attribute_property(dnam_nhnp, db_data);
                for (size_t i=0; i < db_data.size();i++)
                {
                    long nb_prop;
                    db_data[i] >> nb_prop;
                    i++;
                    for (int k=0;k < nb_prop;k++) {
                        std::string pval;
                        db_data[i] >> pval;
                        r.push_back(db_data[i].name);
                        p_values.push_back(pval);
                        i++;
                    }
                    if(p_values.size() > 0)
                        res["p_values"] = p_values;
                }

            }
                break;
            case TSource::SrcDbDevProps: {  //  test/device/1/double_scalar/
                dev = new Tango::DeviceProxy(dnam);
                dev->get_property_list("*", r);
            }
                break;
            case  TSource::SrcDbGetCmdI: { // "tango://test/device/1->get/"
                dev = new Tango::DeviceProxy(dnam);
                Tango::CommandInfo ci = dev->command_query(tsrc.getPoint());
                res["cmd_in_type"] = ci.in_type;
                res["cmd_out_type"] = ci.out_type;
                res["cmd_in_type_str"] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>( ci.in_type));
                res["cmd_out_type_str"] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>( ci.out_type));
                res["cmd_in_desc"] = ci.in_type_desc;
                res["cmd_out_desc"] = ci.out_type_desc;
                r = std::vector<std::string> { "cmd_in_type", "cmd_out_type", "cmd_in_type_str",
                        "cmd_out_type_str", "cmd_in_desc", "cmd_out_desc" };
                res_ismap = true;
            }
                break;
            case TSource::SrcDbClassProps: { //  tango://class(*), tango://hokuto:20000/class(*)
                std::string cl = tsrc.getPropClassNam();
                dbd = db->get_class_property_list(cl);
                dbd >> r;
            }
                break;

            case TSource::SrcDbAProp: {   // tango://hokuto:20000/test/device/1/double_scalar#values
                std::vector<std::string> props = tsrc.getPropNames();
                db_data.push_back(tsrc.getPoint());
                std::vector<std::string> v, keys;
                std::string prop;
                // use device name without tango:// and host:port/
                db->get_device_attribute_property(dnam_nhnp, db_data);
                res_ismap = true;
                if(props.size() > 0) prop = props[0];
                for (size_t i=0; i < db_data.size(); i++)  {
                    long nb_prop;
                    db_data[i] >> nb_prop;
                    i++;
                    for (int k=0; k < nb_prop; k++, i++) {
                        if(std::find(props.begin(), props.end(), db_data[i].name) != props.end()) {
                            db_data[i] >> v;
                            keys.push_back(db_data[i].name);
                            res[db_data[i].name] = v;
                            if(db_data[i].name == prop)
                                r = v;
                        }
                    }
                }
                res["keys"] = keys;
            }
                break;
            case TSource::SrcDbClassProp: { // tango://hokuto:20000/class(pr1,pr2,..)
                std::string c = tsrc.getPropClassNam();
                std::vector<std::string> keys, v, prs = tsrc.getPropNames();
                for(std::string p : prs)
                    db_data.push_back(Tango::DbDatum(p));
                db->get_class_property(c, db_data);
                for(size_t i = 0; i < db_data.size(); i++) {
                    if(!db_data[i].is_empty()) {
                        db_data[i] >> v;
                        res[db_data[i].name] = v;
                        keys.push_back(db_data[i].name);
                        if(i == 0) r = v;
                    }
                }
                res["keys"] = keys;
                res_ismap = true;
            }
                break;
            case TSource::SrcDbDevProp: {
                std::string p;
                std::vector<std::string> keys, v, prs = tsrc.getPropNames();
                for(std::string p : prs)
                    db_data.push_back(Tango::DbDatum(p));
                // use device name without tango:// and host:port/
                db->get_device_property(dnam_nhnp, db_data);
                for(size_t i = 0; i < db_data.size(); i++) {
                    if(!db_data[i].is_empty()) {
                        db_data[i] >> v;
                        res[db_data[i].name] = v;
                        keys.push_back(db_data[i].name);
                        if(i == 0) r = v;
                    }
                }
                res["keys"] = keys;
                res_ismap = true;
            }
                break;
            }
            res["value"] = r;
            d->error |= !r.size();
            if(r.size() == 0) {
                d->message = "not found";
            }
            d->message.size() == 0 ? res["msg"] = std::string("operation \"") + tsrc.getTypeName(t) + "\" successful" : res["msg"] = d->message;
            if(dev)
                delete dev;

        }  catch (const Tango::DevFailed &e) {
            d->error = true;
            d->message = strerror(e);
        }
        delete db;
    } // if(db != nullptr)
    res["err"] = d->error;
    if(d->message.length() > 0)
        res["msg"] = d->message;
    res.putTimestamp();
    return !d->error;
}

bool CuTangoWorld::source_valid(const string &src)
{
    std::regex re = std::regex(d->src_valid_pattern);
    std::smatch m;
    return std::regex_match(src, m, re);
}

const char *CuTangoWorld::source_valid_pattern() const {
    return d->src_valid_pattern;
}

string CuTangoWorld::getLastMessage() const
{
    return d->message;
}

/** \brief returns a string representation of the attribute data format to be used in the
 *         CuData "dfs" property.
 *
 * Valid return values are scalar, vector and matrix
 */
string CuTangoWorld::formatToStr(Tango::AttrDataFormat f) const {
    switch(f) {
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
 * CuData "dfs" property
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

        case Tango::DEVVAR_BOOLEANARRAY:
        {
            std::vector<bool> bv;
            std::vector<std::string> a = argins;
            for(size_t i = 0; i < argins.size(); i++) {
                std::transform(a[i].begin(), a[i].end(), a[i].begin(), ::tolower);
                a[i] == std::string("true") || std::stoi(argins[i]) != 0 ? dd << true : dd << false;
            }
            break;
        }
        case Tango::DEVVAR_SHORTARRAY:
        {
            std::vector<short> sv;
            for(size_t i = 0; i < argins.size(); i++)
                sv.push_back((Tango::DevShort) std::stoi(argins[i]));
            dd << sv;
            break;
        }
        case Tango::DEVVAR_USHORTARRAY:
        {
            std::vector<unsigned short> usv;
            for(size_t i = 0; i < argins.size(); i++)
                usv.push_back((Tango::DevUShort) std::stoi(argins[i]));
            dd << usv;
            break;
        }
        case  Tango::DEVVAR_LONGARRAY:
        {
            std::vector<Tango::DevLong> lv;
            for(size_t i = 0; i < argins.size(); i++)
                lv.push_back((Tango::DevLong) std::stol(argins[i]));
            dd << lv;
            break;
        }
        case  Tango::DEVVAR_ULONGARRAY:
        {
            std::vector<Tango::DevLong> ulv;
            for(size_t i = 0; i < argins.size(); i++)
                ulv.push_back((Tango::DevULong) std::stoul(argins[i]));
            dd << ulv;
            break;
        }
        case    Tango::DEVVAR_FLOATARRAY:
        {
            std::vector<Tango::DevFloat> fv;
            for(size_t i = 0; i < argins.size(); i++)
                fv.push_back((Tango::DevFloat) std::stof(argins[i]));
            dd << fv;
            break;
        }
        case  Tango::DEVVAR_DOUBLEARRAY:
        {
            std::vector<double> dv;
            for(size_t i = 0; i < argins.size(); i++)
                dv.push_back((Tango::DevDouble) std::stod(argins[i]));
            dd << dv;
            break;
        }
        case Tango::DEVVAR_STRINGARRAY:
        {
            std::vector<std::string> nonconstv = argins;
            dd << nonconstv;
            break;
        }
        default:
            perr("CuTangoWorld::toDeviceData: in_type %ld not supported by the library", in_type);

        } /* switch */
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
    int tango_type = attinfo["dt"].toInt();
    Tango::AttrDataFormat tango_format = static_cast<Tango::AttrDataFormat>(attinfo["df"].toInt());
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
    else if(tango_format == Tango::IMAGE)
        perr("cutango-world: toDeviceAttribute Tango::IMAGE not supported yet");

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
    int tango_type = attinfo["dt"].toLongInt();
    Tango::AttrDataFormat tango_fmt = static_cast<Tango::AttrDataFormat>(attinfo["df"].toInt());
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

        }
        else if (tango_fmt == Tango::IMAGE) {
            perr("cutango-world.cpp toDeviceAttribute: Tango::IMAGE not supported yet"); /* if */
        }
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

std::string CuTangoWorld::make_fqdn_src(const std::string& src) {
    string::size_type	end1 = src.find(".");
    string fusrc = src; // full source
    if (end1 == string::npos) {
        //get host name without tango://
        string::size_type	start = src.find("tango://");
        if (start == string::npos)
            start = 0;
        else
            start = 8;	//tango:// len
        string::size_type	end2 = src.find(":", start);
        string th = src.substr(start, end2);
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        struct addrinfo *result;
        int ret = getaddrinfo(th.c_str(), NULL, &hints, &result);
        if (ret != 0)
            d->message = std::string(__func__) + ": getaddrinfo: " + th + ": " + std::string(gai_strerror(ret));
        else if(result == nullptr)
            d->message = std::string(__func__) + ": getaddrinfo: " + th + ": no info";
        else if (result->ai_canonname == NULL)
            d->message = std::string(__func__) + ": getaddrinfo: " + th + ": no info";
        else
            fusrc = string(result->ai_canonname) + src.substr(end2);
        //cout << __func__ <<": found domain -> " << with_domain<<endl;
        if(ret == 0 && result != nullptr)
            freeaddrinfo(result); // all done with this structure
    }
    return fusrc;
}

std::string CuTangoWorld::prepend_tgproto(const std::string& s) {
    return s.find("tango://") != 0 ? "tango://" + s : s;
}
