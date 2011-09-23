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

//------------------ConfigurationFrame.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.text.Document;

import sck.OidPrefix;

/** Implements a Swing interface intended to enter the configuration
 *  information needed for the master agent to work.
 *
 *  #author Oscar J. Martin Vega
 *
 */

public class ConfigurationFrame extends JFrame {  
  
  boolean changed = false;

  final boolean invokedOnCreation = !SetupClass.existeArchivo();

  //Construct the frame
  JPanel frame1 = new JPanel(null);
  JPanel frame2 = new JPanel(null);  
  JPanel frame3 = new JPanel();
  JPanel frame4 = new JPanel();  
  JButton cancelButton = new JButton();
  JButton okButton = new JButton();
  GridLayout gridLayout1 = new GridLayout();
  JButton addButton = new JButton();
  JButton delButton = new JButton();
  JTextField oidPrefixTextField = new JTextField();
  JTextField portTextField = new JTextField();
  GridLayout gridLayout2 = new GridLayout();
  ConfigurationTableModel model = new ConfigurationTableModel(invokedOnCreation);
  JTable table = new JTable(model);
  JButton helpButton = new JButton();  

  public ConfigurationFrame() {
       /* Si este frame es llamado para la creacion de la tabla de
        * configuracion, salvaeAgentsproperties es un Properties vacio,
        * pero si es llamado para una actualizacion de la tabla de confi-
        * guracion, entonces toma el valor de los slaveAgentsPorts de
        * la clase Master Agent; en este ultimo caso no olvidar finalizar
        * con MasterAgent.slaveAgentsPorts=this.slaveAgentsPorts;    */    
        
    //invokedOnCreation = SetupClass.existeArchivo();      
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);  
        
    try  {
      iniciarGUI();
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  

//Component initialization

  synchronized private void iniciarGUI() throws Exception  {    
    this.setSize(new Dimension(524, 347));    
    frame3.setLayout(gridLayout2);    
    frame4.setLayout(gridLayout1);
    cancelButton.setText("Cancel");
    cancelButton.setToolTipText("Cancelling will abandon changes introduced");
    cancelButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        cancelButton_actionPerformed(e);
      }
    });
    okButton.setText("OK");
    okButton.setToolTipText("Apply all changes done");
    okButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        okButton_actionPerformed(e);
      }
    });
    addButton.setText("Add");
    addButton.setToolTipText("Values introduced on textfields will be added to configuration information");
    addButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        addButton_actionPerformed(e);
      }
    });
    delButton.setText("Del");
    delButton.setToolTipText("Erase the selected row ");
    delButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        delButton_actionPerformed(e);
      }
    });
    oidPrefixTextField.setFont(new Font("Helvetica", 0, 15));
    oidPrefixTextField.setToolTipText("Introduce object identifier prefix in this textfield");
    oidPrefixTextField.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyPressed(KeyEvent e) {
        oidPrefixTextField_keyPressed(e);
      }
    });
    portTextField.setFont(new Font("Helvetica", 0, 15));
    portTextField.setToolTipText("Introduce port number in this textfield");
    portTextField.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyPressed(KeyEvent e) {
        portTextField_keyPressed(e);
      }
    });
    helpButton.setText("Help");
    helpButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        helpButton_actionPerformed(e);
      }
    });
    this.setResizable(false);
    this.setTitle("Master agent configuration");
    frame1.setBounds(0, 0, -1, -1);
    this.getContentPane().add(frame1, null);
    frame2.setBounds(4, 0, 421, 237);
    frame1.add(frame2, null);
    table.setBounds(0, 1, 418, 231);
    table.setFont(new Font("Helvetica", 0, 15));
    table.setShowGrid(true);
    frame2.add(table, null);
    frame3.setBounds(5, 238, 420, 41);
    frame1.add(frame3, null);
    frame3.add(oidPrefixTextField, null);
    frame3.add(portTextField, null);
    frame4.setBounds(118, 281, 194, 32);
    frame1.add(frame4, null);
    frame4.add(okButton, null);
    frame4.add(cancelButton, null);    
    addButton.setBounds(433, 76, 75, 26);
    frame1.add(addButton, null);    
    delButton.setBounds(433, 104, 75, 26);
    frame1.add(delButton, null);
    helpButton.setBounds(433, 174, 75, 26);
    frame1.add(helpButton, null);    
  }
//Overriden so we can exit on System Close

  synchronized protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
     if (!invokedOnCreation) System.exit(0);      
     this.dispose();   
    } //if
  }   // method processWindowEvent


class ConfigurationTableModel extends AbstractTableModel {        
    
    Properties slaveAgentsPorts; 
    Vector finstro = new Vector();    
          
    public ConfigurationTableModel(boolean invokedOnCreation) {           
      slaveAgentsPorts = MasterAgent.slaveAgentsPorts;                    
    }
    
