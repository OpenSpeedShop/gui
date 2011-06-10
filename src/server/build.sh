#!/bin/bash

OPENSS_SRC="/home/dane/src/openspeedshop-2.0.1"
OPENSS_LIB="/opt/oss/lib64"

SOURCES="main Socket SocketServer OpenSpeedShopCLI FileSystem"
OPENSS_INCLUDES="framework cli queries message"

for INCLUDE in ${OPENSS_INCLUDES}; do INCLUDES="${INCLUDES} -I${OPENSS_SRC}/libopenss-${INCLUDE}"; done
for LIBRARY in ${OPENSS_INCLUDES}; do LIBRARIES="${LIBRARIES} -lopenss-${LIBRARY}"; done

rm -f openss-server *.o *.gch

# Compile the sources
for SOURCE in ${SOURCES}; do
  COMMAND="g++ -c -g3 -fexceptions ${INCLUDES} -I/usr/include/python2.4 ${SOURCE}.cpp"
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
