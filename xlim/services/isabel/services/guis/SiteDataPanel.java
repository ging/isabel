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

import services.isabel.lib.*;
import services.isabel.services.options.Options;
import services.isabel.services.options.XEDLSiteDataPanel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.*;
import java.util.Iterator;

import javax.swing.*;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SiteDescription;
import xedl.lib.xedl.*;

/**
 *
 * @author  lailoken
 */
public class SiteDataPanel extends javax.swing.JPanel implements ServiceDefinitions, ActionListener{
  
  // ---------- SESSION AGENDA ----------
  private Agenda agenda;
  
  // ---------- FICHERO DE TRAZAS ---------
  private PrintWriter outTraceFile;
  
  // ---- DEFINE EL NOMBRE DEL FICHERO DE TRAZAS ----
  private static final String DEFAULT_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"siteDataPanel.log";
  //SSS
  private static final String START_SESSION_SERVER_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"start_server.log";
  //CONNECT TO
  private static final String ISABEL_CONNECT_TO_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"connect_to.log";

  // ---- panel actual ----
  private int panel;
  
  private String oldID = "";
  private String oldPasswd = "";
  private String defaultID = "";
  private String defaultLocation = "";
  // esta varible guarda los ids de los participantes conocidos de la session:
  private String participants = "";
  // Varibles que guardan el flow y la session que se utilizan para averiguar los participantes
  private String session = "";
  private String oldSession = "";
  private String flow = "";
  
  private static final String DEFAULT_MSG = "The personal data below will identify you in this session. The nickname must be unique in the session.";

  private static final String PARTICIPANTS_MSG = DEFAULT_MSG ;
  
  
  /** Creates new form SessionDataPanel */
  public SiteDataPanel(int panelType) {
	
	panel = panelType;
    try {
    	switch (panel) {
    	      case SSS_SERVICE:
    	              outTraceFile =  new PrintWriter (new BufferedWriter (new FileWriter (START_SESSION_SERVER_TRACES, true)),true);
    	      break;
    	      case CONNECT_TO_SERVICE:
    	              outTraceFile =  new PrintWriter (new BufferedWriter (new FileWriter (ISABEL_CONNECT_TO_TRACES, true)),true);
              break;
              default:
    	              outTraceFile =  new PrintWriter (new BufferedWriter (new FileWriter (DEFAULT_TRACES, true)),true);
    	      break;
    	}
 } catch (IOException ioe) {
      outTraceFile.println("SiteDataPanel()");
      ioe.printStackTrace();
      System.exit(0);
    }
    try {
      if (Constants.AGENDA_ROOT_DIR != null) agenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else agenda = new Agenda();
    } catch (Exception e) {
      outTraceFile.println("SiteDataPanel()");
      e.printStackTrace(outTraceFile);
    }
    initComponents();
    initSiteDataValues();
  }

  /**
   * Devuelve el antiguo id (si es que hubo modificiacion); Devuelve "" si no hay antiguo ID.
   */
  public String getOldId() {
    return oldID;
  }
  
  /**
   * Devuelve el antiguo Passwd (si es que hubo modificiacion); Devuelve "" si no hay antiguo Passwd.
   */
  public String getOldPasswd() {
    return oldPasswd;
  }
  
  /** 
   * Returns the site ID field
   */
  public String getSiteID() {
    return idField.getText();
  }

  /** 
   * Returns the site Location field
   */
  public String getSiteLocation() {
    return locationField.getText();
  }

  /**
   * para conseguir el nombre del profile
   */
  public String getProfileName() {
	  return (String) profile_combo.getSelectedItem();
  }
  
  
  /**
   * Asigna un valor al campo session
   */
  public void setSession(String value) {
    session = value;
  }
  
  /**
   * Asigna un valor al campo participants
   */
  public void setParticipants(String value) {
    participants = value;
  }

  /**
   * Asigna un valor al campo flow
   */
  public void setFlow(String value) {
    flow = value;
  }

  /**
   * Asigna un valor al campo OldId
   */
  public void setOldId(String value) {
    oldID = value;
  }
  
  /**
   * Asigna un valor al campo OldPasswd
   */
  public void setOldPasswd(String value) {
    oldPasswd = value;
  }
  
  /**
   * Asigna un valor al campo ID
   */
  public void setSiteID(String value) {
    idField.setText(value);
  }
  
  /**
   * Asigna un valor al campo Location
   */
  public void setSiteLocation(String value) {
    locationField.setText(value);
  }
  
