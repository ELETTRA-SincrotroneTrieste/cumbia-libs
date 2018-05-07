# cumbia-libs documentation branch for github pages

### The gh-pages branch

This branch (*gh-pages*) hosts the documentation for the cumbia-libs project.

To regenerate the documentation after the library has been modified (and then publish it on https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia/html/index.html), go into the cumbia-libs sources and execute the script

scripts/cubuild.sh  docs

A *docs* directory under the top level *cumbia-libs* source folder will be generated.

Copy the contents of the *docs* directory into this directory, commit and push to the gh-pages branch.

> cd path/to/src/cumbia-libs

> ./scripts/cubuild.sh [tango] [epics] docs

The *tango* and *epics* options enable doc generation for the cumbia-tango/qumbia-tango-controls and cumbia-epics/qumbia-epics-controls respectively.

> cp -a docs/html /path/to/cumbia-libs-gh-pages


#### Check out the *gh-pages* branch

If there is no copy of the *gh-pages* branch check it out executing:

> cd /path/to/

> git clone  https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git -b gh-pages cumbia-libs-gh-pages


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


