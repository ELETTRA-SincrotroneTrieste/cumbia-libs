# cumbia-libs documentation branch for github pages

This branch hosts the documentation for the cumbia-libs project.

To regenerate the documentation after the library has been modified (and then publish it on https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia/html/index.html), go into the cumbia-libs sources and execute the script

scripts/cubuild.sh  docs

A *docs* directory under the top level *cumbia-libs* source folder will be generated.

Copy the contents of the *docs* directory into this directory, commit and push to the gh-pages branch.

> cd path/to/src/cumbia-libs

> ./scripts/cubuild.sh  docs

> cp -a docs/* /path/to/cumbia-libs-gh-pages

> cd /path/to/cumbia-libs-gh-pages

> git commit . -m "cumbia documentation updates for version x.y.z"

Tag if you wish...

> git push

The  updated documentation should be then available at


https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia/html/index.html


