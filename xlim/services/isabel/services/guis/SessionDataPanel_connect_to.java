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
 * quitado el urlCombo el 2 de Febrero de 2006, para cuando hagamos un historial
 * recuperar del cvs la versión del 1 de febrero y jugar con eso
 */

package services.isabel.services.guis;

import services.isabel.lib.*;

import java.awt.GridBagConstraints;
import java.awt.event.*;
import java.io.*;

import javax.swing.*;

import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.SERVICE;
import xedl.lib.jaxb.SESSION;
import xedl.lib.xedl.*;

/**
 *
 * @author  lailoken
 */
public class SessionDataPanel_connect_to extends javax.swing.JPanel  implements ServiceDefinitions{

  // ---------- SESSION AGENDA ----------
  private Agenda agenda;
  
  // ---------- FICHERO DE TRAZAS ---------
  private PrintWriter outTraceFile;
  
  // ---- DEFINE EL NOMBRE DEL FICHERO DE TRAZAS ----
  private static final String TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"connect_to.log";

  // ---- panel actual ----
  private int panel;
  
  //url por defecto
  private String default_url;
  
  // ---- Indica si la URL pertenece a una sesion definida en local (ya sea master o remote), una sesion remota o un fichero ----
  // ---- Solo se usa para LOAD_XEDL
  public static final int LOCAL_URL = 0;
  public static final int REMOTE_URL = 1;
  public static final int FILE_URL = 2;
  public static final int UNKNOWN_URL = 3;
  
  // ---- Panel con los datos del sitio (passwd y nickname)
  private SiteDataPanel siteDataPanel;
  
    /** Creates new form SessionDataPanel */
  public SessionDataPanel_connect_to(int panelType, JPanel siteDPanel, String the_url) {
    panel = panelType;
    siteDataPanel = (SiteDataPanel) siteDPanel;
    optionpane = null;
    default_url = the_url;
    try {
      outTraceFile =  new PrintWriter (new BufferedWriter (new FileWriter (TRACES, true)),true);
    } catch (IOException ioe) {
      ioe.printStackTrace();
      System.exit(0);
    }
    try {
      if (Constants.AGENDA_ROOT_DIR != null) agenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else agenda = new Agenda();
    } catch (Exception e) {
      e.printStackTrace(outTraceFile);
    }
    initComponents();
    loadInitValues();
    //refreshFields();
  }
  
