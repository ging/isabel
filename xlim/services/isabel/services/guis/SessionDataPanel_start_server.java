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
 * SessionDataPanel.java
 *
 * Created on 30 de agosto de 2004, 13:33
 */

package services.isabel.services.guis;

import services.isabel.services.ac.*;

import services.isabel.lib.*;
import services.isabel.services.options.Options;

import java.awt.Cursor;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.InetAddress;
import java.util.*;

import javax.swing.*;

import xedl.lib.jaxb.SERVICE;
import xedl.lib.jaxb.SESSION;
import xedl.lib.xedl.*;



/**
 * Clase que implementa un panel de sesion para el servicio start server
 * @author  Enrique
 */
public class SessionDataPanel_start_server extends javax.swing.JPanel  implements ServiceDefinitions{

  // ---------- SESSION AGENDA ----------
  private Agenda agenda;
  
  // ---------- FICHERO DE TRAZAS ---------
  private PrintWriter outTraceFile;
  
  // ---- DEFINE EL NOMBRE DEL FICHERO DE TRAZAS ----
  private static final String TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"start_server.log";
    
  private static final String DEFAULT_SERVICE_NAME = "meeting.act";
  private static final String DEFAULT_SERVICE_QUALITY = "1M";
  
  // ---- Indica si la URL pertenece a una sesion definida en local (ya sea master o remote), una sesion remota o un fichero ----
  // ---- Solo se usa para LOAD_XEDL
  public static final int LOCAL_URL = 0;
  public static final int REMOTE_URL = 1;
  public static final int FILE_URL = 2;
  public static final int UNKNOWN_URL = 3;
  
    
    /** Creates new form SessionDataPanel */
  public SessionDataPanel_start_server() {
    try {
      outTraceFile =  new PrintWriter (new BufferedWriter (new FileWriter (TRACES, true)),true);
    } catch (IOException ioe) {
      ioe.printStackTrace();
      System.exit(0);
    }
    try {
    	//creamos la agenda
      if (Constants.AGENDA_ROOT_DIR != null) agenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else agenda = new Agenda();
    } catch (Exception e) {
      e.printStackTrace(outTraceFile);
    }
    //inicializamos los componentes
    initComponents();
    refreshFields();
  }
  
  
  /**
   * Habilita o deshabilita el campo Passwd
   */
  public void setEnabledPasswd(boolean value) {
    passwdLabel.setEnabled(value);
    passwdField.setEnabled(value);
  }
  
  
  /**
   * Permite que el campo Passwd sesa editable o no
   */
  public void setEditablePasswd(boolean value) {
    passwdField.setEditable(value);
  }
  
  
  /** 
   * Returns the session URL field
   */
  public String getSessionURL() {
	  return result_url.getText();
  }
  
  /** 
   * Returns the session name field
   */
  public String getSessionName() {
    return nameText.getText();
  }

  /** 
   * Returns the session service field
   */
  public String getSessionService() {
    return serviceCombo.getSelectedItem().toString();
  }

  /** 
   * Returns the session quality field
   */
  public String getSessionQuality() {
    return qualityCombo.getSelectedItem().toString();
  }
 
  
  /**
   * Returns the site Passwd field
   */
  public String getSitePasswd() {
    char[] answerInParts = passwdField.getPassword();
    String answer = "";
    for (int i=0;i<answerInParts.length;i++) answer = answer + answerInParts[i];
    return answer;
  }
  
