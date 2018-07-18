.pragma library

var SrcType = Object.freeze({"Tango":0x1, "Epics":0x2 })

var SrcPatterns = {}

SrcPatterns[SrcType.Tango] = "([A-Za-z0-9_]+(:\d+)\/)*(([A-Za-z0-9_\.]+\/[A-Za-z0-9_\.]+\/[A-Za-z0-9_\.]+\/[A-Za-z0-9_\.]+)|([A-Za-z0-9_\.]+\/[A-Za-z0-9_\.]+\/[A-Za-z0-9_\.]+\->[A-Za-z0-9_\.]+))"
SrcPatterns[SrcType.Epics] = "[A-Za-z0-9_]+:[A-Za-z0-9_\.]+"

/*! \brief returns the first command line argument that can be interpreted as a source
 *         or an empty string if no command line argument matches a source pattern
 *
 * @param pattern_types an OR combination of the flags TangoSrcPattern and EpicsSrcPattern
 */
function firstSourceFromArgs(pattern_types) {
    return sourceFromArgs(0, pattern_types)
}

function sourceFromArgs(index, pattern_types) {
    var slist = sourcesFromArgs(pattern_types)
    if(slist.length > index)
        return slist[index]
    return ""
}

function sourcesFromArgs(pattern_types) {

    if(pattern_types === undefined) {
        console.log("CmdLineUtils.sourcesFromArgs: empty paramter pattern_types")
        console.log("CmdLineUtils.sourcesFromArgs: must be an OR combination of TangoSrcPattern and EpicsSrcPattern")
    }

    var patterns = []
    for(var key in SrcPatterns) {
        if(key & pattern_types)
            patterns.push(SrcPatterns[key])
    }

    var srcs = []
    for(var i = 1; i < Qt.application.arguments.length; i++) {
        var arg = Qt.application.arguments[i]

        for(var pat = 0; pat < patterns.length; pat++) {
            var re = new RegExp(patterns[pat], "g")
            var ma = arg.match(re)
            if(ma !== null && ma.length === 1) {
                srcs.push(arg)
                break;
            }
        }
    }
    return srcs;
}



