/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <fstream>

#include <time.h>

#include "Log.h"


// static global logger with wrapper function
Logger& Log() {
	static Logger* g_logger = new Logger();
	return *g_logger;
}

// null stream w/o streambuffer, badbit always set
struct TISCH_SHARED nullstream: public std::ostream {
	nullstream(): std::ios(0), std::ostream(NULL) { }
	void clear( std::ios::iostate ) { }
};


// main class
Logger::Logger() {
	m_ostream = &(std::cerr);
	m_nstream = new nullstream();
	m_level = LOG_INFO;
}

Logger::~Logger() {
	delete m_nstream;
}

std::ostream& Logger::get( LogLevel level ) {
	std::ostream* res = m_nstream;
	if (level <= m_level) { 
		res = m_ostream;
		*res << time(NULL) << " ";
	}
	return *res;
}

void Logger::set( std::ostream& stream ) { m_ostream = &stream; }
void Logger::set( std::string file ) { m_ostream = new std::ofstream( file.c_str(), std::ios::app | std::ios::out ); }

void Logger::level( LogLevel level ) { m_level = level; }

