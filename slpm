#!/bin/bash

function ask_flags () {
    if [ "$2" = "" ]; then
        local FLAG_NAME=$1
        NEW_FLAGS=""
    else
        local FLAG_NAME=$2
        NEW_FLAGS=$1
    fi
    while true; do
        read -p " [Info] $FLAG_NAME: $NEW_FLAGS. Do you want to add flags?(y/n) " yn
        case $yn in
            [Yy]* ) read -p "Additional flags: " ADDED_FLAGS; NEW_FLAGS="$NEW_FLAGS $ADDED_FLAGS"; break;;
            [Nn]* ) break;;
            * ) echo "Please answer yes or no";;
        esac
    done
}

function sync () {
    # Backup previous list
    if [ -f "slpm_gentoo_package_list" ]; then
        cp /etc/slpm_gentoo_package_list /etc/slpm_gentoo_package_list.bak
        rm /etc/slpm_gentoo_package_list 
    fi

    for (( i=0; i<=255; i++ )); do
        # Get index of folder in https://mirror.yandex.ru/gentoo-distfiles/distfiles/
        INDEX=$(printf '%#x' $i | cut -dx -f2)
        [ $i -lt "16" ] && INDEX="0"$INDEX
        echo $index

        # Parses all filenames
        curl $GENTOO_MIRROR/distfiles/$INDEX/ | grep -o -e 'href=\".*\"' | cut -d \" -f 2 >> /etc/slpm_gentoo_package_list 
    done
}

function help () {
    echo "# slpm install ARCHIVE_NAME --- compiles and installs from archive"
    echo "# slpm remove NAME-VERSION  --- removes installed via slpm apps"
    echo "# slpm show                 --- list installed via slpm apps"
    echo "# slpm remote PACKAGE       --- downloads archive from gentoo repos(name should be as in \"slpm list\")"
    echo "# slpm sync                 --- parse gentoo repo for available packages"
    echo "# slpm list                 --- prints list of available packages from gentoo repo"
}

# Try to source config
if [ ! -f "/etc/slpm.conf" ]; then
    echo "[Error] Configuration file /etc/slpm.conf does not exist"
    exit
fi
source /etc/slpm.conf

# Check variables
# TGT_PREFIX     - directory where installation files are stored
# TMP_PREFIX     - directory where temporare installation files are stored
# CONF_FLAGS     - default flags for ./configure files
# MAKE_FLAGS     - default flags for 'make' command
# MAKE_INS_FLAGS - default flags for 'make install' command
# DIR_INST       - directory where archives with install packages are stored
# DIR_CACHE      - directory where archives with removed packages are stored
# GENTOO_MIRROR  - Gentoo mirror to get sources from

if [ -z ${TGT_PREFIX+x} ]; then echo "[Error] Variable TGT_PREFIX is not set in /etc/slpm.conf"; exit; fi
if [ -z ${TMP_PREFIX+x} ]; then echo "[Error] Variable TMP_PREFIX is not set in /etc/slpm.conf"; exit; fi
if [ -z ${CONF_FLAGS+x} ]; then echo "[Error] Variable CONF_FLAGS is not set in /etc/slpm.conf"; exit; fi
if [ -z ${MAKE_FLAGS+x} ]; then echo "[Error] Variable MAKE_FLAGS is not set in /etc/slpm.conf"; exit; fi
if [ -z ${MAKE_INS_FLAGS+x} ]; then echo "[Error] Variable MAKE_INS_FLAGS is not set in /etc/slpm.conf"; exit; fi
if [ -z ${DIR_INST+x} ]; then echo "[Error] Variable DIR_INST is not set in /etc/slpm.conf"; exit; fi
if [ -z ${DIR_CACHE+x} ]; then echo "[Error] Variable DIR_CACHE is not set in /etc/slpm.conf"; exit; fi
if [ -z ${GENTOO_MIRROR+x} ]; then echo "[Error] Variable GENTOO_MIRROR is not set in /etc/slpm.conf"; exit; fi

