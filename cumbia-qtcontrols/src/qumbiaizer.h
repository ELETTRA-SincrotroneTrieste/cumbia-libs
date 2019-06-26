#ifndef QUMBIAIZER_H
#define QUMBIAIZER_H

#include <QObject>
#include <quvaluefilter.h>

class CuData;
class QumbiaizerPrivate;

/** \brief QTangoizes anything (not really anything, but...)
  *
  * This base class is reimplemented by QTWatcher and QTWriter to use QTangoCore
  * in order to update widgets that are not QTango aware.
  */
class Qumbiaizer : public QObject
{
    Q_OBJECT

public:
    /* Compatible parent object types supported */
    enum Type { Invalid = -1, Void, Int, Double, UInt, Short, String, Bool,
                BoolVector, DoubleVector, StringVector, IntVector };

    /** \brief Supported auto configuration parameters with which you can invoke slots on the
  * attached refreshee
  *
  * @see attach
  * @see setAutoConfSlot
  */
    enum AutoConfType { Min, Max, MinWarn, MaxWarn, MinErr, MaxErr,
                        StdUnit, Unit, DisplayUnit, Description, Label };

    Qumbiaizer(QObject *parent);

    virtual ~Qumbiaizer();

    bool error() const;

    QString message() const;

    virtual void attach(QObject *object, const char *method,
                        const char* setPointSlot = NULL,
                        Qt::ConnectionType connType = Qt::AutoConnection);

    void attach(short *s);

    void attach(double *d);

    void attach(int *i);

    void attach(unsigned int *u);

    void attach(QString *qs);

    void attach(bool *b);

    void attach( QStringList *sl);

    void attach( QVector<double> * v);

    void attach( QVector<int> *vi);

    void attach( QVector<bool> *vb);

    Type type() const;

    QString slot() const;

    /** \brief returns the method name, without the full signature.
      *
      * For instance if slot() returns setValue(int), this method will
      * return setValue
      */
    QString methodName() const;

    QString setPointSlot() const;

    QString setPointMethodName() const;

    void *data();

    Qt::ConnectionType connectionType() const;

    /** \brief when minimum and maximum values are available, these slots are invoked.
      *
      * These slots are invoked when the auto configuration is available.
      * The QTWatcher api provides shortcuts to set minimum, maximum, min err, min warn, units.
      * The supported data types for slots to be invoked are the ones enumerated in AutoConfType.
      * Slots must take exactly <strong>one argument</strong>, and must be of type<br/>
      * <strong>int, double, short, unsigned int, QString</strong>.<br/>
      * Nothing else is supported.
      *
      * @see AutoConfType
    */
    void setAutoConfSlot(AutoConfType act, const char* slot);

    /** \brief writes into the in_type parameter the type of the input argument of
      * the given method of the object obj.
      *
      * @param the method name, which has to have only one argument, for instance
      *        setMinimum(int), setValue(double), setText(QString)
      *
      * @param obj the QObject on which to look for the type of input argument.
      *
      * @param in_type a char pointing to previously allocated memory.
      *
      * @return true the method succeeded
      * @return false the method failed (method signature not found or input argument
      *         count greater than one.
      */
    bool inTypeOfMethod(const QString &method, QObject *obj, char* in_type);

    /** \brief extracts the signal/slot code at the beginning of the method name
     *
     *
     * methods are specified via SIGNAL and SLOT macros, so we have to remove
     * signals and slots codes from the method. See qt/src/corelib/kernel/qobject.h
     *  and qt/src/corelib/kernel/qobject.cpp.
     * Returns the extracted code and removes the code from the method parameter if
     * the code is QSIGNAL_CODE or QSLOT_CODE.
     */
    int extractCode(QString& method);

    /** \brief removes filter from the object.
      *
      * This method does not delete filter.
      */
    void removeRefreshFilter(QuValueFilter *filter);

    /** \brief install the QTRefreshFilter filter into the QTangoizer.
      *
      * Whenever data is updated, your implementation of the filter methods
      * is invoked so that the read data can be modified before being displayed
      * on a widget or stored into a variable.
      * For each kind of data there is a method.
      *
      * @see QTRefreshFilter
      */
    void installRefreshFilter(QuValueFilter *filter);

    /** \brief returns the currently installed refresh filter.
      *
      * @return the currently installed refresh filter.
      */
    QuValueFilter *refreshFilter() const;

    /** \brief Call this one before setSource to ensure that only one read is performed.
      *
      * @param singleShot true a single read is performed, then unsetSource is automatically
      *        called and the reader disconnected.
      * @param singleShot false the object behaves as usual, with the specified RefreshMode.
      *
      * Call this method before setSource.
      * <strong>Warning</strong>: reimplementing QTWatcher::refresh() method bypasses
      * the singleShot property check and so setting singleShot to true will not
      * work as expected.
      */
    void setSingleShot(bool singleShot);

