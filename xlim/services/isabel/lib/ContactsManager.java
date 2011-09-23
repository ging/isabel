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
 * ContactsManager.java
 *
 * Created on 13 de julio de 2004, 13:32
 */

package services.isabel.lib;

import java.awt.*;
import java.io.*;
import java.util.*;

import javax.swing.*;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

/**
 *
 * @author  lailoken
 */
public class ContactsManager extends javax.swing.JFrame {
  
  
  // DEFINE EL NOMBRE DEL FICHERO XERL PARA ESTE SERVICIO
  //private static final String XERL_NAME = Servutilities.CONTACTS_MGR_XERL;

  // Variable que contiene el directorio donde esta la agenda
  private static final String AGENDA_ROOT_DIR = System.getProperty("agenda.root.dir");
  
  // Variable que contiene el directorio donde esta la agenda
  private static final String CONTACTS_ROOT_DIR = System.getProperty("contacts.root.dir");
  
  
  public void start() {
    show();    
  }

  public String getSelectedContact() {
    Object[] selectedContacts = contactsList.getSelectedValues();
    if (selectedContacts.length == 1) return selectedContacts[0].toString();
    else return "";
  }
  
  
  public JPanel getContactPanel() {
    return generalPanel;
  }
  
  /** Creates new form ContactsManager */
  public ContactsManager() throws IOException{
	 
    initComponents();
    if (AGENDA_ROOT_DIR != null) agenda = new Agenda(AGENDA_ROOT_DIR);
    else agenda = new Agenda();
    if (CONTACTS_ROOT_DIR != null) contacts = new Contacts(CONTACTS_ROOT_DIR);
    else contacts = new Contacts();
    initSessionCombo();
    initContactsList();
    setSiteList(sessionCombo.getSelectedItem().toString());
  }
  
  
  private void initContactsList() {
    LinkedList cList = contacts.getContacts();
    for (int i=0; i<cList.size();i++) {
      contactsListModel.addElement(cList.get(i));
    }
    contactsList.setModel(contactsListModel);
  }
  
  
  private void setSiteList(String session) {
    // Borramos todos los elementos de la lista:
    siteListModel.clear();
    // SAcamos la lista de sitios del XEDL de la session.
    try {
      xedl = new XEDL ( agenda.getFullXedl(session).getAbsolutePath());
      LinkedList cList = new SiteList(xedl).getSiteIds();
      for (int i=0; i<cList.size();i++) {
        siteListModel.addElement(cList.get(i));
      }
      siteList.setModel(siteListModel);
    } catch (IOException ioe) {
      // No hacemos nada...
    } catch (XEDLException xedle) {
      
    }
  }
  
/*  private void initContactsList() {
    final LinkedList cList = contacts.getContacts();
    contactsList.setModel(new AbstractListModel() {
        LinkedList list = cList;
        public int getSize() { return list.size(); }
        public Object getElementAt(int i) { return list.get(i); }
    });
  }
  
  private void setSiteList(String session) {
    // SAcamos la lista de sitios del XEDL de la session.
    try {
      xedl = new XEDL (agenda.getFullXedl(session).getAbsolutePath());
      final LinkedList cList = xedl.createSiteList().getSiteIds();
      siteList.setModel(new AbstractListModel() {
          LinkedList list = cList;
          public int getSize() { return list.size(); }
          public Object getElementAt(int i) { return list.get(i); }
      });
    } catch (IOException ioe) {
      // No hacemos nada...
    } catch (XEDLException xedle) {
      
    }
  }  
*/  

  public boolean addContact(String newId,NewContactPanel menu) {
    // A�adimos el nuevo contacto.
    contacts.createContact(newId,menu.getFullName(),menu.getIps());
    // Actualizamos el actual contact
    //Acualizamos la lista....
    // Borramos el contacto de la lista (en el caso de que existiera)
    contactsListModel.removeElement(newId);
    // Y lo volvemos a meter
    contactsListModel.addElement(newId);
    updateSelectedContactInfo();
    return true;
  }