  public SessionDataPanel_connect_to(int panelType, JPanel siteDPanel) {	  
	  this(panelType, siteDPanel, null);
  }
  /** 
   * Returns the URL type
   */
  public int getUrlType() {
    int urlType = UNKNOWN_URL;
    String url = urlCombo.getEditor().getItem().toString();
    // Hay que ver si la URL dada pertenece a una sesion definida en local, a una remota o a un fichero:
    // Primero comprobamos si tiene formato de url de isabel, es decir isabel://flow/SESSION
    String session = Servutilities.getSessionFromUrl(url);
    if (!session.equalsIgnoreCase("")) {
      // Tiene formato de Iurl
      // Ahora tenemos que ver si pertenece a una sesion definida en local o es una remota:
      if ((agenda.isLocalSession(session)) || (agenda.isRemoteSession(session))) urlType = LOCAL_URL;
      else urlType = REMOTE_URL;
    } else {
      // NO tiene formato de url isabelina. COmprobamos que es una URL que pertenece a un fichero que existe:
      File xmlFile = new File (url);
      if (xmlFile.exists()) urlType = FILE_URL;
      else urlType = UNKNOWN_URL;
    }
    return urlType;
  }

  
  /** 
   * Returns the session URL field
   */
  public String getSessionURL() {
    return urlCombo.getSelectedItem().toString();
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
   * Load values depending on the value of Name
   */
  /*Ya no hace falta hacerle un load
  public void loadSessionValuesBySessionName(String selectedSession) {
    // Borramos lo que haya en los combos que vamos a actualizar
    urlCombo.removeAllItems();
    try {
      String url = "isabel://" + Servutilities.getLocalAddrs()[0].getHostAddress() + "/" + selectedSession;
      urlCombo.addItem(url);
      urlCombo.setSelectedItem(url);
    } catch (IOException ioe) {
      outTraceFile.println("loadSessionValuesBySessionName(" + selectedSession + ")");
      ioe.printStackTrace(outTraceFile);
    }
    loadSessionValues(selectedSession);
  }
  */
  /**
   * Load values depending on the value of URL
   */
  public void loadSessionValuesBySessionUrl(String url) {
    // Borramos lo que haya en los combos que vamos a actualizar
    String urlSession = Servutilities.getSessionFromUrl(url);
    loadSessionValues(urlSession);
  }

  /**
   * Load service and quality values for the given session name
   */
  public void loadSessionValues(String selectedSession) {
    // Buscamos la sesion en la agenda
    try {
      // Borramos lo que haya en los combos que vamos a actualizar
      File xedlFile;
      switch (panel) {
        case SSS_SERVICE:
            xedlFile = agenda.getOriginalXedl(selectedSession);
          break;
        default:
            xedlFile = agenda.getFullXedl(selectedSession);
          break;
      }
      Edl the_edl = new XEDL (xedlFile.getAbsolutePath()).getEdl();
      SERVICE service = the_edl.getSESSION().getSERVICE();
      outTraceFile.println("Values for session " + selectedSession + ": " + service.getSERVICENAME() + " " + service.getSERVICEQUALITY());
     } catch (IOException ioe) {
      outTraceFile.println("loadSessionValues(" + selectedSession + ")");
      ioe.printStackTrace(outTraceFile);
    } catch (XEDLException xedle) {
      outTraceFile.println("loadSessionValues(" + selectedSession + ")");
      xedle.printStackTrace(outTraceFile);
    }
  }

  
  /**
   * Load the URL combo with the urls of the saved sessions
   */
  
  public void loadUrlCombo() {
      String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_CONNECT_TO;
      LineNumberReader urlLine = null;
	try {
	  if(default_url!=null && !default_url.equals(""))
	  {
	     urlCombo.addItem(default_url);
	  }
	  File histo = new File(history_file);
	  if(histo.exists())
	  {
		  urlLine = new LineNumberReader(new FileReader(history_file));	
	      String readUrl = null;
		  while((readUrl=urlLine.readLine())!=null)
	      {
			  urlCombo.addItem(readUrl);
	      }
	  }	  
    } catch (FileNotFoundException e) {
		e.printStackTrace();
	}
	catch (IOException e) {
		e.printStackTrace();
	}
	
  }
  
  
  public void setEnabledSessionFields (boolean sessionUrl, boolean urlFileButton, boolean sessionName, boolean sessionService, boolean sessionQuality) {
    urlLabel.setEnabled(sessionUrl);
    //urlCombo.setEnabled(sessionUrl);
    urlFormatLabel.setEnabled(sessionUrl);
    urlFormat.setEnabled(sessionUrl);
    urlExampleLabel.setEnabled(sessionUrl);
    urlExample.setEnabled(sessionUrl);
  }
    
  public void setEditableSessionFields (boolean sessionUrl, boolean sessionName, boolean sessionService, boolean sessionQuality) {
    //urlCombo.setEditable(sessionUrl);
  }
    
  /**
   * Refresh fields parameters.
   */
  
  
  /**
   * Load saved parameters.
   */
  public void loadInitValues() {
    try {
      switch (panel) {
        case SSS_SERVICE:
          // Desahibilitamos URL (con boton), QUALITY y SERVICE
          setEnabledSessionFields(false,false,true,false,false);
          setEditableSessionFields(false,false,false,false);
          //loadNameCombo();
          break;
        case CONNECT_TO_SERVICE:
          // Deshabilitamos boton URL, NAME, SERVICE y QUALITY
          setEnabledSessionFields (true,false,false,false,false);
          setEditableSessionFields(true,      false,false,false);
          loadUrlCombo();
          break;
        case LOAD_XEDL_SERVICE:
          // Deshabilitamos NAME, SERVICE y QUALITY
          setEnabledSessionFields (true,true,false,false,false);
          setEditableSessionFields(true,     false,false,false);
          //loadUrlCombo();
          break;
        default:
          // Deshabilitamos boton URL, NAME, SERVICE y QUALITY
          setEnabledSessionFields (true,false,false,false,false);
          setEditableSessionFields(true,      false,false,false);
          break;
      }
    } catch (Exception e) {
      e.printStackTrace(outTraceFile);
    }
  }

  public void saveValues() {
    switch (panel) {
      case CONNECT_TO_SERVICE:
        try {
          String[] params = {"CONNECT_TO-url", urlCombo.getEditor().getItem().toString()};
          Servutilities.saveParamsToFile(Constants.SAVED_VALUES_FILE,params);
        } catch (FileNotFoundException fnfe) {
          outTraceFile.println("saveValues()");
          fnfe.printStackTrace(outTraceFile);
        } catch (IOException ioe) {
          outTraceFile.println("saveValues()");
          ioe.printStackTrace(outTraceFile);
        }
        break;
      case LOAD_XEDL_SERVICE:
        try {
          String[] params = {"LOAD_XEDL-url", urlCombo.getEditor().getItem().toString()};
          Servutilities.saveParamsToFile(Constants.SAVED_VALUES_FILE,params);
        } catch (FileNotFoundException fnfe) {
          outTraceFile.println("saveValues()");
          fnfe.printStackTrace(outTraceFile);
        } catch (IOException ioe) {
          outTraceFile.println("saveValues()");
          ioe.printStackTrace(outTraceFile);
        }
        break;
      default:
        break;
    }
  }
  
  /** This method is called from within the constructor to
   * initialize the form.
   */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        sessionInfo = new javax.swing.JLabel();
        urlLabel = new javax.swing.JLabel();
        urlCombo = new javax.swing.JComboBox();
        urlFormat = new javax.swing.JLabel();
        urlFormatLabel = new javax.swing.JLabel();
        urlExampleLabel = new javax.swing.JLabel();
        urlExample = new javax.swing.JLabel();
        urlExample2Label = new javax.swing.JLabel();
        urlExample2 = new javax.swing.JLabel();
        urlExample3Label = new javax.swing.JLabel();
        urlExample3 = new javax.swing.JLabel();
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

        urlLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        urlLabel.setText("URL or IP * :");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
        add(urlLabel, gridBagConstraints);

        urlCombo.setEditable(true);
        urlCombo.setMinimumSize(new java.awt.Dimension(210, 19));
        urlCombo.setPreferredSize(new java.awt.Dimension(210, 19));
        urlCombo.getEditor().getEditorComponent().addKeyListener(new KeyAdapter() {
          public void keyReleased(KeyEvent e) {
            //refreshFields();
          };
          public void keyTyped(KeyEvent evt) {
				if(evt.getKeyChar()=='\n')
				{
					System.out.println("aqui aqui");
					optionpane.setValue("Connect");
				}			
			}
        });
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 10.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 15, 3, 3);
        add(urlCombo, gridBagConstraints);

        
        urlFormat.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
        urlFormat.setText("isabel://ip_address/session_name");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 10.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 3, 0);
        add(urlFormat, gridBagConstraints);

        urlFormatLabel.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
        urlFormatLabel.setText("URL Format:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 3, 0);
        add(urlFormatLabel, gridBagConstraints);

        urlExampleLabel.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
        urlExampleLabel.setText("Example 1:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 3, 0);
        add(urlExampleLabel, gridBagConstraints);

        urlExample.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
        urlExample.setText("isabel://myhost.mydomain.com/mysession");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 10.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 0, 0);
        add(urlExample, gridBagConstraints);
        
        urlExample2Label.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
        urlExample2Label.setText("Example 2:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 3, 0);
        add(urlExample2Label, gridBagConstraints);

        urlExample2.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
        urlExample2.setText("10.20.10.30");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 10.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 0, 0);
        add(urlExample2, gridBagConstraints);
        /*
        urlExample3Label.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
        urlExample3Label.setText("Example 3:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 3, 0);
        add(urlExample3Label, gridBagConstraints);

        urlExample3.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
        urlExample3.setText("machine.mydomain.com");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 10.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 0, 0);
        add(urlExample3, gridBagConstraints);
        */
        passwdLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        passwdLabel.setText("Password:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
        add(passwdLabel, gridBagConstraints);
        

        //passwdField.setColumns(2);
        passwdField.setMinimumSize(new java.awt.Dimension(160, 19));
        passwdField.setPreferredSize(new java.awt.Dimension(160, 19));    
        passwdField.setFont(new java.awt.Font("SansSerif", 0, 11));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 11.0;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.insets = new java.awt.Insets(3, 15, 3, 3);
        add(passwdField, gridBagConstraints);
        

    }
    // </editor-fold>//GEN-END:initComponents

    public void setOptionPane(JOptionPane opti) {
    	optionpane = opti;    	
    }
    
    private void urlComboActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_urlComboActionPerformed
        //refreshFields();
    }//GEN-LAST:event_urlComboActionPerformed
  
  public static void main (String[] args) {
    int panel = SSS_SERVICE;
    SiteDataPanel siteDataPanel = new SiteDataPanel(panel);
    SessionDataPanel_connect_to sDataPanel = new SessionDataPanel_connect_to(panel,siteDataPanel);
    //Object[] message = {siteDataPanel};
    JFrame frame = new JFrame();
    frame.getContentPane().setLayout(new java.awt.GridLayout(0, 1));
    JPanel panel1 = new JPanel();
    panel1.setLayout(new java.awt.GridLayout(1, 0));
    JPanel panel2 = new JPanel();
    panel2.setLayout(new java.awt.GridLayout(1, 0));
    panel1.add(sDataPanel);
    panel2.add(siteDataPanel);
    frame.getContentPane().add(panel1);
    frame.getContentPane().add(panel2);
    frame.pack();
    frame.show();
    
    SessionDataPanel_connect_to sessionDataPanel = new SessionDataPanel_connect_to(panel, siteDataPanel);
    Object[] message = {sessionDataPanel};

    JFrame parentFrame = new JFrame();
    int resultPanel = JOptionPane.showConfirmDialog(parentFrame, message, "PRUEBA",JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);
    parentFrame.dispose();
    frame.dispose();
    sessionDataPanel.saveValues();
  }
  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel sessionInfo;
    private javax.swing.JComboBox urlCombo;
    private javax.swing.JLabel urlExample;
    private javax.swing.JLabel urlExampleLabel;
    private javax.swing.JLabel urlExample2;
    private javax.swing.JLabel urlExample2Label;
    private javax.swing.JLabel urlExample3;
    private javax.swing.JLabel urlExample3Label;
    private javax.swing.JLabel urlFormat;
    private javax.swing.JLabel urlFormatLabel;
    private javax.swing.JLabel urlLabel;

    private javax.swing.JPasswordField passwdField;
    private javax.swing.JLabel passwdLabel;
    private JOptionPane optionpane;
}
