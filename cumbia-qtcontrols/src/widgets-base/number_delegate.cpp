#include <QObject>
#include <cumacros.h>
#include <math.h>
#include "number_delegate.h"
#include "dec_int_from_format.h"

NumberDelegate::NumberDelegate()
{
    d_intDigits = 3;
    d_decDigits = 2;
}

bool NumberDelegate::configureNumber(const QString& fmt)
{
    bool decodeOk;
    QString format;
    if(fmt.isEmpty())
        format = "%.3f";
    else
        format = fmt;

    /* first apply format, if - correctly - specified */
    DecIntFromFormat diff(format);
    decodeOk = diff.decode();
    if(decodeOk) {
        if(!diff.integerDefaults()) /* found a configuration format, not the default coming from DecIntFromFormat */
            d_intDigits = diff.numIntDigits();
        if(!diff.decimalDefaults())
            d_decDigits = diff.numDecDigits();
    }
    return decodeOk;
}

void NumberDelegate::configureNumber(const double min, const double max, const QString& format)
{
    int maxFromId = 3;
    configureNumber(format);

    /* integer digits is overwritten if there is a maximum and minimum */
    maxFromId = 1 + (int) log10(qMax(max, -min));
    if(max != min && maxFromId != d_intDigits)
    {
        pinfo("maximum value %f has changed integer digits from %d (format) to %d",
              max, d_intDigits, maxFromId);
        d_intDigits = maxFromId;
    }
    else
        pinfo("NumberDelegate configureNumber: integer digits unchanged");

    // 		printf("valori di configurazione: %d %d %f %f\n", d_intDigits, d_decDigits, max, min);
}

