/*************************************************************************\
*                                                                         *
*                             TISCH Framework                             *
*                                                                         *
*    "Tangible Interactive Surfaces for Collaboration between Humans"     *
*                                                                         *
*  The TISCH framework supports cross-platform development of multitouch  *
*  applications. For more details, visit the Sourceforge website:         *
*                                                                         *
*                      http://tisch.sourceforge.net/                      *
*                                                                         *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*                                                                         *
*  This program is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU Lesser General Public License (LGPL) as  *
*   published by the Free Software Foundation, either version 3 of the    *
*             License, or (at your option) any later version.             *
*                                                                         *
\*************************************************************************/

#ifndef _TISCH_H_
#define _TISCH_H_

#define TISCH_VERSION "libTISCH 2.0 Alpha 1 \"Ice Age\""
#define TISCH_COPY    "(c) 2010 by Florian Echtler <floe@butterbrot.org>"

#define TISCH_PORT_RAW     3332 // UDP port for uncalibrated blob positions (TUIO/OSC data)
#define TISCH_PORT_CALIB   3333 // TUIO/OSC default UDP port for calibrated blob positions
#define TISCH_PORT_EVENT 0x7AB1 // base TCP port for processed gestures/events - "TABL"

// some magic defines to make Visual Studio happy with classes in a DLL
#ifdef _MSC_VER
  #if defined(_WINDLL) || defined(_USRDLL)
    #define TISCH_SHARED __declspec( dllexport )
  #else
    #define TISCH_SHARED __declspec( dllimport )
  #endif
#else // !_MSC_VER
  #define TISCH_SHARED
#endif

#ifndef TISCH_PREFIX
  #define TISCH_PREFIX ""
#endif

#endif // _TISCH_H_
