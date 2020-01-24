#include "cuepics-world.h"
#include "cuepics-world-config.h"
#include <cudataquality.h>
#include <cumacros.h>
#include <regex>
#include <vector>
#include <string>
#include <regex>


/*************************************************************************\
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* Copyright (c) 2002 Berliner Elektronenspeicherringgesellschaft fuer
*     Synchrotronstrahlung.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/*
 *  Author: Ralph Lange (BESSY)
 *
 *  Modification History
 *  2009/03/31 Larry Hoff (BNL)
 *     Added field separators
 *  2009/04/01 Ralph Lange (HZB/BESSY)
 *     Added support for long strings (array of char) and quoting of nonprintable characters
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <alarm.h>
#include <epicsTime.h>
#include <epicsString.h>

char dblFormatStr[30] = "%g"; /* Format string to print doubles (-efg options) */
char timeFormatStr[30] = "%Y-%m-%d %H:%M:%S.%06f"; /* Time format string */
char fieldSeparator = ' ';          /* OFS default is whitespace */

int enumAsNr = 0;        /* used for -n option - get DBF_ENUM as number */
int charArrAsStr = 0;    /* used for -S option - treat char array as (long) string */
double caTimeout = 1.0;  /* wait time default (see -w option) */
capri caPriority = DEFAULT_CA_PRIORITY;  /* CA Priority */

#define TIMETEXTLEN 28          /* Length of timestamp text buffer */


/*+**************************************************************************
 *
 * Function:	dbr2str
 *
 * Description:	Print (convert) additional information contained in dbr_...
 *
 * Arg(s) In:	value  -  Pointer to dbr_... structure
 *              type   -  Numeric dbr type
 *
 * Return(s):	Pointer to static output string
 *
 **************************************************************************-*/

/* Definitions for sprintf format strings and matching argument lists */

#define FMT_TIME                                \
    "    Timestamp:        %s"

#define ARGS_TIME(T)                            \
    timeText

#define FMT_STS                                 \
    "    Status:           %s\n"                \
    "    Severity:         %s"

#define ARGS_STS(T)                             \
    stat_to_str(((struct T *)value)->status),   \
    sevr_to_str(((struct T *)value)->severity)

#define ARGS_STS_UNSIGNED(T)                    \
    stat_to_str_unsigned(((struct T *)value)->status),  \
    sevr_to_str_unsigned(((struct T *)value)->severity)

#define FMT_ACK                                 \
    "    Ack transient?:   %s\n"                \
    "    Ack severity:     %s"

#define ARGS_ACK(T)                             \
    ((struct T *)value)->ackt ? "YES" : "NO",   \
    sevr_to_str_unsigned(((struct T *)value)->acks)

#define FMT_UNITS                               \
    "    Units:            %s"

#define ARGS_UNITS(T)                           \
    ((struct T *)value)->units

#define FMT_PREC                                \
    "    Precision:        %d"

#define ARGS_PREC(T)                            \
    ((struct T *)value)->precision

#define FMT_GR(FMT)                             \
    "    Lo disp limit:    " #FMT "\n"          \
    "    Hi disp limit:    " #FMT "\n"          \
    "    Lo alarm limit:   " #FMT "\n"          \
    "    Lo warn limit:    " #FMT "\n"          \
    "    Hi warn limit:    " #FMT "\n"          \
    "    Hi alarm limit:   " #FMT

#define ARGS_GR(T,F)                            \
    (F)((struct T *)value)->lower_disp_limit,   \
    (F)((struct T *)value)->upper_disp_limit,   \
    (F)((struct T *)value)->lower_alarm_limit,  \
    (F)((struct T *)value)->lower_warning_limit, \
    (F)((struct T *)value)->upper_warning_limit, \
    (F)((struct T *)value)->upper_alarm_limit

#define FMT_CTRL(FMT)                           \
    "    Lo ctrl limit:    " #FMT "\n"          \
    "    Hi ctrl limit:    " #FMT

#define ARGS_CTRL(T,F)                          \
    (F)((struct T *)value)->lower_ctrl_limit,   \
    (F)((struct T *)value)->upper_ctrl_limit


/* Definitions for the actual sprintf calls */

