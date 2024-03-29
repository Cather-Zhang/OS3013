#! /bin/bash

NAME=goatmalloc
WORKING_DIR=$(pwd)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

usage () {
    echo "usage: run-tests.sh [-h] [-v] [-t test] [-c] [-s] [-d testdir]"
    echo "  -h                help message"
    echo "  -v                verbose"
    return 0
}


verbose=0

args=`getopt hvsct:d: $*`
if [[ $? != 0 ]]; then
    usage; exit 1
fi

set -- $args
for i; do
    case "$i" in
    -h)
  usage
  exit 0
        shift;;
    -v)
        verbose=1
        shift;;
    esac
done


if [[ "$WORKING_DIR" != "$SCRIPT_DIR" ]]; then
  echo "Please rerun this script from: $SCRIPT_DIR"
  exit 1
fi

PARENT_DIR="$(dirname "$SCRIPT_DIR")"

if ! [[ -f "$PARENT_DIR/$NAME.c" ]]; then
    echo "Error: Could not find $NAME.c in directory: $PARENT_DIR"
    exit 1 
fi

if ! [[ -f "$PARENT_DIR/$NAME.h" ]]; then
    echo "Error: Could not find $NAME.h in directory: $PARENT_DIR"
    exit 1 
fi


if [[ -x "$NAME" ]]; then
    echo "Removing old $NAME binary."
    rm "$NAME" 
fi

if [[ -x "$NAME" ]]; then
    echo "Error: Could not remove old "$NAME" binary."
    exit 1 
fi

cp "../$NAME.h" ./

rc=0

if [[ verbose -eq 1 ]]; then
  gcc -Wall -Werror tests.c "../$NAME.c" -o "$NAME" &&
    ./"$NAME" 1 &&
    ./"$NAME" 2 &&
    ./"$NAME" 3 &&
    ./"$NAME" 4 &&
    ./"$NAME" 5
  rc=$?

else
  gcc -Wall -Werror tests.c "../$NAME.c" -o "$NAME" &&
    ./"$NAME" 1 > /dev/null &&
    ./"$NAME" 2 > /dev/null &&
    ./"$NAME" 3 > /dev/null &&
    ./"$NAME" 4 > /dev/null &&
    ./"$NAME" 5 > /dev/null 
  rc=$?
fi
  
if ! [[ $rc -eq 0 ]]; then
  builtin echo -e "\e[31mRemaining tests failed!\e[0m"
  exit 1
fi


