/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Scanner.h"
#include <dbus/dbus.h>

#include <iostream>
#include <stdio.h>

#include <stdexcept>

Scanner::Scanner( const char* _dev, int _minrssi, int _maxage ): Thread() {

	dev = _dev;
	maxage = _maxage;
	minrssi = _minrssi;

	err = new DBusError;
  dbus_error_init(err);
  conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, err);

  if (dbus_error_is_set(err)) throw std::runtime_error(std::string("Connection Error ") + err->message); 
  if (!conn) throw std::runtime_error("Unable to create DBUS connection.");

	query();
}


Scanner::~Scanner() {
	dbus_error_free(err);
	dbus_connection_close(conn);
	delete err;
}


DevMap* Scanner::getDevices() {
	DevMap* copy = new DevMap;
	lock();
	for (DevMap::iterator btdev = devices.begin(); btdev != devices.end(); btdev++) {
		if (btdev->second.rssi >= minrssi) copy->insert( *btdev );
		//std::cout << btdev->first << " " << btdev->second.name << " ";
		//std::cout << btdev->second.rssi << " " << btdev->second.age << std::endl;
	}
	//std::cout << std::endl;
	release();
	return copy;
}


int Scanner::query() {

	DBusMessage* msg;
	DBusMessageIter args;
	DBusPendingCall* pending;

	// create a new method call and check for errors
	msg = dbus_message_new_method_call( "org.bluez",         // target for the method call
	                                    dev,                 // object to call on
	                                    "org.bluez.Adapter", // interface to call on
	                                    "DiscoverDevices"    // method name
	                                  ); //"DiscoverDevicesWithoutNameResolving"

	if (!msg) return -1;

	// append arguments
	dbus_message_iter_init_append(msg, &args);
	
	// send message and get a handle for a reply
	if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) return -2;

	if (!pending) return -3;

	dbus_connection_flush(conn);
	dbus_message_unref(msg);

	// add a rule for which messages we want to see
	std::string match = std::string("type='signal',interface='org.bluez.Adapter',path='") + dev + "'";
	dbus_bus_add_match( conn, match.c_str(), err ); 
	dbus_connection_flush( conn );

	if (dbus_error_is_set(err)) throw std::runtime_error(std::string("Match Error ") + err->message); 

	return 0;
}


void* Scanner::run() {

	DBusMessage* msg;
	DBusMessageIter args;

	// loop listening for signals being emitted
	while (true) {

		// non blocking read of the next available message
		dbus_connection_read_write(conn, 100);
		msg = dbus_connection_pop_message(conn);
		if (!msg) continue;

		lock();

		for (DevMap::iterator btdev = devices.begin(); btdev != devices.end();) {
			btdev->second.age++;
			if (btdev->second.age > maxage) devices.erase(btdev++);
			else ++btdev;
		}

		if (dbus_message_is_signal( msg, "org.bluez.Adapter", "RemoteDeviceFound" )) {
			
			char* addr;
			int devclass;
			int16_t rssi;

			dbus_message_iter_init( msg, &args );
			dbus_message_iter_get_basic( &args, &addr );
			dbus_message_iter_next( &args );
			dbus_message_iter_get_basic( &args, &devclass );
			dbus_message_iter_next( &args );
			dbus_message_iter_get_basic( &args, &rssi );

			devices[addr].rssi = rssi;
			devices[addr].age  = 0;
		}

		if (dbus_message_is_signal( msg, "org.bluez.Adapter", "RemoteNameUpdated" )) {

			char* addr;
			char* name;

			dbus_message_iter_init( msg, &args );
			dbus_message_iter_get_basic( &args, &addr );
			dbus_message_iter_next( &args );
			dbus_message_iter_get_basic( &args, &name );

			devices[addr].name = name;
			devices[addr].age  = 0;
		}

		if (dbus_message_is_signal( msg, "org.bluez.Adapter", "DiscoveryCompleted" )) query();
		if (dbus_message_is_signal( msg, "org.bluez.Adapter", "DiscoveryStarted" )) ; //std::cout <<" off we go..\n";

		release();

		// free the message
		dbus_message_unref(msg);
	}

	return 0;
}

