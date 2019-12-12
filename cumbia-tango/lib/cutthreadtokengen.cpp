#include "cutthreadtokengen.h"
#include <map>

class CuTThreadTokenGenPrivate {
public:
    // token map: map device name to
    std::map <std::string, int> tkmap, user_map;
    int pool_siz, count;
    std::string thread_tok_prefix;
};

/*!
 * \brief CuTThreadTokenGen::CuTThreadTokenGen constructor
 *
 * Build a Tango thread token generator. A *thread token* determines how threads are grouped
 * in Cumbia. When Cumbia::registerActivity is called, the thread token will assign an activity
 * to the thread with the given token.
 *
 * \param pool_siz the desired upper limit on the thread count.
 * \param tok_prefix a string prefix to prepend to the thread number in the token returned by
 *        generate
 *
 * @see CuTThreadTokenGen::generate
 * @see Cumbia::setThreadTokenGenerator
 * @see Cumbia::threadToken
 * @see Cumbia::registerActivity
 */
CuTThreadTokenGen::CuTThreadTokenGen(int pool_siz, const std::string& tok_prefix) {
    d = new CuTThreadTokenGenPrivate;
    d->pool_siz = pool_siz;
    d->count = 0;
    d->thread_tok_prefix = tok_prefix;
}

CuTThreadTokenGen::~CuTThreadTokenGen() {
    delete d;
}

/*!
 * \brief This function implements the CuThreadTokenI::generate method and is invoked by Cumbia::threadToken
 *        function to provide a token (in the form of CuData) to be used to group threads together.
 *
 * \par User defined mapping
 * If a user defined map has been defined by calling CuTThreadTokenGen::map, it is used to group threads accordingly.
 * In that case, the defined thread pool size is ignored.
 *
 * \par Automatic token generation
 * If the user defined map is empty and the thread pool size is greater than zero, automatic device grouping
 * into threads takes place, so that:
 * \li sources with the same device belong to the same thread
 * \li the total number of threads is at most threadPoolSize.
 *
 * If the *in* parameter does not contain the "device" key, it is returned as is.
 * If the *device* value in the input argument is not in the user map (i.e. the user
 * only partially defines the device/thread association), the input data is returned as is. This
 * can result in an additional thread, since threadPoolSize is ignored when the user map size
 * is greater than zero.
 *
 * \param in
 * \return
 */
CuData CuTThreadTokenGen::generate(const CuData &in) {
    if(in.containsKey("device") && d->pool_siz > 0) {
        const std::string dev = in["device"].toString();
        int thr_no;
        if(d->user_map.count(dev) > 0) {
            thr_no = d->user_map[dev];
        }
        else {
            if(d->tkmap.count(dev) > 0) // group by device
                thr_no = d->tkmap[dev];
            else { // new dev: see if pool siz allows for a new thread otherwise reuse one
                ++d->count <= d->pool_siz ? thr_no = d->count : thr_no = d->count % d->pool_siz + 1;
                d->tkmap[dev] = thr_no;
            }
        }
        return CuData("th_tok", d->thread_tok_prefix + std::to_string(thr_no));
    }
    return in;
}

/*!
 * \brief User defined associations between device names and thread number
 * \param device the device name
 * \param which the thread, as integer, to associate to the given device
 *
 * \note If the user defined map is not empty, its elements only are used by the generate function.
 *       In that case, no automatic generation will take place and if a device is not in the user map
 *       the token will be left untouched.
 *
 * @see generate
 */
void CuTThreadTokenGen::map(const std::string &device, int which)
{
    d->user_map[device] = which;
}

/*!
 * \brief CuTThreadTokenGen::which_thread returns which thread has been assigned to the given device
 * \param device the name of the Tango device
 *
 * \return an integer representing which thread has been assigned to the given device
 *
 * The user defined map is searched first, the automatically generated map afterwards.
 * If the device has not been mapped, -1 is returned.
 */
int CuTThreadTokenGen::which_thread(const std::string &device) const
{
    if(d->user_map.count(device) > 0)
        return d->user_map[device];
    else if(d->tkmap.count(device) > 0)
        return d->tkmap[device];
    return -1;
}

/*!
 * \brief Removes the prefix from tok and returns the
 * \param tok
 * \return the integer obtained from the given token after removing the prefix, or -1
 *         if tok does not exactly match prefix + number
 */
int CuTThreadTokenGen::which_from_token(const std::string& tok) const {
    std::string idx_as_str(tok);
    idx_as_str.erase(0, d->thread_tok_prefix.size());
    int idx = -1;
    try {
        idx = std::stoi(idx_as_str);
    }
    catch(const std::invalid_argument &ia) {
        perr("CuTThreadTokenGen::which_from_token: no index found in token \"%s\" (prefix: \"%s\")", tok.c_str(), d->thread_tok_prefix.c_str());
    }
    return idx;
}

/*!
 * \brief Returns the list of devices whose sources belong to the thread with the given token
 * \param token the thread token
 * \return list of devices whose sources live in the same thread with the given token
 */
std::list<std::string> CuTThreadTokenGen::devicesForToken(const std::string& token) const {
    int idx = which_from_token(token);
    std::list<std::string> devs;
    for(std::map<std::string, int>::const_iterator it = d->user_map.begin(); it != d->user_map.end(); ++it) {
        if(it->second == idx)
            devs.push_back(it->first);
    }
    for(std::map<std::string, int>::const_iterator it = d->tkmap.begin(); it != d->tkmap.end(); ++it) {
        if(it->second == idx)
            devs.push_back(it->first);
    }
    printf("CuTThreadTokenGen::devicesForToken: devs for %s\n", token.c_str());

    for(std::list<std::string>::const_iterator it = devs.begin(); it != devs.end(); ++it)
        printf("- %s, ", it->c_str());

    printf("\n");
    return devs;
}

/*!
 * \brief Returns the prefix used for the token name
 * \return the string used as prefix to name the token
 */
std::string CuTThreadTokenGen::tok_prefix() const {
    return d->thread_tok_prefix;
}

/*!
 * \brief CuTThreadTokenGen::threadPoolSize returns the upper limit on the number of threads
 *        created and managed by CuThreadService
 *
 * \return the thread pool size, initialised either in the constructor or with setThreadPoolSize
 */
int CuTThreadTokenGen::threadPoolSize() const
{
    return d->pool_siz;
}

/*!
 * \brief CuTThreadTokenGen::setThreadPoolSize change the thread pool size
 *
 * \param siz the upper limit on the number of threads created and managed by CuThreadService
 *
 * \note
 * Changes to the thread pool size after activities have been registered do not affect
 * the threads already running. For example, their number will not be reduced and their
 * grouping will not be changed.
 *
 */
void CuTThreadTokenGen::setThreadPoolSize(int siz) {
    d->pool_siz = siz;
}
