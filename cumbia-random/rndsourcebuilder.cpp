#include "rndsourcebuilder.h"
#include <QRegularExpression>
#include <cumacros.h>
#include <QtDebug>

RndSourceBuilder::RndSourceBuilder() {

}

/*!
 * \brief Class constructor with parameters
 * \param nam the desired source name, with or without the *random:* prefix (if missing, it will be added)
 * \param size
 * \param min
 * \param max
 * \param period
 * \param label
 */
RndSourceBuilder::RndSourceBuilder(const QString &nam, int size, double min, double max, int period, const QString& label)
{
    options["period"] = period;
    options["min"] = min;
    options["max"] = max;
    options["label"] = label.toStdString();
    options["size"] = size;
    (nam.startsWith("random:") || nam.startsWith("random:/") || nam.startsWith("random://")) ?
                name = nam : name = "random://" + nam;
}

/*!
 * \brief Define period, min and max from src
 * \param src a source with the pattern specified as follows
 *
 * \par Source patterns
 * \li "[random:|random://]name[/subname1/.../subnameN]/size/min/max[/period][/label]"
 *
 * *Examples*
 * \code
 *  random:a/random/vector/1000/0/500/500/v1
 * \endcode
 *
 * \li *random:* specifies we want to use the cumbia-random module. Optional. If omitted, it will be
 *     automatically prepended to the source
 * \li *a/random/vector* a name of your choice. The only rule is to avoid three or four "/" separated
 *     numbers, that are used for size/min/max and the optional period
 * \li 1000 (size) the size of the data to be generated: 1: scalar; > 1 vector
 * \li 0 (min) the minimum value that can be generated
 * \li 500 (max) the maximum value that can be generated
 * \li 500 (period, optional) the refresh period, in milliseconds. If omitted, 1 second is chosen
 *
 * \code
 * a/random/scalar/1/0/1000
 * \endcode
 *
 * Generates a scalar value, between 0 and 1000, every second (period is omitted)
 *
 * \code
 * a/random/pair/2/0/5/100
 * \endcode
 *
 * Generates a pair of values between 0 and 5 at 10 Hz
 *
 * In the last two examples, *random:* will be prepended to the name of the source.
 * If you access the *name* attribute of this class, you will read *random:a/random/pair/2/0/5/100*
 *
 */
void RndSourceBuilder::fromSource(const QString &source)
{
    QString src(source);
    QRegularExpressionMatch match;
    // try match size/min/max/period/anithing.../label
    // label is the section following the last /
    // .*/(\d+)/([\-\.0-9]+)/([\-\.0-9]+)/(\d+).*(?:/([A-Za-z0-9\-_]+))*
    QRegularExpression re(".*/(\\d+)/([\\-\\.0-9]+)/([\\-\\.0-9]+)/(\\d+)[.]*(?:/([A-Za-z0-9\\-_]+))*");
    match = re.match(src);

    if(match.hasMatch() && match.capturedTexts().size() > 4) { // and capturedTexts > 5
        options["size"] = match.captured(1).toInt();
        options["min"] = match.captured(2).toDouble();
        options["max"] = match.captured(3).toDouble();
        options["period"] = match.captured(4).toInt();
        if(match.capturedTexts().size() > 5 && !match.captured(5).isEmpty())
            options["label"] = match.captured(5).toStdString();
    }
    else {
        // match with size/min/max[/label] only
        // min and max can be negative: [\-]*
        // label is the section following the last /
        // .*/(\d+)/([\-\.0-9]+)/([\-\.0-9]+)[.]*(?:/([A-Za-z0-9\-_]+))*
        re.setPattern(".*/(\\d+)/([\\-\\.0-9]+)/([\\-\\.0-9]+)[.]*(?:/([A-Za-z0-9\\-_]+))*");
        match = re.match(src);
        if(match.hasMatch() && match.capturedTexts().size() > 3) {
            options["size"] = match.captured(1).toInt();
            options["min"] = match.captured(2).toDouble();
            options["max"] = match.captured(3).toDouble();
            if(match.capturedTexts().size() > 4 && match.captured(4).size() > 0)
                options["label"] = match.captured(4).toStdString();
        }
    }

//    qDebug() << __PRETTY_FUNCTION__ << match.capturedTexts();
    if(!match.hasMatch()) {
        options["size"] = 1;
        options["min"] = 0;
        options["max"] = 1000;
        options["period"] = 1000;

        perr("RndSourceBuilder.fromSource: pattern is ");
        perr("\"[random:|random://]name[/subname1/.../subnameN]/size/min/max[/period][/label]\"");
        perr("Using name \%s\" with default options: \"%s\"", qstoc(src), options.toString().c_str());
    }
    // js file?
    re.setPattern(".*file://(.*\\.js)/");
    match  = re.match(src);
    if(match.hasMatch()) {
        if(match.capturedTexts().size() > 1)
            options["jsfile"] = match.captured(1).toStdString();
    }
    // thread token: match the first
    re.setPattern("(?::|/)([A-Za-z0-9_\\-]+)");
    match = re.match(src);
    if(match.hasMatch() && match.capturedTexts().size() == 2)
        options["thread_token"] = match.captured(1).toStdString();

    (src.startsWith("random:") || src.startsWith("random:/") || src.startsWith("random://")) ?
                name = src : name = "random://" + src;
}

/*!
 * \brief Use the JavaScript function defined in filename to generate the data
 * \param filename the name of the *.js* file
 */
void RndSourceBuilder::setFGenFromFile(const QString &filename) {
    options["jsfile"] = filename.toStdString();
}
