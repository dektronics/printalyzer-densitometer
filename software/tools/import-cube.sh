#!/bin/bash

#
# This script helps with importing framework source files from an STM32CubeMX
# code generation dump. It does not prepare the complete project, and is
# mainly intended for use in tracking framework code changes.
#
# Files copied by this script may overwrite local changes within the project,
# so changed files should be carefully inspected before checking them in.
#
# This script can also be used as documentation on how the STM32CubeMX code
# dump is translated into the project directory structure.
#

#
# Validate arguments to get input and output paths
#
if [[ -f "$1/.mxproject" ]]; then
    CUBEDIR=$1
else
    echo "Need a valid CubeMX output path"
    exit 1
fi

if [[ -z "$2" ]]; then
    if [[ -f ".project" ]]; then
        PROJDIR="`readlink -f .`"
    elif [[ -f "../.project" ]]; then
        PROJDIR="`readlink -f ..`"
    fi
elif [[ -f "$2/.project" ]]; then
    PROJDIR=`readlink -f $2`
fi

if [[ ! -f "$PROJDIR/.project" ]]; then
    echo "Need a valid project path"
    exit 1
fi

echo "CubeMX path: $CUBEDIR"
echo "Project path: $PROJDIR"

#
# Linker configuration files
#
cp "${CUBEDIR}/"*.ld "${PROJDIR}/"

#
# Project core sources
#
mkdir -p "${PROJDIR}/src/system"
cp "${CUBEDIR}/Core/Startup/"startup_stm32l072kztx.s "${PROJDIR}/src/system/"
cp "${CUBEDIR}/Core/Inc/"stm32l0xx*.h "${PROJDIR}/src/system/"
cp "${CUBEDIR}/Core/Src/"stm32l0xx*.c "${PROJDIR}/src/system/"
cp "${CUBEDIR}/Core/Src/"sys*.c "${PROJDIR}/src/system/"
cp "${CUBEDIR}/Core/ThreadSafe/"* "${PROJDIR}/src/system/"

#
# External driver code
#
mkdir -p "${PROJDIR}/external/drivers/stm32l0xx/src"
cp -r "${CUBEDIR}/Drivers/STM32L0xx_HAL_Driver/Src/"* "${PROJDIR}/external/drivers/stm32l0xx/src/"

mkdir -p "${PROJDIR}/external/drivers/stm32l0xx/include"
cp -r "${CUBEDIR}/Drivers/STM32L0xx_HAL_Driver/Inc/"* "${PROJDIR}/external/drivers/stm32l0xx/include/"

mkdir -p "${PROJDIR}/external/drivers/cmsis/include"
cp -r "${CUBEDIR}/Drivers/CMSIS/Device/ST/STM32L0xx/Include/"* "${PROJDIR}/external/drivers/cmsis/include/"
cp -r "${CUBEDIR}/Drivers/CMSIS/Include/"* "${PROJDIR}/external/drivers/cmsis/include/"

#
# External FreeRTOS code
#
mkdir -p "${PROJDIR}/external/freertos"
cp -r "${CUBEDIR}/Middlewares/Third_Party/FreeRTOS/Source/"* "${PROJDIR}/external/freertos/"
