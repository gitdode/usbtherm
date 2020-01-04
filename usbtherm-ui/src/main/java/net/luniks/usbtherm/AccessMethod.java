/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

/**
 * Enumeration of the available methods to access the device.
 * 
 * @author torsten.roemer@luniks.net
 *
 */
public enum AccessMethod {

	/**
	 * Read the temperature value from the device file 
	 * created by the Linux driver.
	 */
	DEVICEFILE, 
	
	/**
	 * Read the temperature value with the libusb interface 
	 * (platform independent).
	 */
	LIBUSB
	
}
