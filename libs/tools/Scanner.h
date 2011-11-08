/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <Thread.h>

#include <string>
#include <map>


class DBusConnection;
class DBusError;


struct BTInfo {

	BTInfo(): name(), rssi(0), age(0), blob(0) { }

	std::string name;
	int rssi,age,blob;
};


typedef std::map<std::string,BTInfo> DevMap;

class Scanner: public Thread {

	public:

		Scanner( const char* dev = "/org/bluez/hci0", int _minrssi = -60, int _maxage = 100 );
		virtual ~Scanner();

		virtual void* run();

		DevMap* getDevices(); 

	protected:

		int query();

		DBusConnection* conn;
		DBusError* err;

		DevMap devices;

		int maxage,minrssi;

		const char* dev;

};

#endif // _SCANNER_H_

