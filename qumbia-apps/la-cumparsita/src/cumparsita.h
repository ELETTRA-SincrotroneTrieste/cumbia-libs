#ifndef Cumparsita_H
#define Cumparsita_H

#include <QWidget>
#include <qulogimpl.h>


class CuData;
class CumbiaPool;

/*! \mainpage
 * *la-cumparsita* app interprets *Qt designer* UI files and make them indistinguishable from a
 * dedicated compiled application.
 *
 * If there is no logic in the application, *la-cumparsita* can be used to run UI files generated
 * by the Qt designer.
 *
 * This can be used to create simple *demos* that can be later imported into a more complex cumbia
 * application and compiled.
 *
 *
 * \par Usage
 *
 * \li Open the Qt designer
 * \li Create a *new form* (e.g. widget) either from the pop up dialog offered by the designer at start up
 *     or through the *File* -> *New...* menu action.
 * \li Populate the form with the desired components, chosen from the left pane *Widget Box* list. Please
 *     note the *cumbia widgets* are located in the *cumbia-qtcontrols* section. You may need to scroll down
 *     a little to find them.
 * \li The cumbia widgets need either a *source* (readers) or a *target* (writers). Set them right clicking
 *     on a *cumbia-qtcontrols* component and picking the *Edit Connection* action.
 *     *Wildcards* can be used for Tango sources: *$1/double_scalar, $2/double_scalar*, and so on...
 *     $1 will be replaced with the *first Tango device name* found on the command line, $2 with the second...
 * \li Once done, save the form.
 *
 * \li From the terminal, move to the directory where the *ui* form is.
 * \li Execute *la-cumparsita form.ui [tg/device/1 tg/device/2] *  (Tango example)
 *
 * \note
 * The *ui* file can be later used in  a dedicated  C++ *cumbia application*
 *
 * \par Further reading
 * *la-cumparsita* sources can contain formulas and JS functions.
 * Read and find some *la-cumparsita* screenshots <a href="../../cuformula/html/index.html">here</a>.
 */
class Cumparsita : public QWidget
{
    Q_OBJECT

public:
    explicit Cumparsita(QWidget *parent = nullptr);

    QObject *get_cumbia_customWidgetCollectionInterface() const;

    ~Cumparsita();

private:
};

#endif // Cumparsita_H
