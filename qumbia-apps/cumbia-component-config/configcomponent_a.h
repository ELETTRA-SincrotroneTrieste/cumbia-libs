#ifndef CONFIGCOMPONENT_A_H
#define CONFIGCOMPONENT_A_H

#include <string>
#include <vector>

class ConfigComponent_A_Private {
public:
    std::string fpath;
};

class ConfigComponent_A
{
public:
    ConfigComponent_A(const std::string& confpath) {
        d = new ConfigComponent_A_Private;
        d->fpath = confpath;
    }

    virtual ~ConfigComponent_A() {
        delete d;
    }

    /*!
     * \brief Returns the name of the configuration component
     */
    virtual std::string name() const = 0;

    /*!
     * \brief returns the filename made up of the path passed into the class constructor + name
     */
    virtual std::string filenam() const {
        return d->fpath.at(d->fpath.length() - 1) == '/' ? d->fpath + name() : d->fpath + "/" + name();
    }

    /*!
     * \brief processes the option obtained from menu_get_option
     * \param option result obtained from a previous call to menu_get_option
     * \return the new or updated line in the configuration file, that usually
     *         is the return value of update
     */
    virtual std::string process(int option) = 0;

    /*!
     * \brief returns the option from the user, typically chosen through a menu
     * \return > 0 a valid option that will be passed to process
     * \return = 0 hand over processing to history
     * \return < 0 cancel
     */
    virtual int menu_get_option() = 0;

    /*!
     * \brief update internal data without saving on configuration file
     * \param value the value, typically one line, to insert into the configuration.
     *        The component may update an existing line instead.
     * \return the line that was added or that replaced another configuration line
     */
    virtual std::string update(const char* value) = 0;

    /*!
     * \brief write on filenam() the new contens
     * \return  true if successfully saved, false otherwise
     */
    virtual bool save() = 0;

    /*!
     * \brief pretty print configuration component
     */
    virtual void print() = 0;

    /*!
     * \brief get error message
     * \return  string with the error occurred or empty string
     */
    virtual std::string error() const = 0;
    virtual bool history_enabled() const = 0;

private:
    ConfigComponent_A_Private *d;
};

#endif // CONFIGCOMPONENTI_H
