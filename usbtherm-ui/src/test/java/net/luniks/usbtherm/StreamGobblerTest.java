package net.luniks.usbtherm;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import org.junit.Assert;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.runners.MockitoJUnitRunner;

@RunWith(MockitoJUnitRunner.class)
public class StreamGobblerTest {
	
	private final ExecutorService executor = Executors.newSingleThreadExecutor();
	
	@Rule
	public ExpectedException thrown = ExpectedException.none();
	
	@Mock
	private InputStream inputStream;

	@Test
	public void testReadFail() throws IOException, InterruptedException, ExecutionException {
		Mockito.when(inputStream.read(Mockito.any(byte[].class), Mockito.anyInt(), Mockito.anyInt()))
		.thenThrow(new IOException("test failure"));
		
		thrown.expect(ExecutionException.class);
		thrown.expectMessage("test failure");
		
		final StreamGobbler gobbler = new StreamGobbler(inputStream);
		final Future<String> output = executor.submit(gobbler);
		
		output.get();
	}
	
	@Test
	public void testReadSingleLine() throws InterruptedException, ExecutionException {
		final ByteArrayInputStream inputStream = new ByteArrayInputStream("blubb".getBytes());
		
		final StreamGobbler gobbler = new StreamGobbler(inputStream);
		final Future<String> output = executor.submit(gobbler);
		
		Assert.assertEquals("blubb", output.get());
	}

	@Test
	public void testReadMultiLine() throws InterruptedException, ExecutionException {
		final String multiLine = String.format("bla%sblubb", System.lineSeparator());
		final ByteArrayInputStream inputStream = new ByteArrayInputStream(multiLine.getBytes());
		
		final StreamGobbler gobbler = new StreamGobbler(inputStream);
		final Future<String> output = executor.submit(gobbler);
		
		Assert.assertEquals(multiLine, output.get());
	}
}