#define PRN_DBR_STS(T)                          \
    sprintf(str,                                \
    FMT_STS,                            \
    ARGS_STS(T))

#define PRN_DBR_TIME(T)                                         \
    epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr,   \
    &(((struct T *)value)->stamp));         \
    sprintf(str,                                                \
    FMT_TIME "\n" FMT_STS,                              \
    ARGS_TIME(T), ARGS_STS(T))

#define PRN_DBR_GR(T,F,FMT)                             \
    sprintf(str,                                        \
    FMT_STS "\n" FMT_UNITS "\n" FMT_GR(FMT),    \
    ARGS_STS(T), ARGS_UNITS(T), ARGS_GR(T,F))

#define PRN_DBR_GR_PREC(T,F,FMT)                                        \
    sprintf(str,                                                        \
    FMT_STS "\n" FMT_UNITS "\n" FMT_PREC "\n" FMT_GR(FMT),      \
    ARGS_STS(T), ARGS_UNITS(T), ARGS_PREC(T), ARGS_GR(T,F))

#define PRN_DBR_CTRL(T,F,FMT)                                                   \
    sprintf(str,                                                                \
    FMT_STS "\n" FMT_UNITS "\n" FMT_GR(FMT) "\n" FMT_CTRL(FMT),         \
    ARGS_STS(T), ARGS_UNITS(T), ARGS_GR(T,F),    ARGS_CTRL(T,F))

#define PRN_DBR_CTRL_PREC(T,F,FMT)                                                      \
    sprintf(str,                                                                        \
    FMT_STS "\n" FMT_UNITS "\n" FMT_PREC "\n" FMT_GR(FMT) "\n" FMT_CTRL(FMT),   \
    ARGS_STS(T), ARGS_UNITS(T), ARGS_PREC(T), ARGS_GR(T,F),    ARGS_CTRL(T,F))

#define PRN_DBR_STSACK(T)                       \
    sprintf(str,                                \
    FMT_STS "\n" FMT_ACK,               \
    ARGS_STS_UNSIGNED(T), ARGS_ACK(T))

#define PRN_DBR_X_ENUM(T)                               \
    n = ((struct T *)value)->no_str;                    \
    PRN_DBR_STS(T);                                     \
    sprintf(str+strlen(str),                            \
    "\n    Enums:            (%2d)", n);    \
    for (i=0; i<n; i++)                                 \
    sprintf(str+strlen(str),                        \
    "\n                      [%2d] %s", i,  \
    ((struct T *)value)->strs[i]);


/* Make a good guess how long the dbr_... stuff might get as worst case */
#define DBR_PRINT_BUFFER_SIZE                                           \
    50                        /* timestamp */                         \
    + 2 * 30                    /* status / Severity */                 \
    + 2 * 30                    /* acks / ackt */                       \
    + 20 + MAX_UNITS_SIZE       /* units */                             \
    + 30                        /* precision */                         \
    + 6 * 45                    /* graphic limits */                    \
    + 2 * 45                    /* control limits */                    \
    + 30 + (MAX_ENUM_STATES * (20 + MAX_ENUM_STRING_SIZE)) /* enums */  \
    + 50                        /* just to be sure */

//char *dbr2str (const void *value, unsigned type)
//{
//    static char str[DBR_PRINT_BUFFER_SIZE];
//    char timeText[TIMETEXTLEN];
//    int n, i;

//    switch (type) {
//    case DBR_STRING:   /* no additional information for basic data types */
//    case DBR_INT:
//    case DBR_FLOAT:
//    case DBR_ENUM:
//    case DBR_CHAR:
//    case DBR_LONG:
//    case DBR_DOUBLE: break;

//    case DBR_CTRL_STRING:       /* see db_access.h: not implemented */
//    case DBR_GR_STRING:         /* see db_access.h: not implemented */
//    case DBR_STS_STRING:  PRN_DBR_STS(dbr_sts_string); break;
//    case DBR_STS_SHORT:   PRN_DBR_STS(dbr_sts_short); break;
//    case DBR_STS_FLOAT:   PRN_DBR_STS(dbr_sts_float); break;
//    case DBR_STS_ENUM:    PRN_DBR_STS(dbr_sts_enum); break;
//    case DBR_STS_CHAR:    PRN_DBR_STS(dbr_sts_char); break;
//    case DBR_STS_LONG:    PRN_DBR_STS(dbr_sts_long); break;
//    case DBR_STS_DOUBLE:  PRN_DBR_STS(dbr_sts_double); break;