    private void refresh() {
      finstro.clear();
      finstro.add(new String("OBJECT IDENTIFIER PREFIX"));      
      finstro.add(new String("SUBAGENT'S PORT"));        
           
      if (slaveAgentsPorts.size()!=0) {  
      
      try {
      
        Vector claves = new Vector();        
        
        for (Enumeration keys = slaveAgentsPorts.keys();keys.hasMoreElements();) {
          String nextOne = (String)keys.nextElement(); 
          
          if (! nextOne.equals("default")) {                      
            OidPrefix unaClave = new OidPrefix(nextOne);
            claves.add(unaClave);
          }
          
        // Sort it to show it on screen in lexicographycal order
          OidPrefix.sort(claves);
        }              
        
        // construct finstro
        String temp = new String();
        for (int i=0; i<claves.size();i++) {
          temp = (String) ((OidPrefix) claves.get(i)).toString();          
            
          finstro.add(temp);
          finstro.add(slaveAgentsPorts.getProperty(temp));
        }        
                
        if (slaveAgentsPorts.containsKey(new String("default"))) {
          finstro.add(new String("default"));
          finstro.add(slaveAgentsPorts.getProperty(new String("default")));
        }
        
      } catch(java.io.InvalidObjectException e) {
          System.out.println("\n\n\tFatal error: invalid Oid prefix in configuration file!!\n");
          System.out.println("This error has ocurred because the contents of the configuration file\nagentports.dat have been externally changed in a wrong way.\n");
          System.out.println("You should delete the file agentports.dat and run the master agent again; it\nwill ask you to introduce the configuration information again.\n\n");          
          System.exit(0);
      }
      }  
    }
    
