#ifndef CUTHREADLISTENER_H
#define CUTHREADLISTENER_H

#include <cudata.h>

class CuUserData;

/*! \brief interface for a listener that is notified when progress is done in the background
 *         and data is ready
 *
 * Listeners will implement three methods, that are *callbacks* used to deliver data
 * from the background *activity thread* to the *listener's thread* (in graphical
 * applications often called the *gui* thread or *main thread*):
 *
 * \li onProgress: callback that delivers to the listener's thread progress
 *     information on the ongoing background operation in the CuActivity.
 *     Triggered by CuActivity::publishProgress
 * \li onResult: callback that delivers results to the listener's thread from the background
 *     operation in the CuActivity. Triggered by CuActivity::publishResult
 * \li getToken returns a descriptive token used to characterize the listener
 *
 * Thread listeners are registered to the Cumbia engine in conjunction with the associated CuActivity
 * through the method Cumbia::registerActivity.
 *
 * Please see the \ref md_src_tutorial_cuactivity documentation for an example.
 *
 * @see CuActivity
 */
class CuThreadListener
{
public:

    virtual ~CuThreadListener() {  }

    /*!
     * \brief onProgress notifies the listener's thread about the progress of the ongoing
     *        work in the *activity*'s background thread
     *
     * \param step the current step
     * \param total the total number of steps
     * \param data CuData bundle storing (partial) results
     */
    virtual void onProgress(int step, int total, const CuData& data) = 0;

    /*!
     * \brief onResult delivers to the listener's thread the results of the work
     *        performed by the *activity*'s background thread
     *
     * \param data CuData bundle storing the results computed in the background *activity*
     */
    virtual void onResult(const CuData& data) = 0;

    /*!
     * \brief pointer to vector of data flavour, for best performance
     *
     * \param datalist a pointer to a std::vector of CuData
     *
     * \since 2.0
     */
    virtual void onResult(const std::vector<CuData>* datalist) = 0;

    /*!
     * \brief onResult delivers to the listener's thread the results of the work
     *        performed by the *activity*'s background thread
     *
     * \param datalist a std::vector of CuData bundle storing a list of results computed in the background *activity*
     */
    virtual void onResult(const std::vector<CuData>& datalist) = 0;

    /*!
     * \brief custom user-data flavor of the above
     *
     * \param pointer to user defined CuUserData data
     *
     * \note This method does nothing by default and does not need to be implemented
     * by subclasses
     */
    virtual void onResult(const CuUserData* u) { (void) u; };

    /*!
     * \brief getToken returns a token characterizing the listener
     *
     * \return a CuData with key/value pairs describing the listener.
     */
    virtual CuData getToken() const = 0;
};

#endif // CUTHREADLISTENER_H
