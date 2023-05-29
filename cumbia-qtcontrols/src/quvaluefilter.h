#ifndef QUVALUEFILTER_H
#define QUVALUEFILTER_H

#include <cudata.h>
#include <QVector>

/** \brief Filters a value extracted from a CuData during the update method invoked
  *        when new data is available.
  *
  * \ingroup utils
  *
  * Used in conjunction with QuWatcher or QuWriter, it allows to perform some last time
  * modifications to the value extracted from the CuData.
  * You must subclass TValueFilter and provide your implementation of one of the needed
  * methods, according to the type of data you know is contained into the CuData.
  *
  * \par Example
  * \code
  *
  * class MyFilter  : public TValueFilter
  {
    public:
        // dealing with integer values, we filter the intVal which has been extracted from v.
        //
        void filter(const CuData& v, int& intVal, bool read, TValueFilter::State state);
  };

  // cpp file
  void MyFilter::filter(const CuData& v, int& intVal, bool read, TValueFilter::State state)
  {
    if(state == TValueFilter::Update)
        intVal = intVal * 10;
  }

  // The value is multiplied by ten and, if for instance a label is updated with intVal, then
  // the displayed value will be visualized multiplied by ten.
  * \endcode
  *
  */
class QuValueFilter
{
public:

    /** \brief the situation in which the data is extracted.
      *
      * <ul>
      * <li>Update: new data arrival</li>
      * <li>AutoConfiguration: extracted data is taken from the first value read during auto configuration</li>
      * </ul>
      */
    enum State { Update, AutoConfiguration };

    /** \brief The class constructor.
      *
      * Supposing you are using QuWatcher.
      *
      * \code
      * QLabel *label = new QLabel(this);
      * QuWatcher *watcher = new QuWatcher(this);
      * watcher->attach(label, SLOT(setText(const QString&)));
      * MyFilter *filter = new MyFilter();
      * watcher->installRefreshFilter(filter);
      * watcher->setSource("test/device/1/long_scalar");
      * \endcode
      *
      * The filter will multiply by ten the long_scalar value before
      * displaying it in the label, if MyFilter is implemented as in the code
      * snippet above.
      */
    QuValueFilter();


    /** \brief Filter the integer value intValue
      *
      * This method does nothing: it must be reimplemented in a subclass to manipulate
      * intValue according to your needs.
      *
      * @param variant the CuData from which you can fetch information about tango data stored
      * @param intValue the value, passed by reference, that you can modify according to your needs
      * @param read if true, we are extracting a read value, if false a set point has been extracted
      * @param updateState the update state. It tells us whether we have just read new data or
      *        if this is the value taken from the auto configuration process.
      *
      */
    virtual void filter(const CuData &variant, int &intValue, bool read, State updateState);


    virtual void filter(const CuData &variant, short int &shValue, bool read, State updateState);

    /** \brief The same as above, but for unsigned integer values.
      *
      * @see filter(const CuData& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, unsigned int &uintValue, bool read, State updateState);

    /** \brief The same as above, but for double values.
      *
      * @see filter(const CuData& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, double &doubleValue, bool read, State updateState);

    /** \brief The same as above, but for QString values.
      *
      * @see filter(const CuData& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, QString &strValue, bool read, State updateState);


    /** \brief The same as above, but for boolean values.
      *
      * @see filter(const CuData& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, bool &booleanValue, bool read, State updateState);

    /** \brief The same as above, but for boolean vectors.
      *
      * @see filter(const CuData& variant, int &booleanValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, QVector<bool> &booleanVect, bool read, State updateState);

    /** \brief The same as above, but for double vectors.
      *
      * @see filter(const CuData& variant, int &doubleValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, QVector<double> &doubleVect, bool read, State updateState);

    /** \brief The same as above, but for int vectors.
      *
      * @see filter(const CuData& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, QVector<int> &intVect, bool read, State updateState);

    /** \brief The same as above, but for string lists.
      *
      * @see filter(const CuData& variant, int &strValue, bool read, State updateState)
      */
    virtual void filter(const CuData& variant, QStringList &stringList, bool read, State updateState);


    /** \brief The same as above, but for string lists.
      *
      * @see filter(const CuData& variant, int &strValue, bool read, State updateState)
      */
    virtual void filter(const CuData& in_d, CuData &out_d, bool read, State updateState);
};

#endif // QTREFRESHFILTER_H
