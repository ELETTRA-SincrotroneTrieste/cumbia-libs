#include "auth.h"
#include "botdb.h"
#include "botconfig.h"

Auth::Auth(BotDb *db, BotConfig *cfg)
{
    m_db = db;
    m_cfg = cfg;
    m_limit = -1;
}

/**
 * @brief Auth::isAuthorized returns true if the type of operation is authorized to the user uid
 *
 * If the TBotMsgDecoder::Type *operation type* is either TBotMsgDecoder::Monitor  or
 * TBotMsgDecoder::Alert, you can call limit method to get the limit on the number of
 * *monitors* (monitor or alert operations) for the user uid
 *
 * @param uid the user id
 * @param t   TBotMsgDecoder::Type type of operation
 * @return true the user is authorized
 * @return false the user is not authorized
 *
 * In case isAuthorized returns false, you can call the method reason to know why.
 */
bool Auth::isAuthorized(int uid, TBotMsgDecoder::Type t)
{
    m_limit = -1;
    m_reason.clear();
    QString operation;
    switch(t) {
    case TBotMsgDecoder::Host:
        operation = "host";  // column name in auth_limits
        break;
    case TBotMsgDecoder::Monitor:
    case TBotMsgDecoder::Alert:
        // column name in auth_limits (with the final `s': alert+monitor)
        operation = "monitors";
        break;
    case TBotMsgDecoder::AttSearch:
    case TBotMsgDecoder::Search:
    case TBotMsgDecoder::ReadFromAttList:
        operation = "dbsearch"; // search Tango database
        break;
    default:
        break;
    }
    if(m_limit < 0) {
        m_limit = m_db->isAuthorized(uid, operation);
        if(m_limit < 0) {
            m_reason = "Unauthorized: still waiting for authorization";
        }
        else if(!operation.isEmpty() && m_limit == 0) {
            // get defaults from BotConfig
            m_limit = m_cfg->getDefaultAuth(operation);
            if(m_limit < 0)
                m_reason = "Default configuration for the operation \"" + operation + "\" not found";

        }
        else { // operation empty: special auth not required
            m_limit = 1;
        }
    }
    return  m_limit > 0;
}

int Auth::limit() const
{
    return m_limit;
}

QString Auth::reason() const
{
    return m_reason;
}
