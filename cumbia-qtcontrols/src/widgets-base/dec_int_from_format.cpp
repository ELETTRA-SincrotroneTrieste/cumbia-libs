#include "dec_int_from_format.h"

#include <QRegularExpression>
#include <QStringList>
#include <cumacros.h>

QRegularExpression fmtre;
QRegularExpression flRe1("%\\.{1,1}([0-9]+)f\\b");
QRegularExpression flRe2("%([0-9]+)\\.{1,1}([0-9]+)f\\b");

DecIntFromFormat::DecIntFromFormat(QString f)
{
    d_error = false;
    d_intDefaults =  d_decDefaults = true;
    d_intDigits = 3;
    d_decDigits = 2;
    d_errorMsg = "No error";
    d_format = f;
}

bool DecIntFromFormat::decode() {
    bool ok = false;
//    printf("\e[1;36mDecIntFromFormat::decode() format %s\e[0m\n", qstoc(d_format));
    /* add ' in [0-9] to recognize "%'d" */
    fmtre.setPattern("%(\\d+)d\\b");
    QRegularExpressionMatch ma = fmtre.match(d_format);
    ok = ma.hasMatch();
    if(ok) /* integer */ {
        d_decDefaults = false;
        d_decDigits = 0;
        d_intDefaults = ma.capturedTexts().size() == 0;
        if(!d_intDefaults) {

            d_intDigits = ma.capturedTexts().at(1).toInt();
            d_intDefaults = false;
        }
    }
    else {
        if(d_format == "%f")
            return true;
        fmtre.setPattern("%[0-9]*\\.{1,1}[0-9]+f\\b");
        ma = fmtre.match(d_format);
        ok = ma.hasMatch();
        if(ok) {
            /* type %.3f  decimal digits only */
            /* type %2.3f integer and decimal digits */
            QRegularExpressionMatch re1m = flRe1.match(d_format);
            if(re1m.hasMatch() && re1m.captured().size() > 1) {
                d_decDigits = re1m.captured(1).toInt();
                d_decDefaults = false;
            }
            else { // %x.yf
                QRegularExpressionMatch re2m = flRe2.match(d_format);
                if(re2m.hasMatch() && re2m.captured().size() > 2) {
                    d_decDefaults = false;
                    d_decDigits = re2m.captured(2).toInt();
                    d_intDigits = re2m.captured(1).toInt() - d_decDigits - ( d_decDigits > 0 ? 1 : 0);
                    d_intDefaults = d_intDigits <= 0;
                }
            }
        }
    }
    if(!ok)
        perr("DecIntFromFormat::decode(): error or unsupported format \"%s\"", qstoc(d_format));
    return ok;
}


