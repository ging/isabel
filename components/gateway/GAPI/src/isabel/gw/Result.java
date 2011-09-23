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
package isabel.gw;
import java.lang.String;

/**
 * This class is used to manage result values.
 */
public class Result {

  public static final int OK_CODE = 0;
  public static final String OK_DESC  = "OK: The operation was successfully completed.";
  
  public static final int ERROR = -1;
  public static final String ERROR_DESC = "ERROR: Undefined error. Operation failed.";
  
  public static final int ERROR_IMP = -1;
  public static final String ERROR_IMP_DESC = "ERROR: Operation not implemented.";
  
  public static final int ERROR_CLIENT_NOT_FOUND = -2;
  public static final String ERROR_CLIENT_NOT_FOUND_DESC = "ERROR: Client not found";
  
  public static final int ERROR_FATAL = -3;
  public static final String ERROR_FATAL_DESC = "ERROR: fatal error";
  
  /**
   * Codigo de resultado.
   */
  int code;
  /**
   * Descripcion del codigo de resultado.
   */
  String description; 
  
  /**
   * Constructor.
   * @param        code  The result code.
   * @param        description Text format result code string (OK, Login error, ...).
   */
  public Result(int code, String description)
  {
	  this.code = code;
	  this.description = description;
  }


  /**
   * Returns true if the operation was completed successfully, otherwise returns false.
   * @return       boolean 
   */
  public boolean succeded(  )
  {
	  boolean ret = false;
	  if (code >= 0)
	  {
		  ret = true;
	  }
	  return ret;
  }


  /**
   * Returns The result code.
   * @return       int
   */
  public int getCode(  )
  {
	  return code;
  }


  /**
   * Returns text format result code string.
   * @return       String
   */
  public String getDescription(  )
  {
	  return description;
  }
}
