# SLPM - Simple LFS Package Manager

## About
    SLPM is able to install packages from tar archives, track them and remove.

## Dependencies
* bash
* GNU coreutils
* curl
* wget
* make
* b2sum (Blake2b hash)

## Configuration
    SLPM reads config file from /etc/slpm.conf, which is a shell script
    with different variables definitions.
### Variables
    TGT_PREFIX     - directory where installation files are stored
    TMP_PREFIX     - directory where temporare installation files are stored
    CONF_FLAGS     - default flags for ./configure files
    MAKE_FLAGS     - default flags for 'make' command
    MAKE_INS_FLAGS - default flags for 'make install' command
    DIR_INST       - directory where archives with install packages are stored
    DIR_CACHE      - directory where archives with removed packages are stored
    GENTOO_MIRROR  - Gentoo mirror to get sources from

## Installation process
    Syntax: slpm install package.tar.xz

    1) Extracts archive tar xf package.tar.xz
    2) If package is already installed, ask for reinstalling
    3) Creates package/build directory and enters it
    3) If package/configure exists, reads $CONF_FLAGS and runs it with --prefix=TMP_PREFIX
    4) If package/Makefile exists, reads $MAKE_FLAGS and $MAKE_INS_FLAGS and runs make && make install
    5) Parses all files in $TMP_PREFIX directory and copies them to $TGT_DIRECTORY
    5) Exits building and package directory and deletes them
    6) Moves package archive to DIR_INST and saves list of installed files in DIR_INST/package/filelist

## Removing process
    Syntax: slpm remove PACKAGE-VERSION

    1) Parses $DIR_INSTAL/package/filelist and deletes corresponding files
    2) Moves archive from DIR_INST/package to DIR_CACHE
    3) Removes DIR_INST/package

## Get list of installed
    Syntax: slpm show

    Basically just 'ls' of $DIR_INST

## Downloading from Gentoo repos
    Syntax: slpm remote PACKAGE_NAME

    1) Calculates index of folder using Blake2b hash-function
    2) Downloads PACKAGE_NAME from corresponding folder

## Syncronization
    Syntax: slpm sync

    Writes list of all files available to download from Gentoo repos to /etc/slpm_gentoo_package_list

## Get list of all available packages
    Syntax: slpm list

    Note: convenient to use with fzf as "slpm list | fzf" to search
