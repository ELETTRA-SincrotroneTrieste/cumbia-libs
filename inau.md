> git submodule update --remote --recursive

Example output, where *cumbia-libs* is updated:

```bash
From https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs
   2d2311138d..3addd14977  master     -> origin/master
 * [new tag]               2.1.5      -> 2.1.5
Submodule path 'cumbia-libs': checked out '3addd14977cf4e73e7f443e79c8855f8d11d5f6d'
```

You may want to check the *checked out hash* with the expected
hash in *cumbia-libs*, with *git log* from within *cumbia-libs* itself.

### commit changes after submodule update

> git commit -a -m "version 2.1.5"

```bash
 2 files changed, 2 insertions(+), 2 deletions(-)
```

> git push

Tag:

>  git tag -a 2.1.5 -m "version 2.1.5 with ng updates"

> git push --tags
