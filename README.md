# SLPM - Simple LFS Package Manager

## About
    SLPM is able to install packages from tar archives, track them and remove.

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
    
