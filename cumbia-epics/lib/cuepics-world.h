#ifndef CUEPICS_WORLD_H
#define CUEPICS_WORLD_H

#include <string>
#include <cudata.h>
#include <cadef.h>

/*************************************************************************\
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
 *
 */
#include <epicsTime.h>

/* Convert status and severity to strings */
#define stat_to_str(stat)                                       \
        ((stat) >= 0 && (stat) <= (signed)lastEpicsAlarmCond) ? \
        epicsAlarmConditionStrings[stat] : "??"

#define sevr_to_str(stat)                                       \
        ((stat) >= 0 && (stat) <= (signed)lastEpicsAlarmSev) ?  \
        epicsAlarmSeverityStrings[stat] : "??"

#define stat_to_str_unsigned(stat)              \
        ((stat) <= lastEpicsAlarmCond) ?        \
        epicsAlarmConditionStrings[stat] : "??"

#define sevr_to_str_unsigned(stat)              \
        ((stat) <= lastEpicsAlarmSev) ?         \
        epicsAlarmSeverityStrings[stat] : "??"

/* The different versions are necessary because stat and sevr are
 * defined unsigned in CA's DBR_STSACK structure and signed in all the
 * others. Some compilers generate warnings if you check an unsigned
 * being >=0 */


#define DEFAULT_CA_PRIORITY 0  /* Default CA priority */
#define DEFAULT_CA_TIMEOUT 1.0     /* Default CA timeout */

class CuMonitorActivity;

/* Structure representing one PV (= channel) */
typedef struct
{
    char name[256];
    chid  ch_id;
    long  dbfType;
    long  dbrType;
    unsigned long nElems;       // True length of data in value
    unsigned long reqElems;     // Requested length of data
    int status, ctrl_status;
    void* value;
    epicsTimeStamp tsPreviousC;
    epicsTimeStamp tsPreviousS;
    char firstStampPrinted;
    char onceConnected;
    CuMonitorActivity *monitor_activity;
} CuPV;


extern int tsSrcServer;     /* Timestamp source flag (-t option) */
extern int tsSrcClient;     /* Timestamp source flag (-t option) */
extern int enumAsNr;        /* Used for -n option (get DBF_ENUM as number) */
extern int charArrAsStr;    /* used for -S option - treat char array as (long) string */
extern double caTimeout;    /* Wait time default (see -w option) */
extern char dblFormatStr[]; /* Format string to print doubles (see -e -f option) */
extern char fieldSeparator; /* Output field separator */
extern capri caPriority;    /* CA priority */

extern char *val2str (const void *v, unsigned type, int index);
extern char *dbr2str (const void *value, unsigned type);
extern void print_time_val_sts (CuPV *CuPV, unsigned long reqElems);
extern int  create_pvs (CuPV *pvs, int nPvs, caCh *pCB );
extern int  connect_pvs (CuPV *pvs, int nPvs );



class CuEpicsWorldPrivate;
class CuActivity;

class CuEpicsWorld
{
public:
    CuEpicsWorld();

    virtual ~CuEpicsWorld();

    std::string getLastMessage() const;

    void fillThreadInfo(CuData &d, const CuActivity *a);

    bool error() const;

    bool source_valid(const std::string &s) const;

    void extractData(const CuPV *_pv, CuData &da) const;

    std::string extractException(struct exception_handler_args excargs, CuData& d)const;

    template <class T> void putTimestamp(void* ep_data, CuData& d) const;

    template <class T> void putCtrlData(void* ep_data, CuData& d) const;

    void setSrcPatterns(const std::vector<std::string> &p);

    std::vector<std::string> srcPatterns() const;

private:

    CuEpicsWorldPrivate *d;

};

#endif // UTILS_H
