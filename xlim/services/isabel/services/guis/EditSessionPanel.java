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
 * EditSessionPanel.java
 *
 * Created on 5 de septiembre de 2004, 18:03
 */

package services.isabel.services.guis;

import javax.swing.*;

import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

/**
 *
 * @author  lailoken
 */
public class EditSessionPanel extends javax.swing.JPanel {

  // En esta variable se almacena la info del sitio seleccionado en el combo
  SITE selectedSite;
  
  // En esta varaible se almacena el nombre de la sesion seleccionada en el combo
  String selectedSessio;
  
    
  private void loadSessionValues() {
    
  }
  
  private void loadSiteValues() {
    
  }

  private void loadSiteCombo() {
    // 1.- Metemos el sitio local (el master)
    // 2.- Metemos los sitios definidos en el fichero ORIGINAL.
    // 3.- Metemos todos los sitios que estan suscritos

  }
  
  private void initFields() {
    deactivateField("bwCheckBox");
    deactivateField("exportCheckBox");
    deactivateField("roleCheckBox");
    deactivateField("errorProtectionCheckBox");
    deactivateField("connectIpCheckBox");
    deactivateField("connectionModeCheckBox");
    deactivateField("publicNameCheckBox");
    activateField("openSessionCheckBox");
    activateField("autoEnableCheckBox");
    deactivateField("portCheckBox");
    activateField("qualityCheckBox");
    activateField("serviceCheckBox");
    activateField("nameCheckBox");
  }
  
  private void activateField(String field) {
    // Ponemos los campos asociados a este checkbox en negro y los habilitamos
    if (field.equals("bwCheckBox")) {
      bwCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      // Symmetric
      symmetricBW.setEnabled(true);
      bwCombo.setEnabled(true);
      // Asymmetric
      asymmetricBW.setEnabled(true);
      upBwLabel.setEnabled(true);
      upBwCombo.setEnabled(true);
      downBwLabel.setEnabled(true);
      downBwCombo.setEnabled(true);
      refreshBwFields();
    }
    if (field.equals("exportCheckBox")) {
      //exportCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      exportCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      exportTextField.setEnabled(true);
    }
    if (field.equals("roleCheckBox")) {
      //roleCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      roleCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      roleCombo.setEnabled(true);
    }
    if (field.equals("errorProtectionCheckBox")) {
      //errorProtectionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      errorProtectionCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      protectionModeCombo.setEnabled(true);
      //nLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      //nLabel.setForeground(new java.awt.Color(0, 0, 0));
      //nSpinner.setEnabled(true);
      //kLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      //kLabel.setForeground(new java.awt.Color(0, 0, 0));
      //kSpinner.setEnabled(true);
    }
    if (field.equals("connectIpCheckBox")) {
      //connectIpCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      connectIpCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      connectToSiteCombo.setEnabled(true);
      ipLabel.setEnabled(true);
      ipCombo.setEnabled(true);
    }
    if (field.equals("connectionModeCheckBox")) {
      //connectionModeCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      connectionModeCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      connectionModeCombo.setEnabled(true);
    }
    if (field.equals("publicNameCheckBox")) {
      //publicNameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      publicNameCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      publicName.setEnabled(true);
    }
    
    
    if (field.equals("openSessionCheckBox")) {
      //openSessionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      openSessionCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      //infoLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      infoLabel.setForeground(new java.awt.Color(0, 0, 0));
      autoEnableCombo.setEnabled(true);
      autoEnableCombo.setEnabled(true);
      newParticipantButton.setEnabled(true);
      deleteParticipantButton.setEnabled(true);
      addParticipantButton.setEnabled(true);
      //subscribedSites.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      subscribedSites.setForeground(new java.awt.Color(0, 0, 0));
      //allowedSites.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      allowedSites.setForeground(new java.awt.Color(0, 0, 0));
      siteList.setEnabled(true);
      participantList.setEnabled(true);
    }
    
    // Campos de session
    if (field.equals("autoEnableCheckBox")) {
      //autoEnableCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      autoEnableCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      autoEnableCombo.setEnabled(true);
    }
    if (field.equals("portCheckBox")) {
      //portCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      portCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      portSpinner.setEnabled(true);
    }
    if (field.equals("qualityCheckBox")) {
      //qualityCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      qualityCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      qualityCombo.setEnabled(true);
    }
    if (field.equals("serviceCheckBox")) {
      //serviceCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      serviceCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      serviceCombo.setEnabled(true);
    }
    if (field.equals("nameCheckBox")) {
      //nameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      nameCheckBox.setForeground(new java.awt.Color(0, 0, 0));
      nameCombo.setEnabled(true);
    }
  }

