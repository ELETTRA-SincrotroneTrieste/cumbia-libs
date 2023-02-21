#ifndef CUCONTROLSREADERA_H
#define CUCONTROLSREADERA_H

#include <string>
#include <vector>
#include <assert.h>

class QString;
class Cumbia;
class CuData;
class CuDataListener;

/*! \brief abstract class defining an interface for cumbia-qtcontrols readers
 *
 * This abstract class defines an interface for readers. cumbia-qtcontrols readers
 * do not know about the engine used to connect and read from the control system.
 *
 * The method getCumbia returns a pointer to the Cumbia engine currently used by
 * the reader.
 * getDataListener returns a pointer to the CuDataListener that is listening for
 * read updates. The CuDataListener and the Cumbia engine are passed to CuControlsReaderA
 * in the constructor.
 *
 * All the other are pure virtual methods that must be implemented in the subclass.
 *
 */
class CuControlsReaderA
{
public:

    /*! the class constructor initialises the reader.
     *
     * @param c a pointer to the Cumbia instance in use.
     * @param l a CuDataListener that will receive updates.
     *
     * \par Note both parameters must be not null
     */
    CuControlsReaderA(Cumbia *c, CuDataListener *l)
    {
        assert(c != NULL && l != NULL);
        m_cumbia = c;
        m_listener = l;
    }

    virtual ~CuControlsReaderA() {}

    /*! \brief connect the specified source to the control system engine and start reading.
     *
     * @param s the name of the source. The syntax depends on the engine.
     *
     * Subclasses can check the parameter to ensure that its form is valid.
     */
    virtual void setSource(const QString& s) = 0;

    /*!  \brief returns the name of the source
     *
     * @return the name of the source configured with setSource.
     *
     * If wildcards were used in setSource, source will return the complete source,
     * with the substitutions.
     */
    virtual QString source() const = 0;

    /*! \brief disconnect the source
     *
     * stop reading.
     */
    virtual void unsetSource() = 0;

    /*! \brief send data to the reader
     *
     * @param d a bundle of key/value pairs defining the data to send to the reader
     *
     * @see getData
     */
    virtual void sendData(const CuData& d) = 0;

    /*! \brief get data from the reader
     *
     * @param d_ino a bundle of key/value pairs defining the data to send to the reader
     *        and containing the results filled by the reader once the function
     *        returns
     */
    virtual void getData(CuData& d_ino) const = 0;

    /*! \brief returns a pointer to the Cumbia implementation that was passed to the
     *         class constructor
     *
     * @return a pointer to the Cumbia implementation used by the reader
     */
    Cumbia *getCumbia() const { return m_cumbia; }

    /*! \brief return the CuDataListener that was specified in the constructor.
     *
     * @return the CuDataListener that receives new data updates.
     */
    CuDataListener *getDataListener() const { return m_listener; }

private:
    Cumbia *m_cumbia;
    CuDataListener *m_listener;
};

#endif // CUCONTROLSREADERI_H
