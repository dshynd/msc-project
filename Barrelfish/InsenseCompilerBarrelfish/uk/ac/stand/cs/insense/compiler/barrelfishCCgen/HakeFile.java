package uk.ac.stand.cs.insense.compiler.barrelfishCCgen;

import java.io.IOException;
import java.io.PrintStream;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import uk.ac.stand.cs.insense.compiler.barrelfishCCgen.fileHandling.FileTracker;
import uk.ac.stand.cs.insense.compiler.barrelfishCCgen.fileHandling.OutputFile;
import uk.ac.stand.cs.insense.compiler.cgen.IMakefile;
import uk.ac.standrews.cs.nds.util.ErrorHandling;

/**
 * @author dh
 *         Generates a Hakefile suitable for inclusion with Barrelfish systems
 * 
 */
public class HakeFile implements IMakefile {

	private static String name = "Hakefile";

	private final String projectName;
	
	private final String HAKEFILE_OPEN = "[ build application {"; 
	private final String HAKE_FILECLOSE = "}]";
	
	private final String ADD_LIBRARIES = "addLibraries = [" + Code.DQUOTE + Code.LIBRARY_NAME + Code.DQUOTE + "],";
	
	private final String CFILES_OPEN = "cFiles = [";
	private final String CFILES_CLOSE = "],";
	
	private final String ADD_CFLAGS_OPEN = "addCFlags = [";
	private final String ADD_CFLAGS_CLOSE = "]";
	private final String[] ADD_CFLAGS_DEFAULTS = {
			"-D COPY_ON_SEND=YES",
			"-D HEAPS=HEAP_SHARED"
	};

	HakeFile(String projectName) {
		String fileName = Paths.get(projectName).getFileName().toString();
		int extPos = fileName.lastIndexOf('.');
		if(extPos > 0){ fileName = fileName.substring(0, extPos); }
		this.projectName = fileName;
	}

	@Override
	public void generateMakeFile() {
		try {
			OutputFile f = new OutputFile(name);
			PrintStream ps = f.getStream();
			FileTracker ft = FileTracker.instance();
			List<String> implFiles = ft.getImplFilenames();

			ps.println(HAKEFILE_OPEN);
			
			ps.println(Code.TAB3 + "target = " + Code.DQUOTE + projectName + Code.DQUOTE + Code.COMMA);
			
			ps.println(Code.TAB3 + CFILES_OPEN);
			printArray(implFiles, ps);
			ps.println(Code.TAB3 + CFILES_CLOSE);
			
			ps.println(Code.TAB3 + ADD_LIBRARIES);
			
			ps.println(Code.TAB3 + ADD_CFLAGS_OPEN);
			printArray(Arrays.asList(ADD_CFLAGS_DEFAULTS), ps);
			ps.println(Code.TAB3 + ADD_CFLAGS_CLOSE);
			
			ps.println(HAKE_FILECLOSE);
			
			f.close();
		} catch (IOException e) {
			ErrorHandling.exceptionError(e, "Opening file: " + name);
		}
	}
	
	private void printArray(Iterable<String> toPrint, PrintStream ps){
		Iterator<String> i = toPrint.iterator();
		while(i.hasNext()) {
			String str = Code.TAB4 + Code.DQUOTE + i.next() + Code.DQUOTE;  
			if(i.hasNext())
				ps.println(str + Code.COMMA);
			else
				ps.println(str);
		}
	}
}