  private void deactivateField(String field) {
    // Ponemos los campos asociados a este checkbox en negro y los habilitamos
    if (field.equals("bwCheckBox")) {
      //bwCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
      bwCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      // Symmetric
      symmetricBW.setEnabled(false);
      bwCombo.setEnabled(false);
      // Asymmetric
      asymmetricBW.setEnabled(false);
      upBwLabel.setEnabled(false);
      upBwCombo.setEnabled(false);
      downBwLabel.setEnabled(false);
      downBwCombo.setEnabled(false);
    }
    if (field.equals("exportCheckBox")) {
      //exportCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      exportCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      exportTextField.setEnabled(false);
    }
    if (field.equals("roleCheckBox")) {
      //roleCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      roleCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      roleCombo.setEnabled(false);
    }
    if (field.equals("errorProtectionCheckBox")) {
      //errorProtectionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      errorProtectionCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      protectionModeCombo.setEnabled(false);
      //nLabel.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      //nLabel.setForeground(new java.awt.Color(102, 102, 102));
      //nSpinner.setEnabled(false);
      //kLabel.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      //kLabel.setForeground(new java.awt.Color(102, 102, 102));
      //kSpinner.setEnabled(false);
    }
    if (field.equals("connectIpCheckBox")) {
      //connectIpCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      connectIpCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      connectToSiteCombo.setEnabled(false);
      ipLabel.setEnabled(false);
      ipCombo.setEnabled(false);
    }
    if (field.equals("connectionModeCheckBox")) {
      //connectionModeCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      connectionModeCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      connectionModeCombo.setEnabled(false);
    }
    if (field.equals("publicNameCheckBox")) {
      //publicNameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      publicNameCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      publicName.setEnabled(false);
    }
    
    
    if (field.equals("openSessionCheckBox")) {
      //openSessionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      openSessionCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      //infoLabel.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      infoLabel.setForeground(new java.awt.Color(102, 102, 102));
      autoEnableCombo.setEnabled(false);
      newParticipantButton.setEnabled(false);
      deleteParticipantButton.setEnabled(false);
      addParticipantButton.setEnabled(false);
      //subscribedSites.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      subscribedSites.setForeground(new java.awt.Color(102, 102, 102));
      //allowedSites.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      allowedSites.setForeground(new java.awt.Color(102, 102, 102));
      siteList.setEnabled(false);
      participantList.setEnabled(false);
    }
    
    // Campos de sesion
    if (field.equals("autoEnableCheckBox")) {
      //autoEnableCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      autoEnableCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      autoEnableCombo.setEnabled(false);
    }
    if (field.equals("portCheckBox")) {
      //portCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      portCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      portSpinner.setEnabled(false);
    }
    if (field.equals("qualityCheckBox")) {
      //qualityCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      qualityCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      qualityCombo.setEnabled(false);
    }
    if (field.equals("serviceCheckBox")) {
      //serviceCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      serviceCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      serviceCombo.setEnabled(false);
    }
    if (field.equals("nameCheckBox")) {
      //nameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 0, 12));
      nameCheckBox.setForeground(new java.awt.Color(102, 102, 102));
      nameCombo.setEnabled(false);
    }
  }

  private void saveSite (){
    // Almacena en el fichero ORIGINAL todos los datos seleccionados del sitio seleccionado en el combo
  }
  
  private void restoreSite() {
    // recupera los datos del sitio almacenados en el fichero de subscripcion y en el ORIGINAL
  }
  
  private void saveAll() {
    // Guarda los datos de sesion y sale
    
  }

