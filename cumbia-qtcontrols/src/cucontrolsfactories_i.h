#ifndef CUCONTROLSFACTORIES_H
#define CUCONTROLSFACTORIES_H

class CuControlsWriterA;
class CuControlsReaderA;
class CuControlsPropertyReaderA;
class Cumbia;
class CuDataListener;
class CuData;

/*! \brief cumbia-qtcontrols writer factory. Creates an instance of a CuControlWriterA
 *         implementation.
 *
 * \ingroup core
 *
 * \li CuTWriterFactory and CuEpWriterFactory implement CuControlsWriterFactoryI for Tango and
 * Epics respectively;
 * \li CuTControlsWriter and CuEpControlsWriter implement CuControlsWriterA for Tango and Epics
 * respectively.
 *
 * Subclasses must implement the create, clone and setOptions pure virtual methods.
 */
class CuControlsWriterFactoryI
{
public:
    virtual ~CuControlsWriterFactoryI() {}

    /*! \brief Subclasses implement the create method to return an instance of a
     *         CuControlsWriterA implementation.
     *
     * @return a pointer to an instance of a CuControlsWriterA implementation
     */
    virtual CuControlsWriterA *create(Cumbia *, CuDataListener *) const = 0;

    /*! \brief clone an instance.
     *
     * Clones the current instance
     */
    virtual CuControlsWriterFactoryI* clone() const = 0;

    /*! \brief set options to be used when creating the writer.
     *
     * This is engine dependent.
     *
     * @param options a CuData containing key/value pairs that are specific of an
     *        engine.
     *
     * Options are normally set within the create method, when a CuControlsWriterA
     * is instantiated.
     */
    virtual void setOptions(const CuData& options) = 0;
};

/*! \brief cumbia-qtcontrols reader factory. Creates an instance of a CuControlsReaderA
 *         implementation.
 *
 * \ingroup core
 *
 * \li CuTReaderFactory and CuEpReaderFactory implement CuControlsReaderFactoryI for Tango and
 * Epics respectively;
 * \li CuTControlsReader and CuEpControlsReader implement CuControlsReaderA for Tango and Epics
 * respectively.
 *
 * Subclasses must implement the create, clone and setOptions pure virtual methods.
 */
class CuControlsReaderFactoryI
{
public:
    virtual ~CuControlsReaderFactoryI() {}

    /*! \brief Subclasses implement the create method to return an instance of a
     *         CuControlsReaderA implementation.
     *
     * @return a pointer to an instance of a CuControlsWriterA implementation
     */
    virtual CuControlsReaderA *create(Cumbia *, CuDataListener *) const = 0;

    /*! \brief clone an instance.
     *
     * Clones the current instance
     */
    virtual CuControlsReaderFactoryI* clone() const = 0;

    /*! \brief set options to be used when creating the reader.
     *
     * This is engine dependent.
     *
     * @param options a CuData containing key/value pairs that are specific of an
     *        engine.
     *
     * Options are normally set within the create method, when a CuControlsReaderA
     * is instantiated.
     */
    virtual void setOptions(const CuData& options) = 0;
};

#endif // CUCONTROLSFACTORIES_H
