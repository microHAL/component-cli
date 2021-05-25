/*
 * bsp_linux.cpp
 *
 *  Created on: May 22, 2021
 *      Author: pokas
 */

#include <ports/linux/General/consoleIODevice_linux.h>
#include "microhal_bsp.h"

using namespace microhal;

microhal::IODevice &debugPort = linux::consoleIODev;
