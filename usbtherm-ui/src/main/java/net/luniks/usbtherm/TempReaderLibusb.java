/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.IOException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

/**
 * Implementation of {@linkplain TempReader} using the libusb interface to
 * read the temperature value from the device.
 * 
 * @author torsten.roemer@luniks.net
 *
 */
public class TempReaderLibusb implements TempReader {
	
	private static final String EXECUTABLE = "usbtherm";
	
	private final ExecutorService executor = Executors.newSingleThreadExecutor();

	/**
	 * Reads the temperature value using the libusb interface and returns it.
	 * 
	 * @return temperature value
	 * @throws IOException if execution of the libusb interface fails, 
	 * or there is a problem with the device.
	 */
	@Override
	public String read() throws IOException {
		final ProcessBuilder builder = new ProcessBuilder(EXECUTABLE);
		builder.redirectErrorStream(true);
		final Process process = builder.start();
		final Future<String> output = executor
				.submit(new StreamGobbler(process.getInputStream()));
		try {
			final boolean completed = process.waitFor(500, TimeUnit.MILLISECONDS);
			if (! completed) {
				throw new IOException("Timeout (500 ms) reading temperature value");
			}
			final int exit = process.exitValue();
			if (exit != 0) {
				final String message = String.format("Failed reading temperature value: %s", output.get());
				throw new IOException(message);
			}
			
			return output.get();
		} catch (final InterruptedException | ExecutionException e) {
			throw new IOException(e.getMessage());
		}
	}

}
