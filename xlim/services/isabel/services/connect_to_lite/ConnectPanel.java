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
package services.isabel.services.connect_to_lite;



/**
 *
 * @author  lailoken
 */
import services.isabel.lib.*;
import java.awt.event.*;
import java.io.*;
import java.util.*; 


import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

public class ConnectPanel extends javax.swing.JPanel {
  
  // Fichero de valores de ultima sesion del panel LoadxedlPanel
  private static final String SAVED_VALUES_FILE = Constants.XLIM_CFG_WORK_DIR + Constants.FILE_SEPARATOR +"connectPanel.cfg";

  private String oldID = "";
  private String oldPasswd = "";
  
  /**
   * Devuelve el campo URL.
   */
  public String getUrlField() {
    return urlField.getSelectedItem().toString();
  }
    
  /**
   * Devuelve el campo id.
   */
  public String getIdField() {
    return idField.getText();
  }
    
  /**
   * Devuelve el antiguo id (si es que hubo modificiacion); Devuelve "" si no hay antiguo ID.
   */
  public String getOldId() {
    return oldID;
  }

  /**
   * Devuelve el campo passwd.
   */
  public String getPasswdField() {
    return passwdField.getText();
  }
    
  /**
   * Devuelve el campo syncroSlides.
   */
  public boolean getSyncroSlides() {
    return syncroSlidesBox.isSelected();
  }

  /**
   * Establece el campo id
   */    
  public void setIdField(String id) {
    idField.setText(id);
  }
    
  /**
   * Establece el campo passwd
   */    
  public void setPasswdField(String passwd) {
    passwdField.setText(passwd);
  }
    

  public void refreshFields() {
    String[] urlParts;
    String urlSession = "";
    idField.setEditable(true);
    try {
      Agenda isabelAgenda;
      if (Constants.AGENDA_ROOT_DIR != null) isabelAgenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else isabelAgenda = new Agenda();

      String[] url_total_parts;
      String[] url_parts;
      try {
        //System.out.println ("KKDLAVAK: " +urlField.getSelectedItem().toString());
        // ANTIGUO REFRESH url_total_parts = urlField.getSelectedItem().toString().split("//"); // con esto separamos el protocolo (isabel://) del resto de la url
        url_total_parts = urlField.getEditor().getItem().toString().split("//"); // con esto separamos el protocolo (isabel://) del resto de la url
        if (url_total_parts.length == 2) {
          // La url contiene el protocolo: isabel://
          url_parts = url_total_parts[1].split("/");
        } else {
          // La url no contiene el protocolo: isabel://
          url_parts = url_total_parts[0].split("/");
        }
        //ahora sin mayúsculas todo
        //urlSession = url_parts[1].toUpperCase();
        urlSession = url_parts[1];
      } catch (Exception ee) {
        //System.out.println("SALIDA 11");        
        //idField.setText("");
        idField.setEditable(true);
        passwdField.setText("");
        passwdField.setEditable(true);
      }
      //Ahora para esta sesion tenemos que pillar el local.xedl.
      XEDL localXEDL = new XEDL (isabelAgenda.getLocalXedl(urlSession).getPath());
      SiteList localSiteList = new SiteList(localXEDL);
      SITE localSite = localSiteList.getSite(String.valueOf(localSiteList.getSiteIds().get(0)));
      String newID;
      try {
        //System.out.println("SALIDA 0");        
        newID = localSite.getID();
      } catch (Exception getIDException) {
        //System.out.println("SALIDA 1");
        //newID = idField.getText();
        newID = "";
      }
      idField.setText(newID);
      if (!newID.equals("")) {
        //System.out.println("SALIDA 2");
        idField.setEditable(false);
        // Miramos si tiene passwd; si lo tiene no dejamos editar el campo passwd.
        String oldPasswd = localSite.getAccessControl();
        if (oldPasswd != null) {
          try {
            //System.out.println("SALIDA 3");
            // el campo accessControl de XEDL tiene dos partes separadas por espacio. Nos interesa la segunda.
            oldPasswd = oldPasswd.split(" ")[1];
            passwdField.setText(oldPasswd);
            passwdField.setEditable(false);
          } catch (Exception e) {
            //System.out.println("SALIDA 4");
          }
        } else {
          //System.out.println("SALIDA 5");
          passwdField.setText("");
          passwdField.setEditable(true);
        }        
      } else {
        //System.out.println("SALIDA 6");
        idField.setEditable(true);
        passwdField.setText("");
        passwdField.setEditable(true);
      }
    } catch (Exception ee) {
      //idField.setText("");
      //System.out.println("SALIDA 7");
      idField.setEditable(true);
      passwdField.setText("");
      passwdField.setEditable(true);
    }
  }
  