  /**
   * Habilita o deshabilita el campo ID
   */
  public void setEnabledID(boolean value) {
    idLabel.setEnabled(value);
    idField.setEnabled(value);
  }
  
  
  /**
   * Permite que el campo ID sea editable o no
   */
  public void setEditableID(boolean value) {
    idField.setEditable(value);
  }
  

  
  /*
   * Activa/desactiva los campos del panel en funcion de los parametros dados
   */
  public void setEnabledSiteFields (boolean siteIp, boolean siteId, boolean siteLocation, boolean sitePasswd, boolean participantsList) {     
      
      
      idLabel.setEnabled(siteId);
      idField.setEnabled(siteId);
      idExamplesLabel.setEnabled(siteId);

      if (participantsList) instructionsTextArea.setText(PARTICIPANTS_MSG);
      
      locationLabel.setEnabled(siteLocation);
      locationField.setEnabled(siteLocation);
      locationExamplesLabel.setEnabled(siteLocation);

  }

  /*
   * Activa/desactiva los campos del panel en funcion de los parametros dados
   * ahora también con la etiqueta profile y el  combo y el botón de profiles
   */
  public void setEnabledSiteFields (boolean siteIp, boolean siteId, boolean siteLocation, boolean sitePasswd, boolean participantsList, boolean profile, boolean mandatory) {     
      
      
      idLabel.setEnabled(siteId);
      idField.setEnabled(siteId);
      idExamplesLabel.setEnabled(siteId);

      if (participantsList) instructionsTextArea.setText(PARTICIPANTS_MSG);
      
      locationLabel.setEnabled(siteLocation);
      locationField.setEnabled(siteLocation);
      locationExamplesLabel.setEnabled(siteLocation);
      
      profile_label.setEnabled(profile);
      profile_combo.setEnabled(profile);
      edit_profile.setEnabled(profile);
      
      this.mandatory.setEnabled(mandatory);
  }
  
  /** 
   * Pone los campos del panel editables o no en funcion de los paramteros dados
   */
  public void setEditableSiteFields (boolean siteIp, boolean siteId, boolean siteLocation, boolean sitePasswd) {     
       
        idField.setEditable(siteId);
        locationField.setEditable(siteLocation);
  }
  
  
  /**
   * Chequea el ID introducido y lo compara con los de los ids de los participantes conocidos.
   * Si coinciden, cambia el ejemplo del Id por un mensaje ID NOT ALLOWED!
   * @returns true si el id no coincide con ninguno de los participantes
   */
  public boolean checkID() {
    String[] participantsList = participants.split(" ");
    boolean result = true;
    for (int i=0;i<participantsList.length;i++) {
      if (participantsList[i].equalsIgnoreCase(getSiteID())) {
        result = false;
      }
    }
    //Cambiamos el texto del ejemplo si es necesario
    if (!result) {
      idExamplesLabel.setForeground(new java.awt.Color(255, 0, 0));
      idExamplesLabel.setText("NICKNAME NOT ALLOWED!.");
      idExamplesLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    } else {
      idExamplesLabel.setForeground(new java.awt.Color(0, 0, 0));
      idExamplesLabel.setText("(e.g. MIT, UPM, NASA...)");
      idExamplesLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    }
    return result;
  }
  
