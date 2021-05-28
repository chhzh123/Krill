#!/bin/sh

echo "CP"
make exp CP=1

echo "LJ"
make exp LJ=1

echo "RMAT"
make exp RMAT=1

echo "TW"
make exp TW=1

echo "FT"
make exp FT=1

echo "Done!"