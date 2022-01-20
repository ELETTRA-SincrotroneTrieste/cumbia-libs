#ifndef CUTANGO_WORLD_H
#define CUTANGO_WORLD_H

#include <string>
#include <cudata.h>
#include <cudataquality.h>
#include <tango.h>

class CuTangoWorldPrivate;
class CuActivity;
class TSource;

class CuTangoWorld
{
public:
    CuTangoWorld();

    virtual ~CuTangoWorld();

    std::string strerror(const Tango::DevFailed& e);
    std::string strerror(const Tango::DevErrorList &e);

    void fillThreadInfo(CuData &d, const CuActivity *a);

    void extractData(Tango::DeviceAttribute *p_da, CuData &d);
    void extractData(Tango::DeviceData *dd, CuData& d);

    Tango::DeviceData toDeviceData(const CuVariant &argins, const CuData& cmdinfo);
    Tango::DeviceData toDeviceData(const std::vector<std::string> &argins, const CuData &cmdinfo);

    Tango::DeviceAttribute toDeviceAttribute(const std::string& name, const CuVariant &argin, const CuData &attinfo);
    Tango::DeviceAttribute toDeviceAttribute(const string& aname, const std::vector<std::string> &argins, const CuData &attinfo);

    void fillFromAttributeConfig(const Tango::AttributeInfoEx &ai, CuData& d);
    void fillFromCommandInfo(const Tango::CommandInfo &cu, CuData& d);

    bool cmd_inout(Tango::DeviceProxy *dev,
              const std::string& cmd,
              Tango::DeviceData &din,
              bool has_argout,
              CuData& data);

    bool cmd_inout(Tango::DeviceProxy *dev,
              const std::string& cmd,
              CuData& data);

    bool read_att(Tango::DeviceProxy *dev, const string &attribute, CuData &res);

    bool read_atts(Tango::DeviceProxy *dev,
                   std::vector<string> &attnamlist, // <- not const because of Tango read_attributes
                   std::vector<CuData> &att_datalist,
                   std::vector<CuData> *reslist, int results_offset);

    bool write_att(Tango::DeviceProxy *dev,
                   const string &attnam,
                   const CuVariant& argins,
                   const CuData &point_info,
                   CuData &data);

    bool get_att_config(Tango::DeviceProxy *dev, const string &attribute, CuData& res, bool skip_read_att = false);
    bool get_command_info(Tango::DeviceProxy *dev, const string &cmd, CuData& cmd_info);
    bool get_att_props(Tango::DeviceProxy *dev, const string &attribute, CuData& res, const std::vector<std::string> props);
    bool get_properties(const std::vector<CuData> &in_list, CuData& res, const string &dbhost = "");
    bool db_get(const TSource& tsrc, CuData& res);

    bool source_valid(const std::string& src);
    const char *source_valid_pattern() const;

    std::string getLastMessage() const;

    std::string formatToStr(Tango::AttrDataFormat f) const;
    std::string cmdArgTypeToDataFormat(Tango::CmdArgType t)  const;

    bool error() const;
    void setSrcPatterns(const std::vector<std::string> &pat_regex);
    std::vector<std::string> srcPatterns() const;

    Tango::Database *getTangoDb(const string &dbhost);

    void putDateTime(const Tango::TimeVal& ttv, CuData& data);
    std::string dateTimeToStr(time_t *tp) const;
    CuDataQuality toCuQuality(Tango::AttrQuality q) const;

    std::string make_fqdn_src(const std::string& src) const;
    std::string prepend_tgproto(const std::string& src) const;

private:
    CuTangoWorldPrivate *d;
};

#endif // UTILS_H
