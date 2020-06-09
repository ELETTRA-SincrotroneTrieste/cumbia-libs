#ifndef CUHTTPTANGOSRCHELPER_H
#define CUHTTPTANGOSRCHELPER_H

#include "../../cuhttpsrchelper_i.h"
#include "cuhttptangosrc.h"

class CuHttpTangoSrcHelperPrivate {
public:
    int operations;
};

/*!
 * \brief Help prepare sources that are specific to the Tango engine to be sent
 *        through the cumbia-http module, which is native engine agnostic.
 *
 * By default, no operation is enabled. One shall call setOperation to enable the
 * desired processing option.
 *
 * \par example
 * \code
    CumbiaHttp *cuhttp = new CumbiaHttp(....);
    CuHttpTangoSrcHelper *tg_src_h = new CuHttpTangoSrcHelper();
    tg_src_h->setOperation(CuHttpTangoSrcHelper::PrependHost, true);
    cuhttp->addSrcHelper(tg_src_h);
 * \endcode
 *
 * The CuHttpRegisterEngine class sets up and installs a default CuHttpTangoSrcHelper with the
 * CuHttpTangoSrcHelper::PrependHost option enabled, like in the snippet above.
 *
 */
class CuHttpTangoSrcHelper : public CuHttpSrcHelper_I
{
public:
    enum Operations { None = 0, PrependHost = 0x01 };

    CuHttpTangoSrcHelper();
    ~CuHttpTangoSrcHelper();

    void setOperation(Operations o, bool enable);
    int operations() const;

    // CuHttpSrcHelper_I interface
public:
    std::string native_type() const;
    bool is_valid(const std::string &src)  const;
    std::string prepare(const std::string &in) const;
    int can_monitor(const std::string & s) const;
    std::string get(const std::string &src, const char* what) const;

private:
    CuHttpTangoSrcHelperPrivate d;
};

#endif // CUHTTPTANGOSRCHELPER_H
