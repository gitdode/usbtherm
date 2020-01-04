package net.luniks.usbtherm;

import java.io.IOException;

import org.junit.Assert;
import org.junit.Test;

public class TempReaderLibusbTest {

	/**
	 * Fails if:<br>
	 * - libusb interface not found in PATH
	 * - device not connected
	 * 
	 * @throws IOException
	 */
	@Test
	public void testRead() throws IOException {
		final TempReaderLibusb reader = new TempReaderLibusb();
		
		final String temp = reader.read();
		
		Assert.assertFalse(temp.isEmpty());
	}
	
}
