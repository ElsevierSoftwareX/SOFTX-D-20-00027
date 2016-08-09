#!/bin/bash
GIT=`which git`
LOCALVER=`$GIT rev-list HEAD | wc -l`
if [ $LOCALVER \> 0 ] ; then
    VER=`$GIT rev-list origin/master | wc -l`
    VER_DIFF=$(($LOCALVER-$VER))
    if [ $VER_DIFF != 0 ] ; then
        VER="$VER+$VER_DIFF"
    fi
    if $GIT status | grep -q "modified:" ; then
        VER="${VER}M"
    fi

    BRANCH=`git rev-parse --abbrev-ref HEAD`
    if [ ! ${BRANCH} == "master" ]; then
        VER="${VER} (${BRANCH})"
    fi

    VER=`echo ${VER} | sed 's/\ //'`
    COMMIT=`git describe --always`

    OUT="\
#ifndef VERSION_H_
#define VERSION_H_

#define GIT_COMMIT \"${COMMIT}\"
#define GIT_REVISION \"${VER}\"

#endif
"
    printf "$OUT"
fi