  /**
   * Carga los valores por defecto (del fichero Options.xml) para los campos ID
   * y location.
   */
  public void loadOptionsFileValues() {
    try {
      File optionsFile = agenda.getOptionsFile();
      if (optionsFile.exists()) {
        //Cogemos el ID por defecto
    	
    	Edl miedl = new XEDL( optionsFile.getAbsolutePath()).getEdl();
    	SiteDescription todos = (SiteDescription) miedl.getSiteDescription();
    	java.util.List<SITE> sitios = todos.getSITE();
    	
    	SITE sitecero = sitios.get(0);
    	idField.setText(sitecero.getID());
    	//Cogemos el PublicName por defecto:
        defaultLocation = sitecero.getPUBLICNAME();
        locationField.setText(defaultLocation.substring(defaultLocation.indexOf(".")+1, defaultLocation.length()));
        profile_combo.setSelectedItem("Default");        
      }
    } catch (XEDLException xedle) {
      // Que falle solo implica que no rellenamos los campos... no es peligroso
      outTraceFile.println("loadOptionsFileValues()");
      xedle.printStackTrace(outTraceFile);
    } 
  }
  
  
  /**
   * Carga los valores iniciales en los campos del panel
   */
  public void initSiteDataValues() {
    try {
      switch (panel) {
        case LOAD_XEDL_SERVICE:
          siteInfo.setEnabled(false);
          instructionsTextArea.setEnabled(false);
          setEnabledSiteFields(false, false, false, false, false, false, false);
          setEditableSiteFields(false, false, false, false);
          break;
        case IRIS_SERVICE:
          try {
            siteInfo.setEnabled(true);
            instructionsTextArea.setEnabled(true);
            setEnabledSiteFields(true, true, true, true, true);
            setEditableSiteFields(true, true, true, true);
            // Cargamos los valores del ip combo:
            String savedIps = Servutilities.loadParamFromFile(Constants.SAVED_VALUES_FILE, "IRIS-ip");
            if (savedIps != null) {
              String[] ips = savedIps.split(" ");
              /*ya no hay ipcombo
              for (int i=0; i<ips.length; i++) {
                // Metemos cada valor en el combo
                ipCombo.addItem(ips[i]);
              }
              ipCombo.setSelectedItem(ips[ips.length-1]);
            */
            }
          } catch (FileNotFoundException fnfe) {
            // Que falle solo implica que no rellenamos los campos... no es peligroso
            outTraceFile.println("initSiteDataValues()");
            fnfe.printStackTrace(outTraceFile);
          } catch (IOException ioe) {
            // Que falle solo implica que no rellenamos los campos... no es peligroso
            outTraceFile.println("initSiteDataValues()");
            ioe.printStackTrace(outTraceFile);
          } catch (Exception e) {
            outTraceFile.println("initSiteDataValues()");
            e.printStackTrace(outTraceFile);
          }
          break;
        case REMOTE_CONTACT_SERVICE:
          siteInfo.setEnabled(true);
          instructionsTextArea.setEnabled(true);
          setEnabledSiteFields(false, true, true, false,false);
          setEditableSiteFields(false, true, true, false);
          loadOptionsFileValues();
          break;
        case CONTACT_SERVICE:
          siteInfo.setEnabled(true);
          instructionsTextArea.setEnabled(true);
          setEnabledSiteFields(false, true, true, false,false);
          setEditableSiteFields(false, true, true, false);
          loadOptionsFileValues();
          break;
        default:
          flow = "";
          siteInfo.setEnabled(true);
          instructionsTextArea.setEnabled(true);
          setEnabledSiteFields(false, true, true, true,true);
          setEditableSiteFields(false, true, true, true);
          loadOptionsFileValues();
          break;
      }
    } catch (Exception e) {
      outTraceFile.println("initSiteDataValues()");
      e.printStackTrace(outTraceFile);
    }
  }

    
  
  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        siteInfo = new javax.swing.JLabel();
        mandatory = new JLabel();
        instructionsTextArea = new javax.swing.JTextArea();
        participantsScrollPane = new javax.swing.JScrollPane();
        participantsTextArea = new javax.swing.JTextArea();
        idLabel = new javax.swing.JLabel();
        idField = new javax.swing.JTextField();
        idExamplesLabel = new javax.swing.JLabel();
        locationLabel = new javax.swing.JLabel();
        locationField = new javax.swing.JTextField();
        locationExamplesLabel = new javax.swing.JLabel();
        profile_label = new JLabel();
        profile_combo = new JComboBox();
        edit_profile = new JButton();
        
        setLayout(new java.awt.GridBagLayout());

