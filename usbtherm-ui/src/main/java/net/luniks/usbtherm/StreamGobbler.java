/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

/**
 * {@linkplain Callable} reading from a given {@linkplain InputStream}
 * into a string using the platform default encoding. Its purpose is to be
 * submitted to an {@linkplain ExecutorService}. The data read can be obtained
 * from {@linkplain Future#get()}.
 * 
 * @author torsten.roemer@luniks.net
 *
 */
public class StreamGobbler implements Callable<String> {

	private final InputStream input;
	
	/**
	 * Creates an instance reading from the given {@linkplain InputStream}.
	 * 
	 * @param input
	 */
	public StreamGobbler(final InputStream input) {
		this.input = input;
	}
	
	@Override
	public String call() throws IOException {		
		final StringBuilder builder = new StringBuilder();
		final BufferedReader reader = new BufferedReader(new InputStreamReader(input));
		String line = null;
		while ((line = reader.readLine()) != null) {
			if (builder.length() > 0) {
				builder.append(System.lineSeparator());
			}
			builder.append(line);
		}
		
		return builder.toString();
	}
	
}
