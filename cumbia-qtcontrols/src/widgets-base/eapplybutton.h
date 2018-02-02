#ifndef EAPPLYBUTTON_H
#define EAPPLYBUTTON_H

#include "econstants.h"

#include <QPushButton>

class EApplyButtonPrivate;

/** @private
 */
class EApplyButton : public QPushButton
{
	Q_OBJECT
public:
	EApplyButton(QWidget *parent);
    ~EApplyButton(){}
	
	bool isModified() { return d_modified; }

public slots:
	/**
	 * tell the widget that the value to which it's associated
	 * has changed, so the color of the font changes to red
	 */
	void valueModified(double);
	/**
	 * resets font color to black
	 */
	void clearModified();

private:
	QColor defaultTextColor;
	bool d_modified;

    EApplyButtonPrivate *d_ptr;
};

#endif
