# CCP4M-Final
This is the new CCP4M and will be the edition which I will work on.

[![Build Status](https://travis-ci.org/Electrux/CCP4M-Final.svg?branch=master)](https://travis-ci.org/Electrux/CCP4M-Final)

There is a **build.sh** shell script to build the project on your system if there is no release binary for your system or you want the latest git commit version.

You will be required to install the dependencies manually, which are: **yaml-cpp**, and **libcurl**/**curl**.

Once build.sh generates the binary, it can be copied over to **/usr/local/bin** to run the program directly from any directory.

For Debian based distros, install the dependencies using the following command:

**sudo apt-get install libboost-all-dev libyaml-cpp-dev libcurl3 libcurl4-openssl-dev curl**