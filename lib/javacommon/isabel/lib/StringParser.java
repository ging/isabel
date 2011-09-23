/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * StringParser.java
 *
 * Created on 12 de noviembre de 2001, 18:12
 */

package isabel.lib;

import java.text.ParseException;

import java.util.ArrayList;
import java.util.Iterator;
import java.io.*; 

/** 
 * This class divide a string into words.
 * The string can contain quotes and curly-brackets in order to group several words into
 * a single word.
 * Also, backslashes can be used to scape spaces, quotes, curlybrackets and backslashes.
 *
 * @author Santiago Pavon
 * @author Fernando Escribano
 * @version 2.0
 */


public class StringParser {
    
    // PARSER STATES
    final int SEPARATOR        = 0; // Removing spaces
    final int SCAPE_SEPARATOR  = 1; // Scape any char when in separator
    final int IN_BRACKETS      = 2; // Inside curly brackets
    final int SCAPE_BRACKET    = 3; // Scape a closing curly bracket when inside brackets
    final int IN_QUOTES        = 4; // Inside double quotes
    final int IN_WORD          = 6; // Inside a word, but outside brackets and quotes
    final int SCAPE_WORD       = 7; // Scape any char when inside a word
    
    // The List with the parsed String
    private ArrayList<String> list;

    // An iterator to access the words.
    private Iterator it;


    /** Creates a new StringParser object for the specified String.
     * @param s The String to be parsed.
     * @exception <tt>Exception</tt> if curly brakets or the quotes are not closed.
     */
    public StringParser(String s) throws ParseException{
        list = parse(s);
        it = list.iterator();
    }


    /**
     *  Reset the iteration to the initial state.
     */
    public void reset() {
        it = list.iterator();
    }


    /**
     * Tests if there are more tokens available from this parsed string. 
     * If this method returns <tt>true</tt>, then a subsequent call to 
     * <tt>nextToken</tt> will successfully return a token.
     *
     * @return  <code>true</code> if and only if there is at least one token 
     *          in the string after the current position; <code>false</code> 
     *          otherwise.
     */
    public boolean hasMoreTokens() {
        return it.hasNext();
    }


    /**
     * Returns the number of tokens in the parsed string.
     *
     * @return  number of generated tokens.
     */
    public int size() {
        return list.size();
    }



    /**
     * Returns the next token from this string tokenizer.
     *
     * @return     the next token from this string tokenizer.
     * @exception  NoSuchElementException  if there are no more tokens in this
     *               parsed string.
     */
    public String nextToken() {
        return ((String)it.next());
    }
    
    /** Gets the value of the token next to the given value. This is useful when the
     * String has a "option1 value1 option2 value2 ..." structure.
     *  @param optionName Option name;
     *  @param defaultValue Value to return if option doesn't exists.
     */
    public String getOptionValue( String optionName, 
				  String defaultValue) {

	int pos = indexOf(optionName);
	if (pos == -1) {
	    return defaultValue;
	} else {
	    return tokenAt(pos+1);
	}
    }


    /**
     * Returns token at a given position.
     *
     * @return     token at position pos-th.
     * @exception  IndexOutOfBoundsException  if there is no such at token in the
     *               parsed string.
     */
    public String tokenAt(int pos) {
        return list.get(pos);
    }
    


    /**
     *  Searches for the first occurence of the given argument.
     *  @param value The desired element. 
     *  @return The index of the first occurrence of the argument, or -1 if not found.
     */
    public int indexOf(String value) {
	return list.indexOf(value);
    }


    /**
     * This method split the string s into words. A word is anything
     * delimeted by spaces, quotes, or curly brackes. A quotation mark,
     * a curly bracket (opened or closed) or a white space preceded by a backslash
     * makes no effect. The tokens are returned in a list structure.
     * If the curly brakets or the quotes are not closed, an exception is thrown.
     * @param s The String to be parsed.
     * @return An <tt>ArrayList</tt> with a word in each position.
     * @throws <tt>Exception</tt> if curly brakets or the quotes are not closed.
     */