    /** \brief set single shot and then destroy the object right after the execution
      *
      * <h3>Important note</h3> <p>The object <strong>is destroyed even if the execution
      * is not successfully accomplished</strong>. This goes both for readers and for writers.
      * </p>
      * <h3> Note</h3> <p> Auto destruction for writers is possible only if the
      * QTangoComProxyWriter::ExecutionMode is set to Synchronous (the default)
      * </p>
      */
    void setAutoDestroy(bool autoDestroy);

    /** \brief returns true if the object auto destroys itself after first execution.
      *
      * @see setAutoDestroy
      */
    bool autoDestroy() const;

    /** \brief returns the singleShot property.
      *
      * @return true only one read is performed (or was performed)
      * @return false the reader behaves as readers usually behave
      */
    bool singleShot() const;

    void setToolTipsDisabled(bool disable);

    bool toolTipsDisabled() const;

    const char *type_str(Type t) const;

signals:

    /** \brief auto configuration signal.
      *
      *
      * QTangoizer class can perform some simple auto configuration (see setAutoConfSlot method)
      * when the associated widgets have suitable slots to set minimum, maximum, text values.
      * In every case, this signal is <strong>emitted before any auto configuration is done</strong>
      * by QTangoizer derived classes.
      * This allows you to set minimum and maximum values (for example) without worrying to initialize
      * the first value read by yourself. Actually, if the setPointSlot parameter of the
      * void attach(QObject *refreshee, const char *slot, const char* setPointSlot = NULL, Qt::ConnectionType connType = Qt::AutoConnection);
      * method is not NULL, the first value read is set by means of that slot.
      */
    void configured(const CuData &);

    /** \brief a signal that contains the error message related to setSource - or setTargets - failures.
     *
     * Connect this signal to any slot accepting a QString as input parameter if you want to know the error message
     * related to the setSource() or setTargets() failure.
     * Mapped from QTangoCommunicationHandle::connectionErrorMessage()
     * @param msg the error message
     */
    void connectionErrorMessage(const QString& msg);

   /** \brief Emitted by the refresh method. The message is taken from the TVariant
    *
    * @param message either the error message or other piece of information from the source (e.g. timestamp)
    * @see readOk
    */
    void refreshMessage(const QString& message);

    /** \brief this signal is emitted by the QuWatcher::onUpdate method and the success
    * parameter indicates whether the reading is successful or not.
    *
    * @param success true if read was successful, false otherwise
    *
    * \par Note
    * success contains the value of \code !data["err"].toBool() \endcode
    *
    */
    void readOk(bool success);

    /*! \brief newData for short data types
     *
     * @param s value as short
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to short
     */
    void newData(short s);

    /*! \brief newData for int data types
     *
     * @param i value as int
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to int
     */
    void newData(int i);

    /*! \brief newData for double data types
     *
     * @param d value as double
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to double
     */
    void newData(double d);

    /*! \brief newData for string data types
     *
     * @param s value as QString
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to string
     */
    void newData(const QString& s);

    void newData();

    /*! \brief newData for unsigned int data types
     *
     * @param ui  value as unsigned
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to unsigned int
     */
    void newData(unsigned int ui);

    /*! \brief newData for unsigned short data types
     *
     * @param us value as unsigned short
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to unsigned short
     */
    void newData(unsigned short us);

    /*! \brief newData for unsigned bool data types
     *
     * @param b value as bool
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to bool
     */
    void newData(bool b);

    /*! \brief newData for vector of double data types
     *
     * @param dv value as QVector<double> (const reference)
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to a vector of double
     */
    void newData(const QVector<double> &);

    /*! \brief newData for vector of int data types
     *
     * @param vi value as QVector<int> (const reference)
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to a vector of int
     */
    void newData(const QVector<int> &vi);

    /*! \brief newData for vector of boolean
     *
     * @param bv value as QVector<bool> (const reference)
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to a vector of bool
     */
    void newData(const QVector<bool> & bv);

    /*! \brief newData for string list
     *
     * @param slv value as QStringList (const reference)
     *
     * \par Note 1
     * The source must provide this precise data type
     *
     * \par Note 2
     * The method extracts data["value"] and converts it to a list of strings
     */
    void newData(const QStringList & slv);

    /*! \brief newData signal passing the whole data bundle
     *
     * @param data value as full CuData bundle
     *
     * \par Note
     * This signal is always emitted after the *type specific* version of the method.
     * It can be used in particular cases where the whole information from CuData is required
     * (name of the source, data type, timestamp, message, and so on..)
     */
    void newData(const CuData& data);

protected:
    QumbiaizerPrivate * quizer_ptr;

protected slots:

    virtual void configure(const CuData& data);

    /* although refresh is used in readers (QTWatcher), it can be used in auto configuration
     * to invoke the auto configuration method.
     * read means extract the read value from v, not the set point. If you want to update the value
     * with set point value instead of read point, pass false as second parameter.
     */
    virtual void updateValue(const CuData& v, bool read = true, const char* customMethod = NULL,
                             QuValueFilter::State updateState = QuValueFilter::Update);

private:
    Q_DECLARE_PRIVATE(Qumbiaizer)
};


#endif // QTANGOIZER_H