  /**
   * Método que actualiza la URL que se muestra al usuario, la que luego tiene que enviar
   * a la gente para que se conecte a la sesión
   */
  public void actualizaURL(String selectedSession) {
    // Borramos lo que haya en los combos que vamos a actualizar
    
    try {
      String address = "";
      InetAddress add = Servutilities.getRealIPAdrress();
      if(add!=null)
      {
    	  address = add.getHostAddress();
      }
      else
      {
    	  address = Servutilities.getLocalAddresses();
      }
      String url = "isabel://" + address + "/" + selectedSession;
      result_url.setText(url);
    } catch (IOException ioe) {
      outTraceFile.println("actualizaURL(" + selectedSession + ")");
      ioe.printStackTrace(outTraceFile);
    }    
  }

  
  public void saveValues() {
      try {
          outTraceFile.println("Selected session: " +nameText.getText());
          String[] params = {"SSS-session", nameText.getText()};
          Servutilities.saveParamsToFile(Constants.SAVED_VALUES_FILE,params);
        } catch (FileNotFoundException fnfe) {
          outTraceFile.println("saveValues()");
          fnfe.printStackTrace(outTraceFile);
        } catch (IOException ioe) {
          outTraceFile.println("saveValues()");
          ioe.printStackTrace(outTraceFile);
        }        
    
  }
  
  
  private void initComponents() {
    java.awt.GridBagConstraints gridBagConstraints;

    sessionInfo = new javax.swing.JLabel();
    urlLabel = new javax.swing.JLabel();
    result_url = new javax.swing.JLabel();
    nameLabel = new javax.swing.JLabel();
    nameText = new JTextField();
    serviceLabel = new javax.swing.JLabel();
    serviceCombo = new javax.swing.JComboBox();
    qualityLabel = new javax.swing.JLabel();
    qualityCombo = new javax.swing.JComboBox();
    passwdLabel = new javax.swing.JLabel();
    passwdField = new javax.swing.JPasswordField();
    

    setLayout(new java.awt.GridBagLayout());

    setMinimumSize(new java.awt.Dimension(439, 168));
    setPreferredSize(new java.awt.Dimension(460, 188));
    sessionInfo.setFont(new java.awt.Font("MS Sans Serif", 1, 16));
    sessionInfo.setText("Session Information");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(0, 3, 5, 0);
    add(sessionInfo, gridBagConstraints);

    
    nameLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    nameLabel.setText("Session Name *:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    add(nameLabel, gridBagConstraints);

    
    nameText.setMinimumSize(new java.awt.Dimension(160, 19));
    nameText.setPreferredSize(new java.awt.Dimension(160, 19));
    nameText.setText(Servutilities.get_name_from_history());
    nameText.addKeyListener(new KeyAdapter() {
        public void keyReleased(KeyEvent e) {
            actualizaURL(nameText.getText());
          };
    });
    
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.gridwidth = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 15, 3, 3);
    add(nameText, gridBagConstraints);

    serviceLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    serviceLabel.setText("Service:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 5;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    add(serviceLabel, gridBagConstraints);

    serviceCombo.setEditable(false);
    serviceCombo.setMinimumSize(new java.awt.Dimension(160, 19));
    serviceCombo.setPreferredSize(new java.awt.Dimension(160, 19));    
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 5;
    gridBagConstraints.gridwidth = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 15, 3, 3);
    add(serviceCombo, gridBagConstraints);

    qualityLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    qualityLabel.setText("Quality:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 6;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    add(qualityLabel, gridBagConstraints);

    // Obtenemos los valores del fichero act.qualities
    qualityCombo.setModel(new javax.swing.DefaultComboBoxModel(readQualities()));
    qualityCombo.setSelectedItem("2 M");    
    qualityCombo.setEditable(false);
    qualityCombo.setMinimumSize(new java.awt.Dimension(160, 19));
    qualityCombo.setPreferredSize(new java.awt.Dimension(160, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 6;
    gridBagConstraints.gridwidth = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 15, 3, 3);
    add(qualityCombo, gridBagConstraints);
    

    passwdLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    passwdLabel.setText("Password:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    add(passwdLabel, gridBagConstraints);
    

    //passwdField.setColumns(16);
    passwdField.setMinimumSize(new java.awt.Dimension(160, 19));
    passwdField.setPreferredSize(new java.awt.Dimension(160, 19));    
    passwdField.setFont(new java.awt.Font("SansSerif", 0, 11));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.insets = new java.awt.Insets(3, 15, 3, 3);
    add(passwdField, gridBagConstraints);
    
    
    urlLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    urlLabel.setText("The URL for the session will be:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(10, 10, 18, 0);
    add(urlLabel, gridBagConstraints);

    
    result_url.setEnabled(true);
    result_url.setMinimumSize(new java.awt.Dimension(210, 19));
    result_url.setPreferredSize(new java.awt.Dimension(210, 19));    
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.insets = new java.awt.Insets(10, 10, 18, 3);
    add(result_url, gridBagConstraints);
        
    
  }

  private String[] readQualities() {
	  Vector v= new Vector();
	  String[] result= new String[1];
	  String line;
	  try{
		  FileReader reader= new FileReader(Constants.QUALITY_FILE);
		  BufferedReader buffer= new BufferedReader(reader);
		  line = buffer.readLine();
		  while(line != null) {
			  if (!line.equals("")) v.add(line);
			  line= buffer.readLine();
		  }
		  buffer.close();
		  result= new String[v.size()];
		  for (int i=0; i < v.size(); i++){
			  result[i]=(String)v.elementAt(i);
		  }		  
	  }catch (Exception e){
		  ErrorPanel p= new ErrorPanel(e.getMessage(),Constants.QUALITY_FILE);
		  System.exit(-1);
	  }
	  return result;
  }
  
  
  private void refreshFields(){	  
      //Getting services installed
      ServiceNames services = new ServiceNames();
      String[] serviceNames = services.getServicesPublicNames();      
      this.serviceCombo.removeAllItems();
      int i = 0;
      String name_service_selected = Servutilities.get_service_from_history();
      int indice_selected = 1;
      while (i<serviceNames.length){
          this.serviceCombo.addItem(serviceNames[i]);
          if(serviceNames[i].equals(name_service_selected))
          {
        	  indice_selected = i;
          }
          i++;
      }     
      this.serviceCombo.setSelectedIndex(indice_selected);
      qualityCombo.setSelectedItem(Servutilities.get_quality_from_history());
      actualizaURL(Servutilities.get_name_from_history());
    }
  
  
  public void saveXEDL() {
		try{
	        String actualSessionName = this.nameText.getText();            
	        if (actualSessionName==null || actualSessionName.equals("")) return;
	        createSession(actualSessionName);                                       
	    }catch(Exception e){
	        e.printStackTrace();
	    }
	  }
  
  
  public void createSession(String sessionName) throws IOException, XEDLException{
      if (this.agenda!=null){
          this.agenda.createSession(sessionName,true);
          SESSION sesion = new SESSION();
          sesion.setID(sessionName);
          //como serviceName en vez de Conference o Localtest hay que poner el nombre de la actividad
          //conference.act o localtest.act etc
          String service_tmp = (String)serviceCombo.getSelectedItem();
          ServiceNames services = new ServiceNames();
          String service_good = services.getPrivateNameFor(service_tmp);
          
          sesion.setSERVICENAME(service_good);
          sesion.setSERVICEQUALITY((String)qualityCombo.getSelectedItem());
          sesion.setOPENSESSION(true);
	      sesion.setACCESSCONTROL("");
	      //vemos que version de isabel estamos usando y la metemos en DeliveryPlatform
	      String deliv = "Isabel " + Servutilities.getIsabelVersionClear() + " " + Servutilities.getIsabelRelease();
	      sesion.setDELIVERYPLATFORM(deliv);
          XEDL newXEDL = new XEDL();
          newXEDL.getEdl().setSESSION(sesion);
          	
          newXEDL.save( agenda.getOriginalXedl(sessionName).getAbsolutePath());
          
      }
  }
  
  
  // Variables declaration 
  private javax.swing.JTextField nameText;
  private javax.swing.JLabel nameLabel;
  private javax.swing.JComboBox qualityCombo;
  private javax.swing.JLabel qualityLabel;
  private javax.swing.JComboBox serviceCombo;
  private javax.swing.JLabel serviceLabel;
  private javax.swing.JLabel sessionInfo;
  private javax.swing.JLabel result_url;
  private javax.swing.JLabel urlLabel;
  private javax.swing.JPasswordField passwdField;
  private javax.swing.JLabel passwdLabel;
  
}
