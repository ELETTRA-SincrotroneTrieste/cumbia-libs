#ifndef CUHTTPSRCHELPER_I_H
#define CUHTTPSRCHELPER_I_H

#include <string>

/*!
 * \brief The CuHttpSrcHelper_I interface defines a helper for a CuHttpSource that is unaware of
 *        what engine lies behind the source it is storing.
 *
 * A list of CuHttpSrcHelper_I implementations can be registered to CumbiaHttp.
 * Then each source can exploit an engine specific helper to get some useful information.
 * For example, a CuHttpSource may be storing a Tango source that needs to prepend the
 * *tango host* to the name. CuHttpSource doesn't know anything about Tango but the client
 * application can register a CuHttpTangoSrcHelper, that implements CuHttpSrcHelper_I, to
 * *process* the source before it is handed to the native engine.
 *
 * As said, multiple engines can be provided and all the methods in subclasses shall return
 * a value clearly indicating the case when the implementation *doesn't handle* the given
 * source.
 *
 * \par Example
 * If a Tango and EPICS helper are registered, a source named *sys/tg_test/1/double_scalar*
 * implies *native_type* and *process* return an empty string and can_monitor returns 0 in
 * the EPICS helper, while they return non empty string and non zero in the Tango implementation.
 *
 * \par See also
 * CuHttpTangoSrcHelper
 */
class CuHttpSrcHelper_I
{
public:
    virtual ~CuHttpSrcHelper_I() {}

    /*!
     * \brief return the native type of the source or an empty string if the
     *        helper does not recognize the source
     * \return the native type, e.g. "tango", "epics", or empty if the implementation
     *         does not recognize the source by its form
     */
    virtual std::string native_type() const = 0;

    /*!
     * \brief returns true if the source passed is recognised and can be handled by the
     *        implementation.
     *
     * \param src the source name as provided by a client
     *
     * \return true if src has been recognized by the implementation, false otherwise
     */
    virtual bool is_valid(const std::string& src) const = 0;

    /*!
     * \brief prepare the input source to be perfectly managed by the specific engine
     *
     * \param in input source as specified by the client
     * \return input source processed by the specific engine subclass, or an empty string if
     *         the helper doesn't handle the source.
     *
     * \par Example
     * The Tango implementation may prepend the *tango host* to the source name to prepare
     * it to be sent through the http module
     */
    virtual std::string prepare(const std::string& in) const = 0;

    /*!
     * \brief returns whether or not the source src can be monitored
     * \param src the source name
     * \return 0 if the helper doesn't know, 1 if can be monitored, -1 if not
     */
    virtual int can_monitor(const std::string& src) const = 0;

    /*!
     * \brief get something from the source, according to what
     * \param what the implementation will know what *what* means and return something
     * \return a string that is part of the source or something else according to *what*
     *
     * \par Example
     * Get Tango arguments, Tango host, device name or the *class name* from a Tango source
     * (See CuHttpTangoSrc)
     */
    virtual std::string get(const std::string &src, const char* what) const = 0;
};

#endif // CUHTTPSRCHELPER_I_H
