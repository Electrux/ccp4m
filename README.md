# ccp4m
This is the new CCP4M and will be the edition which I will work on.

[![Build Status](https://travis-ci.org/Electrux/ccp4m.svg?branch=master)](https://travis-ci.org/Electrux/ccp4m) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/d646fd7232004db58611b4eacfd5f691)](https://www.codacy.com/app/Electrux/CCP4M-Final?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Electrux/ccp4m&amp;utm_campaign=Badge_Grade)

## Installation

There is a **build.sh** shell script to build the project on your system if there is no release binary for your system or you want the latest git commit version.

You will be required to install the dependencies manually, which are: **yaml-cpp**, and **libcurl**/**curl**.

Once build.sh generates the binary, it can be copied over to **/usr/local/bin** to run the program directly from any directory.

### For Arch users

The software is available on the AUR under the name **ccp4m**.

### Dependencies

For Debian based distros, install the dependencies using the following command:
```
sudo apt-get install libboost-all-dev libyaml-cpp-dev libcurl3 libcurl4-openssl-dev curl
```

If you have Ubuntu 18.04, use the following command ( just replaces libcurl3 with libcurl4 ):
```
sudo apt-get install libboost-all-dev libyaml-cpp-dev libcurl4 libcurl-openssl-dev curl
```

For Arch based distros, if you want to build from source, install the **curl** package and **yaml-cpp** package. That should be enough.

## Instructions

 **Note:** All the flag based parameters ( ones starting with '--' ) in any command are optional.

### Creating a new project
To create a new project, the following command is used.
```
ccp4m project new test --lang c++ --version 0.1 --std 14 --author.name YourName --author.email YourEmail@emailprovider.com --build.name TestBuild --build.type bin --dir test
```

[![asciicast](https://asciinema.org/a/176310.png)](https://asciinema.org/a/176310)

### Building a project
To build a project, the following command is used. Optionally, the build name can be provided to build a specific target. If not provided, it will build all the targets.
```
ccp4m project build < optional: target build >
```

[![asciicast](https://asciinema.org/a/176313.png)](https://asciinema.org/a/176313)

### Executing a project
To run a project, the following command is used. You need to give a build target as a parameter so that it knows what to run.
```
ccp4m project run < target build > < arguments for the executable binary >
```

[![asciicast](https://asciinema.org/a/176314.png)](https://asciinema.org/a/176314)

### Setting/Changing options in configuration
To change a configuration, the following command is used.
```
ccp4m project set < parameter > < value >
```
The **parameter** can be one of the following for now ( give me suggestions for more please ):

	name - Project Name
	version - Project Version
	license - Project License
	std - Project Standard
	compile_flags - Project Compile Flags

[![asciicast](https://asciinema.org/a/176315.png)](https://asciinema.org/a/176315)

### Cleaning directories

#### Cleaning project directories
The project directory is purged of the buildfiles, lib, and bin directories using the following command.
```
ccp4m project clean
```

[![asciicast](https://asciinema.org/a/176316.png)](https://asciinema.org/a/176316)

#### Cleaning CCP4M directories
The ~/.ccp4m directory can be cleared of logs and downloaded license files using the following command.
```
ccp4m clean < optional: log / license >
```

[![asciicast](https://asciinema.org/a/176317.png)](https://asciinema.org/a/176317)

### Adding additional things
Additional items ( libraries, build targets, includes, and source files ) can be added.

#### Includes
```
ccp4m project add inc < include file name >
```
Include file name can contain directory as well ( but should not contain extension ).

[![asciicast](https://asciinema.org/a/176320.png)](https://asciinema.org/a/176320)

#### Sources
```
ccp4m project add src < source file name > < optional: target build >
```
Target build, if provided, will be the one for which the source file is valid ( depending on regex used for other sources in a build ).

[![asciicast](https://asciinema.org/a/176321.png)](https://asciinema.org/a/176321)

#### Builds
```
ccp4m project add build < build name > --type < bin/lib > --main_src < main source for this build > --exclude < comma separated list of sources to exclude > --srcs < comma separated list of sources to include ( other than main source ) >
```
Note that if main source is not provided, it will be taken as *src/main.c(pp)* by default.
Also, if the type is *lib*, an extra parameter **--build_type** may be provided whose value is either *statc* or *dynamic*, by default it is static.

[![asciicast](https://asciinema.org/a/176322.png)](https://asciinema.org/a/176322)

#### Libraries
```
ccp4m project add lib < library name > --version < version > --inc_flags < include flags ( space separated ) > --lib_flags < library flags ( space separated ) >
```
By default, version is *0*, include flags and library flags are *empty*.

[![asciicast](https://asciinema.org/a/176323.png)](https://asciinema.org/a/176323)
