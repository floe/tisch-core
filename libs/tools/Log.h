/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include <ostream>

enum LogLevel {
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG
};


class TISCH_SHARED Log {

  public:

     Log();
    ~Log();

    std::ostream& get( LogLevel level = LOG_INFO );

		void set( std::ostream& stream );

  private:

    std::ostream* m_stream;
		LogLevel m_level;
};

Log& g_log();

#endif // _LOG_H_