if [ $1 = "install" ]; then
    # === Extract info about package === #
    # Archive filename
    ARCHIVE=$2

    # Name and folder to extract sources
    NAME="${ARCHIVE%-*}"

    # Version
    VERSION=$(echo $ARCHIVE | sed 's/.*-//' | sed 's/.tar.*//')

    # Folder with sources
    SRC_DIR="$NAME-$VERSION"
    # ================================== #

    # Check if it exists
    if [ ! -f "$ARCHIVE" ]; then
        echo " [Error] File $ARCHIVE does not exist"
        exit
    fi

    echo "Installing $NAME with version $VERSION"

    # Unpack it
    echo " [Info] Unpacking archive to $NAME-$VERSION/"
    tar xf $ARCHIVE

    # Enter build folder
    cd $SRC_DIR && mkdir build && cd build

    # Configure to temporary prefix 
    if [ -f "../configure" ]; then
        echo " [Info] Current TMP_PREFIX is $TMP_PREFIX"

        ask_flags $CONF_FLAGS CONF_FLAGS
        CONF_FLAGS=$NEW_FLAGS

        ../configure $(echo "--prefix=$TMP_PREFIX $CONF_FLAGS")
    else
        echo " [Error] configure file does not exist"
        exit
    fi

    # === make === #
    if [ -f "../Makefile" ] || [ -f "./Makefile" ]; then
        ask_flags $MAKE_FLAGS MAKE_FLAGS
        MAKE_FLAGS=$NEW_FLAGS
        make $MAKE_FLAGS

        ask_flags $MAKE_INS_FLAGS MAKE_INS_FLAGS
        MAKE_INS_FLAGS=$NEW_FLAGS
        make $MAKE_INS_FLAGS install
    else
        echo " [Error] Makefile does not exist"
        exit
    fi

    # Get list of all files to be installed
    TMP_FILES=$(find $TMP_PREFIX -type f)

    # Change prefix to target
    TGT_FILES=$(find $TMP_PREFIX -type f | sed 's|'$TMP_PREFIX'|'$TGT_PREFIX'|')

    # Convert to arrays
    readarray -t TMP_FILES_ARRAY <<<"$TMP_FILES"
    readarray -t TGT_FILES_ARRAY <<<"$TGT_FILES"

    # Install to final location
    for (( i=0; i<${#TMP_FILES_ARRAY[@]}; i++ )); do
        cp -v ${TMP_FILES_ARRAY[i]} ${TGT_FILES_ARRAY[i]}
    done

    # Save archive and list of installed files to DIST_INST
    [ ! -d "$DIR_INST" ] && mkdir $DIR_INST

    DIR_INST_SPEC=$(echo "$DIR_INST/$SRC_DIR") # Subfolder called NAME-VERSION
    [ ! -d "$DIR_INST_SPEC" ] && mkdir $(echo "$DIR_INST_SPEC")

    cp -v $ARCHIVE $DIR_INST_SPEC

    [ -f "$DIR_INST_SPEC/filelist" ] &&  rm -v $(echo "$DIR_INST_SPEC/filelist")
    echo $TGT_FILES > $(echo "$DIR_INST_SPEC/filelist")

    # Cleaning up
    cd ../../
    rm -rv $SRC_DIR
    rm -r $(echo "$TMP_PREFIX/*")

    echo "Installation complete"
elif [ $1 = "remove" ]; then
    NAME=$2
    if [ $NAME = "" ]; then echo "Specify package to remove"; exit; fi
    if [ ! -d "$DIR_INST/$NAME" ]; then echo "Package is not installed"; exit; fi

    while true; do
        read -p " Are you sure you want to delete $NAME?(y/n) " yn
        case $yn in
            [Yy]* )
                # Get list of installed files
                TGT_FILES=$(cat $(echo "$DIR_INST/$NAME/filelist"))

                # Convert to array and remove
                readarray -t TGT_FILES_ARRAY <<<"$TGT_FILES"
                for (( i=0; i<${#TGT_FILES_ARRAY[@]}; i++ )); do
                    rm -v ${TGT_FILES_ARRAY[i]}
                done

                mv -v $(echo "$DIR_INST/$NAME/*.tar.*") $DIR_CACHE
                rm -rv $(echo "$DIR_INST/$NAME")

                echo "Package removed"

                exit ;;

            [Nn]* ) break;;
            * ) echo "Please answer yes or no";;
        esac
    done
elif [ $1 = "show" ]; then
    ls $DIR_INST
elif [ $1 = "remote" ]; then
    NAME=$2
    if [ $NAME = "" ]; then echo "Specify package to download"; exit; fi
    if ! grep -Fxq "$NAME" /etc/slpm_gentoo_package_list; then
        echo "[ Error ] $NAME does not exist in repo. Try \"# slpm sync\" or check for misspelling"
        exit
    fi
    
    # Generate index of folder from filename
    INDEX=$(printf '%s' $NAME | b2sum | cut -c1-2)
    wget $GENTOO_MIRROR/distfiles/$INDEX/$NAME
elif [ $1 = "sync" ]; then
    sync 
elif [ $1 = "list" ]; then
    cat /etc/slpm_gentoo_package_list
elif [ $1 = "help" ]; then
    help
else
    echo "Wrong usage. Available commands:"
    help
fi
