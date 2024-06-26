#include "cutango-world.h"
#include "cutango-world-config.h"
#include "tsource.h"
#include <unordered_map>
#include <cumacros.h>
#include <chrono>
#include <regex>
#include <sys/types.h> // getaddrinfo
#include <sys/socket.h> // getaddrinfo
#include <netdb.h> // getaddrinfo
#include <type_traits> // std::is_same (Tango::DevLong, int)

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

static  CuTangoWorldConfig t_world_conf;

class CuTangoWorldPrivate
{
public:
    bool error;
    std::string message;
    // this is used in source_valid
    //  [A-Za-z_0-9_\-\.\,\*/\+\:\(\)>#{}]+
    char src_valid_pattern[128] = "[A-Za-z_0-9_\\-\\.\\,\\*/\\+\\:&\\(\\)>#{}]+";
    std::vector <std::string> src_patterns;
};

CuTangoWorld::CuTangoWorld() {
    d = new CuTangoWorldPrivate();
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
 * CuDType::Time_us and CuDType::Time_us *are not set* by extractData. It must be set
 * manually by the caller. There is no date/time information in Tango::DeviceData.
 *
 *
 */
void CuTangoWorld::extractData(Tango::DeviceData *data, CuData& da)
{
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    int t = (Tango::CmdArgType) data->get_type();
    da[CuDType::DataFormatStr] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>(t));
    da[CuDType::DataType] = t;

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
            Tango::DevState temp;
            *data >> temp;
            da[CuDType::Value] = t_world_conf.stateString(temp);
            da[CuDType::State] = static_cast<long int>(temp);
            da[CuDType::StateColor] = t_world_conf.stateColorName(temp);
            break;
        }
        case Tango::DEV_BOOLEAN:
        {
            bool booltmp;
            *data >> booltmp;
            da[CuDType::Value] = booltmp;
            break;
        }
        case Tango::DEV_SHORT:
        {
            short temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEV_USHORT:
        {
            unsigned short temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEV_LONG:
        {
            Tango::DevLong temp;
            *data >> temp;
            da[CuDType::Value] = (long) temp;
            break;
        }
        case Tango::DEV_LONG64: {
            long int temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEV_ULONG:
        {
            Tango::DevULong temp;
            *data >> temp;
            da[CuDType::Value] = static_cast<unsigned long> (temp);
            break;
        }
        case Tango::DEV_ULONG64: {
            long unsigned int temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEV_FLOAT:
        {
            Tango::DevFloat temp;
            *data >> temp;
            da[CuDType::Value] = (float) temp;
            break;
        }
        case Tango::DEV_DOUBLE:
        {
            double temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEV_STRING:
        {
            std::string temp;
            *data >> temp;
            da[CuDType::Value] = temp;
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
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEVVAR_USHORTARRAY:
        {
            vector<unsigned short> temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEVVAR_LONGARRAY:
        {
            vector<Tango::DevLong> temp;
            vector<long int> li; // !! Tango DevLong
            *data >> temp;
            const size_t & siz = temp.size();
            li.resize(siz);
            for(size_t i = 0; i < siz; i++)
                li[i] = static_cast<long int>(temp[i]);
            da[CuDType::Value] = li;
            break;
        }
        case Tango::DEVVAR_ULONGARRAY:
        {
            vector<unsigned long> temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEVVAR_FLOATARRAY:
        {
            vector<float> temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEVVAR_DOUBLEARRAY:
        {
            vector<double> temp;
            *data >> temp;
            da[CuDType::Value] = temp;
            break;
        }
        case Tango::DEVVAR_STRINGARRAY:
        {
            std::vector<std::string> temp;
            *data >> temp;
            da[CuDType::Value] = temp;
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
void CuTangoWorld::extractData(Tango::DeviceAttribute *p_da, CuData &dat, int xflags) {
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    const Tango::AttrQuality quality = p_da->get_quality();
    const Tango::AttrDataFormat f = p_da->get_data_format();
    const int &tt = p_da->get_type();
    bool w = false;
    if(xflags > ExtractValueOnly) {
        w = (p_da->get_nb_written() > 0);
        Tango::TimeVal tv = p_da->get_date();
        putDateTime(tv, dat);
        CuDataQuality cuq = toCuQuality(quality);
        dat[CuDType::Quality] = cuq.toInt();
        if(xflags > ExtractMinimal) {
            dat[CuDType::QualityColor] = cuq.color();
            dat[CuDType::QualityString] = cuq.name();
            dat[CuDType::DataFormatStr] = formatToStr(f);
        }
    }
    try{
        if(quality == Tango::ATTR_INVALID) {
            d->message = "CuTangoWorld.extractData: attribute quality invalid - ";
            d->message += strerror(p_da->get_err_stack());
            d->error = true;
        }
        else if(p_da->is_empty())  {
            d->message = "CuTangoWorld.extractData: attribute " + p_da->get_name() + " is empty";
            d->error = true;
        }
        else if(tt == Tango::DEV_DOUBLE) {
            std::vector<double> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if(f == Tango::IMAGE)
                dat.set(CuDType::Value, CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x()));
            if(w) {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if(f == Tango::IMAGE)
                    dat.set(CuDType::WriteValue, CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x()));
            }
        }
        else if(tt == Tango::DEV_LONG) {
            if(std::is_same<Tango::DevLong, long int>::value) {
                std::vector<long int> v;
                p_da->extract_read(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::Value] = v[0];
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::Value] = v;
                else if(f == Tango::IMAGE)
                    dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
                if(w) {
                    p_da->extract_set(v);
                    if(f == Tango::SCALAR)
                        dat[CuDType::WriteValue] = v[0];
                    else if(f == Tango::SPECTRUM)
                        dat[CuDType::WriteValue] = v;
                    else if(f == Tango::IMAGE)
                        dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
                }
            }
            // Tango::DevLong is long int
            else if(std::is_same<Tango::DevLong, int>::value) {
                std::vector<int> v;
                p_da->extract_read(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::Value] = v[0];
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::Value] = v;
                else if(f == Tango::IMAGE)
                    dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
                if(w) {
                    p_da->extract_set(v);
                    if(f == Tango::SCALAR)
                        dat[CuDType::WriteValue] = v[0];
                    else if(f == Tango::SPECTRUM)
                        dat[CuDType::WriteValue] = v;
                    else if(f == Tango::IMAGE)
                        dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
                }
            }
        }
        else if(p_da->get_type() == Tango::DEV_LONG64)
        {
            std::vector<Tango::DevLong64> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = static_cast<long int>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if(f == Tango::IMAGE)
                dat.set(CuDType::Value, CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x()));
            if(w) {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if(f == Tango::IMAGE)
                    dat.set(CuDType::WriteValue, CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x()));
            }
        }
        else if(tt == Tango::DEV_STATE) {
            if(f == Tango::SCALAR)   {
                Tango::DevState state;
                *p_da >> state;
                dat[CuDType::Value] = t_world_conf.stateString(state);
                dat[CuDType::State] = static_cast<long int>(state);
                dat[CuDType::StateColor] = t_world_conf.stateColorName(state);
            }
            else if(f == Tango::SPECTRUM || f == Tango::IMAGE) {
                std::vector<Tango::DevState> v;
                std::vector<std::string> temp;
                std::vector<std::string> state_colors;
                std::vector<long int>tempi;
                p_da->extract_read(v);
                const size_t& siz = v.size();
                tempi.resize(siz);
                temp.resize(siz);
                state_colors.resize(siz);
                for(size_t i = 0; i < siz; i++) {
                    temp[i] = t_world_conf.stateString(v[i]);
                    tempi[i] = v[i];
                    state_colors[i] = t_world_conf.stateColorName(v[i]);
                }
                if(f == Tango::SPECTRUM) {
                    dat[CuDType::Value] = temp;
                    dat[CuDType::State] = tempi;
                    dat[CuDType::StateColor] = state_colors;
                } else {
                    dat[CuDType::Value] = CuVariant(temp, p_da->get_dim_y(), p_da->get_dim_x());
                    dat[CuDType::State] = CuVariant(tempi, p_da->get_dim_y(), p_da->get_dim_x());
                    dat[CuDType::StateColor] = CuVariant(state_colors, p_da->get_dim_y(), p_da->get_dim_x());
                }
            }
            if(w) {
                if(f == Tango::SCALAR) {
                    Tango::DevState state;
                    *p_da >> state;
                    dat[CuDType::WriteValue] = t_world_conf.stateString(state);
                    dat[CuDType::StateColor] = t_world_conf.stateColorName(state);
                    dat["w_state"] = static_cast<long int>(state);
                }
                else if(f == Tango::SPECTRUM || f == Tango::IMAGE)
                {
                    std::vector<Tango::DevState> v;
                    std::vector<long int>tempi;
                    std::vector<std::string> temp;
                    std::vector<std::string> state_colors;
                    p_da->extract_set(v);
                    const size_t &siz = v.size();
                    for(size_t i = 0; i < siz; i++) {
                        temp.push_back(t_world_conf.stateString(v.at(i)));
                        tempi.push_back(static_cast<long int>(v.at(i)));
                        state_colors.push_back(t_world_conf.stateColorName(v.at(i)));
                    }
                    if(f == Tango::SPECTRUM) {
                        dat[CuDType::WriteValue] = temp;
                        dat["w_state"] = temp;
                        dat["w_state_color"] = state_colors;
                    } else if (f == Tango::IMAGE) {
                        dat[CuDType::WriteValue] = CuVariant(temp, p_da->get_written_dim_y(), p_da->get_written_dim_x());
                        dat["w_state"] = CuVariant(tempi, p_da->get_written_dim_y(), p_da->get_written_dim_x());
                        dat["w_state_color"] = CuVariant(state_colors, p_da->get_written_dim_y(), p_da->get_written_dim_x());
                    }
                }
            }
        }
        else if(tt == Tango::DEV_ULONG)
        {
            std::vector<Tango::DevULong> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = static_cast<unsigned long>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_ULONG64) {
            std::vector<Tango::DevULong64> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = static_cast<unsigned long>(v.at(0));
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_SHORT || tt == 100) /* 100! bug */
        {
            std::vector<short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_USHORT)
        {
            std::vector<unsigned short> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_UCHAR)
        {
            std::vector<unsigned char> v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE) {
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            }
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }

        }
        else if(tt == Tango::DEV_INT) {
            std::vector<int>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_FLOAT)  {
            std::vector<float>  v;
            p_da->extract_read(v);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = v.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = v;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(v, p_da->get_dim_y(), p_da->get_dim_x());
            if(w)
            {
                p_da->extract_set(v);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = v.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = v;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(v, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_BOOLEAN) {
            std::vector<bool> vboo;
            p_da->extract_read(vboo);
            if(f == Tango::SCALAR)
            {
                bool b = vboo.at(0);
                dat[CuDType::Value] = b;
            }
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = vboo;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(vboo, p_da->get_dim_y(), p_da->get_dim_x());

            if(w)
            {
                p_da->extract_set(vboo);
                if(f == Tango::SCALAR)
                {
                    bool bset = vboo.at(0);
                    dat[CuDType::WriteValue] = bset;
                }
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = vboo;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(vboo, p_da->get_written_dim_y(), p_da->get_written_dim_x());
            }
        }
        else if(tt == Tango::DEV_STRING) {
            std::vector<std::string> vs;
            p_da->extract_read(vs);
            if(f == Tango::SCALAR)
                dat[CuDType::Value] = vs.at(0);
            else if(f == Tango::SPECTRUM)
                dat[CuDType::Value] = vs;
            else if (f == Tango::IMAGE)
                dat[CuDType::Value] = CuVariant(vs, p_da->get_dim_y(), p_da->get_dim_x());
            if(w) {
                p_da->extract_set(vs);
                if(f == Tango::SCALAR)
                    dat[CuDType::WriteValue] = vs.at(0);
                else if(f == Tango::SPECTRUM)
                    dat[CuDType::WriteValue] = vs;
                else if (f == Tango::IMAGE)
                    dat[CuDType::WriteValue] = CuVariant(vs, p_da->get_written_dim_y(), p_da->get_written_dim_x());
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
    if(d->message.length() > 0)  d->message.clear();
    dat[CuDType::Type] = "property";  // dat["type"]
    dat[CuDType::DataFormat] = ai.data_format;
    dat[CuDType::DataFormatStr] = formatToStr(ai.data_format); /* as string */
    dat[CuDType::DataType] = ai.data_type;
    dat[CuDType::Description] = ai.description;
    ai.display_unit != std::string("No display unit") ? dat["display_unit"] = ai.display_unit : dat["display_unit"] = "";
    dat[CuDType::NumberFormat] = ai.format;
    dat[CuDType::Label] = ai.label;  // dat["label"]
    dat["max_alarm"] = ai.max_alarm;
    dat[CuDType::MaxDimX] = ai.max_dim_x;
    dat[CuDType::MaxDimY] = ai.max_dim_y;
    dat[CuDType::Max] = ai.max_value;
    dat[CuDType::Min] = ai.min_value;
    dat["min_alarm"] = ai.min_alarm;
    dat[CuDType::Name] = ai.name;
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
    long int dimx = dat[CuDType::Value].getSize(); // if !contains value, empty variant, 0 dimx
    if(dimx > 0)
        dat[CuDType::DimX] = dimx;
}

void CuTangoWorld::fillFromCommandInfo(const Tango::CommandInfo &ci, CuData &d)
{
    d[CuDType::Type] = "property";  // d["type"]
    d["cmd_name"] = ci.cmd_name;
    d[CuDType::InType] = ci.in_type;  // d["in_type"]
    d[CuDType::OutType] = ci.out_type;  // d["out_type"]
    d[CuDType::InTypeDesc] = ci.in_type_desc;  // d["in_type_desc"]
    d[CuDType::OutTypeDesc] = ci.out_type_desc;  // d["out_type_desc"]
    d["display_level"] = ci.disp_level;
    d[CuDType::DataType] = ci.out_type;

    /* fake data_format property for commands */
    switch(ci.out_type)
    {
    case Tango::DEV_BOOLEAN:
    case Tango::DEV_SHORT: case Tango::DEV_LONG: case Tango::DEV_FLOAT: case Tango::DEV_DOUBLE:
    case Tango::DEV_USHORT: case Tango::DEV_ULONG: case Tango::DEV_STRING: case Tango::DEV_STATE:
    case Tango::CONST_DEV_STRING: case Tango::DEV_UCHAR: case Tango::DEV_LONG64: case Tango::DEV_ULONG64:
    case Tango::DEV_INT:
        d[CuDType::DataFormat] = Tango::SCALAR;
        d[CuDType::DataFormatStr] = formatToStr(Tango::SCALAR); /* as string */
        break;
    case Tango::DEVVAR_STATEARRAY:
    case Tango::DEVVAR_LONG64ARRAY: case Tango::DEVVAR_ULONG64ARRAY:
    case Tango::DEVVAR_BOOLEANARRAY: case Tango::DEVVAR_CHARARRAY:
    case Tango::DEVVAR_SHORTARRAY: case Tango::DEVVAR_LONGARRAY:
    case Tango::DEVVAR_FLOATARRAY: case Tango::DEVVAR_DOUBLEARRAY:
    case Tango::DEVVAR_USHORTARRAY: case Tango::DEVVAR_ULONGARRAY:
    case Tango::DEVVAR_STRINGARRAY: case Tango::DEVVAR_LONGSTRINGARRAY:
    case Tango::DEVVAR_DOUBLESTRINGARRAY:
        d[CuDType::DataFormat] = Tango::SPECTRUM;
        d[CuDType::DataFormatStr] = formatToStr(Tango::SPECTRUM); /* as string */
        break;
    default:
        d[CuDType::DataFormat] = Tango::FMT_UNKNOWN;
        d[CuDType::DataFormatStr] = formatToStr(Tango::FMT_UNKNOWN); /* as string */
    };
}

bool CuTangoWorld::read_att(Tango::DeviceProxy *dev, const std::string &attribute, CuData &res, int xtractflags) {
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    try {
        Tango::DeviceAttribute da = dev->read_attribute(attribute.c_str());
        extractData(&da, res, xtractflags);
    }
    catch(Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
        res.putTimestamp();
    }
    res[CuDType::Err] = d->error;
    if(d->message.length() > 0)
        res[CuDType::Message] = d->message;
    res[CuDType::Color] = t_world_conf.successColor(!d->error);
    return !d->error;
}

// dev: pointer to device proxy
// p_v_p: pointer to vector with data about the attribute name and an optional cached value
//        from an earlier reading (depending on update policy updpo)
// reslist: vector of results where data from read_attributes shall be appended
// updpo: update policy: always update, update timestamp only or do nothing when data doesn't change
//        OR combination of CuDataUpdatePolicy enum values
//
bool CuTangoWorld::read_atts(Tango::DeviceProxy *dev,
                             std::vector<std::string>& p_v_an, // att names
                             std::vector<CuData>& va,  // att cache, ordered same as att names
                             std::vector<CuData> &reslist, // reserve()d by caller (CuPollingActivity)
                             int updpo, int xtract_flags)
{
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    size_t offset = reslist.size();
    // auto t1 = std::chrono::high_resolution_clock::now();
    // std::string atts;
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
        std::vector<Tango::DeviceAttribute> *devattr = dev->read_attributes(p_v_an);

        // auto t2 = std::chrono::high_resolution_clock::now();
        // for(int i = 0; i < p_v_an.size(); i++)
        //     atts += p_v_an[i] + ",";
        // printf("[thread: 0x%lx] CuTangoWorld::read_atts  %s (%s) dev->read_attributes took %.1fms\n",
        //        pthread_self(),  dev->name().c_str(), atts.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()/1000.0);

        for(size_t i = 0; i < devattr->size(); i++) {
            Tango::DeviceAttribute *p_da = &(*devattr)[i];
            p_da->set_exceptions(Tango::DeviceAttribute::failed_flag);
            if(updpo & CuDataUpdatePolicy::PollUpdateAlways) { // check flags
                reslist.push_back(va[i]);
                extractData(p_da,  reslist[offset], xtract_flags);
                reslist[offset][CuDType::Err] = d->error;
                if(d->message.length() > 0 ) {
                    if(xtract_flags > ExtractMinimal || (d->error && xtract_flags > ExtractValueOnly))
                        reslist[offset][CuDType::Message] = d->message;
                }
                if(xtract_flags > ExtractMinimal)
                    reslist[offset][CuDType::Color] = t_world_conf.successColor(!d->error);
                offset++;
            }
            else {
                CuData rv;
                extractData(p_da, rv, xtract_flags);
                // note that if !d->error, d->message is empty
                // m_cache_upd compares new value rv with cached (or first time empty)
                // va[i] (value, w_value, err, quality). If changed, returns true and
                // updateds va[i], that will cache the new data for the next time
                bool changed = d->error || m_cache_upd(va[i], rv);
                if(changed) { // update exactly as above
                    reslist.push_back(va[i]);
                    reslist[offset][CuDType::Err] = d->error; // error flag, always
                    if(xtract_flags > ExtractMinimal) { // timestamp, if flags > ExtractMinimal
                        reslist[offset][CuDType::Time_ms] = rv[CuDType::Time_ms];
                        reslist[offset][CuDType::Time_us] = rv[CuDType::Time_us];
                    }
                    if(d->message.length() > 0) {
                        if(xtract_flags > ExtractMinimal || (d->error && xtract_flags > ExtractValueOnly))
                            reslist[offset][CuDType::Message] = d->message;
                    }
                    if(xtract_flags > ExtractMinimal)
                        reslist[offset][CuDType::Color] = t_world_conf.successColor(!d->error);
                    offset++;
                }
                else if(updpo & CuDataUpdatePolicy::OnPollUnchangedTimestampOnly) {
                    // it makes sense to place timestamp here even if xtract_policy is minimal
                    reslist.push_back(CuData(CuDType::Time_us, rv[CuDType::Time_us]));
                    reslist.push_back(CuData(CuDType::Time_ms, rv[CuDType::Time_ms]));
                    reslist[offset][CuDType::Src] = va[i][CuDType::Src];
                    offset++;
                }
                else if(updpo & CuDataUpdatePolicy::OnPollUnchangedNoUpdate) {
                    // do nothing
                }
            }
        }

        // auto t3 = std::chrono::high_resolution_clock::now();

        // printf("[thread: 0x%lx] CuTangoWorld  extract data %s (%s) took %.1fms\n",
        //        pthread_self(),  dev->name().c_str(), atts.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()/1000.0);

        // printf("[thread: 0x%lx] \e[1;36mCuTangoWorld read attrributes + extract %s (%s) took %.1fms\e[0m\n",
        //        pthread_self(),  dev->name().c_str(), atts.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(t3-t1).count()/1000.0);

        delete devattr;
    }
    catch(Tango::DevFailed &e)
    {
        d->error = true;
        d->message = strerror(e);
        for(size_t i = 0; i < p_v_an.size(); i++) {
            reslist.push_back(va[i]);
            reslist[offset][CuDType::Err] = d->error;
            if(d->message.length() > 0)
                reslist[offset][CuDType::Message] = d->message;
            reslist[offset][CuDType::Color] = t_world_conf.successColor(!d->error);
            reslist[offset].putTimestamp();
            offset++;
        }
    }
    return !d->error;
}

// returns true if one of { CuDType::Value, CuDType::Err, CuDType::Message, CuDType::Quality, CuDType::WriteValue } key values changed
//
bool CuTangoWorld::m_cache_upd(CuData &cache_d, const CuData &nd) const {
    const CuDType::Key keys[5] = { CuDType::Value, CuDType::Err, CuDType::Message, CuDType::Quality, CuDType::WriteValue };
    short i, changed = 0;
    for(i = 0; i < 5; i++) {
        const CuDType::Key &key = keys[i];
        if(cache_d[key] != nd[key]) {  // changed: update cache_d
            cache_d[key] = nd[key];
            changed++;
        }
    }
    return changed > 0;
}

// cmd_inout version 2: from the data argument, guess input DeviceData
// data must contain the output from a previous call to get_command_info
bool CuTangoWorld::cmd_inout(Tango::DeviceProxy *dev,
                             const std::string& cmd,
                             CuData& data)
{
    bool has_argout = data[CuDType::OutType].toLongInt() != Tango::DEV_VOID;  // data["out_type"]
    Tango::DeviceData din = toDeviceData(data[CuDType::Args], data);
    return cmd_inout(dev, cmd, din, has_argout, data);
}

bool CuTangoWorld::cmd_inout(Tango::DeviceProxy *dev,
                             const std::string& cmd,
                             Tango::DeviceData& din,
                             bool has_argout,
                             CuData& data)
{
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
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
    data[CuDType::Err] = d->error;
    if(d->message.length() > 0) data[CuDType::Message] = d->message;
    //  data[CuDType::Color] = t_world_conf.successColor(!d->error);
    return !d->error;
}

bool CuTangoWorld::write_att(Tango::DeviceProxy *dev,
                             const string &attnam,
                             const CuVariant& argins,
                             const CuData& point_info,
                             CuData &data)
{
    // assert(dev != NULL);
    try
    {
        Tango::DeviceAttribute da = toDeviceAttribute(attnam, argins, point_info);
        if(!d->error)
        {
            dev->write_attribute(da);
            d->message = "WRITE OK: \"" + data[CuDType::Device].toString() + "/" + attnam + "\"";
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
    if(d->message.length() > 0)  d->message.clear();
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

bool CuTangoWorld::get_att_config(Tango::DeviceProxy *dev,
                                  const string &attribute,
                                  CuData &dres,
                                  bool skip_read_att,
                                  int extract_flags)
{
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    /* first read attribute to get the value */
    if(!skip_read_att) {
        //        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        read_att(dev, attribute, dres, extract_flags);
        //        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //        printf("CuTangoWorld::get_att_config, read_attribute, %s, %ld, us\n",
        //              dres.s(CuDType::Src).c_str(), std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }
    //
    // read attribute may fail, (example: device server is online but read
    // generates an internal exception). get_attribute_config may be successful
    // nevertheless. An unsuccessful read_attribute here is not an error.
    //
    Tango::AttributeInfoEx aiex;
    try {
        //        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        aiex = dev->get_attribute_config(attribute);
        //        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //        printf("CuTangoWorld::get_att_config, get_attribute_config, %s, %ld, us\n",
        //               dres.s(CuDType::Src).c_str(), std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
        fillFromAttributeConfig(aiex, dres);
    }
    catch(Tango::DevFailed &e) {
        d->error = true;
        d->message = strerror(e);
    }
    dres[CuDType::Err] = d->error;
    if(extract_flags > ExtractMinimal)
        dres[CuDType::Color] = t_world_conf.successColor(!d->error);
    return !d->error;
}

bool CuTangoWorld::get_att_props(Tango::DeviceProxy *dev,
                                 const string &attribute_name,
                                 CuData &dres,
                                 const std::vector<string> props)
{
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
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
                std::vector<std::string> pp = dres[CuDType::Properties].toStringVector();
                pp.push_back(prop);
                dres[CuDType::Properties] = pp;
            }
        }
        catch (Tango::DevFailed &e)
        {
            d->error = true;
            d->message = strerror(e);
        }
    }
    dres[CuDType::Color] = t_world_conf.successColor(!d->error);
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

        if(in.containsKey(CuDType::Device) && in.containsKey(CuDType::Attribute))
            daprops[in[CuDType::Device].toString()].push_back(in);
        else if(in.containsKey(CuDType::Device) && !in.containsKey(CuDType::Attribute)) // device property
            dprops[in[CuDType::Device].toString()].push_back(in);
        else if(in.containsKey(CuDType::Class)) // class property
            cprops[in[CuDType::Class].toString()].push_back(in);
    }
    Tango::Database *db = getTangoDb(dbhost);
    d->error = (db == nullptr);
    if(!d->error) {
        if(d->message.length() > 0)  d->message.clear();
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
                    const std::string &attname = (*dit)[CuDType::Attribute].toString();
                    if(find(req_a.begin(), req_a.end(), attname) == req_a.end()) {
                        db_data.push_back(Tango::DbDatum(attname.c_str()));
                        req_a.push_back(attname);
                        if((*dit).containsKey(CuDType::Name))
                            req_p.push_back((*dit)[CuDType::Name].toString());
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
                    if((*dit).containsKey(CuDType::Name) )
                        name = (*dit)[CuDType::Name].toString();

                    if(name.length() > 0 && name.find("*") == std::string::npos)
                        db_data.push_back(Tango::DbDatum((*dit)[CuDType::Name].toString().c_str()));
                    else if(name.length() > 0)
                        dev_prop_wildcards[(*dit)[CuDType::Device].toString()] = name; // wildcard specified
                    else  {
                        dev_prop_wildcards[(*dit)[CuDType::Device].toString()] = "*";
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
            // if no CuDType::Name key is found, then get the list of class properties for the given class CuDType::Name
            for(std::unordered_map<std::string, std::list< CuData> >::const_iterator it = cprops.begin(); it != cprops.end(); ++it)
            {
                std::vector<std::string> cl_names; // list of class names to use with get_class_property_list
                Tango::DbData db_data;
                for(std::list<CuData>::const_iterator dit = it->second.begin(); dit != it->second.end(); ++dit) {
                    if((*dit).containsKey(CuDType::Name) && (*dit)[CuDType::Name].toString().length() > 0)
                        db_data.push_back(Tango::DbDatum((*dit)[CuDType::Name].toString().c_str()));
                    else if(!(*dit).containsKey(CuDType::Name)) {
                        cl_names.push_back((*dit)[CuDType::Class].toString());
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
        res[CuDType::Err] = d->error;
        if(d->message.length() > 0)
            res[CuDType::Message] = d->message;
        delete db;
    } // if !d->error (i.e. db != nullptr)
    return !d->error;
}

bool CuTangoWorld::db_get(const TSource &tsrc, CuData &res) {
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
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
        res[CuDType::Pattern] = p;  // res["pattern"]
        if(dnam.length() > 0)
            res[CuDType::Device] = dnam;
        if(tgh.length() > 0)
            res["tango_host"] = tgh;
        if(tsrc.getPoint().length() > 0)
            res[CuDType::Point] = tsrc.getPoint();
        if(tsrc.getFreePropNam().length() > 0)
            res[CuDType::Property] = tsrc.getFreePropNam();
        if(tsrc.getPropClassNam().length() > 0)
            res[CuDType::Class] = tsrc.getPropClassNam();

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
                res[CuDType::InType] = ci.in_type;
                res[CuDType::OutType] = ci.out_type;
                res[CuDType::InTypeStr] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>( ci.in_type));
                res[CuDType::OutTypeStr] = cmdArgTypeToDataFormat(static_cast<Tango::CmdArgType>( ci.out_type));
                res["cmd_in_desc"] = ci.in_type_desc;
                res["cmd_out_desc"] = ci.out_type_desc;
                r = std::vector<std::string> { "cmd_in_type", "cmd_out_type", "cmd_in_type_str",
                                             "cmd_out_type_str", "cmd_in_desc", "cmd_out_desc" };
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
                res[CuDType::Keys] = keys;
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
                res[CuDType::Keys] = keys;
            }
            break;
            case TSource::SrcDbDevProp: {
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
                res[CuDType::Keys] = keys;
            }
            break;
            }
            res[CuDType::Value] = r;
            d->error |= !r.size();
            if(r.size() == 0) {
                d->message = "not found";
            }
            d->message.size() == 0 ? res[CuDType::Message] = std::string("operation \"") + tsrc.getTypeName(t) + "\" successful" : res[CuDType::Message] = d->message;
            if(dev)
                delete dev;

        }  catch (const Tango::DevFailed &e) {
            d->error = true;
            d->message = strerror(e);
        }
        delete db;
    } // if(db != nullptr)
    res[CuDType::Err] = d->error;
    if(d->message.length() > 0)
        res[CuDType::Message] = d->message;
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
 *         CuData CuDType::DataFormatStr property.
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
 * CuData CuDType::DataFormatStr property
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

/*!
 * \brief return the source patterns associated to the tango domain
 * \return  a vector of strings with the source regex patterns
 */
std::vector<string> CuTangoWorld::srcPatterns() const {
    if(d->src_patterns.size() == 0) {
        d->src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+/.+");
        d->src_patterns.push_back("[A-Za-z0-9_\\-\\.\\$]+->.+");

        // support class property syntax, such as: hokuto:20000/TangoTest(ProjectTitle,Description),
        // hokuto:20000/TangoTest(*)
        d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\]+(?:[\\(A-Za-z0-9_\\-,\\)\\s*]+)");

        // free properties and device exported
        // tango://ken:20000/#Sequencer#TestList
        // ken:20000/#test/de*/*(*)
        //    d->src_patterns.push_back("(?:tango://){1,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\-\\.\\$#\\*/]+(?:\\(\\*\\)){0,1}");
        d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\-\\.\\$#\\*/]+(?:\\(\\*\\)){0,1}");
        // tango domain search [tango://]hokuto:20000/ or [tango://]hokuto:20000/*
        d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){1}[*]{0,1}");

        // support tango://host:PORT/a/b/c/d and tango://host:PORT/a/b/c->e
        // when CumbiaPool.guessBySrc needs to be used
        // with the two above only, sources like "tango://hokuto:20000/test/device/1/double_scalar"
        // or "hokuto:20000/test/device/1/double_scalar" are not identified
        d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+/.+");
        d->src_patterns.push_back("(?:tango://){0,1}(?:[A-Za-z0-9_\\-\\.:]+/){0,1}[A-Za-z0-9_\\\\-\\\\.\\\\$]+->.+");
    }
    return d->src_patterns;
}

Tango::DeviceData CuTangoWorld::toDeviceData(const CuVariant &arg,
                                             const CuData &info)
{
    bool type_match = false;
    d->error = false;
    if(d->message.length() > 0)  d->message.clear();
    long in_type = info[CuDType::InType].toLongInt();  // info["in_type"]
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
    if(d->message.length() > 0)  d->message.clear();
    long in_type = cmdinfo[CuDType::InType].toLongInt();  // cmdinfo["in_type"]
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
    if(d->message.length() > 0)  d->message.clear();
    bool ok;
    std::string attname = name;
    Tango::DeviceAttribute da;
    CuVariant::DataType t = arg.getType();
    int tango_type = attinfo[CuDType::DataType].toInt();
    Tango::AttrDataFormat tango_format = static_cast<Tango::AttrDataFormat>(attinfo[CuDType::DataFormat].toInt());
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
    if(d->message.length() > 0)  d->message.clear();
    int tango_type = attinfo[CuDType::DataType].toLongInt();
    Tango::AttrDataFormat tango_fmt = static_cast<Tango::AttrDataFormat>(attinfo[CuDType::DataFormat].toInt());
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
    data[CuDType::Time_ms] = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    data[CuDType::Time_us] = static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6;
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

/*!
 * \brief given a source, replace the tango host section with its FQDN name
 * \param src any source
 * \return a string with the FQDN tango host name
 *
 * if
 * - no tango host is found in src
 * - the detected tango host may be already a FQDN name (one or more `.'s are found)
 * then the method returns a string equal to the input string
 *
 * The returned string will leave intact the "tango://" protocol prefix in the source,
 * if present.
 */
std::string CuTangoWorld::make_fqdn_src(const string &src) const {
    int getai_ok = 0;
    std::string fusrc(src), tgho;
    if(d->message.length() > 0)  d->message.clear();
    // tango host regex: capture between optional tango:// and ":PORT"
    std::regex tghre("(?:tango://){0,1}(.*):\\d+/.*");
    std::smatch hma; // host match
    bool ma = std::regex_search(src, hma, tghre);
    if(ma && hma.size() > 1)
        tgho = hma[1].str();
    string::size_type end1 = tgho.find("."), end2;
    if(tgho.length() > 0 /*&& end1 == string::npos*/) { // exclude a possibly already FQDN name
        end2 = src.rfind(":"); // find last ':', to avoid matching semicolon in tango://
        //get host name without tango://
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM;
        // If hints.ai_flags includes the AI_CANONNAME flag, then the
        // ai_canonname field of the first of the addrinfo structures in the
        // returned list is set to point to the official name of the host.
        hints.ai_flags = AI_CANONNAME;
        struct addrinfo *result, *rp;
        getai_ok = getaddrinfo(tgho.c_str(), NULL, &hints, &result);
        if(getai_ok != 0) {
            d->message = __func__ + std::string(": getaddrinfo error resolving: ") + tgho + ": " + gai_strerror(getai_ok);
        }
        else if(result == nullptr) {
            d->message  = __func__ + std::string(": getaddrinfo did not return domain information for ") + tgho + ": " + gai_strerror(getai_ok);
        }
        for (rp = result; rp != NULL; rp = rp->ai_next) {
            if(rp->ai_canonname != nullptr) {
                fusrc = string(rp->ai_canonname) + src.substr(end2); // [tango://]full.host.name:PORT/tg/dev/nam/attribute
            }
        }
        if(getai_ok == 0 && result) {
            freeaddrinfo(result);
        }
    }
    else if(tgho.length() == 0)
        d->message = __func__ + std::string(" no tango host in source ") + src;
    else if(end1 != std::string::npos)
        d->message = __func__ + std::string(" possibly already FQDN name ") + tgho;
    d->error = getai_ok != 0;
    return src.find("tango://") == 0 ? "tango://" + fusrc : fusrc;
}

/*!
 * \brief prepend "tango://" protocol string to src if not already specified
 * \param src the input source
 * \return "tango://" + src, unless src already contains "tango://" at some point
 */
std::string CuTangoWorld::prepend_tgproto(const std::string &src) const {
    return src.find("tango://") == std::string::npos ? "tango://" + src : src;
}

void CuTangoWorld::orb_cleanup() const {
    Tango::ApiUtil::cleanup();
}
