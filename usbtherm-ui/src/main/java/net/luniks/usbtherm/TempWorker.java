package net.luniks.usbtherm;

import java.util.concurrent.ExecutionException;
import java.util.logging.Logger;

import javax.swing.JLabel;
import javax.swing.SwingWorker;

/**
 * Implementation of SwingWorker reading the temperature and updating the label.
 * 
 * @author dode@luniks.net
 *
 */
public class TempWorker extends SwingWorker<Float, Void> {
	
	private static final Logger logger = Logger.getAnonymousLogger();
	
	private final TempReader tempReader;
	private final JLabel tempLabel;
	
	/**
	 * Constructs an instance using the given reader and updating the given
	 * label.
	 * 
	 * @param reader reader reading the temperature value
	 * @param thermLabel label being updated with the temperature value
	 */
	public TempWorker(final TempReader reader, final JLabel thermLabel) {
		this.tempReader = reader;
		this.tempLabel = thermLabel;
	}
		
	/**
	 * Formats the temperature value and updates the label.
	 */
	@Override
	protected void done() {
		try {
			final String text = String.format("%3.1f °C", get());
			tempLabel.setText(text);
		} catch (final InterruptedException | ExecutionException e) {
			tempLabel.setText("Error");
			logger.severe(e.getLocalizedMessage());
		}
	}

	/**
	 * Reads the temperature and returns it.
	 */
	@Override
	protected Float doInBackground() throws Exception {
		return Float.valueOf(tempReader.read());
	}
}
