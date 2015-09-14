package uk.ac.stand.cs.insense.compiler.barrelfishCCgen.fileHandling;

import java.io.IOException;

public class HeaderFile extends OutputFile {
	public HeaderFile(String fileName) throws IOException {
		super(fileName);
		FileTracker.instance().addHeaderFile( fileName );
	}
	
}