  private void refreshBwFields() {
    if (symmetricBW.isSelected()) {
      // Deshabilitamos los campos de asymmetric
      upBwCombo.setEnabled(false);
      upBwLabel.setEnabled(false);
      downBwCombo.setEnabled(false);
      downBwLabel.setEnabled(false);
      // y habilitamos los de symmetric
      bwCombo.setEnabled(true);
    } else {
      // Deshabilitamos los campos de symmetric      
      bwCombo.setEnabled(false);
      // y habilitamos los de asymmetric
      upBwCombo.setEnabled(true);
      upBwLabel.setEnabled(true);
      downBwCombo.setEnabled(true);
      downBwLabel.setEnabled(true);
    }
  }

  /** Creates new form EditSessionPanel */
  public EditSessionPanel() {
    initComponents();
    initFields();
    loadSessionValues();
    loadSiteValues();
  }
  
  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  private void initComponents() {//GEN-BEGIN:initComponents
    java.awt.GridBagConstraints gridBagConstraints;

    centralPanel = new javax.swing.JTabbedPane();
    sessionBorderPanel = new javax.swing.JPanel();
    sessionPanel = new javax.swing.JPanel();
    nameCheckBox = new javax.swing.JCheckBox();
    nameCombo = new javax.swing.JComboBox();
    serviceCheckBox = new javax.swing.JCheckBox();
    serviceCombo = new javax.swing.JComboBox();
    qualityCheckBox = new javax.swing.JCheckBox();
    qualityCombo = new javax.swing.JComboBox();
    jTextArea1 = new javax.swing.JTextArea();
    portCheckBox = new javax.swing.JCheckBox();
    portSpinner = new javax.swing.JSpinner();
    autoEnableCheckBox = new javax.swing.JCheckBox();
    autoEnableCombo = new javax.swing.JComboBox();
    openSessionBorderPanel = new javax.swing.JPanel();
    openSessionPanel = new javax.swing.JPanel();
    openSessionCheckBox = new javax.swing.JCheckBox();
    infoLabel = new javax.swing.JLabel();
    exchangePanel = new javax.swing.JPanel();
    siteScrollPane = new javax.swing.JScrollPane();
    siteList = new javax.swing.JList();
    buttonPanel = new javax.swing.JPanel();
    newParticipantButton = new javax.swing.JButton();
    deleteParticipantButton = new javax.swing.JButton();
    addParticipantButton = new javax.swing.JButton();
    participantScrollPane = new javax.swing.JScrollPane();
    participantList = new javax.swing.JList();
    subscribedSites = new javax.swing.JLabel();
    allowedSites = new javax.swing.JLabel();
    siteBorderPanel = new javax.swing.JPanel();
    sitePanel = new javax.swing.JPanel();
    siteIntroText = new javax.swing.JTextArea();
    chooseSiteLabel = new javax.swing.JLabel();
    siteCombo = new javax.swing.JComboBox();
    typeSiteLabel = new javax.swing.JLabel();
    restrictionLabel = new javax.swing.JLabel();
    publicNameCheckBox = new javax.swing.JCheckBox();
    publicName = new javax.swing.JTextField();
    connectionPanel = new javax.swing.JPanel();
    connectionModeCheckBox = new javax.swing.JCheckBox();
    connectionModeCombo = new javax.swing.JComboBox();
    connectIpCheckBox = new javax.swing.JCheckBox();
    connectToSiteCombo = new javax.swing.JComboBox();
    ipLabel = new javax.swing.JLabel();
    ipCombo = new javax.swing.JComboBox();
    errorProtectionPanel = new javax.swing.JPanel();
    errorProtectionCheckBox = new javax.swing.JCheckBox();
    protectionModeCombo = new javax.swing.JComboBox();
    roleCheckBox = new javax.swing.JCheckBox();
    roleCombo = new javax.swing.JComboBox();
    exportCheckBox = new javax.swing.JCheckBox();
    exportTextField = new javax.swing.JTextField();
    bwPanel = new javax.swing.JPanel();
    symmetricBW = new javax.swing.JRadioButton();
    asymmetricBW = new javax.swing.JRadioButton();
    upBwLabel = new javax.swing.JLabel();
    upBwCombo = new javax.swing.JComboBox();
    downBwLabel = new javax.swing.JLabel();
    downBwCombo = new javax.swing.JComboBox();
    bwCombo = new javax.swing.JComboBox();
    bwCheckBox = new javax.swing.JCheckBox();
    buttonsPanel = new javax.swing.JPanel();
    saveButton = new javax.swing.JButton();
    restoreButton = new javax.swing.JButton();

    setLayout(new java.awt.CardLayout());

    centralPanel.setBorder(new javax.swing.border.TitledBorder(""));
    centralPanel.setFont(new java.awt.Font("MS Sans Serif", 1, 14));
    centralPanel.setAutoscrolls(true);
    sessionBorderPanel.setLayout(new java.awt.GridLayout(1, 0));

    sessionPanel.setLayout(new java.awt.GridBagLayout());

    sessionPanel.setBorder(new javax.swing.border.TitledBorder(""));
    nameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    nameCheckBox.setSelected(true);
    nameCheckBox.setText("Name:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
    sessionPanel.add(nameCheckBox, gridBagConstraints);

    nameCombo.setPreferredSize(new java.awt.Dimension(110, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    sessionPanel.add(nameCombo, gridBagConstraints);

    serviceCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    serviceCheckBox.setSelected(true);
    serviceCheckBox.setText("Service:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
    sessionPanel.add(serviceCheckBox, gridBagConstraints);

    serviceCombo.setPreferredSize(new java.awt.Dimension(110, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    sessionPanel.add(serviceCombo, gridBagConstraints);

    qualityCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    qualityCheckBox.setSelected(true);
    qualityCheckBox.setText("Quality:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
    sessionPanel.add(qualityCheckBox, gridBagConstraints);

    qualityCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "128 K", "256 K", "384 K", "512 K", "1 M", "2 M", "10 M" }));
    qualityCombo.setSelectedItem("2 M");
    qualityCombo.setPreferredSize(new java.awt.Dimension(110, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    sessionPanel.add(qualityCombo, gridBagConstraints);

    jTextArea1.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
    jTextArea1.setFont(new java.awt.Font("SansSerif", 1, 12));
    jTextArea1.setLineWrap(true);
    jTextArea1.setText("As Session Administrator, you can define the session parameters such as its name and quality, the session number or the type of service (telemeeting, teleclass, teleconference...) to be used.");
    jTextArea1.setWrapStyleWord(true);
    jTextArea1.setPreferredSize(new java.awt.Dimension(380, 60));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 14, 0);
    sessionPanel.add(jTextArea1, gridBagConstraints);

    portCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    portCheckBox.setText("Session Number:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 40, 3, 3);
    sessionPanel.add(portCheckBox, gridBagConstraints);

    portSpinner.setPreferredSize(new java.awt.Dimension(70, 18));
    portSpinner.setValue(new Integer(1));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 3;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    sessionPanel.add(portSpinner, gridBagConstraints);

    autoEnableCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    autoEnableCheckBox.setSelected(true);
    autoEnableCheckBox.setText("Enable Auto Start:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 40, 3, 3);
    sessionPanel.add(autoEnableCheckBox, gridBagConstraints);

    autoEnableCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "No", "Yes" }));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 3;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    sessionPanel.add(autoEnableCombo, gridBagConstraints);

    sessionBorderPanel.add(sessionPanel);

    centralPanel.addTab("Session Information", sessionBorderPanel);

    openSessionBorderPanel.setBorder(new javax.swing.border.TitledBorder(""));
    openSessionPanel.setLayout(new java.awt.GridBagLayout());

    openSessionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    openSessionCheckBox.setText("The access to the session will be restricted. ");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    openSessionPanel.add(openSessionCheckBox, gridBagConstraints);

    infoLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    infoLabel.setText("Only the following terminals could join the sesion:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 20, 6, 0);
    openSessionPanel.add(infoLabel, gridBagConstraints);

    exchangePanel.setLayout(new java.awt.GridBagLayout());

    siteScrollPane.setPreferredSize(new java.awt.Dimension(150, 200));
    siteScrollPane.setViewportView(siteList);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    exchangePanel.add(siteScrollPane, gridBagConstraints);

    buttonPanel.setLayout(new java.awt.GridBagLayout());

    newParticipantButton.setText("New ...");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.insets = new java.awt.Insets(2, 0, 2, 0);
    buttonPanel.add(newParticipantButton, gridBagConstraints);

    deleteParticipantButton.setText("<-- Delete");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 10);
    buttonPanel.add(deleteParticipantButton, gridBagConstraints);

    addParticipantButton.setText("Add -->");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(2, 0, 2, 0);
    buttonPanel.add(addParticipantButton, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    exchangePanel.add(buttonPanel, gridBagConstraints);

    participantScrollPane.setPreferredSize(new java.awt.Dimension(150, 200));
    participantScrollPane.setViewportView(participantList);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    exchangePanel.add(participantScrollPane, gridBagConstraints);

    subscribedSites.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    subscribedSites.setText("Actual Subscribed terminals:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 4, 0);
    exchangePanel.add(subscribedSites, gridBagConstraints);

    allowedSites.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    allowedSites.setText("Allowed termnas:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 4, 0);
    exchangePanel.add(allowedSites, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    openSessionPanel.add(exchangePanel, gridBagConstraints);

    openSessionBorderPanel.add(openSessionPanel);

    centralPanel.addTab("List Of Paticipants", openSessionBorderPanel);

    siteBorderPanel.setLayout(new java.awt.GridBagLayout());

    siteBorderPanel.setBorder(new javax.swing.border.TitledBorder(""));
    sitePanel.setLayout(new java.awt.GridBagLayout());

    siteIntroText.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
    siteIntroText.setEditable(false);
    siteIntroText.setFont(new java.awt.Font("SansSerif", 1, 12));
    siteIntroText.setLineWrap(true);
    siteIntroText.setText("As Session Administrator, you can force some parameters for the subscribed terminals that will join the session. Also new terminals can be created and their parameters can be edited.");
    siteIntroText.setWrapStyleWord(true);
    siteIntroText.setMinimumSize(new java.awt.Dimension(370, 60));
    siteIntroText.setPreferredSize(new java.awt.Dimension(390, 60));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 5, 0);
    sitePanel.add(siteIntroText, gridBagConstraints);

    chooseSiteLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    chooseSiteLabel.setText("1.- Type a nickname that identifies the site or select one from the list:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 2, 5);
    sitePanel.add(chooseSiteLabel, gridBagConstraints);

    siteCombo.setEditable(true);
    siteCombo.setPreferredSize(new java.awt.Dimension(120, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 5);
    sitePanel.add(siteCombo, gridBagConstraints);

    typeSiteLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    typeSiteLabel.setText("(XXX terminal)");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 2, 5);
    sitePanel.add(typeSiteLabel, gridBagConstraints);

    restrictionLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    restrictionLabel.setText("2.- Select the parameters you want to force, and edit them:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 2, 5);
    sitePanel.add(restrictionLabel, gridBagConstraints);

    publicNameCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    publicNameCheckBox.setText("Location:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 2, 3);
    sitePanel.add(publicNameCheckBox, gridBagConstraints);

    publicName.setPreferredSize(new java.awt.Dimension(240, 20));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 19);
    sitePanel.add(publicName, gridBagConstraints);

    connectionPanel.setLayout(new java.awt.GridBagLayout());

    connectionModeCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    connectionModeCheckBox.setText("Connection Mode:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 2, 3);
    connectionPanel.add(connectionModeCheckBox, gridBagConstraints);

    connectionModeCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Unicast", "Multicast", "Unicast/Multicast Gateway" }));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    connectionPanel.add(connectionModeCombo, gridBagConstraints);

    connectIpCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    connectIpCheckBox.setText("Connect to site");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 2, 3);
    connectionPanel.add(connectIpCheckBox, gridBagConstraints);

    connectToSiteCombo.setPreferredSize(new java.awt.Dimension(80, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 2);
    connectionPanel.add(connectToSiteCombo, gridBagConstraints);

    ipLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    ipLabel.setText("IP:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 3;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(0, 15, 0, 0);
    connectionPanel.add(ipLabel, gridBagConstraints);

    ipCombo.setEditable(true);
    ipCombo.setPreferredSize(new java.awt.Dimension(120, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 4;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(0, 2, 0, 2);
    connectionPanel.add(ipCombo, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 5;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 0, 0);
    sitePanel.add(connectionPanel, gridBagConstraints);

    errorProtectionPanel.setLayout(new java.awt.GridBagLayout());

    errorProtectionCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    errorProtectionCheckBox.setText("Error Protection (F.E.C. overhead):");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 3);
    errorProtectionPanel.add(errorProtectionCheckBox, gridBagConstraints);

    protectionModeCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "None", "10%", "15%", "20%", "30%", "50%", "100%" }));
    protectionModeCombo.setMinimumSize(new java.awt.Dimension(60, 19));
    protectionModeCombo.setPreferredSize(new java.awt.Dimension(60, 19));

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.insets = new java.awt.Insets(0, 9, 0, 33);
    errorProtectionPanel.add(protectionModeCombo, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 6;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 0, 0);
    sitePanel.add(errorProtectionPanel, gridBagConstraints);

    roleCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    roleCheckBox.setText("Role:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 0, 0);
    sitePanel.add(roleCheckBox, gridBagConstraints);

    roleCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "IN: Interactive", "WS: Webcast Server", "FS: FlowServer", "RC: Recorder", "GW: Gateway", "MS: Media Server" }));
    roleCombo.setPreferredSize(new java.awt.Dimension(120, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    sitePanel.add(roleCombo, gridBagConstraints);

    exportCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    exportCheckBox.setText("Export Displays to:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 0, 0);
    sitePanel.add(exportCheckBox, gridBagConstraints);

    exportTextField.setPreferredSize(new java.awt.Dimension(120, 20));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    sitePanel.add(exportTextField, gridBagConstraints);

    bwPanel.setLayout(new java.awt.GridBagLayout());

    symmetricBW.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    symmetricBW.setSelected(true);
    symmetricBW.setText("Symmetric:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    bwPanel.add(symmetricBW, gridBagConstraints);

    asymmetricBW.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    asymmetricBW.setText("Asymmetric:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    bwPanel.add(asymmetricBW, gridBagConstraints);

    upBwLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    upBwLabel.setText("UP:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 1;
    bwPanel.add(upBwLabel, gridBagConstraints);

    upBwCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "56 K", "128 K", "256 K", "384 K", "512 K", "1 M", "2 M" }));
    upBwCombo.setSelectedItem("2 M");
    upBwCombo.setPreferredSize(new java.awt.Dimension(80, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 3;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 0, 0);
    bwPanel.add(upBwCombo, gridBagConstraints);

    downBwLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    downBwLabel.setText("DOWN:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 4;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(0, 15, 0, 0);
    bwPanel.add(downBwLabel, gridBagConstraints);

    downBwCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "56 K", "128 K", "256 K", "384 K", "512 K", "1 M", "2 M" }));
    downBwCombo.setSelectedItem("2 M");
    downBwCombo.setPreferredSize(new java.awt.Dimension(80, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 5;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 0, 0);
    bwPanel.add(downBwCombo, gridBagConstraints);

    bwCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "56 K", "128 K", "256 K", "384 K", "512 K", "1 M", "2 M" }));
    bwCombo.setSelectedItem("2 M");
    bwCombo.setPreferredSize(new java.awt.Dimension(80, 19));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    bwPanel.add(bwCombo, gridBagConstraints);

    bwCheckBox.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    bwCheckBox.setText("Bandwidth:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 10, 0, 0);
    bwPanel.add(bwCheckBox, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 9;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    sitePanel.add(bwPanel, gridBagConstraints);

    buttonsPanel.setLayout(new java.awt.GridBagLayout());

    saveButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    saveButton.setText("Save Site");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
    buttonsPanel.add(saveButton, gridBagConstraints);

    restoreButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    restoreButton.setText("Restore Site");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
    buttonsPanel.add(restoreButton, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 10;
    gridBagConstraints.gridwidth = 5;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(10, 0, 5, 0);
    sitePanel.add(buttonsPanel, gridBagConstraints);

    siteBorderPanel.add(sitePanel, new java.awt.GridBagConstraints());

    centralPanel.addTab("Participants Information", siteBorderPanel);

    add(centralPanel, "card2");

  }//GEN-END:initComponents
  
  public static void main (String[] args) {
    EditSessionPanel editSessionPanel = new EditSessionPanel();
    Object[] message = {editSessionPanel};

    JFrame parentFrame = new JFrame();
    int resultPanel = JOptionPane.showConfirmDialog(parentFrame, message, "PRUEBA",JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);
    parentFrame.dispose();
    //sessionDataPanel.saveValues(SSS_PANEL);
  }
  
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton addParticipantButton;
  private javax.swing.JLabel allowedSites;
  private javax.swing.JRadioButton asymmetricBW;
  private javax.swing.JCheckBox autoEnableCheckBox;
  private javax.swing.JComboBox autoEnableCombo;
  private javax.swing.JPanel buttonPanel;
  private javax.swing.JPanel buttonsPanel;
  private javax.swing.JCheckBox bwCheckBox;
  private javax.swing.JComboBox bwCombo;
  private javax.swing.JPanel bwPanel;
  private javax.swing.JTabbedPane centralPanel;
  private javax.swing.JLabel chooseSiteLabel;
  private javax.swing.JCheckBox connectIpCheckBox;
  private javax.swing.JComboBox connectToSiteCombo;
  private javax.swing.JCheckBox connectionModeCheckBox;
  private javax.swing.JComboBox connectionModeCombo;
  private javax.swing.JPanel connectionPanel;
  private javax.swing.JButton deleteParticipantButton;
  private javax.swing.JComboBox downBwCombo;
  private javax.swing.JLabel downBwLabel;
  private javax.swing.JCheckBox errorProtectionCheckBox;
  private javax.swing.JPanel errorProtectionPanel;
  private javax.swing.JPanel exchangePanel;
  private javax.swing.JCheckBox exportCheckBox;
  private javax.swing.JTextField exportTextField;
  private javax.swing.JLabel infoLabel;
  private javax.swing.JComboBox ipCombo;
  private javax.swing.JLabel ipLabel;
  private javax.swing.JTextArea jTextArea1;
  private javax.swing.JCheckBox nameCheckBox;
  private javax.swing.JComboBox nameCombo;
  private javax.swing.JButton newParticipantButton;
  private javax.swing.JPanel openSessionBorderPanel;
  private javax.swing.JCheckBox openSessionCheckBox;
  private javax.swing.JPanel openSessionPanel;
  private javax.swing.JList participantList;
  private javax.swing.JScrollPane participantScrollPane;
  private javax.swing.JCheckBox portCheckBox;
  private javax.swing.JSpinner portSpinner;
  private javax.swing.JComboBox protectionModeCombo;
  private javax.swing.JTextField publicName;
  private javax.swing.JCheckBox publicNameCheckBox;
  private javax.swing.JCheckBox qualityCheckBox;
  private javax.swing.JComboBox qualityCombo;
  private javax.swing.JButton restoreButton;
  private javax.swing.JLabel restrictionLabel;
  private javax.swing.JCheckBox roleCheckBox;
  private javax.swing.JComboBox roleCombo;
  private javax.swing.JButton saveButton;
  private javax.swing.JCheckBox serviceCheckBox;
  private javax.swing.JComboBox serviceCombo;
  private javax.swing.JPanel sessionBorderPanel;
  private javax.swing.JPanel sessionPanel;
  private javax.swing.JPanel siteBorderPanel;
  private javax.swing.JComboBox siteCombo;
  private javax.swing.JTextArea siteIntroText;
  private javax.swing.JList siteList;
  private javax.swing.JPanel sitePanel;
  private javax.swing.JScrollPane siteScrollPane;
  private javax.swing.JLabel subscribedSites;
  private javax.swing.JRadioButton symmetricBW;
  private javax.swing.JLabel typeSiteLabel;
  private javax.swing.JComboBox upBwCombo;
  private javax.swing.JLabel upBwLabel;
  // End of variables declaration//GEN-END:variables
  
}
