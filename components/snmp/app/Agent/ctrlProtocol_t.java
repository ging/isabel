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
//*******************************************************************
//FILE: ctrlProtocol_t
//
//DESCRIPTION:
//
//    This class analizes and executes the primitives that arrive to the
// control protocol or by the shell.
//

import java.util.*;

public class ctrlProtocol_t {

    //Attributes:
    //-----------
    String               AgentVersion = "SNMPv1";
    threadsManager_t     threadsManager;
    Hashtable            commandList;

    //Constructor:
    //------------
    ctrlProtocol_t (threadsManager_t threadsManager) {
        this.threadsManager = threadsManager;

        commandList = new Hashtable(5);
        commandList.put("snmp_nop","0");
        commandList.put("snmp_bye","0");
        commandList.put("snmp_quit","0");
        commandList.put("snmp_prueba","2");
        commandList.put("snmp_gui","1");
        //If you want to add a new command, you should:
        // * change the size of the hashtable.
        // * add a line like: commandList.put("commandName","Numbre of parameters")
        // * add a new entrance in the analizeCommand function.
        // * program a function to do what the command orders.
    }//End od constructor

    //Command Parser:
    //---------------
    // The return values are:
    //        1.- If the syntax of the command is right --> isOK=true & errorMessage=null
    //        2.- If the syntax is incorrect --> isOK=false & errorMessage=description of the error
    private command_t parseCommand(String commandStr) {
        command_t command;
        try {
            if (commandStr.charAt(commandStr.length()-1)!=')')
                throw new Exception ();
            StringTokenizer st  = new StringTokenizer(commandStr,"(",false);
            String instruction  = st.nextToken();
            String parameters   = st.nextToken();
            int NumberOfParameters =
                Integer.parseInt((String)commandList.get(instruction));
            parameters=parameters.substring(0,parameters.length()-1);
            StringTokenizer stp = new StringTokenizer(parameters,",",false);
            if (NumberOfParameters!= stp.countTokens())
                throw new Exception ("ERROR: wrong number of parameters");
            if (NumberOfParameters==0)
                return new command_t(instruction,null,true,null);
            else
                return new command_t(instruction,parameters.substring(0,parameters.length()),true,null);
        } catch (NoSuchElementException e) {
            return (new command_t(null,null,false,"ERROR: command not known"));
        } catch (Exception e) {
            return (new command_t(null,null,false,"ERROR: syntax error"));
        }
    }//End of parseCommand

    //function that analize the commands and executes the proper code.
    public String analizeCommand (String commandStr) {
        commandStr=commandStr.trim();
        command_t command = parseCommand(commandStr);
        if (command.getIsOK()) {
            if ((command.getInstruction()).equals("snmp_nop"))
                return (snmp_nop (command));
            if ((command.getInstruction()).equals("snmp_bye"))
                return (snmp_bye (command));
            if ((command.getInstruction()).equals("snmp_quit"))
                return (snmp_quit(command));
            if ((command.getInstruction()).equals("snmp_prueba"))
                return (snmp_prueba(command));
            if ((command.getInstruction()).equals("snmp_gui"))
                return (snmp_gui(command));
        }
        return (command.getErrorMessage());

    }//End of analizaCommand
    //End of Command Parser

    //Functions for each of the acceptable commands:
    //----------------------------------------------

    private String snmp_nop (command_t command) {
        return ("OK");
    }

    private String snmp_bye(command_t command) {
        System.exit(0);
        return("OK");
    }

    private String snmp_quit(command_t command) {
        System.exit(0);
        return("OK");
    }

    private String snmp_prueba(command_t command) {
        return("OK");
    }

    private String snmp_gui(command_t command) {
        boolean value = command.getParameters().equals("1");
        if (value) { //Let's try to launch the Monitor GUI
            if (threadsManager.getMonitorGUI()==null)
                threadsManager.setMonitorGUI(new MonitorGUI_t(threadsManager.getDataBaseHandler(), threadsManager));
            return ("OK");
        } else {
            if (threadsManager.getMonitorGUI()!=null) {
                threadsManager.deleteAllFrames();
                threadsManager.getMonitorGUI().setFinish(true);
                threadsManager.MonitorGUIOff();
            }
        }
        return ("OK");
    }
}//End of ctrlProtocol

//--------------------------------------------------------------
//CLASS command_t: is an internal class that contains all
// the information relative to a command that has arrived
// to the control port or by the shell.
//--------------------------------------------------------------

class command_t {

    //Attributes:
    //-----------
    private

        String     instruction;
        String     parameters;
        boolean     isOK;
        String     errorMessage=null;

    //Constructor:
    //------------
    public

        command_t (String instruction, String parameters, boolean isOK, String errorMessage) {
            this.instruction    = instruction;
            this.parameters    = parameters;
            this.isOK            = isOK;
            this.errorMessage    = errorMessage;
            }

    //Functions to access and modify the attributes of this class:
    public

        String    getInstruction        ()    { return instruction    ; }
        boolean    getIsOK                ()    { return isOK           ; }
        String    getParameters        ()    { return parameters     ; }
        String    getErrorMessage    ()    { return errorMessage   ; }

        void setIsOK (boolean isOK)                { this.isOK         = isOK        ; }
        void setInstruction (String instruction)   { this.instruction  = instruction ; }
        void setParameters  (String parameters)    { this.parameters   = parameters  ; }
        void setErrorMessage (String errorMessage) { this.errorMessage = errorMessage; }

}//End of command_t
