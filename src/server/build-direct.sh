#!/bin/bash

TARGET="libopenss-direct.so"
VER_MAJ="0"
VER_MIN="4"

ERROR=0

if [ -z $OPENSS_SRC ]; then
  OPENSS_SRC="~/src/openss"
fi
if [ -z $OPENSS_PREFIX ]; then
  OPENSS_PREFIX="/opt/openss"
fi
if [ -z $OPENSS_LIB ]; then
  OPENSS_LIB="${OPENSS_PREFIX}/lib64"
fi

if [ ! -d $OPENSS_SRC ]; then
  echo "ERROR: OpenSpeedShop source path not found at: \"$OPENSS_SRC\"; try setting \$OPENSS_SRC"
  ERROR=1
fi
if [ ! -d $OPENSS_PREFIX ]; then
  echo "ERROR: OpenSpeedShop installation path not found at: \"$OPENSS_PREFIX\"; try setting \$OPENSS_PREFIX"
  ERROR=1
fi
if [ ! -d $OPENSS_LIB ]; then
  echo "ERROR: OpenSpeedShop library path not found at: \"$OPENSS_LIB\"; try setting \$OPENSS_LIB"
  ERROR=1
fi


if [ -z $PYTHON_VER ]; then 
  PYTHON_VER=$(python --version 2>&1 | sed -r "s/^Python ([0-9]+\.[0-9]+)\..+\$/\1/g")
fi
if [ -z $PYTHON_PATH ]; then
  PYTHON_PATH="/usr/include/python${PYTHON_VER}"
fi
if [ ! -d $PYTHON_PATH ]; then
  echo "ERROR: a valid Python header directory is not found at: \"$PYTHON_PATH\"; try setting \$PYTHON_PATH"
  ERROR=1
fi


if [ $ERROR ]; then exit 1; fi


SOURCES="Direct OpenSpeedShopCLI"
OPENSS_INCLUDES="framework cli queries message"

INCLUDES="-I${PYTHON_PATH}"
for INCLUDE in ${OPENSS_INCLUDES}; do INCLUDES="${INCLUDES} -I${OPENSS_SRC}/libopenss-${INCLUDE}"; done
LIBRARIES="-lpthread"
for LIBRARY in ${OPENSS_INCLUDES}; do LIBRARIES="${LIBRARIES} -lopenss-${LIBRARY}"; done

rm -f *.so* *.o

# Compile the sources
for SOURCE in ${SOURCES}; do
  COMMAND="g++ -c -g3 -fpermissive -fexceptions -fPIC -Wall ${INCLUDES} ${SOURCE}.cpp"
  echo -e "\n$COMMAND"
  ${COMMAND}  2>&1 | egrep -v "^${OPENSS_SRC}"
done

# Link the compiled objects
COMMAND="g++ -shared -Wl,-soname,${TARGET}.${VER_MAJ} -L${OPENSS_LIB} ${LIBRARIES} -o ${TARGET}.${VER_MAJ}.${VER_MIN}"
for SOURCE in ${SOURCES}; do
  COMMAND="${COMMAND} ${SOURCE}.o"
done
echo -e "\n$COMMAND"
${COMMAND}

cp -f ${TARGET}.${VER_MAJ}.${VER_MIN} ${OPENSS_LIB}/
ln -sf ${TARGET}.${VER_MAJ}.${VER_MIN} ${OPENSS_LIB}/${TARGET}.${VER_MAJ}
ln -sf ${TARGET}.${VER_MAJ} ${OPENSS_LIB}/${TARGET}

cp -f Direct.h ${OPENSS_PREFIX}/include/openspeedshop/

