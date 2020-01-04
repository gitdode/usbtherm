/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.IOException;

/**
 * Interface for various implementations reading the temperature value from the
 * device.
 * 
 * @author torsten.roemer@luniks.net
 *
 */
public interface TempReader {

	/**
	 * Reads the temperature value and returns it.
	 * 
	 * @return temperature value
	 * @throws IOException if an exception occurs reading the temperature value
	 */
	String read() throws IOException;
	
}
