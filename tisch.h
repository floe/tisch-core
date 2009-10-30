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
*   Copyright (c) 2006-2009 by Florian Echtler, TUM <echtler@in.tum.de>   *
*                                                                         *
*  This program is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU Lesser General Public License (LGPL) as  *
*   published by the Free Software Foundation, either version 3 of the    *
*             License, or (at your option) any later version.             *
*                                                                         *
\*************************************************************************/

#ifndef _TISCH_H_
#define _TISCH_H_

#define TISCH_PORT_BASE 0x7AB1 // base UDP port - "TABL"

#define TISCH_PORT_RAW    TISCH_PORT_BASE-1  // raw camera image positions - 31408
#define TISCH_PORT_CALIB  TISCH_PORT_BASE    // calibrated blob positions  - 31409
#define TISCH_PORT_EVENT  TISCH_PORT_BASE+1  // processed gestures/events  - 31410

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

#endif // _TISCH_H_