  public void loadSavedValues() {
    // Leemos el fichero de ultimos valores...
    String savedUrl = "";
    try {
      Properties pro = new Properties();
      pro.load(new FileInputStream(SAVED_VALUES_FILE));
      savedUrl = pro.getProperty("url","");
    } catch (IOException ioe){
      // Non pasa nada...
    }
    
    //urlField = new JTextField(getUrlField(),20);
    // Para generar el comobox, cogemos todas las sesiones remotas definidas
    Agenda isabelAgenda = null;
    try {
      if (Constants.AGENDA_ROOT_DIR != null) isabelAgenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else isabelAgenda = new Agenda();
      String[] remoteSessions = isabelAgenda.getRemoteSessions();
      int counter = 0;
      for (int i = 0; i < remoteSessions.length; i++) { //Populate it.
        // Para cada session:
        // Leemos el fichero URL
        try {
          LineNumberReader urlLine = new LineNumberReader(new FileReader(isabelAgenda.getURLFile(remoteSessions[i])));
          // y lo a�adimos al combobox
          String readUrl = urlLine.readLine();
          urlField.addItem(readUrl);
          //System.out.println("read URL: " + readUrl);
          if (readUrl.equals(savedUrl)) {
            //urlField.setSelectedIndex(counter+1);
            //System.out.println("Counter: " + counter);
            urlField.setSelectedIndex(counter);
          }
          counter++;
        } catch (Exception e) {
        }
      }
    } catch (Exception e){
    }    
  }
  
  
  /** Creates new form ConnectToPanel */
  public ConnectPanel() {
    initComponents();
    loadSavedValues();
    refreshFields();
  }
  
  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  private void initComponents() {//GEN-BEGIN:initComponents
    java.awt.GridBagConstraints gridBagConstraints;

    globalPanel = new javax.swing.JPanel();
    titlePanel = new javax.swing.JPanel();
    titleLabel = new javax.swing.JLabel();
    leftPanel = new javax.swing.JPanel();
    scrollPanel = new javax.swing.JScrollPane();
    descriptionArea = new javax.swing.JTextArea();
    rightPanel = new javax.swing.JPanel();
    fieldsPanel = new javax.swing.JPanel();
    sessionInfo = new javax.swing.JLabel();
    urlLabel = new javax.swing.JLabel();
    urlField = new javax.swing.JComboBox();
    siteInfo = new javax.swing.JLabel();
    idLabel = new javax.swing.JLabel();
    idField = new javax.swing.JTextField();
    passwdLabel = new javax.swing.JLabel();
    passwdField = new javax.swing.JPasswordField();
    urlFormat = new javax.swing.JLabel();
    urlExample = new javax.swing.JLabel();
    urlFormatLabel = new javax.swing.JLabel();
    urlExampleLabel = new javax.swing.JLabel();
    syncroSlidesBox = new javax.swing.JCheckBox();
    idModified = new javax.swing.JCheckBox();

    setLayout(new java.awt.GridBagLayout());

    globalPanel.setLayout(new java.awt.GridBagLayout());

    titlePanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(5, 5, 5, 5)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    titleLabel.setFont(new java.awt.Font("Serif", 3, 24));
    titleLabel.setText("Connect To Session");
    titleLabel.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
    titlePanel.add(titleLabel);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    globalPanel.add(titlePanel, gridBagConstraints);

    leftPanel.setLayout(new java.awt.GridBagLayout());

    leftPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    leftPanel.setPreferredSize(new java.awt.Dimension(287, 249));
    scrollPanel.setBorder(null);
    scrollPanel.setPreferredSize(new java.awt.Dimension(260, 200));
    scrollPanel.setAutoscrolls(true);
    descriptionArea.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
    descriptionArea.setColumns(40);
    descriptionArea.setEditable(false);
    descriptionArea.setFont(new java.awt.Font("Microsoft Sans Serif", 1, 12));
    descriptionArea.setLineWrap(true);
    descriptionArea.setRows(10);
    descriptionArea.setText("   Connect your station to an ISABEL session in three simple steps:\n\n   1.- Type or select the URL that defines the session you want to join.\n\n   2.- Type a nickname that identifies your institution. You can protect your nickname with a password.\n\n   3.- Click the ACCEPT button to join the session and feel the ISABEL experience!");
    descriptionArea.setWrapStyleWord(true);
    scrollPanel.setViewportView(descriptionArea);

    leftPanel.add(scrollPanel, new java.awt.GridBagConstraints());

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 3);
    globalPanel.add(leftPanel, gridBagConstraints);

    rightPanel.setLayout(new java.awt.GridBagLayout());

    rightPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)))))));
    fieldsPanel.setLayout(new java.awt.GridBagLayout());

    sessionInfo.setFont(new java.awt.Font("MS Sans Serif", 1, 14));
    sessionInfo.setText("Session Information:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = 6;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 3, 5, 0);
    fieldsPanel.add(sessionInfo, gridBagConstraints);

    urlLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    urlLabel.setText("URL:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    fieldsPanel.add(urlLabel, gridBagConstraints);

    urlField.setEditable(true);
    urlField.setPreferredSize(new java.awt.Dimension(140, 19));
    urlField.getEditor().getEditorComponent().addKeyListener(new KeyAdapter() {
      public void keyReleased(KeyEvent e) {
        urlFieldEditorKeyReleased(e);
      };
    });

    urlField.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        urlFieldActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.gridwidth = 6;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 20, 3, 3);
    fieldsPanel.add(urlField, gridBagConstraints);

    siteInfo.setFont(new java.awt.Font("MS Sans Serif", 1, 14));
    siteInfo.setText("Institution Information:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 6;
    gridBagConstraints.gridwidth = 6;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(10, 3, 5, 0);
    fieldsPanel.add(siteInfo, gridBagConstraints);

    idLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    idLabel.setText("Institution Nickname:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.gridwidth = 6;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    fieldsPanel.add(idLabel, gridBagConstraints);

    idField.setColumns(10);
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 6;
    gridBagConstraints.gridy = 7;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 60, 3, 3);
    fieldsPanel.add(idField, gridBagConstraints);

    passwdLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    passwdLabel.setText("Password:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.gridwidth = 6;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 10, 3, 0);
    fieldsPanel.add(passwdLabel, gridBagConstraints);

    passwdField.setColumns(10);
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 6;
    gridBagConstraints.gridy = 8;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(3, 60, 3, 3);
    fieldsPanel.add(passwdField, gridBagConstraints);

    urlFormat.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
    urlFormat.setText("isabel://ip_address/session_name");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 5;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 3, 0, 0);
    fieldsPanel.add(urlFormat, gridBagConstraints);

    urlExample.setFont(new java.awt.Font("MS Sans Serif", 2, 11));
    urlExample.setText("isabel://flow.dit.upm.es/EURO6IX");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 5;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 3, 0, 0);
    fieldsPanel.add(urlExample, gridBagConstraints);

    urlFormatLabel.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
    urlFormatLabel.setText("URL Format:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.gridwidth = 5;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 60, 0, 0);
    fieldsPanel.add(urlFormatLabel, gridBagConstraints);

    urlExampleLabel.setFont(new java.awt.Font("MS Sans Serif", 3, 11));
    urlExampleLabel.setText("Example:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.gridwidth = 5;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(0, 60, 0, 0);
    fieldsPanel.add(urlExampleLabel, gridBagConstraints);

    syncroSlidesBox.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    syncroSlidesBox.setText("Download Slides for this session");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.gridwidth = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(6, 0, 0, 0);
    fieldsPanel.add(syncroSlidesBox, gridBagConstraints);

    idModified.setFont(new java.awt.Font("MS Sans Serif", 1, 11));
    idModified.setText("Let me change my Nickname");
    idModified.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        idModifiedActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 9;
    gridBagConstraints.gridwidth = 7;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    fieldsPanel.add(idModified, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.insets = new java.awt.Insets(7, 0, 8, 0);
    rightPanel.add(fieldsPanel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(0, 3, 5, 5);
    globalPanel.add(rightPanel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    add(globalPanel, gridBagConstraints);

  }//GEN-END:initComponents

  private void idModifiedActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_idModifiedActionPerformed
    if (idModified.isSelected()) {
      // Solo tocamos si antes NO era editable
      if (!idField.isEditable()) {
        // Guardamos el nickname antiguo
        oldID = getIdField();
        // Hacemos editable el campo ID.
        idField.setEditable(true);
        //oldPasswd = getPasswdField();
        //setPasswdField("");
        //passwdField.setEditable(true);
      }
    } else {
      // Solo tocamos si oldID es distinto de ""
      if (!oldID.equals("")) {
        // Si se ha deseleccionado, volvemos a poner el antiguo nickname y hacemos el campo no editable.
        setIdField(oldID);
        idField.setEditable(false);
        //setPasswdField(oldPasswd);
        //passwdField.setEditable(false);
      }
    }
  }//GEN-LAST:event_idModifiedActionPerformed

  private void urlFieldEditorKeyReleased(java.awt.event.KeyEvent evt) {
    refreshFields();
  }

  
  private void urlFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_urlFieldActionPerformed
    refreshFields();
  }//GEN-LAST:event_urlFieldActionPerformed
  
  
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextArea descriptionArea;
  private javax.swing.JPanel fieldsPanel;
  private javax.swing.JPanel globalPanel;
  private javax.swing.JTextField idField;
  private javax.swing.JLabel idLabel;
  private javax.swing.JCheckBox idModified;
  private javax.swing.JPanel leftPanel;
  private javax.swing.JPasswordField passwdField;
  private javax.swing.JLabel passwdLabel;
  private javax.swing.JPanel rightPanel;
  private javax.swing.JScrollPane scrollPanel;
  private javax.swing.JLabel sessionInfo;
  private javax.swing.JLabel siteInfo;
  private javax.swing.JCheckBox syncroSlidesBox;
  private javax.swing.JLabel titleLabel;
  private javax.swing.JPanel titlePanel;
  private javax.swing.JLabel urlExample;
  private javax.swing.JLabel urlExampleLabel;
  private javax.swing.JComboBox urlField;
  private javax.swing.JLabel urlFormat;
  private javax.swing.JLabel urlFormatLabel;
  private javax.swing.JLabel urlLabel;
  // End of variables declaration//GEN-END:variables
  
}
