package uk.ac.stand.cs.insense.compiler.barrelfishCCgen;

import uk.ac.stand.cs.insense.compiler.cgen.ISend;
import uk.ac.stand.cs.insense.compiler.interfaces.ISymbolTable;
import uk.ac.stand.cs.insense.compiler.symbols.STEntry;
import uk.ac.stand.cs.insense.compiler.types.ArrayType;
import uk.ac.stand.cs.insense.compiler.types.ChannelType;
import uk.ac.stand.cs.insense.compiler.types.ITypeRep;

public class Send extends Code implements ISend {

	private static final String TEMP_VAR = "_temp_var";
	String data = "";
	STEntry channelSTE;
	private int fromContext;
	String proc_end = TAB + "// end of send op ";		

	private String try_except_handler_jump = "";
	private boolean in_try_block = false;
	
	public Send(int fromContext) {
		super();
		this.fromContext = fromContext;
	}

	/* (non-Javadoc)
	 * @see uk.ac.stand.cs.insense.compiler.Ccgen.ISend#append(java.lang.String)
	 */
	public void append( String s ) {
		this.data += s;
	}
		
	public void addChannel(STEntry ste) {
		channelSTE = ste;	
	}
	

	public void setInTryBlock(boolean in_try_block) {
		this.in_try_block = in_try_block;
	}

	/* (non-Javadoc)
	 * @see uk.ac.stand.cs.insense.compiler.Ccgen.ISend#complete()
	 */
	public void complete() {
		
		// generates something like channel_send( this->output, Construct_String0( "hello1" ), sizeof(String_pntr) ); 
		
		String channel = "";
		if( channelSTE != null ) {		// in the case of an error
			channel = channelSTE.contextualName(this.fromContext);
		}
		
		ITypeRep vtype = ((ChannelType) channelSTE.getType()).getChannel_type();
				
		// The conditional is used to put the component variable value 
		// into this->data_struct depending on type
		// Default is passing a reference down the channel 
		
				
		// If sending an array need to generate the function to copy the array
		
		if(vtype instanceof ArrayType){
			if(!ArrayConstructor.generatedConstructorAlready(vtype)){
				ArrayConstructor.generate_generator_function(vtype);
			}
			if(!ArrayConstructor.generatedCopyfunctionAlready(vtype)){
				ArrayConstructor.generate_copy_function(vtype);
			}

		}
		
		// Make call to send function
		super.append(TAB + "// Make call to send op" + NEWLINE + LCB_ + NEWLINE);
		if(requiresCopying(vtype)){
			super.append(TAB + insenseTypeToCTypeName(vtype) + TEMP_VAR + SPACE + EQUALS_ + data + SEMI + NEWLINE );
			super.append("#if" + SPACE + COPY_ON_SEND + SPACE + EQUALSEQUALS_ + "YES" + NEWLINE);
			super.append(TAB + TEMP_VAR + SPACE + EQUALS_ + potential_copy_function_call(vtype, data) + SEMI + NEWLINE );
			super.append(ENDIF_ + NEWLINE);
		} else {
			super.append(TAB + insenseTypeToCTypeName(vtype) + TEMP_VAR + SPACE + EQUALS_ + data + SEMI + NEWLINE );
		}
		if(vtype.isPointerType()){
			super.append(TAB + functionCall("DAL_incRef", TEMP_VAR)+ SEMI + NEWLINE);
		} 
		super.append(TAB);
		String handler = NULL_;
		if(in_try_block){
			handler = AMPERSAND + "ex_handler";
		} else if(fromContext == ISymbolTable.FUNCTION){
			handler = "ex_handler";
		}
		super.append(functionCall("channel_send", channel, AMPERSAND + TEMP_VAR, handler ) + SEMI + NEWLINE);
		super.append(proc_end + NEWLINE + RCB_ + NEWLINE);
		Cgen.get_instance().findEnclosingDelcarationContainer().track_call_space(MSP430Sizes.channelSendCallOverhead(vtype));
	}

}
