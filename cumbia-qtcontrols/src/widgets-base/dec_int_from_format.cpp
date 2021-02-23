#include "dec_int_from_format.h"

#include <QRegExp>
#include <QStringList>
#include <cumacros.h>

DecIntFromFormat::DecIntFromFormat(QString f)
{
    d_error = false;
    d_intDefaults =  d_decDefaults = true;
    d_intDigits = 3;
    d_decDigits = 2;
    d_errorMsg = "No error";
    d_format = f;
}

bool DecIntFromFormat::decode()
{
    int pos = - 1;
    printf("\e[1;36mDecIntFromFormat::decode() format %s\e[0m\n", qstoc(d_format));
    /* add ' in [0-9] to recognize "%'d" */
    QRegExp intRe("%(\\d+)d\\b");
    pos = intRe.indexIn(d_format);
    if(pos >= 0) /* integer */
    {
        d_decDefaults = false;
        d_decDigits = 0;
        d_intDefaults = intRe.capturedTexts().size() > 0;
        if(!d_intDefaults) {
            d_intDigits = intRe.capturedTexts().at(0).toInt();
            d_intDefaults = false;
        }
    }
    else
    {
        if(d_format == "%f")
            return true;
        QRegExp floatRe("%[0-9]*\\.{1,1}[0-9]+f\\b");
        pos = floatRe.indexIn(d_format);
        if(pos >= 0)
        {
            /* type %.3f  decimal digits only */
            QRegExp flRe1("%\\.{1,1}([0-9]+)f\\b");
            /* type %2.3f integer and decimal digits */
            QRegExp flRe2("%([0-9]+)\\.{1,1}([0-9]+)f\\b");
            pos = flRe1.indexIn(d_format);
            if(pos >= 0 && flRe1.capturedTexts().size() > 1)
            {
                d_decDigits = flRe1.capturedTexts().at(1).toInt();
                d_decDefaults = false;
            }
            else // %x.yf
            {
                pos = flRe2.indexIn(d_format);
                if(pos >= 0 && flRe2.capturedTexts().size() > 2)
                {
                    printf("\e[1;36mDecIntFromFormat::decode() captured %d int %d dec d_dec defaults %d d_intDefaults %d\n",
                        flRe2.capturedTexts().at(1).toInt(), flRe2.capturedTexts().at(2).toInt(), d_decDefaults, d_intDefaults);

                    d_decDefaults = false;
                    d_decDigits = flRe2.capturedTexts().at(2).toInt();
                    d_intDigits = flRe2.capturedTexts().at(1).toInt() - d_decDigits - ( d_decDigits > 0 ? 1 : 0);
                    d_intDefaults = d_intDigits <= 0;
                    printf("\e[1;36mDecIntFromFormat::decode() int %d dec %d int def %d dec default %d\n",
                           d_intDigits, d_decDigits, d_decDefaults, d_intDefaults);
                }
            }
        }
    }
    if(pos < 0)
    {
        printf("\e[1;31m* \e[1;36mDecIntFromFormat::decode()\e[0m format error in \"%s\"", qstoc(d_format));
        return false;
    }
    else
        return true;
}


