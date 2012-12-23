#!/bin/bash

OPENSS_SRC="/home/dane/src/OpenSpeedShop"
OPENSS_LIB="/opt/openss/lib64"

PYTHON_VER="2.6"
PYTHON_PATH="/usr/include/python${PYTHON_VER}"

SOURCES="main Socket SocketServer OpenSpeedShopCLI FileSystem"
OPENSS_INCLUDES="framework cli queries message"

INCLUDES="-I${PYTHON_PATH}"
for INCLUDE in ${OPENSS_INCLUDES}; do INCLUDES="${INCLUDES} -I${OPENSS_SRC}/libopenss-${INCLUDE}"; done
LIBRARIES="-lpthread"
for LIBRARY in ${OPENSS_INCLUDES}; do LIBRARIES="${LIBRARIES} -lopenss-${LIBRARY}"; done

rm -f openss-server *.o *.gch

# Compile the sources
for SOURCE in ${SOURCES}; do
  COMMAND="g++ -c -g3 -fexceptions -Wall ${INCLUDES} ${SOURCE}.cpp"
  echo -e "\n$COMMAND"
  ${COMMAND}  2>&1 | egrep -v "^${OPENSS_SRC}"
done

# Link the compiled objects
COMMAND="g++ -L${OPENSS_LIB} ${LIBRARIES} -o openss-server"
for SOURCE in ${SOURCES}; do
  COMMAND="${COMMAND} ${SOURCE}.o"
done
echo -e "\n$COMMAND"
${COMMAND}
