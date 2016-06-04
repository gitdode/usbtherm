/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Reads the temperature value from the device file.
 * 
 * @author dode@luniks.net
 *
 */
public class TempReader {
	
	private final String device;
	
	/**
	 * Constructs a reader reading from the given device file.
	 * 
	 * @param device device file to read from
	 */
	public TempReader(final String device) {
		this.device = device;
	}

	/**
	 * Reads the temperature value from the device file and returns it.
	 * 
	 * @return temperature value
	 * @throws IOException if the device file does not exist, is not readable
	 * or there is a problem with the driver or device.
	 */
	public String read() throws IOException {
		final Path deviceFile = Paths.get(device);
		
		return new String(Files.readAllBytes(deviceFile));
	}
	
}