    public int getRowCount() { 
      refresh();   
      return finstro.size()/2;
    }
    public int getColumnCount() {
      return 2;
    }
    public Object getValueAt(int row, int column) {      
      refresh();      
      return finstro.get(2*row+column);  
      
    }
    public void setValueAt(Object value,int row, int column) {     
      refresh();
      String key = (String) finstro.get(2*row);
      if (column==1) slaveAgentsPorts.setProperty(key, (String)value);
      else {
      /*column==0*/ 
        String port = slaveAgentsPorts.getProperty(key);        
        slaveAgentsPorts.remove(key);
        slaveAgentsPorts.setProperty( (String)value, port);
        
      }           
    }
  }  //ConfigurationTableModel



  synchronized void okButton_actionPerformed(ActionEvent e) {   
 
    if (model.slaveAgentsPorts.size()==0) {
         this.toBack();
         JOptionPane.showMessageDialog(null,"You must specify at least one entry for the master agent thread to work","Master agent",JOptionPane.WARNING_MESSAGE);
         this.toFront(); 
       }   
     
    else { 
      if (changed) try {        

        FileOutputStream fout = new FileOutputStream("agentports.dat");
        model.slaveAgentsPorts.store(fout,"List of the ports corresponding to each Oid prefix");
        fout.close(); 
      
        this.dispose();                 
        
      } catch (IOException ioE) {
         System.err.println(ioE.toString() );
      }
      
      if (!invokedOnCreation) {            
        System.exit(0);         
      }

  } //else       
  }

  synchronized void cancelButton_actionPerformed(ActionEvent e) {
    if (changed) {
      int answer = JOptionPane.showConfirmDialog(null,"If you cancel now the changes done will be abandoned. Cancel?","Master agent",JOptionPane.YES_NO_OPTION);
      if (answer == JOptionPane.YES_OPTION){
       System.exit(0);        
      }        
      if (answer == JOptionPane.NO_OPTION) this.toFront();
    }
    else System.exit(0);
  }

  void addButton_actionPerformed(ActionEvent e) {
  
    try {
    
      Document oidPrefixDocument = oidPrefixTextField.getDocument();
      Document portDocument = portTextField.getDocument();
      
      String oidPrefixString = oidPrefixDocument.getText(0,oidPrefixDocument.getLength());
      String portString = portDocument.getText(0,portDocument.getLength());   
      
      if (oidPrefixString.length()==0) 
        JOptionPane.showMessageDialog(null,
                                "Specify a valid Oid prefix !!",
                                "Master agent",
                                JOptionPane.ERROR_MESSAGE); 
      
      else if (portString.length()==0)  
        JOptionPane.showMessageDialog(null,
                                "Specify a valid port number !!",
                                "Master agent",
                                JOptionPane.ERROR_MESSAGE); 
      
      else {   
        model.slaveAgentsPorts.setProperty(oidPrefixString, portString);
      
        changed = true;
    
        table.repaint();  
      
        oidPrefixTextField.setText("");
        portTextField.setText("");        
      }   
  
    } catch (Exception error) {
      System.err.println(error.toString());
      System.exit(0);
      }      

  }

  void delButton_actionPerformed(ActionEvent e) {

    int[] filasSeleccionadas = table.getSelectedRows();    
    
    String[] clavesABorrar = new String[filasSeleccionadas.length];
    
                // Construccion del array claves a borrar
    for(int i=0; i<filasSeleccionadas.length; i++) {
      clavesABorrar[i] = (String)table.getValueAt(filasSeleccionadas[i],0);       
    }
                // Borrado
    for(int i=0; i<clavesABorrar.length; i++) {
      model.slaveAgentsPorts.remove(clavesABorrar[i]);
    }

    changed = true;
    
    table.repaint();
  }

  void helpButton_actionPerformed(ActionEvent e) {

   JOptionPane.showMessageDialog(null,"Specify an Oid prefix (e.g.: 1.3.6.1.4.1.2781.1. ) and port number (e.g.: 2001), so varizbles mapping this prefix will be forwarded to\nthe corresponding port;  press Add and continue including as many entries as desired;  you can also specify a default port by\nincluding the pair \"default\" and the default port number (e.g.: default : 2002). When you have finished entering data click OK.\nIf you want to delete any entries, just select the rows to be erased and click Del.\n\nIMPORTANT ADVICE: Remember to end every Oid prefix with a dot; the prefix 1.3.6.1.4.1.2781. is not the same than the prefix\n1.3.6.1.4.1.2781  !!","Master agent",JOptionPane.INFORMATION_MESSAGE);


  }

  void oidPrefixTextField_keyPressed(KeyEvent e) {  
  
    try {
     
       char teclaPulsada = e.getKeyChar();
       Character teclaPulsadaFijate = new Character(teclaPulsada);
        
       Document oidPrefixDocument = oidPrefixTextField.getDocument();
       String oidPrefixString = oidPrefixDocument.getText(0,oidPrefixDocument.getLength()) + teclaPulsadaFijate.toString();
       
       final String defaultString = new String("default");

       if (Character.isLetter(teclaPulsada)&(!defaultString.startsWith(oidPrefixString) )) {
                               
           JOptionPane.showMessageDialog(null,
                                 "In this textfield you must indicate an Oid prefix (e.g.: 1.3.6.1.4.1.2781.1. ).\nDon't include any characters like "+e.getKeyChar()+" ! The only string that can be included here is \"default\", to indicate a default port number.",
                                 "Master agent",
                                 JOptionPane.ERROR_MESSAGE); 
           oidPrefixTextField.setText(oidPrefixString.substring(0, oidPrefixString.length()-1));                      
                                                                 
       }  
       
       else if (Character.isWhitespace(teclaPulsada) ) {
      
        JOptionPane.showMessageDialog(null,
                              "In this textfield you must indicate an Oid prefix (e.g.: 1.3.6.1.4.1.2781.1. ).\nDon't include any white spaces !",
                              "Master agent",
                              JOptionPane.ERROR_MESSAGE); 
        oidPrefixTextField.setText(oidPrefixString.substring(0, oidPrefixString.length()-1));                           
        
       }  
       
       else if (teclaPulsadaFijate.equals(new Character('.'))) {
         if (oidPrefixString.length()==1) {
           JOptionPane.showMessageDialog(null,
                                "An Oid prefix can't start with a dot!!",
                                "Master agent",
                                JOptionPane.ERROR_MESSAGE); 
           oidPrefixTextField.setText(oidPrefixString.substring(0, oidPrefixString.length()-1));  
         } 
         
         else if (oidPrefixString.charAt(oidPrefixString.length()-2)=='.') {
           JOptionPane.showMessageDialog(null,
                              "Don't include two following dots in this Oid prefix !!",
                              "Master agent",
                              JOptionPane.ERROR_MESSAGE); 
           oidPrefixTextField.setText(oidPrefixString.substring(0, oidPrefixString.length()-1));  
            
         }                  
                  
       }                                                                       
                     
    } catch(Exception excepcion) {
         System.err.println(excepcion.toString());
         System.exit(0);
      }                 
  }

  void portTextField_keyPressed(KeyEvent e) {
  
      if (Character.isLetter(e.getKeyChar()) ) {
      
        JOptionPane.showMessageDialog(null,
                              "In this textfield you must indicate a port number in digits (e.g.: 1024).\nDon't include any characters like "+e.getKeyChar()+" !",
                              "Master agent",
                              JOptionPane.ERROR_MESSAGE);        
        
        Document portDocument = portTextField.getDocument();
        try {
          String portString = portDocument.getText(0,portDocument.getLength());                      
          portTextField.setText(portString.substring(0, portString.length()-1));                                
        } catch (Exception excepcion) {
            System.err.println(excepcion.toString());
            System.exit(0);        
        }  
      }                       
      
      else if (Character.isWhitespace(e.getKeyChar()) ) {
      
        JOptionPane.showMessageDialog(null,
                              "In this textfield you must indicate a port number in digits (e.g.: 1024).\nDon't include any white spaces !",
                              "Master agent",
                              JOptionPane.ERROR_MESSAGE);        
        
        Document portDocument = portTextField.getDocument();
        try {
          String portString = portDocument.getText(0,portDocument.getLength());                      
          portTextField.setText(portString.substring(0, portString.length()-1));                                
        } catch (Exception excepcion) {
            System.err.println(excepcion.toString());
            System.exit(0);        
        }  
      }                
  }
  
  
  
  
}     //class ConfigurationFrame

