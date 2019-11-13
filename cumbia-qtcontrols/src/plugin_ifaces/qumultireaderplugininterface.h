#ifndef QUMULTIREADERPLUGININTERFACE_H
#define QUMULTIREADERPLUGININTERFACE_H

#include <QObject>


class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class QString;
class QStringList;

/** \brief Interface for a plugin implementing reader that connects to multiple quantities.
 *
 * \ingroup plugins
 *
 * \li Readings can be sequential or parallel (see the init method). Sequential readings must notify when a reading is performed
 *     and when a complete read cycle is over, providing the read data through two Qt signals: onNewData(const CuData& da) and
 *     onSeqReadComplete(const QList<CuData >& data). Parallel readings must notify only when a new result is available, emitting
 *     the onNewData signal.
 *
 * \li A multi reader must be initialised with the init method, that determines what is the engine used to read and whether the reading
 *     is sequential or parallel by means of the read_mode parameter. If the manual_mode_code is negative, the reading is parallel and the
 *     refresh mode is determined by the controls factory, as usual. If the manual_mode_code is non negative <em>it must correspond
 *     to the <strong>manual refresh mode</strong> of the underlying engine.</em>
 *     For example, CuTReader::Manual must be specified for the Tango control system engine in order to let the multi reader
 *     use an internal poller to read the attributes sequentially.
 *
 */
class QuMultiReaderPluginInterface
{
public:
    virtual ~QuMultiReaderPluginInterface() { }

    /** \brief Initialise the multi reader with the desired engine and the read mode.
     *
     * @param cumbia a reference to the cumbia  implementation
     * @param r_fac the engine reader factory
     * @param manual_mode_code the value to be passed to the reading engine in order to make it work in
     *        manual mode (no polling, no event refresh mode) and perform sequential reads or a negative
     *        number to work in parallel mode.
     */
    virtual void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, int manual_mode_code) = 0;

    /** \brief Initialise the multi reader with mixed engine mode and the read mode.
     *
     * @param cumbia a reference to the CumbiaPool engine chooser
     * @param r_fac the CuControlsFactoryPool factory chooser
     * @param manual_mode_code the value to be passed to the reading engine in order to make it work in
     *        manual mode (no polling, no event refresh mode) and perform sequential reads or a negative
     *        number to work in parallel mode.
     */
    virtual void init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, int manual_mode_code) = 0;

    /** \brief set the sources to read from.
     *
     * \note Calling this method replaces the existing sources with the new ones
     *
     * @see addSource
     */
    virtual void setSources(const QStringList& srcs) = 0;

    /** \brief Remove the readers.
     *
     */
    virtual void unsetSources()= 0;

    /** \brief adds a source to the multi reader.
     *
     * Inserts src at index position i in the list. If i <= 0, src is prepended to the list. If i >= size(),
     * src is appended to the list.
     *
     * @see setSources
     */
    virtual void insertSource(const QString& src, int i = -1) = 0;

    /** \brief removes the specified source from the reader
     *
     */
    virtual void removeSource(const QString& src) = 0;

    /** \brief returns the list of the configured sources
     */
    virtual QStringList sources() const = 0;

    /** \brief returns the polling period of the reader.
     *
     * @return the polling period, in milliseconds, of the multi reader.
     */
    virtual int period() const = 0;

    /** \brief Change the reading period, if the reading mode is sequential.
     *
     * \note If the reading mode is parallel, the request is forwarded to every single reader.
     */
    virtual void setPeriod(int ms) = 0;

    /** \brief To provide the necessary signals aforementioned, the implementation must derive from
     *         Qt QObject. This method returns the subclass as a QObject, so that the client can
     *         connect to the multi reader signals.
     *
     * @return The object implementing QuMultiReaderPluginInterface as a QObject.
     */
    virtual const QObject* get_qobject() const = 0;
};

#define QuMultiReaderPluginInterface_iid "eu.elettra.qutils.QuMultiReaderPluginInterface"

Q_DECLARE_INTERFACE(QuMultiReaderPluginInterface, QuMultiReaderPluginInterface_iid)

#endif // QUMULTIREADERPLUGININTERFACE_H
