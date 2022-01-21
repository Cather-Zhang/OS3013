#! /bin/bash

WORKING_DIR=$(pwd)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [[ "$WORKING_DIR" != "$SCRIPT_DIR" ]]; then
  echo "Please rerun this script from: $SCRIPT_DIR"
  exit 1
fi

PARENT_DIR="$(dirname "$SCRIPT_DIR")"

if ! [[ -f "$PARENT_DIR/wunzip.c" ]]; then
    echo "Error: Could not find wunzip.c in directory: $PARENT_DIR"
    exit 1 
fi

if [[ -x wunzip ]]; then
    echo "Removing old wunzip binary."
    rm wunzip 
fi

if [[ -x wunzip ]]; then
    echo "Error: Could not remove old wunzip binary."
    exit 1 
fi

../test-lib/run_tests.sh $*
