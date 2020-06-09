#include "cuhttptangosrchelper.h"
#include "cuhttptangohelper.h"
#include "cuhttptangosrc.h"

#include <QtDebug>
#include <QRegularExpression>
#include <qustringlist.h>

CuHttpTangoSrcHelper::CuHttpTangoSrcHelper() {
    d.operations = None;
}

CuHttpTangoSrcHelper::~CuHttpTangoSrcHelper() { }

/*!
 * \brief enable or disable an operation
 * \param o the operation
 * \param enable true - enable, false - disable
 */
void CuHttpTangoSrcHelper::setOperation(CuHttpTangoSrcHelper::Operations o, bool enable) {
    enable ? d.operations |= o : d.operations &= ~o;
}

int CuHttpTangoSrcHelper::operations() const {
    return  d.operations;
}

/*!
 * \brief Returns "tango"
 */
std::string CuHttpTangoSrcHelper::native_type() const {
    return  "tango";
}

bool CuHttpTangoSrcHelper::is_valid(const std::string &src) const {
    CuHttpTangoHelper tgh;
    QuStringList tgpatterns(tgh.srcPatterns());
    QRegularExpressionMatch ma;
    for(int i = 0; i < tgpatterns.size(); i++) {
        const QString& pa = tgpatterns[i];
        QRegularExpression re(pa);
        ma = re.match(src.c_str());
        qDebug () << __PRETTY_FUNCTION__ << "matching " << re.pattern() << "with src" << src.c_str() << "HAS MATCH" << ma.hasMatch();
        if(ma.hasMatch())
            break;
    }
    return ma.hasMatch();
}

/*!
 * \brief for a valid Tango source, process the source according to the Operations enabled
 * \return
 */
std::string CuHttpTangoSrcHelper::prepare(const std::string& in) const {
    std::string s(in);
    CuHttpTangoSrc ts(in);
    if(in.length() > 0 && d.operations & PrependHost && ts.getTangoHost().size() == 0) {
        std::string th = std::string(getenv("TANGO_HOST"));
        std::string notgprot = ts.remove_tgproto(in);
        notgprot.size() == in.size() ? s = th + "/" + s : s = "tango://" + th + "/" + notgprot;
    }
    qDebug() << __PRETTY_FUNCTION__ << " >>>>>  processed source is " << s.c_str();
    return s;
}

/*!
 * \brief Use CuHttpTangoSrc to determine whether the source is a database fetch operation
 *        If it is, -1 is returned, if it is an attribute or command, 1 is returned.
 *        If the source is not a Tango source, return 0;
 * \return 1, -1, or 0 if we don't know.
 */
int CuHttpTangoSrcHelper::can_monitor(const std::string& s) const {
    if(!is_valid(s))
        return 0; // dunnow
    CuHttpTangoSrc tgs(s);
    return tgs.isDbOp() ? -1 : 1;
}

/*!
 * \brief return a value according to *what* is asked
 * \param what a keyword, e.g. "tango_host"
 * \return a value according to the *what* parameter, or an empty string if an unrecognized operation
 *         is given.
 */
std::string CuHttpTangoSrcHelper::get(const std::string& src, const char *what)  const {
    if(is_valid(src)) {
        CuHttpTangoSrc tgs(src);
        if(strcmp(what, "tango_host"))
            return tgs.getTangoHost();
    }
    return "";
}