        setPreferredSize(new java.awt.Dimension(526, 261));
        siteInfo.setFont(new java.awt.Font("MS Sans Serif", 1, 16));
        siteInfo.setText("Terminal Information");        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 10, 0);
        add(siteInfo, gridBagConstraints);
        
       
        instructionsTextArea.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
        instructionsTextArea.setColumns(43);
        instructionsTextArea.setEditable(false);
        instructionsTextArea.setFont(new java.awt.Font("SansSerif", 1, 12));
        instructionsTextArea.setLineWrap(true);
        instructionsTextArea.setRows(2);
        instructionsTextArea.setText("The personal data below will identify you in this session.  The nickname must be unique in the session.");
        instructionsTextArea.setWrapStyleWord(true);
        instructionsTextArea.setFocusable(false);
        instructionsTextArea.setMinimumSize(new java.awt.Dimension(120, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 5, 0);
        add(instructionsTextArea, gridBagConstraints);

        participantsScrollPane.setBorder(null);
        participantsScrollPane.setFocusable(false);
        participantsScrollPane.setMaximumSize(new java.awt.Dimension(473, 32));
        participantsScrollPane.setMinimumSize(new java.awt.Dimension(473, 0));
        participantsScrollPane.setPreferredSize(new java.awt.Dimension(473, 0));
        participantsScrollPane.setAutoscrolls(true);
        participantsTextArea.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
        participantsTextArea.setEditable(false);
        participantsTextArea.setFont(new java.awt.Font("SansSerif", 1, 12));
        participantsTextArea.setLineWrap(true);
        participantsTextArea.setWrapStyleWord(true);
        participantsTextArea.setDragEnabled(true);
        participantsTextArea.setFocusable(false);
        participantsTextArea.setMaximumSize(new java.awt.Dimension(440, 2147483647));
        participantsTextArea.setMinimumSize(new java.awt.Dimension(440, 0));
        participantsTextArea.setPreferredSize(new java.awt.Dimension(440, 0));
        participantsScrollPane.setViewportView(participantsTextArea);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 3, 5, 0);
        add(participantsScrollPane, gridBagConstraints);

        idLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        idLabel.setText("Nickname:*");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
        add(idLabel, gridBagConstraints);

        idField.setColumns(10);
        idField.setFont(new java.awt.Font("SansSerif", 0, 11));
        idField.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                idFieldKeyReleased(evt);
            }
            public void keyTyped(KeyEvent evt) {
				if(!Servutilities.estapermitido_nick(evt.getKeyChar()))
				{
					//si no es un código permitido consumo el evento
					evt.consume();
				}
			}
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 5, 0);
        add(idField, gridBagConstraints);

        idExamplesLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        idExamplesLabel.setText("(e.g. MIT, UPM, NASA...)");
        idExamplesLabel.setHorizontalTextPosition(javax.swing.SwingConstants.LEADING);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 0);
        add(idExamplesLabel, gridBagConstraints);

        locationLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        locationLabel.setText("Location:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
        add(locationLabel, gridBagConstraints);

        locationField.setColumns(10);
        locationField.setFont(new java.awt.Font("SansSerif", 0, 11));
        locationField.setMinimumSize(new java.awt.Dimension(91, 20));
        locationField.addKeyListener(new KeyAdapter(){
			public void keyTyped(KeyEvent evt) {
				if(!Servutilities.estapermitido_location(evt.getKeyChar()))
				{
					//si no es un código permitido consumo el evento
					evt.consume();
				}				
				Options.setProfile_edited(true);
			}
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 5, 0);
        add(locationField, gridBagConstraints);

        locationExamplesLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        locationExamplesLabel.setText("(e.g. Madrid, Berlin, New York...)");
        locationExamplesLabel.setHorizontalTextPosition(javax.swing.SwingConstants.LEADING);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1000.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 0);
        add(locationExamplesLabel, gridBagConstraints);
        
        profile_label.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        profile_label.setText("Profile:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
        add(profile_label, gridBagConstraints);
        
        profile_combo.setEditable(false);
        rellenaCombo();
        profile_combo.setFont(new java.awt.Font("SansSerif", 0, 15));
        profile_combo.setMinimumSize(locationField.getSize());
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 5, 0);
        profile_combo.addActionListener(this);
        add(profile_combo, gridBagConstraints);
        
        edit_profile.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
        edit_profile.setSelected(true);
        edit_profile.setText("Edit Local Configuration");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 15, 5, 0);        
        add(edit_profile, gridBagConstraints);
        edit_profile.addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(java.awt.event.ActionEvent evt) {
            editLocalConfiguration();
          }
        });
        
        mandatory.setFont(new java.awt.Font("MS Sans Serif", Font.ITALIC, 12));
        mandatory.setText("* mandatory fields");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.LAST_LINE_START;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(10, 3, 5, 0);
        add(mandatory, gridBagConstraints);

    }
    // </editor-fold>//GEN-END:initComponents

    
    public void saveProfile()throws Exception {
    	String pasaber=(String)profile_combo.getSelectedItem();
        File optionsFile = new Agenda().getOptionsFile((String)profile_combo.getSelectedItem());
        XEDL optionsXEDL;
        optionsXEDL = new XEDL( optionsFile.getAbsolutePath());
        
        SiteDescription sitedes = (SiteDescription) optionsXEDL.getEdl().getSiteDescription();        
        SITE optionsSite = sitedes.getSITE().get(0);
        String sitename = optionsSite.getID();
        
        // VALORES QUE PONGO POR DEFECTO
        optionsSite.setNetworkAccessEthernet();
        //optionsSite.setControlCapabilities("full");
       
        optionsSite.setPUBLICNAME(getSiteID() + "." + getSiteLocation());
        optionsSite.setID(getSiteID());
        
        // Lo salvo en el fichero y salgo
        optionsXEDL.save( optionsFile.getAbsolutePath());
    }
    
    
    
    /**
	 * rellena el JComboBox de los profiles
	 */
    private void rellenaCombo() {
		//antes de añadir elementos tengo que vaciar profiles
		//esremove = true;   //no atiendo a eventos
		profile_combo.removeAllItems();
		//esremove = false;
		
		try {
			String[] profs = new Agenda().getProfileNames(true);
			profile_combo.addItem("Default");
			int i;
			for (i = 0; i < profs.length; i++) {
				if(profs[i].equals("Default"))
					continue;
				profile_combo.addItem(profs[i]);
			}
			profile_combo.setSelectedItem(Options.currentProfile);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
    
    
    public void actionPerformed(ActionEvent arg0) {
		//al cambiar el combobox tengo que cargar el nick y location del profile seleccionado
    	//lo pongo como actual
		Options.currentProfile=(String)profile_combo.getSelectedItem();
		File optionsFile;
		try {
			optionsFile = new Agenda().getOptionsFile(Options.currentProfile);
			
        if (!optionsFile.exists())
            return;
        
        XEDL optionsXEDL = new XEDL( optionsFile.getAbsolutePath());
        SiteDescription sitedes = (SiteDescription) optionsXEDL.getEdl().getSiteDescription();        
        SITE optionsSite = sitedes.getSITE().get(0);
        String sitename = optionsSite.getID();
                
        idField.setText(optionsSite.getID());
        locationField.setText(optionsSite.getPUBLICNAME().substring(optionsSite.getID().length() + 1));
		} catch (IOException e) {
			e.printStackTrace();
		}
		catch (XEDLException e) {
			e.printStackTrace();
		}
	}
    
    
  private void idFieldKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_idFieldKeyReleased
    //System.out.println("Entrando..." + participants);
    switch (panel) {
      default:
        checkID();
        // No hacemos nada para los demas casos
        break;
    }
  }//GEN-LAST:event_idFieldKeyReleased
  
  /**
   * metodo que atiende el click en el botón "Edit local Configuration", llama al programa Options
   *
   */
  public void editLocalConfiguration()
  {
	  String args[] = new String[0];
	  Options.currentProfile = (String) profile_combo.getSelectedItem();
	  //System.out.println((String) profile_combo.getSelectedItem());	  
	  final Options the_options = new Options(true);
	  the_options.pack();
	  the_options.setVisible(true);
	  //una vez que vuelve, refresco los campos como cuando cambio el combobox
	  //así que en vez de hacerme un método nuevo llamo al actionperformed que uso ahí
	  rellenaCombo();
	  actionPerformed(null);
  }
  
  
  public static void main (String[] args) {
    SiteDataPanel siteDataPanel = new SiteDataPanel(REMOTE_CONTACT_SERVICE);
    String participantsIds = "averno uno dos tres cuatro";
    String session = "sesion1";
    siteDataPanel.setSession(session);
    //siteDataPanel.changeInstructions("prueba averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro averno uno dos tres cuatro FIN","sesion1");
    //siteDataPanel.changeInstructions("","sesion1");
    //siteDataPanel.changeInstructions(participantsIds,"prueba");
    Object[] message = {siteDataPanel};

    JFrame parentFrame = new JFrame();
    int resultPanel = JOptionPane.showConfirmDialog(parentFrame, message, "PRUEBA",JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);
    parentFrame.dispose();
    //siteDataPanel.saveValues();
    //sessionDataPanel.saveValues(SSS_PANEL);
  }
  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel idExamplesLabel;
    private javax.swing.JTextField idField;
    private javax.swing.JLabel idLabel;
    private javax.swing.JTextArea instructionsTextArea;
    private javax.swing.JLabel locationExamplesLabel;
    private javax.swing.JTextField locationField;
    private javax.swing.JLabel locationLabel;
    private javax.swing.JScrollPane participantsScrollPane;
    private javax.swing.JTextArea participantsTextArea;
    private javax.swing.JLabel siteInfo;
    //ENRIQUE 

    private JLabel profile_label;
    public JComboBox profile_combo;
    private JLabel mandatory;
    private JButton edit_profile;
    // End of variables declaration//GEN-END:variables
	
  
}
