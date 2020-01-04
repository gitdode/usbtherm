/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

package net.luniks.usbtherm;

import java.awt.FlowLayout;
import java.awt.Font;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.SwingUtilities;

/**
 * Simple UI for USBTherm, a simple AVR micro controller based USB thermometer, 
 * displaying and updating the temperature at a certain interval.
 * 
 * @author dode@luniks.net
 *
 */
public class USBTherm {
	
	static final String DEVICE = "/dev/usbtherm0";
	static final int UPDATE_SECS = 1;
	static final String	TEMP_FORMAT = "%3.1f °C";
	static final String	ERROR_MSG = "---.- °C";
	
	private final JLabel tempLabel = new JLabel();
	private final ScheduledExecutorService executor = 
			Executors.newScheduledThreadPool(1);
	
	private final TempReader tempReader;
	
	/**
	 * Initializes the reader with the optional access method 
	 * (DEVICEFILE or LIBUSB (default)) given as first argument and the 
	 * optional device file given as second argument (only relevant for 
	 * access method DEVICEFILE, default is "/dev/usbtherm0").
	 */
	public USBTherm(final String[] args) {
		AccessMethod accessMethod = AccessMethod.LIBUSB; 
		String device = DEVICE;
		if (args.length > 0) {
			accessMethod = AccessMethod.valueOf(args[0]);
		}
		if (args.length > 1) {
			device = args[1];
		}
		
		if (accessMethod == AccessMethod.DEVICEFILE) {
			tempReader = new TempReaderDeviceFile(device);
		} else {
			tempReader = new TempReaderLibusb();
		}
	}

	/**
	 * "Forces" antialiasing (KDE?) for nicer font display and builds the UI.
	 * 
	 * @param args
	 */
	public static void main(final String[] args) {
		// http://docs.oracle.com/javase/7/docs/technotes/guides/2d/flags.html#aaFonts
		System.setProperty("awt.useSystemAAFontSettings","on");
				
		final USBTherm usbtherm = new USBTherm(args);
		SwingUtilities.invokeLater(usbtherm::buildUI);
	}
	
	/**
	 * Builds the UI and starts the executor updating the temperature.
	 */
	private void buildUI() {
		final JFrame frame = new JFrame("USBTherm");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLayout(new FlowLayout(FlowLayout.CENTER, 30, 30));
		
		tempLabel.setFont(Font.decode(Font.SANS_SERIF).deriveFont(50f));
		tempLabel.setText("000.0 °C");
		frame.add(tempLabel);
		
		frame.pack();
		frame.setMinimumSize(frame.getSize());
		frame.setLocationRelativeTo(null);
		frame.setVisible(true);
		
		executor.scheduleAtFixedRate(
				() -> new TempWorker(tempReader, tempLabel).execute(), 
				0, UPDATE_SECS, TimeUnit.SECONDS);
	}
	
}
