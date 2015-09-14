package uk.ac.stand.cs.insense.compiler.barrelfishCCgen.fileHandling;

import java.io.IOException;

public class ImplFile extends OutputFile {
	public ImplFile(String fileName) throws IOException {
		super(fileName);
		FileTracker.instance().addImplFile( fileName );
	}
	
}
