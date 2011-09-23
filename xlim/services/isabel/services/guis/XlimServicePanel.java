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
 * MenuTemplate.java
 *
 * Created on 19 de agosto de 2004, 18:04
 */

package services.isabel.services.guis;

import services.isabel.lib.*;
import java.awt.*;
import javax.swing.*;

/**
 *
 * @author  lailoken
 */
public class XlimServicePanel extends javax.swing.JPanel implements ServiceDefinitions {
 
  private static final int SSS_PANEL = 0;
  private static final int CONNECT_TO_PANEL = 1;
  private static final int LOAD_XEDL_PANEL = 2;
  private static final int IRIS_PANEL = 3;
  private static final int CONTACT_PANEL = 4;
  
  
  private JPanel primaryPanel = null;
  private JPanel secondaryPanel = null;

  // ---- CONSTRUCTORES ----//
  /** Creates a new XlimServicePanel
   * @param ServiceName determines how the panel will be. Defined values:
   *    - SSS_PANEL: Start Session Server Panel
   *    - CONNECT_TO_PANEL: Connect to Session Panel
   *    - IRIS_PANEL: Subscribe new Terminal Panel
   *    - LOAD_XEDL_PANEL: Connect Remote Terminals Panel
   * @param the_url url that will appear in the connect to panel by default
   */
  public XlimServicePanel(int serviceName, String the_url) {
    String iconName;
    String instructions;
    switch (serviceName) {
      case SSS_SERVICE:
        secondaryPanel = new SiteDataPanel(SSS_SERVICE);
        //ENRIQUE cambio el tipo de panel de session por el nuevo que me he creado
        primaryPanel = new SessionDataPanel_start_server();
        //primaryPanel = new SessionDataPanel(SSS_SERVICE, secondaryPanel);
        
        initPanel(new ImageIcon(XlimServicePanel.class.getClass().getResource(SSS_ICON_NAME)),SSS_INSTRUCTIONS,primaryPanel,secondaryPanel);
        break;
      case CONNECT_TO_SERVICE:
        secondaryPanel = new SiteDataPanel(CONNECT_TO_SERVICE);
        primaryPanel = new SessionDataPanel_connect_to(CONNECT_TO_SERVICE,secondaryPanel, the_url);
        initPanel(new ImageIcon(XlimServicePanel.class.getClass().getResource(CONNECT_TO_ICON_NAME)),CONNECT_TO_INSTRUCTIONS,primaryPanel,secondaryPanel);
        break;
      case EDIT_SESSION_SERVICE:
        primaryPanel = new EditSessionPanel();
        initPanel(new ImageIcon(XlimServicePanel.class.getClass().getResource(EDIT_SESSION_ICON_NAME)),EDIT_SESSION_INSTRUCTIONS,primaryPanel);
        break;
      default:
        secondaryPanel = new SiteDataPanel(DEFAULT_SERVICE);
        primaryPanel = new SessionDataPanel_connect_to(DEFAULT_SERVICE,secondaryPanel);
        initPanel(new ImageIcon(XlimServicePanel.class.getClass().getResource(DEFAULT_ICON_NAME)),DEFAULT_INSTRUCTIONS,primaryPanel,secondaryPanel);
        break;
    }
  }
  
  /** Creates a new XlimServicePanel
   * @param ServiceName determines how the panel will be. Defined values:
   *    - SSS_PANEL: Start Session Server Panel
   *    - CONNECT_TO_PANEL: Connect to Session Panel
   *    - IRIS_PANEL: Subscribe new Terminal Panel
   *    - LOAD_XEDL_PANEL: Connect Remote Terminals Panel
   */
  public XlimServicePanel(int serviceName) {
	  this(serviceName, null);
  }

  /** Creates a new XlimServicePanel
   * @param icon the image displayed in the top left of the panel
   * @param instructions the instructions for the user of this service
   * @param panel the panel to be shown in the right of the Panel.
   */
  public XlimServicePanel (Icon icon, String instructions, JPanel panel) {
    initPanel(icon, instructions, panel);
  }
  
