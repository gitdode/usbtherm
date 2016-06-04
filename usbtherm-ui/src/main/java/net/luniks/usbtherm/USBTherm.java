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
 * Simple UI for USBTherm, displaying and updating the temperature
 * at a certain interval.
 * 
 * @author dode@luniks.net
 *
 */
public class USBTherm {
	
	private static final String DEVICE = "/dev/usbtherm0";
	private static final int UPDATE_SECS = 1;
	
	private final JLabel tempLabel = new JLabel();
	private final ScheduledExecutorService executor = 
			Executors.newScheduledThreadPool(1);
	
	private TempReader tempReader;
	
	/**
	 * Initializes the reader with the device given as first argument, 
	 * default is "/dev/usbtherm0" if no argument is given.
	 */
	public USBTherm(final String[] args) {
		String device = DEVICE;
		if (args.length > 0) {
			device = args[0];
		}
		tempReader = new TempReader(device);
	}

	/**
	 * Enables antialiasing for nicer font display and builds the UI.
	 * 
	 * @param args
	 */
	public static void main(final String[] args) {
		System.setProperty("awt.useSystemAAFontSettings","on");
		System.setProperty("swing.aatext", "true");
				
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
		frame.setLocationRelativeTo(null);
		frame.setVisible(true);
		
		executor.scheduleAtFixedRate(
				() -> (new TempWorker(tempReader, tempLabel).execute()), 
				0, UPDATE_SECS, TimeUnit.SECONDS);
	}
	
}