//    case DBR_TIME_STRING: PRN_DBR_TIME(dbr_time_string); break;
//    case DBR_TIME_SHORT:  PRN_DBR_TIME(dbr_time_short); break;
//    case DBR_TIME_FLOAT:  PRN_DBR_TIME(dbr_time_float); break;
//    case DBR_TIME_ENUM:   PRN_DBR_TIME(dbr_time_enum); break;
//    case DBR_TIME_CHAR:   PRN_DBR_TIME(dbr_time_char); break;
//    case DBR_TIME_LONG:   PRN_DBR_TIME(dbr_time_long); break;
//    case DBR_TIME_DOUBLE: PRN_DBR_TIME(dbr_time_double); break;

//    case DBR_GR_CHAR:
//        PRN_DBR_GR(dbr_gr_char, char, %8d); break;
//    case DBR_GR_INT:
//        PRN_DBR_GR(dbr_gr_int,  int,  %8d); break;
//    case DBR_GR_LONG:
//        PRN_DBR_GR(dbr_gr_long, long int, %8ld); break;
//    case DBR_GR_FLOAT:
//        PRN_DBR_GR_PREC(dbr_gr_float,  float, %g); break;
//    case DBR_GR_DOUBLE:
//        PRN_DBR_GR_PREC(dbr_gr_double, double, %g); break;
//    case DBR_GR_ENUM:
//        PRN_DBR_X_ENUM(dbr_gr_enum); break;
//    case DBR_CTRL_CHAR:
//        PRN_DBR_CTRL(dbr_ctrl_char,   char,     %8d); break;
//    case DBR_CTRL_INT:
//        PRN_DBR_CTRL(dbr_ctrl_int,    int,      %8d); break;
//    case DBR_CTRL_LONG:
//        PRN_DBR_CTRL(dbr_ctrl_long,   long int, %8ld); break;
//    case DBR_CTRL_FLOAT:
//        PRN_DBR_CTRL_PREC(dbr_ctrl_float,  float,  %g); break;
//    case DBR_CTRL_DOUBLE:
//        PRN_DBR_CTRL_PREC(dbr_ctrl_double, double, %g); break;
//    case DBR_CTRL_ENUM:
//        PRN_DBR_X_ENUM(dbr_ctrl_enum); break;
//    case DBR_STSACK_STRING:
//        PRN_DBR_STSACK(dbr_stsack_string); break;
//    default : strcpy (str, "can't print data type");
//    }
//    return str;
//}

class CuEpicsWorldPrivate
{
public:
    bool error;
    std::string message;
    CuEpicsWorldConfig t_world_conf;
    std::vector<std::string> src_patterns;
};

CuEpicsWorld::CuEpicsWorld()
{
    d = new CuEpicsWorldPrivate();
    d->src_patterns.push_back("[A-Za-z0-9_\\./\\:\\-\\+]+");
}

CuEpicsWorld::~CuEpicsWorld()
{
    delete d;
}

void CuEpicsWorld::fillThreadInfo(CuData &dat, const CuActivity* a)
{
    char info[32];
    sprintf(info, "0x%lx", pthread_self());
    dat["worker_thread"] = std::string(info);
    sprintf(info, "%p", a);
    dat["worker_activity"] = std::string(info);
}

bool CuEpicsWorld::source_valid(const std::string &s) const
{
     return std::regex_match(s, std::regex("[A-Za-z0-9_\\./\\:\\-\\+]+"));
}

