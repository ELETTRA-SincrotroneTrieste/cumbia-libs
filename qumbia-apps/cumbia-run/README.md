# cumbia run and cumbia build

The *cumbia run* and *cumbia build* tools help build and run applications, allowing users and developers to 
recall commands and compile projects easily. Remembering the project name is all they need to download, build
and update it from github (gitlab) or run the app, even when the latter requires long command line arguments.

# Use cases

## 1 cumbia run from a directory containing the app

### 1.1

If the command 

> cumbia run

is given from a directory containing the binary, or the binary is located under the *bin* subdir, then if

- one of the following files is found: *RUN RUN.txt run run.txt README README.txt README.md* and
- one line starting with *args* followed by command line arguments is found

the binary is run with the given arguments

### 1.2

If, in addition to the conditions in *1.1*, 

- one or more lines starting wirh *export* are found

then what follows is used to set the environment before launching the binary

### 1.3

If 

- one of the following files is found: *RUN RUN.txt run run.txt README README.txt README.md* and
- one line starting with *[test]* is found alongside the *[target]* placeholder within what follows

the rest of the line that follows *test* is used as a complete command line and *[target]* is replaced with the found binary.

For example

> [test] TANGO_HOST=tom:20000


### 1.4

If 

- no file with name in *RUN RUN.txt run run.txt README README.txt README.md* is found
*or if *
- none of those files contains lines starting with either *args* or *[test]*

the binary is run with no arguments
