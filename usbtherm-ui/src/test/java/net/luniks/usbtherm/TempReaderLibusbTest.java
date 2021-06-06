/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.IOException;

import org.junit.Assert;
import org.junit.Ignore;
import org.junit.Test;

public class TempReaderLibusbTest {

	/**
	 * Fails if:
	 * <ul>
	 *   <li>libusb interface not found in PATH</li>
	 *   <li>device not connected</li>
	 * </ul>
	 * 
	 * @throws IOException
	 */
        @Ignore 
	@Test
	public void testRead() throws IOException {
		final TempReaderLibusb reader = new TempReaderLibusb();
		
		final String temp = reader.read();
		
		Assert.assertFalse(temp.isEmpty());
	}
	
}
