/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.File;
import java.io.IOException;
import java.nio.file.AccessDeniedException;
import java.nio.file.NoSuchFileException;

import org.junit.Assert;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

public class TempReaderTest {
	
	@Rule
	public ExpectedException thrown = ExpectedException.none();

	@Test
	public void testNoSuchDevice() throws IOException {
		thrown.expect(NoSuchFileException.class);
		
		final TempReader reader = new TempReader("/dev/nosuch");
		reader.read();
	}
	
	@Test
	public void testAccessDenied() throws IOException {
		thrown.expect(AccessDeniedException.class);
		
		final TempReader reader = new TempReader("/dev/mem");
		reader.read();
	}
	
	@Test
	public void testRead() throws IOException {	
		final File file = new File("src/test/resources/file");
		final TempReader reader = new TempReader(file.getAbsolutePath());
		final String read = reader.read();
		
		Assert.assertEquals("content", read);
	}

}
