#ifndef CUCONTEXTI_H
#define CUCONTEXTI_H

class CuContext;

/** \brief Interface for cumbia classes delegating to CuContext the communication link creation and management.
 *
 * It is suggested that classes using a CuContext to create and manage the connection with the underlying engine
 * implement this interface, so that clients can rely on the getContext method to get a reference to CuContext.
 *
 */
class CuContextI
{
public:
    virtual ~CuContextI() {}

    /*! \brief Return a reference to the CuContext in use
     *
     * @return a pointer to the CuContext used by the subclass.
     */
    virtual CuContext *getContext() const = 0;
};

#endif // CUCONTEXTI_H
