#!/bin/bash
if ( [ "${1}" == "--debug" ] || [ "${2}" == "--debug" ] ); then debug=1; fi
origin="${PWD}"; if [ -d "${1}" ]; then cd "${1}"; fi

images="Media/Images"
html="doc/html"
logo="Logo_320.png"
doxyfile="Doxyfile"

echo -ne "Building documentation... "
if [ -f "${doxyfile}" ]; then

  #Generate the documentation
  echo -ne "\n  Running doxygen. Please wait... "

  if [ ${debug} ]; then echo ""; doxygen "${doxyfile}"
  else doxygen "${doxyfile}" > "/dev/null" 2> "/dev/null"; fi

  if [ ${?} -ne "0" ]; then
    echo -ne "[FAIL]\nDoxygen returned errors\n"
    cd "${origin}"
    exit ${?}
  else
    echo -ne "[ OK ]\n"
  fi

  #Copy the logo image into the documentation directory
  echo -ne "  Copying logo image... "
  if ( [ -f "${images}/${logo}" ] && [ -d "${html}" ] ); then
    cp "${images}/${logo}" "${html}/"
	if [ ! -f "${html}/${logo}" ]; then
	  echo -ne "[FAIL]\nLogo image failed to copy\n"
      cd "${origin}"
      exit 1
    else
      echo -ne "[ OK ]\n"
	fi

  else

    echo -ne "[FAIL]\nDirectory structure not found and/or file is missing; cannot copy logo image\n"
    cd "${origin}"
    exit 1

  fi

else

  echo -ne "[FAIL]\nDoxyfile not found; cannot create documents\n"
  cd "${origin}"
  exit 1

fi

cd "${origin}"
exit 0

