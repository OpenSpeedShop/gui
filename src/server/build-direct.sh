#!/bin/bash

TARGET="libopenss-direct.so"
VER_MAJ="0"
VER_MIN="1"

OPENSS_SRC="/home/dane/src/OpenSpeedShop"
OPENSS_PREFIX="/opt/openss"
OPENSS_LIB="${OPENSS_PREFIX}/lib64"

PYTHON_VER="2.6"
PYTHON_PATH="/usr/include/python${PYTHON_VER}"

SOURCES="Direct OpenSpeedShopCLI"
OPENSS_INCLUDES="framework cli queries message"

INCLUDES="-I${PYTHON_PATH}"
for INCLUDE in ${OPENSS_INCLUDES}; do INCLUDES="${INCLUDES} -I${OPENSS_SRC}/libopenss-${INCLUDE}"; done
LIBRARIES="-lpthread"
for LIBRARY in ${OPENSS_INCLUDES}; do LIBRARIES="${LIBRARIES} -lopenss-${LIBRARY}"; done

rm -f *.so* *.o

# Compile the sources
for SOURCE in ${SOURCES}; do
  COMMAND="g++ -c -g3 -fexceptions -fPIC -Wall ${INCLUDES} ${SOURCE}.cpp"
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

