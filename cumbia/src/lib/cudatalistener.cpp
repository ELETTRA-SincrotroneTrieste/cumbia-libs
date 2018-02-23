#include "cudatalistener.h"

/*! @private
 */
class CuDataListenerPrivate
{
public:
    bool invalid;
};

/*! \brief class constructor
 *
 * Builds a new CuDataListener with the invalid flag set to false
 */
CuDataListener::CuDataListener() {
    d = new CuDataListenerPrivate;
    d->invalid = false;
}

/*! \brief invalidates the data listener
 *
 * the invalid flag is set to true
 */
bool CuDataListener::invalidate()
{
    d->invalid = true;
}

/*! \brief  set the data listener to *valid*
 *
 * the invalid flag is reset to false
 */
void CuDataListener::setValid()
{
    d->invalid = false;
}

/*! \brief class destructor
 *
 * class destructor
 */
CuDataListener::~CuDataListener() {
    delete d;
}

/*! \brief returns true if the data listener is invalid
 *
 * @return true the data listener has been marked as invalid
 * @return false the data listener is still valid
 *
 * \par Example
 * This method is used by the cumbia-tango CuTReader class, from
 * CuTReader::removeDataListener, and from the same method of
 * CuTWriter
 */
bool CuDataListener::invalid() const
{
    return d->invalid;
}