void CuEpicsWorld::extractData(const CuPV *pv, CuData &da) const
{
    size_t i;
    std::string msg;
    bool error = false;
    void *val_ptr = dbr_value_ptr(pv->value, pv->dbrType);

    da["data_type"] = pv->dbrType;
    da["data_type_str"] = dbr_type_to_text(pv->dbrType);

    if(pv->nElems == 1) /* scalar */
        da["data_format_str"] = "scalar";
    else if(pv->nElems > 1)
        da["data_format_str"] = "vector";
    else
        da["data_format_str"] = "invalid";

    da["writable"] = static_cast<int>(ca_write_access(pv->ch_id));

    /* event type can be   (ctrl) or value update */
    if(dbr_type_is_CTRL(pv->dbrType))
    {
        da["type"] = "property";
        da["dim_x"] = static_cast<long int>(pv->nElems);
        /* metadata */
        switch (pv->dbrType)
        {
        case DBR_CTRL_DOUBLE:
            putCtrlData<dbr_ctrl_double>(pv->value, da);
            break;
        case DBR_CTRL_SHORT:
            putCtrlData<dbr_ctrl_short>(pv->value, da);
            break;
        case DBR_CTRL_FLOAT:
            putCtrlData<dbr_ctrl_float>(pv->value, da);
            break;
        case DBR_CTRL_ENUM:
            //putCtrlData<dbr_ctrl_enum>(pv->value, da);
            break;
        case DBR_CTRL_CHAR:
            putCtrlData<dbr_ctrl_char>(pv->value, da);
            break;
        case DBR_CTRL_LONG:
            putCtrlData<dbr_ctrl_long>( pv->value, da);
            break;
        case DBR_CTRL_STRING:
        default:
            msg = "CuEpicsWorld.extractData: unsupported DBR_CTRL type " + std::to_string(pv->dbrType);
            error = true;
        }
    }
    else
    {
        da["type"] = "value";

        if(pv->nElems == 1) /* scalar */
        {
            switch (pv->dbrType)
            {
            case DBR_TIME_STRING:
                da["value"] = std::string( ((dbr_string_t *) val_ptr)[0]);
                putTimestamp<dbr_time_string>(pv->value, da);
                break;
            case DBR_TIME_SHORT:
                da["value"] = ((dbr_short_t*) val_ptr)[0];
                putTimestamp<dbr_time_short>(pv->value, da);
                break;
            case DBR_TIME_FLOAT:
                da["value"] = ((dbr_float_t*) val_ptr)[0];
                putTimestamp<dbr_time_float>(pv->value, da);
                break;
            case DBR_TIME_ENUM:
                da["value"] = ((dbr_enum_t *) val_ptr)[0];
                putTimestamp<dbr_time_enum>(pv->value, da);
                break;
            case DBR_TIME_CHAR:
                da["value"] = ((dbr_char_t*) val_ptr)[0];
                putTimestamp<dbr_time_char>(pv->value, da);
                break;
            case DBR_TIME_LONG:
                da["value"] = ((dbr_long_t*) val_ptr)[0];
                putTimestamp<dbr_time_long>( pv->value, da);
                break;
            case DBR_TIME_DOUBLE:
                da["value"] = ((dbr_double_t*) val_ptr)[0];
                putTimestamp<dbr_time_double>( pv->value, da);
                break;
            default:
                error = true;
                msg = "CuEpicsWorld.extractData: cannot convert type %d" + std::to_string(pv->dbrType);
                break;
            }
        }
        else
        {
            if(pv->dbrType == DBR_TIME_STRING)
            {
                std::vector<std::string> vs;
                for (i=0; i < pv->nElems; ++i) {
                    vs.push_back(std::string(((dbr_string_t*) val_ptr)[i]));
                }
                putTimestamp<dbr_time_string>( pv->value, da);
                da["value"] = vs;
            }
            else if(pv->dbrType == DBR_TIME_SHORT)
            {
                std::vector<short> vs;
                for (i=0; i < pv->nElems; ++i) {
                    vs.push_back(((dbr_short_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_short>( pv->value, da);
                da["value"] = vs;
            }
            else if(pv->dbrType ==  DBR_TIME_FLOAT)
            {
                std::vector<float> vf;
                for (i=0; i < pv->nElems; ++i) {
                    vf.push_back(((dbr_float_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_float>( pv->value, da);
                da["value"] = vf;
            }
            else if(pv->dbrType ==  DBR_TIME_ENUM)
            {
                std::vector<int> venum;
                for (i=0; i < pv->nElems; ++i) {
                    venum.push_back(((dbr_enum_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_enum>(pv->value, da);
                da["value"] = venum;
            }
            else if(pv->dbrType ==  DBR_TIME_CHAR)
            {
                std::vector<float> vch;
                for (i=0; i < pv->nElems; ++i) {
                    vch.push_back(((dbr_char_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_char>(pv->value, da);
                da["value"] = vch;
            }
            else if(pv->dbrType ==  DBR_TIME_LONG)
            {
                std::vector<float> vlo;
                for (i=0; i < pv->nElems; ++i) {
                    vlo.push_back(((dbr_long_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_long>(pv->value, da);
                da["value"] = vlo;
            }
            else if(pv->dbrType ==  DBR_TIME_DOUBLE)
            {
                std::vector<double> vdo;
                for (i=0; i < pv->nElems; ++i) {
                    vdo.push_back(((dbr_double_t*) val_ptr)[i]);
                }
                putTimestamp<dbr_time_double>(pv->value, da);
                da["value"] = vdo;
            }
            else
            {
                error = true;
                msg = "CuEpicsWorld.extractData: cannot convert type %d" + std::to_string(pv->dbrType);
            }
        }
    }

    if(!error)
        msg = da["src"].toString() + " [" + da["timestamp_str"].toString() + "] STAT: " + da["status"].toString()
                + " SEV: " + da["severity"].toString() + " QUALITY: " + da["quality_string"].toString();
    da["err"] = error;
    da["msg"] = msg;
}

/** \brief fills the input CuData with exception information.
 *
 * @return a string representation of the available error information
 */
std::string CuEpicsWorld::extractException(exception_handler_args excargs, CuData &da) const
{
    std::string s;
    da["data_type"] = excargs.type;
    da["data_type_str"] = dbr_type_to_text(excargs.type);
    s += "data type: " + da["data_type_str"].toString() + "[" + std::to_string(excargs.type) + "]";

    da["status"] = std::to_string(excargs.stat);
    s += "\nstatus:\t\t" + da["status"].toString();

    da["op"] = excargs.op;
    s += "\noperation:\t\t" + std::to_string(excargs.op);

    if(excargs.pFile)
    {
        da["file"] = std::string(excargs.pFile);
        da["file_lineno"] = excargs.lineNo;
        s += "\nfile:\t\t\"" + da["file"].toString() + "\" @line " + std::to_string(excargs.lineNo);
    }
    if(excargs.ctx)
    {
        da["ctx"] = std::string(excargs.ctx);
        s += "\ncontext:\t\t" + da["ctx"].toString();
    }
    return s;
}

bool CuEpicsWorld::m_ep_caget(CuPV *pv, CuData &res, CaGetMode cagetMode, double timeout)
{
    bool success = true;
    int result;
    char msg[256];
    pv->dbfType = ca_field_type(pv->ch_id);
    pv->nElems = ca_element_count(pv->ch_id);

    if (dbr_type_is_ENUM(pv->dbrType) && cagetMode == DbrTime)
        pv->dbrType = DBR_TIME_STRING;
    else if(cagetMode == DbrTime)
        pv->dbrType = dbf_type_to_DBR_TIME(pv->dbfType);
    else if(cagetMode == DbrCtrl)
        pv->dbrType = dbf_type_to_DBR_CTRL(pv->dbfType);

    // when pv->dbrType is determined, allocate space for pv->value
    pv->realloc_value();

    if(!pv->value) {
        success = false;
        snprintf(msg, 256, "CuEpicsWorld.m_ep_caget error allocating %d bytes for \"%s\" value",
                 dbr_size_n(pv->dbrType, pv->nElems), pv->name);
        res["msg"] = std::string(msg);
    }
    else { // memory alloc ok for value

        if(ca_state(pv->ch_id) == cs_conn) {
            pv->onceConnected = 1;
            result = ca_array_get(pv->dbrType, pv->nElems, pv->ch_id, pv->value);
            pv->status = result;
        }
        else {
            pv->status = ECA_DISCONN;
            success = false;
        }
        result = ca_pend_io(timeout);
        if(result == ECA_TIMEOUT) {
            success = false;
            snprintf(msg, 256, "CuEpicsWorld.m_ep_caget timeout (>%f seconds) while reading \"%s\"",
                     timeout, pv->name);
            res["msg"] = std::string(msg);
        }

    }
    res["err"] = !success;
    if(success) {
        time_t now;
        time(&now);
        snprintf(msg, 256, " successfully read \"%s\"", pv->name);
        res["msg"] = m_get_timestamp() + std::string(msg);
    }
    return true;
}

// map from epics include/alarm.h
// typedef enum {
// epicsSevNone = NO_ALARM,
// epicsSevMinor,
// epicsSevMajor,
// epicsSevInvalid,
// ALARM_NSEV
// } epicsAlarmSeverity;
CuDataQuality CuEpicsWorld::m_setQuality(int sev, const CuData &dat) const
{
    CuDataQuality q;;
    // invalid quality if error
    if(dat["err"].toBool())
        return CuDataQuality(CuDataQuality::Invalid);

    if(sev == epicsSevMinor) {
        q.set(CuDataQuality::Warning);
    }
    else if(sev == epicsSevMajor)
        q.set(CuDataQuality::Alarm);

    return q;
}

std::string CuEpicsWorld::m_get_timestamp()
{
    char outstr[200];
    time_t t;
    struct tm *tmp;
    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL)
        return std::string("localtime error");

   strftime(outstr, sizeof(outstr), "%Y-%m-%d %H:%M:%S", tmp);
   return std::string(outstr);
}

void CuEpicsWorld::caget(const std::string& src, CuData &prop_res,  double timeout)
{
    bool err = false;
    char msg[256] = "";
    int result = ca_context_create(ca_disable_preemptive_callback);
    CuData value_res(prop_res); // copy token information
    if(result != ECA_NORMAL) {
        snprintf(msg, 256, "CuEpicsWorld.caget: CA error %s occurred while trying to start channel access for \"%s\"",
                 ca_message(result), src.c_str());
    }
    else {
        CuPV pv(src.c_str());
        result = connect_pvs(&pv, 1);

        if(result != 0) { // error
            snprintf(msg, 256, "CuEpicsWorld.caget error connecting \"%s\"", src.c_str());
        }
        else  {  // connected
            // 1. get property (dbr ctrl)
            bool success = m_ep_caget(&pv, prop_res,  DbrTime, timeout); // to get timestamp
            if(success) {
                extractData(&pv, prop_res);
                success = m_ep_caget(&pv, prop_res, DbrCtrl, timeout);
                if(success) {
                    extractData(&pv, prop_res);
                    if(ca_write_access(pv.ch_id)) {
                        prop_res["w_value"] = prop_res["value"];
                    }
                }
            }
        }
        ca_context_destroy();
        // pv is destroyed when out of scope and value is freed by CuPV's destructor
    }
    err = strlen(msg) > 0;
    if(err) {
        prop_res["err"] = value_res["err"] = err;
        prop_res["msg"] = value_res["msg"] = std::string(msg);
    } // otherwise leave "err" and "msg" as compiled by extractData
}

void CuEpicsWorld::setSrcPatterns(const std::vector<std::string> &p)
{
    d->src_patterns = p;
}

std::vector<std::string> CuEpicsWorld::srcPatterns() const
{
    return d->src_patterns;
}


template<class T>
void CuEpicsWorld::putTimestamp(void* ep_data, CuData &dt) const
{
    char timeText[TIMETEXTLEN];
    epicsTimeStamp ts = static_cast<T *>(ep_data)->stamp;
    struct timeval tv;
    long int tsms;
    epicsTimeToTimeval(&tv, &ts);
    tsms = tv.tv_sec * 1000.0 + ts.nsec / 1e6;
    dt["timestamp_ms"] = tsms;
    dt["timestamp_ns"] = static_cast<double>(tv.tv_sec) +  static_cast<double>(ts.nsec) * 1e-9;
    epicsTimeToStrftime(timeText, TIMETEXTLEN, "%Y-%m-%d %H:%M:%S", &ts);
    dt["timestamp_str"] = timeText;

    int stat = (static_cast<T *>(ep_data)->status);
    if((stat) >= 0 && (stat) <= (signed)lastEpicsAlarmCond)
        //dt["status"] = std::string(epicsAlarmConditionStrings[stat])
        dt["status"] = stat;
    else
        dt["status"] = "?";

    int sev = (static_cast<T *>(ep_data)->severity);

    // set an engine independent "quality" value, depending on severity
    // and on the "err" value in dt
    CuDataQuality dq = m_setQuality(sev, dt);
    dt["quality"] = dq.toInt();
    dt["quality_color"] = dq.color();
    dt["quality_string"] = dq.name();

    if((sev) >= 0 && (sev) <= (signed)lastEpicsAlarmSev) {
        //dt["severity"] = std::string(epicsAlarmSeverityStrings[sev]);
        dt["severity"] = sev;
    }
    else
        dt["severity"] = "?";
}

template<class T>
void CuEpicsWorld::putCtrlData(void *ep_data, CuData &dt) const
{
    dt["display_unit"] = std::string(static_cast<T *>(ep_data)->units);
    dt["max"] = static_cast<T *>(ep_data)->upper_disp_limit;
    dt["min"] = static_cast<T *>(ep_data)->lower_disp_limit;
    dt["upper_alarm_limit"] = static_cast<T *>(ep_data)->upper_alarm_limit;
    dt["upper_warning_limit"] = static_cast<T *>(ep_data)->upper_warning_limit;
    dt["lower_warning_limit"] = static_cast<T *>(ep_data)->lower_warning_limit;
    dt["lower_alarm_limit"] = static_cast<T *>(ep_data)->lower_alarm_limit;
    dt["upper_ctrl_limit"] = static_cast<T *>(ep_data)->upper_ctrl_limit;
    dt["lower_ctrl_limit"] = static_cast<T *>(ep_data)->lower_ctrl_limit;
}

/*+**************************************************************************
 *
 * Function:	connect_pvs
 *
 * Description:	Connects an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs connected
 *                  1  -  Some PV(s) not connected
 *
 **************************************************************************-*/


int CuEpicsWorld::connect_pvs (CuPV* pvs, int nPvs)
{
    int returncode = create_pvs ( pvs, nPvs, 0);
    if ( returncode == 0 ) {
        /* Wait for channels to connect */
        int result = ca_pend_io (caTimeout);
        if (result == ECA_TIMEOUT)
        {
            if (nPvs > 1)
            {
                fprintf(stderr, "Channel connect timed out: some PV(s) not found.\n");
            } else {
                fprintf(stderr, "Channel connect timed out: '%s' not found.\n",
                        pvs[0].name);
            }
            returncode = 1;
        }
    }
    return returncode;
}


/*+**************************************************************************
 *
 * Function:	create_pvs
 *
 * Description:	Creates an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *              pCB   -  Connection state change callback
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs created
 *                  1  -  Some PV(s) not created
 *
 **************************************************************************-*/

int CuEpicsWorld::create_pvs (CuPV* pvs, int nPvs, caCh *pCB)
{
    int n;
    int result;
    int returncode = 0;
    /* Issue channel connections */
    for (n = 0; n < nPvs; n++) {
        result = ca_create_channel (pvs[n].name,
                                    pCB,
                                    &pvs[n],
                                    caPriority,
                                    &pvs[n].ch_id);
        if (result != ECA_NORMAL) {
            fprintf(stderr, "CA error %s occurred while trying "
                            "to create channel '%s'.\n", ca_message(result), pvs[n].name);
            pvs[n].status = result;
            returncode = 1;
        }
    }

    return returncode;
}


CuPV::CuPV(const char *nam)
{
    strncpy(name, nam, 256);
    ch_id = NULL;
    dbfType = dbrType = -1;
    nElems = 0;       // True length of data in value
    reqElems = 0;     // Requested length of data
    value = NULL;
    status = ctrl_status = -1;
    onceConnected = 0;
    monitor_activity = NULL;
}

CuPV::~CuPV()
{
    pdelete("CuPV %p", this);
    if(value)
        free(value);
}

void CuPV::realloc_value()
{
    if(value)
        free(value);
    value = calloc(1, dbr_size_n(dbrType, nElems));
}
