# cumbia documentation on github.io 

### Note
This document is addressed to developers.

*cumbia* documentation is hosted on *github pages*. Documentation is taken from the 

*gh-pages* 

branch of the *cumbia-libs* source tree.

To check out the documentation first change into a directory where to download the *gh-pages* branch

> cd /path/to/

then clone that branch

>  git clone  https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git -b gh-pages cumbia-libs-gh-pages

Change directory to cumbia sources. There you will execute a script that regenerates all the documentation using *doxygen*
so that it is up to date with the latest sources:

> cd  /path/to/cumbia-libs

> ./scripts/cubuild.sh  [tango]  [epics]  docs


The *tango* and *epics* options enable doc generation for the cumbia-tango/qumbia-tango-controls and cumbia-epics/qumbia-epics-controls respectively.

Copy the fresh documentation to the *cumbia-libs-gh-pages* folder you have cloned earlier

> cp -a docs/html /path/to/cumbia-libs-gh-pages


#### Commit changes to gh-pages branch

Change directory into the *cumbia-libs-gh-pages*

> cd /path/to/cumbia-libs-gh-pages

Commit changes

> git commit . -m "cumbia documentation updates for version x.y.z"

Optional: tag documentation (not shown here)

Push changes to *gh-pages* branch:

> git push

The  updated documentation should be then available at


https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia/html/index.html


Fire up a browser or reload the page to see the fresh documentation.


### Note

Contents equivalent to those discussed in this page are available in the README.md within the *gh-pages* branch.
