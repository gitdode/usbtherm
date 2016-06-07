/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.IOException;

import javax.swing.JLabel;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.runners.MockitoJUnitRunner;

@RunWith(MockitoJUnitRunner.class)
public class TempWorkerTest {

	@Mock
	private TempReader reader;
	
	@Mock
	private JLabel label;
	
	@Test
	public void testException() throws IOException {
		Mockito.when(reader.read()).thenThrow(new IOException());
		
		final TempWorker worker = new TempWorker(reader, label);
		worker.execute();
		
		Mockito.verify(label, Mockito.timeout(1000)).setText("Huh?");
		Mockito.verify(reader).read();
	}
	
	@Test
	public void testHappyPath() throws IOException {
		Mockito.when(reader.read()).thenReturn("200");
		final String formatted = String.format("%3.1f °C", 20f);
		
		final TempWorker worker = new TempWorker(reader, label);
		worker.execute();
		
		Mockito.verify(label, Mockito.timeout(1000)).setText(formatted);
		Mockito.verify(reader).read();
	}

}
