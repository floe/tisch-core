/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include <ostream>
#include <string>


// log levels
enum LogLevel {
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG
};


// nasty hack to generate call location string
#define LOG_STRINGIFY(x) #x
#define LOG_STRING(x) LOG_STRINGIFY(x)
#define LOG_LOCATION "\t\t(" << __FUNCTION__ << " @ " __FILE__ ":" LOG_STRING(__LINE__) ")"

// convenience macros
#define LogError(x) Log().get(LOG_ERROR) << "Error: " << x << LOG_LOCATION << std::endl
#define LogWarn(x)  Log().get(LOG_WARN)  << "Warn:  " << x << LOG_LOCATION << std::endl
#define LogInfo(x)  Log().get(LOG_INFO)  << "Info:  " << x << LOG_LOCATION << std::endl
#define LogDebug(x) Log().get(LOG_DEBUG) << "Debug: " << x << LOG_LOCATION << std::endl


// main logger class
class TISCH_SHARED Logger {

	public:

		 Logger();
		~Logger();

		std::ostream& get( LogLevel level = LOG_INFO );

		void set( std::ostream& stream );
		void set( std::string file );

		void level( LogLevel level );

	private:

		std::ostream* m_ostream;
		std::ostream* m_nstream;

		LogLevel m_level;
};

Logger& Log();

#endif // _LOG_H_