  /** Creates a new XlimServicePanel
   * @param icon the image displayed in the top left of the panel
   * @param instructions the instructions for the user of this service
   * @param rightTopPanel the panel to be shown in the right-top of the Panel.
   * @param rightBottomPanel the panel to be shown in the right-bottom of the Panel.
   */
  public XlimServicePanel (Icon icon, String instructions, JPanel rightTopPanel, JPanel rightBottomPanel) {
    initPanel(icon, instructions, rightTopPanel, rightBottomPanel);
  }
  
  /** Creates new form MenuTemplate */
  public XlimServicePanel() {
    this(DEFAULT_SERVICE);
  }

  /**
   * Returns the secondary panel (the right bottom panel of the GUI)
   */
  public JPanel getPrimaryPanel() {
    return primaryPanel;
  }
  
  /** 
   * Returns the secondary panel (the right bottom panel of the GUI)
   */
  public JPanel getSecondaryPanel() {
    return secondaryPanel;
  }
  
  /** Inicializa el panel para presentarlo en pantalla (le mete el icono, las instrucciones, los panelss...
   */
  public void initPanel(Icon icon, String instructions, JPanel panel) {
    // inicializamos los componentes por defecto
    initComponents();
    
    // Metemos el icono en el panel del icono
    setPanelIcon(icon);
    
    // Metemos las instrucciones en el panel de instrucciones
   setInstructions(instructions);
    
   // Ponemos un borde al rightPanel
   rightPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)))))));

   // Metemos el panel dentro del panel derecho
   rightPanel.add(panel); 
   
   // Igualamos las alturas de los paneles izquierdo y derecho:
    equalizePanels();
    adjustInstructions();

  }
  
  /** Inicializa el panel para presentarlo en pantalla (le mete el icono, las instrucciones, los panelss...
   */
  public void initPanel (Icon icon, String instructions, JPanel rightTopPanel, JPanel rightBottomPanel) {
    // inicializamos los componentes por defecto
    initComponents();
    
    // Metemos el icono en el panel del icono
    setPanelIcon(icon);
    
    // Metemos las instrucciones en el panel de instrucciones
   setInstructions(instructions);
    
    java.awt.GridBagConstraints gridBagConstraints;
    
    // Panel superior derecho
    rightTopBorderPanel = new javax.swing.JPanel();
    rightTopBorderPanel.setLayout(new java.awt.BorderLayout());
    //rightTopBorderPanel.setLayout(new java.awt.GridBagLayout());

    rightTopBorderPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    //rightTopBorderPanel.setMinimumSize(new java.awt.Dimension(420, 180));
    //rightTopBorderPanel.setPreferredSize(new java.awt.Dimension(420, 180));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    //gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    //gridBagConstraints.gridwidth = 2;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    // Metemos el panel que nos dan en este panel...
    rightTopBorderPanel.add(rightTopPanel,java.awt.BorderLayout.CENTER);
    // ... y este a su vez en el rightPanel
    rightPanel.add(rightTopBorderPanel, gridBagConstraints);

    // Panel inferior derecho
    rightBottomBorderPanel = new javax.swing.JPanel();
    //rightBottomBorderPanel.setLayout(new java.awt.GridBagLayout());
    rightBottomBorderPanel.setLayout(new java.awt.BorderLayout());

    rightBottomBorderPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    //rightBottomBorderPanel.setMinimumSize(new java.awt.Dimension(300, 180));
    //rightBottomBorderPanel.setPreferredSize(new java.awt.Dimension(300, 180));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.weightx = 10.0;
    gridBagConstraints.weighty = 10.0;
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    // Metemos el panel que nos dan en este panel...
    rightBottomBorderPanel.add(rightBottomPanel,java.awt.BorderLayout.CENTER);
    
    // ... y este a su vez en el rightPanel
    rightPanel.add(rightBottomBorderPanel, gridBagConstraints);
    
    // Ajustamos las dimensiones de los border a los paneles dados
    Dimension rtpDimension = rightTopPanel.getPreferredSize();
    double rtpHeight = rightTopPanel.getPreferredSize().getHeight();
    double rtpWidth = rightTopPanel.getPreferredSize().getWidth();
    double rtbpWidth = rightTopBorderPanel.getPreferredSize().getWidth();
    Dimension rtbpDimension= new Dimension();
    rtbpDimension.setSize(rtbpWidth, rtpHeight);
    rightTopBorderPanel.setPreferredSize(rtbpDimension);

    Dimension rbpDimension = rightBottomPanel.getPreferredSize();
    double rbpHeight = rightBottomPanel.getPreferredSize().getHeight();
    double rbpWidth = rightBottomPanel.getPreferredSize().getWidth();
    double rbbpWidth = rightBottomBorderPanel.getPreferredSize().getWidth();
    Dimension rbbpDimension = new Dimension();
    rbbpDimension.setSize(rbbpWidth, rbpHeight);
    rightBottomBorderPanel.setPreferredSize(rbbpDimension);
    // ... y las del right panel al de los borders
    double newRightWidth;
    if (rtpWidth > rbpWidth) newRightWidth = rtpWidth;
    else newRightWidth = rbpWidth;
    double newRightHeight = rtpHeight + rbpHeight;
    Dimension newDimension = new Dimension();
    newDimension.setSize(newRightWidth, newRightHeight);
    
    // Igualamos las alturas de los paneles izquierdo y derecho:
    equalizePanels();
    adjustInstructions();
    //try {writeDimensions(rightTopPanel,rightBottomPanel);}
    //catch (Exception e) {}
  }
  
  public void setInstructions(String instructions) {
    if (instructions != null) descriptionArea.setText(instructions);
  }

  public String getInstructions() {
    return descriptionArea.getText();
  }

  
  private void setPanelIcon(Icon icon){
    // En este panel hay dos iconos, uno de los cuales es fijo:
    Icon defIcon = new ImageIcon(XlimServicePanel.class.getClass().getResource("/general/agora.gif"));
    iconLabel2.setIcon(defIcon);
    double defIconHeight = iconLabel2.getMinimumSize().getHeight();
    double defIconWidth = iconLabel2.getMinimumSize().getWidth();
    if (icon != null) {
      iconLabel.setIcon(icon);
      // Ajustamos el panel del icono al tama�o del icono
      double iconHeight = iconLabel.getMinimumSize().getHeight();
      double iconWidth = iconLabel.getMinimumSize().getWidth();
      double panelHeight = picturePanel.getPreferredSize().getHeight();
      double panelWidth = picturePanel.getPreferredSize().getWidth();
      double newPanelHeight= panelHeight;
      double newPanelWidth = panelWidth;
      Dimension dimension;
      //System.out.println(iconWidth +" " +iconHeight + "; " + panelWidth + " " + panelHeight);
      if (iconHeight + defIconHeight > panelHeight) newPanelHeight = iconHeight + defIconHeight + 20; // El 20 es para salvar el borde
      if (iconWidth + defIconWidth > panelWidth) newPanelWidth = iconWidth + defIconWidth + 20; // El 20 es para salvar el borde
      if ((newPanelWidth != panelWidth) || (newPanelHeight != panelHeight)) {
        dimension = new Dimension();
        dimension.setSize(newPanelWidth, newPanelHeight);
        picturePanel.setMinimumSize(dimension);
        picturePanel.setPreferredSize(dimension);
        picturePanel.setMaximumSize(dimension);
      }
    } else {
      // Ajustamos el panel al icono por defecto.
      double panelHeight = picturePanel.getPreferredSize().getHeight();
      double panelWidth = picturePanel.getPreferredSize().getWidth();
      double newPanelHeight= panelHeight;
      double newPanelWidth = panelWidth;
      Dimension dimension;
      //System.out.println(iconWidth +" " +iconHeight + "; " + panelWidth + " " + panelHeight);
      if (defIconHeight > panelHeight) newPanelHeight = defIconHeight + 20; // El 20 es para salvar el borde
      if (defIconWidth > panelWidth) newPanelWidth = defIconWidth + 20; // El 20 es para salvar el borde
      if ((newPanelWidth != panelWidth) || (newPanelHeight != panelHeight)) {
        dimension = new Dimension();
        dimension.setSize(newPanelWidth, newPanelHeight);
        picturePanel.setMinimumSize(dimension);
        picturePanel.setPreferredSize(dimension);
        picturePanel.setMaximumSize(dimension);
      }
    }
  }

  private void writeDimensions(JPanel rightTopPanel, JPanel rightBottomPanel) {
    System.out.println("GENERAL PANELS: left: " + leftPanel.getPreferredSize().toString() + " and right: " + rightPanel.getPreferredSize().toString());
    System.out.println("LEFT PANEL: picture: " + picturePanel.getPreferredSize().toString() + " and instructions: " + instructionsPanel.getPreferredSize().toString());
    System.out.println("RIGHT PANEL (BORDER): top: " + rightTopBorderPanel.getPreferredSize().toString() + " and bottom: " + rightBottomBorderPanel.getPreferredSize().toString());
    System.out.println("RIGHT PANEL (NOT BORDER): top: " + rightTopPanel.getPreferredSize().toString() + " and bottom: " + rightBottomPanel.getPreferredSize().toString());
  }
  
  private void equalizePanels() { 
    // Igualamos las alturas de los paneles izquierdo y derecho:
    double rightHeight = rightPanel.getPreferredSize().getHeight();
    double leftHeight = leftPanel.getPreferredSize().getHeight();
    if (rightHeight != leftHeight)
      if (rightHeight > leftHeight) {
        // El panel derecho es mayor que el izq
        //System.out.println("Old dimensions: left: " + leftPanel.getPreferredSize().toString() + " and right: " + rightPanel.getPreferredSize().toString());
        //outTraceFile.println("Inflating left panel (instructions panel)");
        Dimension newDimension = new Dimension();
        newDimension.setSize(leftPanel.getPreferredSize().getWidth(), rightHeight);
        leftPanel.setPreferredSize(newDimension);
        //System.out.println("New dimensions: left: "+ leftPanel.getPreferredSize().toString() + " and right: " + rightPanel.getPreferredSize().toString());
      } else {
        // panel derecho menor que el izq.
        System.out.println("Old dimensions: left: "+ leftPanel.getPreferredSize().toString() + " and right: " + rightPanel.getPreferredSize().toString());
        //outTraceFile.println("Inflating right panel (data panel)");
        Dimension newDimension = new Dimension();
        newDimension.setSize(rightPanel.getPreferredSize().getWidth(), leftHeight);
        rightPanel.setPreferredSize(newDimension);
        //System.out.println("New dimensions: left: "+ leftPanel.getPreferredSize().toString() + " and right: " + rightPanel.getPreferredSize().toString());
      }
  }
  
  /**
   *  Ajusta el tama�o de las instrucciones al panel de las instrucciones
   */
  private void adjustInstructions() {
    // Dimension del panel izquierdo.
    Dimension leftDimension = leftPanel.getPreferredSize();
    // Dimension del panel  de icono.
    Dimension iconDimension = picturePanel.getPreferredSize();
    // Con esto configuramos las nuevas dimensiones de instructionsPanel y scrollPanel
    double newWidth = iconDimension.getWidth() ;
    double newHeight = leftDimension.getHeight() - iconDimension.getHeight();
    Dimension newInstructionsDimension = new Dimension();
    Dimension newScrollDimension = new Dimension();
    newInstructionsDimension.setSize(newWidth, newHeight);
    newScrollDimension.setSize(newWidth-30, newHeight-20);
    instructionsPanel.setPreferredSize(newInstructionsDimension);
    instructionsPanel.setMinimumSize(newInstructionsDimension);
    instructionsPanel.setMaximumSize(newInstructionsDimension);
    scrollPanel.setPreferredSize(newScrollDimension);
    scrollPanel.setMinimumSize(newScrollDimension);
    scrollPanel.setMaximumSize(newScrollDimension);  
  }
  
  
  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  private void initComponents() {//GEN-BEGIN:initComponents
    java.awt.GridBagConstraints gridBagConstraints;

    leftPanel = new javax.swing.JPanel();
    picturePanel = new javax.swing.JPanel();
    iconLabel = new javax.swing.JLabel();
    iconLabel2 = new javax.swing.JLabel();
    instructionsPanel = new javax.swing.JPanel();
    scrollPanel = new javax.swing.JScrollPane();
    descriptionArea = new javax.swing.JTextArea();
    rightPanel = new javax.swing.JPanel();

    setLayout(new java.awt.GridBagLayout());

    setMinimumSize(new java.awt.Dimension(831, 356));
    leftPanel.setLayout(new javax.swing.BoxLayout(leftPanel, javax.swing.BoxLayout.Y_AXIS));

    picturePanel.setLayout(new java.awt.GridBagLayout());

    picturePanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    picturePanel.setMaximumSize(new java.awt.Dimension(290, 200));
    picturePanel.setPreferredSize(new java.awt.Dimension(290, 160));
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 1;
    gridBagConstraints.insets = new java.awt.Insets(10, 15, 10, 15);
    picturePanel.add(iconLabel, gridBagConstraints);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.insets = new java.awt.Insets(10, 15, 10, 15);
    picturePanel.add(iconLabel2, gridBagConstraints);

    leftPanel.add(picturePanel);

    instructionsPanel.setLayout(new java.awt.GridBagLayout());

    instructionsPanel.setBorder(new javax.swing.border.CompoundBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)), new javax.swing.border.CompoundBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(javax.swing.border.EtchedBorder.RAISED), new javax.swing.border.CompoundBorder(new javax.swing.border.EtchedBorder(), new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 10, 1, 1)))))));
    instructionsPanel.setPreferredSize(new java.awt.Dimension(290, 230));
    scrollPanel.setBorder(null);
    scrollPanel.setPreferredSize(new java.awt.Dimension(260, 190));
    scrollPanel.setAutoscrolls(true);
    descriptionArea.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
    descriptionArea.setColumns(40);
    descriptionArea.setEditable(false);
    descriptionArea.setFont(new java.awt.Font("Microsoft Sans Serif", 1, 12));
    descriptionArea.setLineWrap(true);
    descriptionArea.setRows(10);
    descriptionArea.setText("   Instructions Instructions Instructions Instructions:\n\n   1.- Instructions Instructions Instructions Instructions \n\n   2.- Instructions Instructions Instructions Instructions Instructions Instructions Instructions Instructions.\n\n   3.- Instructions Instructions Instructions Instructions ");
    descriptionArea.setWrapStyleWord(true);
    scrollPanel.setViewportView(descriptionArea);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.weightx = 1.0;
    gridBagConstraints.weighty = 1.0;
    gridBagConstraints.insets = new java.awt.Insets(10, 0, 10, 0);
    instructionsPanel.add(scrollPanel, gridBagConstraints);

    leftPanel.add(instructionsPanel);

    add(leftPanel, new java.awt.GridBagConstraints());

    rightPanel.setLayout(new java.awt.GridBagLayout());

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
    add(rightPanel, gridBagConstraints);

  }//GEN-END:initComponents
  
  private javax.swing.JPanel rightBottomBorderPanel;
  private javax.swing.JPanel rightTopBorderPanel;  
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextArea descriptionArea;
  private javax.swing.JLabel iconLabel;
  private javax.swing.JLabel iconLabel2;
  private javax.swing.JPanel instructionsPanel;
  private javax.swing.JPanel leftPanel;
  private javax.swing.JPanel picturePanel;
  private javax.swing.JPanel rightPanel;
  private javax.swing.JScrollPane scrollPanel;
  // End of variables declaration//GEN-END:variables
  
  public static void main (String[] args) {
    int panel = new Integer (args[0]).intValue();
    Icon icon = new javax.swing.ImageIcon("D:\\dit\\imagenes\\img1.gif");
    String instructions = "instrucciones para pasar el rato";
    SiteDataPanel sidp = new SiteDataPanel(panel);
    SessionDataPanel_connect_to sedp = new SessionDataPanel_connect_to(panel,sidp);
    EditSessionPanel esp = new EditSessionPanel();
    //XlimServicePanel xlimServicePanel = new XlimServicePanel(icon,instructions,sedp,sidp);
    //XlimServicePanel xlimServicePanel = new XlimServicePanel(icon,instructions,sidp);
    XlimServicePanel xlimServicePanel = new XlimServicePanel(panel);
    Object[] message = {xlimServicePanel};

    JFrame parentFrame = new JFrame();
    Object[] options = {"button1", "button2", "button3"};
    int resultPanel = JOptionPane.showOptionDialog(parentFrame, message, "Prueba",JOptionPane.YES_NO_CANCEL_OPTION,JOptionPane.PLAIN_MESSAGE,null,options,options[2]);
    //int resultPanel = JOptionPane.showConfirmDialog(parentFrame, message, "PRUEBA",JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);
    parentFrame.dispose();
    //irisSessionDataPanel.saveValues();
    //sessionDataPanel.saveValues(SSS_PANEL);
  }
}
