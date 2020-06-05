#include "cudatalistener.h"
#include <cumacros.h>

/*! @private
 */
class CuDataListenerPrivate
{
public:
};

/*! \brief class constructor
 *
 * Builds a new CuDataListener with the invalid flag set to false
 */
CuDataListener::CuDataListener() {
    d = new CuDataListenerPrivate;
}

/*! \brief not implemented
 */
bool CuDataListener::invalidate() {
    perr("CuDataListener.invalidate: not implemented - returning false.");
    return false;
}

/*! \brief  not implemented since 1.2.0
 */
void CuDataListener::setValid() {
    perr("CuDataListener.setValid: not implemented.");
}

/*! \brief class destructor
 *
 * class destructor
 */
CuDataListener::~CuDataListener() {
    delete d;
}

/*! \brief returns false since v1.2.0
 */
bool CuDataListener::invalid() const {
    perr("CuDataListener.invalid: not implemented - returning false.");
    return false;
}