    private ArrayList<String> parse(String s) throws ParseException {
	
	int state = SEPARATOR;
	char c;
	StringBuffer token = new StringBuffer();
	int curlyBrackets = 0; // Bracket nested level

	// This variable is true when the first letter of a token is an open curly bracket. In 
	// this case, the open curly bracket is not returned in the token. 
	// This variable is false when the first open curly bracket is placed inside a word. In 
	// this case, the open curly bracket is returned as in the token. 
	boolean word_starts_with_open_bracket = false;

	ArrayList<String> tokens = new ArrayList<String>();
	
	for (int i=0; i<s.length(); i++) {
	    
	    c = s.charAt(i);

	    // System.out.println("i="+i+" c="+c+" State="+state);	    

	    switch (state) {
		
                //-----------------------------------------------------
		
		case SEPARATOR:
		    
		    switch (c) {
			case ' ' :
			case '\t':
			case '\n': 
			    // do nothing   
			    break;

			case '{': 
			    curlyBrackets++;
			    word_starts_with_open_bracket = true;
			    state = IN_BRACKETS;
			    break;

			case '\"':
			    state = IN_QUOTES; 
			    break;

			case '\\':
			    state = SCAPE_SEPARATOR; 
			    break;

			default:
			    token.append(c);
			    state = IN_WORD;
			    break;
                    }
		    break;
		
                //-----------------------------------------------------
		
		case SCAPE_SEPARATOR:
		    
		    token.append(c);
		    state = IN_WORD;
		    break;
		    
                //-----------------------------------------------------
		
		case IN_BRACKETS:
		    
		    switch (c) {
			case '{':
			    curlyBrackets++;
			    token.append(c);
			    break;
			
			case '}':
			    if (curlyBrackets == 1) {
				curlyBrackets--;
				if (word_starts_with_open_bracket) {
				    tokens.add(token.toString());
				    token.delete(0,token.length());
				    word_starts_with_open_bracket = false;
				    state = SEPARATOR;
				} else {
				    token.append(c);
				    state = IN_WORD;
				}
			    } else { 
				if (curlyBrackets > 1) {
				    curlyBrackets--;
				    token.append(c);
				} else {
				    throw new ParseException("INTERNAL ERROR: curlyBracket must be positive.", i);
				}
			    }
			    break;
			
			case '\\':
			    state = SCAPE_BRACKET; 
			    break;
			    
			default:
			    token.append(c);
			    break;
		    } 
		    break;
		
                //-----------------------------------------------------

		case SCAPE_BRACKET:
		    
		    token.append("\\"+c);
		    state = IN_BRACKETS;
		    break;
		
                //-----------------------------------------------------
		
		case IN_QUOTES:
		    
		    switch (c) {
			case '\"':
			    if (curlyBrackets == 0) {
				tokens.add(token.toString());
				token.delete(0,token.length());
				word_starts_with_open_bracket = false;
				state = SEPARATOR;
			    } else {
				token.append(c);
			    } 
			    break;
			    
			case '\\':
			    token.append("\\"+c);
			    state = IN_QUOTES; 
			    break;
			
			default:
			    token.append(c);
			    break;
		    } 
		    break;
		
                //-----------------------------------------------------
		
		case IN_WORD: 
		    
		    switch (c) {
			case ' ' :
			case '\t':
			case '\n': 
			    tokens.add(token.toString());
			    token.delete(0,token.length());
			    word_starts_with_open_bracket = false;
			    state = SEPARATOR;
			    break;

			case '{':
			    curlyBrackets++;
			    token.append(c);
			    state = IN_BRACKETS;
			    break;

			case '\"':
			    state = IN_QUOTES; 
			    break;

			case '\\':
			    state = SCAPE_WORD; 
			    break;
			    
			default:
			    token.append(c);
			    break;
			    
                    }
		    break;
		    
		    //-----------------------------------------------------
		    
		case SCAPE_WORD:
		    
		    token.append(c);
		    state = IN_WORD;
		    break;
	    }
	    
	}

	String aux = token.toString();
	if (!aux.equals("")) 
	    tokens.add(aux);
	
	
	if ( (state != SEPARATOR) && (state != IN_WORD) ) {
	    throw new ParseException("Parse error", -1);
	}
	
	return tokens;
    }


    /**
     *  Helpper method: Apply StringParser to the given String, and returns a String[] with the tokens;
     *
     *	@param str String to parse.
     *  @return a String[] with the tokens os str.
     *  @exception <tt>ParseException</tt> if curly brakets or the quotes are not closed, or other things
     */
    public static String[] toArray(String str) throws ParseException {

	StringParser sp = new StringParser(str);
        String res[]  = new String[sp.size()];
	for (int i=0 ; i<res.length ; i++) {
	    res[i] = sp.tokenAt(i);   
	}
	return res;
    }		    

    // Test
    public static void main(String[] args) throws IOException {
	

	BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

	String line;
	
	System.out.print("Enter a list >>");
	while ( (line = in.readLine()) != null ) {
	    
	    System.out.println("  IN = "+line);
	    
	    try {
		
		StringParser sp = new StringParser(line);
		
		System.out.println("  Acceso como iterator:");
		while (sp.hasMoreTokens()) {
		    System.out.println("     "+sp.nextToken());
		}
		
		System.out.println("  Acceso por indice:");
		for (int i=0 ;  i<sp.size() ; i++) {
		    System.out.println("     "+sp.tokenAt(i));
		}
	    }
	    catch (Exception e) {
		System.out.println("  Excepcion: "+e);
	    }

	    System.out.print("Enter a list >>");
	    
	    
	}
    }
}