  /**
   * Saca ventana de nuevo contacto para a�adir/editar un contacto
   * @param typedId valor que se presenta en el campo id al abrir la ventana de nuevo contacto
   * @param typedFullName valor que se presenta en el campo fullname al abrir la ventana de nuevo contacto
   * @param typedIp valor que se presenta en el campo ip al abrir la ventana de nuevo contacto
   **/
  public void addContact(String typedId,String typedFullName, String typedIp, boolean editMode) {
    LinkedList contactAddresses = new LinkedList();
    contactAddresses.add(typedIp);
    addContact (typedId, typedFullName, contactAddresses,editMode);
  }
    
  /**
   * Saca ventana de nuevo contacto para a�adir/editar un contacto
   * @param typedId valor que se presenta en el campo id al abrir la ventana de nuevo contacto
   * @param typedFullName valor que se presenta en el campo fullname al abrir la ventana de nuevo contacto
   * @param contactAddresses ips que tiene el sitio que se quiere a�adir. 
   **/
  public void addContact(String typedId,String typedFullName, LinkedList contactAddresses, boolean editMode) {
    boolean saveOK = false;
    String typedIp = "";
    // El bucle se pone para que si el usuario pone un ID repetido y dice que no quiere sobreescribir, le salga de nuevo la pantalla de nuevo contacto
    while (!saveOK) { 
      NewContactPanel menu = new NewContactPanel(typedId, typedFullName, contactAddresses);
      Object[] message = {menu};

      JFrame parentFrame = new JFrame();
      String title;
      if (!editMode) title = "New contact...";
      else title = "Edit contact...";
      int resultPanel = JOptionPane.showConfirmDialog(parentFrame, message, title,JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);
      if (resultPanel == JOptionPane.OK_OPTION) {
        // metemos la ultima ip escrita que puede que no se apretara el boton de add ip:
        if (!menu.existIp(menu.getIP())) menu.addIp(menu.getIP());
        // Comprobamos que el ID no esta repetido..
        String newId = menu.getID();
        if (!newId.equals(typedId)) {
          int result = 0;
          if (contacts.exists(newId)) {
            // Mensaje informando que existe ese contacto
            Toolkit.getDefaultToolkit().beep();
            result = JOptionPane.showConfirmDialog(parentFrame, "A contact with name " + newId +" already exists. Overwrite it?", "Contacts Manager",JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
            typedId = menu.getID();
            typedFullName = menu.getFullName();
            typedIp = menu.getIP();
          } else {
            saveOK = addContact(newId,menu);          
            // Borramos el contacto con el antiguo nombre
            contacts.deleteContact(typedId);
            // y lo borramos de la lista
            contactsListModel.removeElement(typedId);
          }
          if (result != JOptionPane.NO_OPTION) {
            saveOK = addContact(newId,menu);          
          }
        } else {
          saveOK = addContact(newId,menu);          
        }
      } else {
        // Ha cancelado la opcion de grabar, asi que salimos sin hacer nada
        saveOK = true;
      }
      parentFrame.dispose();        
    }
  }
  
  
  public void updateSelectedSiteInfo(){
    // Si solo hay un sitio conectado, ponemos la informacion de ese sitio en el siteInfoPanel
    Object[] selectedSites = siteList.getSelectedValues();
    if (selectedSites.length == 1) {
      // Solo hay un sitio seleccionado, actualizamos los campos de siteInfoPanel 
      try{
        SITE site = new SiteList(xedl).getSite(selectedSites[0].toString());
        addressesCombo.removeAllItems();
        selectedSiteField.setText(site.getID());
        fullNameField.setText(site.getPUBLICNAME());
        LinkedList addresses = site.getSiteAddress();
        for (int i=0;i<addresses.size();i++) addressesCombo.addItem(addresses.get(i).toString());
      } catch (Exception e) {
        selectedSiteField.setText("");
        fullNameField.setText("");
        addressesCombo.removeAllItems();
      }
      try {
        selectedSiteField.setToolTipText(selectedSiteField.getText());
        fullNameField.setToolTipText(fullNameField.getText());
        addressesCombo.setToolTipText(addressesCombo.getSelectedItem().toString());
      } catch (Exception e) {
        System.out.println("E: " + e.getMessage());
      }
    } else {
      // ponemos los campos de siteInfoPanel en blanco
      selectedSiteField.setText("");
      fullNameField.setText("");
      addressesCombo.removeAllItems();
    }
  }
  
  public void updateSelectedContactInfo(){
    // Si solo hay un sitio conectado, ponemos la informacion de ese sitio en el siteInfoPanel
    Object[] selectedContacts = contactsList.getSelectedValues();
    if (selectedContacts.length == 1) {
      // Solo hay un sitio seleccionado, actualizamos los campos de siteInfoPanel 
      try{
        XEDL xedl = new XEDL( contacts.getContact(selectedContacts[0].toString()).getAbsolutePath());
        SiteList lista = new SiteList(xedl);
        SITE site = lista.getSite(selectedContacts[0].toString());
        addressesContactCombo.removeAllItems();
        selectedContactField.setText(site.getID());
        fullNameContactField.setText(site.getPUBLICNAME());
        LinkedList contactAddresses = site.getSiteAddress();
        for (int i=0;i<contactAddresses.size();i++) addressesContactCombo.addItem(contactAddresses.get(i).toString());
      } catch (Exception e) {
        selectedContactField.setText("");
        fullNameContactField.setText("");
        addressesContactCombo.removeAllItems();
      }
      try {
        selectedContactField.setToolTipText(selectedContactField.getText());
        fullNameContactField.setToolTipText(fullNameContactField.getText());
        addressesContactCombo.setToolTipText(addressesContactCombo.getSelectedItem().toString());
      } catch (Exception e) {
        System.out.println("E: " + e.getMessage());
      }
    } else {
      // ponemos los campos de siteInfoPanel en blanco
      selectedContactField.setText("");
      fullNameContactField.setText("");
      addressesContactCombo.removeAllItems();
    }
  }
  
  private void initSessionCombo() {
    // Metemos cada nombre de sesion de la agenda en el combo
    String[] localSessions = agenda.getLocalSessions();
    for (int i=0; i<localSessions.length; i++) sessionCombo.addItem(localSessions[i]);
    String[] remoteSessions = agenda.getRemoteSessions();
    for (int i=0; i<remoteSessions.length; i++) sessionCombo.addItem(remoteSessions[i]);
  }
  
  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  private void initComponents() {//GEN-BEGIN:initComponents
    java.awt.GridBagConstraints gridBagConstraints;

    generalPanel = new javax.swing.JPanel();
    titlePanel = new javax.swing.JPanel();
    titleLabel = new javax.swing.JLabel();
    mainPanel = new javax.swing.JPanel();
    leftPanel = new javax.swing.JPanel();
    sessionLabel = new javax.swing.JLabel();
    sessionCombo = new javax.swing.JComboBox();
    sitesLabe = new javax.swing.JLabel();
    siteScrollPanel = new javax.swing.JScrollPane();
    siteList = new javax.swing.JList();
    siteInfoPanel = new javax.swing.JPanel();
    selectedSiteLabel = new javax.swing.JLabel();
    selectedSiteField = new javax.swing.JTextField();
    fullNameLabel = new javax.swing.JLabel();
    fullNameField = new javax.swing.JTextField();
    addressesLabel = new javax.swing.JLabel();
    addressesCombo = new javax.swing.JComboBox();
    centralPanel = new javax.swing.JPanel();
    addButton = new javax.swing.JButton();
    addAllButton = new javax.swing.JButton();
    deleteButton = new javax.swing.JButton();
    newContactButton = new javax.swing.JButton();
    editButton = new javax.swing.JButton();
    rightPanel = new javax.swing.JPanel();
    jLabel1 = new javax.swing.JLabel();
    contactsScrollPanel = new javax.swing.JScrollPane();
    contactsList = new javax.swing.JList();
    contactInfoPanel = new javax.swing.JPanel();
    selectedContactLabel = new javax.swing.JLabel();
    selectedContactField = new javax.swing.JTextField();
    fullNameContactLabel = new javax.swing.JLabel();
    fullNameContactField = new javax.swing.JTextField();
    addressesContactLabel = new javax.swing.JLabel();
    addressesContactCombo = new javax.swing.JComboBox();
    quitPanel = new javax.swing.JPanel();
    quitButton = new javax.swing.JButton();

    getContentPane().setLayout(new java.awt.GridBagLayout());

    setTitle("Contacts Manager");
    addWindowListener(new java.awt.event.WindowAdapter() {
      public void windowClosing(java.awt.event.WindowEvent evt) {
        exitForm(evt);
      }
    });

    generalPanel.setLayout(new java.awt.GridBagLayout());

    titlePanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(5, 5, 5, 5)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    titlePanel.setPreferredSize(new java.awt.Dimension(214, 68));
    titleLabel.setFont(new java.awt.Font("Serif", 3, 24));
    titleLabel.setText("Contacts Agenda");
    titleLabel.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
    titlePanel.add(titleLabel);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    generalPanel.add(titlePanel, gridBagConstraints);

    mainPanel.setLayout(new java.awt.GridBagLayout());

    mainPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(5, 5, 5, 5)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)))))));
    leftPanel.setLayout(new java.awt.GridBagLayout());

    leftPanel.setMinimumSize(new java.awt.Dimension(260, 180));
    leftPanel.setPreferredSize(new java.awt.Dimension(261, 359));
    sessionLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    sessionLabel.setText("Selected session:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = 2;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    leftPanel.add(sessionLabel, gridBagConstraints);

    sessionCombo.setMinimumSize(new java.awt.Dimension(124, 19));
    sessionCombo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        sessionComboActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    leftPanel.add(sessionCombo, gridBagConstraints);

    sitesLabe.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    sitesLabe.setText("Terminals in selected Session:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    gridBagConstraints.insets = new java.awt.Insets(5, 15, 5, 5);
    leftPanel.add(sitesLabe, gridBagConstraints);

    siteScrollPanel.setPreferredSize(new java.awt.Dimension(180, 200));
    siteList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
      public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
        siteListValueChanged(evt);
      }
    });

    siteScrollPanel.setViewportView(siteList);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(5, 15, 5, 15);
    leftPanel.add(siteScrollPanel, gridBagConstraints);

    siteInfoPanel.setLayout(new java.awt.GridBagLayout());

    siteInfoPanel.setBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED));
    selectedSiteLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    selectedSiteLabel.setText("Selected Terminal:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    siteInfoPanel.add(selectedSiteLabel, gridBagConstraints);

    selectedSiteField.setEditable(false);
    selectedSiteField.setPreferredSize(new java.awt.Dimension(91, 20));
    siteInfoPanel.add(selectedSiteField, new java.awt.GridBagConstraints());

    fullNameLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    fullNameLabel.setText("Full Name:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    siteInfoPanel.add(fullNameLabel, gridBagConstraints);

    fullNameField.setEditable(false);
    fullNameField.setPreferredSize(new java.awt.Dimension(91, 20));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    siteInfoPanel.add(fullNameField, gridBagConstraints);

    addressesLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    addressesLabel.setText("IP Addresses:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    siteInfoPanel.add(addressesLabel, gridBagConstraints);

    addressesCombo.setBackground(new java.awt.Color(217, 217, 233));
    addressesCombo.setPreferredSize(new java.awt.Dimension(91, 19));
    addressesCombo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        addressesComboActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    siteInfoPanel.add(addressesCombo, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
    leftPanel.add(siteInfoPanel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    mainPanel.add(leftPanel, gridBagConstraints);

    centralPanel.setLayout(new java.awt.GridBagLayout());

    centralPanel.setPreferredSize(new java.awt.Dimension(170, 199));
    addButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    addButton.setIcon(new ImageIcon(ContactsManager.class.getClass().getResource("/navigation/Forward16.gif")));
    addButton.setText("Add to Agenda");
    addButton.setToolTipText("Add the selected terminal to the Agenda.");
    addButton.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
    addButton.setPreferredSize(new java.awt.Dimension(125, 25));
    addButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        addButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 5);
    centralPanel.add(addButton, gridBagConstraints);

    addAllButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    addAllButton.setIcon(new ImageIcon(ContactsManager.class.getClass().getResource("/navigation/Forward16.gif")));
    addAllButton.setText("Add All to Agenda");
    addAllButton.setToolTipText("Add all the terminals of this session to Agenda.");
    addAllButton.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
    addAllButton.setPreferredSize(new java.awt.Dimension(160, 25));
    addAllButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        addAllButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 37, 5);
    centralPanel.add(addAllButton, gridBagConstraints);

    deleteButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    deleteButton.setText("Delete Contact");
    deleteButton.setToolTipText("Delete the selected contact from the Agenda.");
    deleteButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        deleteButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 4;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 22, 5);
    centralPanel.add(deleteButton, gridBagConstraints);

    newContactButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    newContactButton.setText("New Contact...");
    newContactButton.setToolTipText("Create a new contact in the Agenda.");
    newContactButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        newContactButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 5);
    centralPanel.add(newContactButton, gridBagConstraints);

    editButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    editButton.setText("Edit Contact");
    editButton.setToolTipText("Edit the selected contact of the Agenda.");
    editButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        editButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 5);
    centralPanel.add(editButton, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.insets = new java.awt.Insets(5, 10, 5, 10);
    mainPanel.add(centralPanel, gridBagConstraints);

    rightPanel.setLayout(new java.awt.GridBagLayout());

    rightPanel.setPreferredSize(new java.awt.Dimension(270, 387));
    jLabel1.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    jLabel1.setText("Contacts in Agenda:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.insets = new java.awt.Insets(50, 5, 10, 0);
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    rightPanel.add(jLabel1, gridBagConstraints);

    contactsScrollPanel.setPreferredSize(new java.awt.Dimension(180, 200));
    contactsList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
      public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
        contactsListValueChanged(evt);
      }
    });

    contactsScrollPanel.setViewportView(contactsList);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(0, 5, 5, 5);
    rightPanel.add(contactsScrollPanel, gridBagConstraints);

    contactInfoPanel.setLayout(new java.awt.GridBagLayout());

    contactInfoPanel.setBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED));
    selectedContactLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    selectedContactLabel.setText("Contact:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    contactInfoPanel.add(selectedContactLabel, gridBagConstraints);

    selectedContactField.setEditable(false);
    selectedContactField.setPreferredSize(new java.awt.Dimension(91, 20));
    contactInfoPanel.add(selectedContactField, new java.awt.GridBagConstraints());

    fullNameContactLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    fullNameContactLabel.setText("Full Name:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    contactInfoPanel.add(fullNameContactLabel, gridBagConstraints);

    fullNameContactField.setEditable(false);
    fullNameContactField.setPreferredSize(new java.awt.Dimension(91, 20));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    contactInfoPanel.add(fullNameContactField, gridBagConstraints);

    addressesContactLabel.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    addressesContactLabel.setText("IP Addresses:");
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 5, 5, 5);
    contactInfoPanel.add(addressesContactLabel, gridBagConstraints);

    addressesContactCombo.setBackground(new java.awt.Color(217, 217, 233));
    addressesContactCombo.setPreferredSize(new java.awt.Dimension(91, 19));
    addressesContactCombo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        addressesContactComboActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 1;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
    contactInfoPanel.add(addressesContactCombo, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 3;
    gridBagConstraints.gridwidth = 3;
    gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
    rightPanel.add(contactInfoPanel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 2;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    mainPanel.add(rightPanel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    generalPanel.add(mainPanel, gridBagConstraints);

    quitPanel.setLayout(new java.awt.GridBagLayout());

    quitPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(5, 5, 5, 5)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)))))));
    quitButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    quitButton.setText("Quit");
    quitButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        quitButtonActionPerformed(evt);
      }
    });

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    gridBagConstraints.insets = new java.awt.Insets(5, 450, 5, 0);
    quitPanel.add(quitButton, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 2;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
    generalPanel.add(quitPanel, gridBagConstraints);

    getContentPane().add(generalPanel, new java.awt.GridBagConstraints());

    pack();
  }//GEN-END:initComponents

  private void addressesComboActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addressesComboActionPerformed
    try {
      addressesCombo.setToolTipText(addressesCombo.getSelectedItem().toString());
    } catch (Exception e) {
    }
  }//GEN-LAST:event_addressesComboActionPerformed

  private void addressesContactComboActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addressesContactComboActionPerformed
    try {
      addressesContactCombo.setToolTipText(addressesContactCombo.getSelectedItem().toString());
    } catch (Exception e) {
    }
  }//GEN-LAST:event_addressesContactComboActionPerformed

  private void editButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_editButtonActionPerformed
    // Llamamos a formulario para que nos den nombre, full name y address
    // Lanzamos una ventana donde le pedimos datos para este nuevo contacto
    boolean saveOK = false;
    Object[] selectedContacts = contactsList.getSelectedValues();
    if (selectedContacts.length == 1) {
      try{
        XEDL xedl = new XEDL( contacts.getContact(selectedContacts[0].toString()).getAbsolutePath());
        SiteList lista = new SiteList(xedl);
        SITE site = lista.getSite(selectedContacts[0].toString());
        LinkedList contactAddresses = site.getSiteAddress();
        String typedId = site.getID();
        String typedFullName = site.getPUBLICNAME();
        String typedIp = "";
        addContact(typedId,typedFullName, contactAddresses,true);
      } catch (Exception e) {
      }    
    }
  }//GEN-LAST:event_editButtonActionPerformed

  private void newContactButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newContactButtonActionPerformed
    // Llamamos a formulario para que nos den nombre, full name y address
    // Lanzamos una ventana donde le pedimos datos para este nuevo contacto
    boolean saveOK = false;
    String typedId = "";
    String typedFullName = "";
    String typedIp = "";
    addContact(typedId,typedFullName, typedIp,false);
  }//GEN-LAST:event_newContactButtonActionPerformed

  private void contactsListValueChanged(javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_contactsListValueChanged
    updateSelectedContactInfo();
  }//GEN-LAST:event_contactsListValueChanged

  private void siteListValueChanged(javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_siteListValueChanged
    updateSelectedSiteInfo();
  }//GEN-LAST:event_siteListValueChanged

  private void addAllButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addAllButtonActionPerformed
    addAllContacts();
  }//GEN-LAST:event_addAllButtonActionPerformed

  private void quitButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_quitButtonActionPerformed
    quit();
  }//GEN-LAST:event_quitButtonActionPerformed

  private void deleteButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deleteButtonActionPerformed
    deleteContacts();
  }//GEN-LAST:event_deleteButtonActionPerformed

  private void addButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addButtonActionPerformed
    addContacts();
  }//GEN-LAST:event_addButtonActionPerformed

  private void sessionComboActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sessionComboActionPerformed
    sessionSelected();
  }//GEN-LAST:event_sessionComboActionPerformed
  
  /** Exit the Application */
  private void exitForm(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_exitForm
    quit();
  }//GEN-LAST:event_exitForm
  
  
  private void quit() {
    //System.exit(0);
    dispose();
  }
  
  private void contactProperties() {
    
  }
  
  private void deleteContacts() {
    // Cogemos la lista de los contactos seleccionados en la lista
    Object[] selectedContacts = contactsList.getSelectedValues();
    // Lo eliminamos...
    for (int i=0; i<selectedContacts.length;i++) {
      //System.out.println("Valor seleccionado: " + selectedContacts[i].toString());
      contacts.deleteContact(selectedContacts[i].toString());
      //Acualizamos la lista....
      contactsListModel.removeElement(selectedContacts[i]);
    }
  }
  
  private void addAllContacts() {
    siteList.setSelectionInterval(0,siteListModel.getSize()-1);
    addContacts();
  }
  
  
  private void addContacts() {
    // Cogemos la lista de los contactos seleccionados en la lista
    Object[] selectedSites = siteList.getSelectedValues();
    // Los a�adimos...
    for (int i=0; i<selectedSites.length;i++) {
      //System.out.println("Valor seleccionado: " + selectedSites[i].toString());
      // Miramos si ya existe ese contacto en los contactos
      int result = 0;
      if (contacts.exists(selectedSites[i].toString())) {
        JFrame parentFrame = new JFrame();
        // Sonidito al canto...
        Toolkit.getDefaultToolkit().beep();
        result = JOptionPane.showConfirmDialog(parentFrame, "A contact with name " + selectedSites[i].toString() +" already exists. Overwrite it?", "Contacts Manager",JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
        parentFrame.dispose();
      }
      if (result != javax.swing.JOptionPane.NO_OPTION) {   
    	  SiteList lista = new SiteList(xedl);
        contacts.createContact(selectedSites[i].toString(),lista.getSite(selectedSites[i].toString()));
        //Acualizamos la lista....
        // Borramos el contacto de la lista (en el caso de que existiera)
        contactsListModel.removeElement(selectedSites[i]);
        // Y lo volvemos a meter
        contactsListModel.addElement(selectedSites[i]);
      }
    }
  }
  
  private void sessionSelected() {
    // Cuando se selecciona una nueva session, hay que cargar los sitios en la siteList
    setSiteList(sessionCombo.getSelectedItem().toString());
  }
  
  /**
   * @param args the command line arguments
   */
  public static void main(String args[]) {
    try {
      new ContactsManager().show();
    } catch (IOException ioe) {
      
    }
  }
  
  
  private XEDL xedl;
  private Contacts contacts;
  private Agenda agenda;  
  private DefaultListModel contactsListModel= new DefaultListModel();;
  private DefaultListModel siteListModel= new DefaultListModel();;
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton addAllButton;
  private javax.swing.JButton addButton;
  private javax.swing.JComboBox addressesCombo;
  private javax.swing.JComboBox addressesContactCombo;
  private javax.swing.JLabel addressesContactLabel;
  private javax.swing.JLabel addressesLabel;
  private javax.swing.JPanel centralPanel;
  private javax.swing.JPanel contactInfoPanel;
  private javax.swing.JList contactsList;
  private javax.swing.JScrollPane contactsScrollPanel;
  private javax.swing.JButton deleteButton;
  private javax.swing.JButton editButton;
  private javax.swing.JTextField fullNameContactField;
  private javax.swing.JLabel fullNameContactLabel;
  private javax.swing.JTextField fullNameField;
  private javax.swing.JLabel fullNameLabel;
  private javax.swing.JPanel generalPanel;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JPanel leftPanel;
  private javax.swing.JPanel mainPanel;
  private javax.swing.JButton newContactButton;
  private javax.swing.JButton quitButton;
  private javax.swing.JPanel quitPanel;
  private javax.swing.JPanel rightPanel;
  private javax.swing.JTextField selectedContactField;
  private javax.swing.JLabel selectedContactLabel;
  private javax.swing.JTextField selectedSiteField;
  private javax.swing.JLabel selectedSiteLabel;
  private javax.swing.JComboBox sessionCombo;
  private javax.swing.JLabel sessionLabel;
  private javax.swing.JPanel siteInfoPanel;
  private javax.swing.JList siteList;
  private javax.swing.JScrollPane siteScrollPanel;
  private javax.swing.JLabel sitesLabe;
  private javax.swing.JLabel titleLabel;
  private javax.swing.JPanel titlePanel;
  // End of variables declaration//GEN-END:variables
  
}
