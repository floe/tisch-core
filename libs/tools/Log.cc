/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

Log& g_log() {
	static Log* g_logger = new Log();
	return *Log;
}

class TISCH_SHARED nstream: public std::ostream {
	nstream(): std::ostream() { }
	std::ostream& put( char c ) { return *this; }
	std::ostream& write( const char* s, std::streamsize n ) { return *this; }
};

nstream g_nstream();


Log::Log() { }
Log::~Log() { }

std::ostream& Log::get( LogLevel level = LOG_INFO ) {
	if (level >= m_level) return m_stream;
	else return g_nstream;
}

void Log::set( std::ostream& stream ) { m_stream = stream; }
