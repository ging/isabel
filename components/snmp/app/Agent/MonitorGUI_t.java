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
//***************************************************************
//
//FILE: MonitorGUI_t.java
//
//DESCRIPTION:
// This is the main frame and thread of the GUI of the monitor
// It shows all the data from the different components and let's
// you draw graphics and access to the help in html format.
//

import java.lang.*;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.table.*;
import javax.swing.event.*;

public class MonitorGUI_t extends FrameRef implements java.lang.Runnable {

    //Attributes:
    //-----------
    DataBaseHandler_t           DataBaseHandler;
    threadsManager_t            threadsManager;
    Thread                      thisThread;

    //For the representation of the MIB data:
    //---------------------------------------
    audioTableModel_t           audioTModel;
    audioJTable_t               audioTable;

    videoTableModel_t           videoTModel;
    videoJTable_t               videoTable;

    shDisplayTableModel_t       shDisplayTModel;
    shDisplayJTable_t           shDisplayTable;

    irouterRCVTableModel_t      irtRCVTModel;
    irouterRCVJTable_t          irtRCVTable;
    JLabel                      RCVTotalValue;

    irouterSNDTableModel_t      irtSNDTModel;
    irouterSNDJTable_t          irtSNDTable;
    JLabel                      SNDTotalValue;

    String                      selectedTable;

    //Common graphics elements:
    //-------------------------
    JTabbedPane                 upperTabbedPane;
    JMenuBar                    menuBar;
    JMenu                       fileMenu;
    JMenu                       helpMenu;
    JMenu                       unitsMenu;
    JRadioButtonMenuItem        KBySgRBMI, KbiSgRBMI, BySgCRBMI, FRSgRBMI;
    JMenu                       BWSubmenu, FRSubmenu;
    ButtonGroup                 BWBGroup, FRBGroup;

    void setSelectedTable (String sTable) {
        selectedTable = sTable;
    }

    MonitorGUI_t (DataBaseHandler_t DataBaseHandler,
                  threadsManager_t threadsManager
                 ) {
        super("ISABEL Monitor and SNMP Agent");

        this.DataBaseHandler   = DataBaseHandler;
        this.threadsManager    = threadsManager;

        //Graphics Initializations:
        //-------------------------

        //Menu:
        createMenu();
        setJMenuBar(menuBar);

        //Central Panel: where all data will be displayed
        Component contents = createComponents();

        //Frame initializations:
        //setResizable(false);
        getContentPane().add(contents, BorderLayout.CENTER);

        //Finish setting up the frame, and show it.
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });

        pack();
        setSize(750,350);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setVisible(true);

        //Start of this thread:
        //---------------------
        thisThread = new Thread(this);
        thisThread.start();
    }

    //Function for the creation of all the elements in the Menu:
    //----------------------------------------------------------

    protected void createMenu() {
        menuBar= new JMenuBar();
        MenuListener myMenuListener = new MenuListener(this);

        // 1.- Create the file-menu:
        // -------------------------
        fileMenu = new JMenu ("Options");
        fileMenu.setMnemonic(KeyEvent.VK_O);

        // 1.1.- Create the items in the file-menu and add them to the menu:
        JMenuItem optionsMItem = new JMenuItem ("Port configuration...");
        optionsMItem.setMnemonic(KeyEvent.VK_P);
        optionsMItem.addActionListener(myMenuListener);
        JMenuItem noVisMItem= new JMenuItem("Dismiss");
        noVisMItem.setMnemonic(KeyEvent.VK_R);
        noVisMItem.addActionListener(myMenuListener);
        JMenuItem exitMItem=new JMenuItem("Exit");
        exitMItem.setMnemonic(KeyEvent.VK_E);
        exitMItem.addActionListener(myMenuListener);

        fileMenu.add(optionsMItem);

        if(!threadsManager.noExitButton()) {
	    fileMenu.addSeparator();
	    fileMenu.add(noVisMItem);
            fileMenu.add(exitMItem);
        }

        // 1.2.-Add the file-menu to the menu bar:
        menuBar.add(fileMenu);

        // 2.- Create the units-menu:
        // --------------------------
        unitsMenu = new JMenu("Units");
        unitsMenu.setMnemonic(KeyEvent.VK_U);

        // 2.1.- Create the items in the units-menu and add them to the menu:
        BWSubmenu = new JMenu("Bandwidth (BW)...");
        BWSubmenu.setMnemonic(KeyEvent.VK_B);
        BWBGroup = new ButtonGroup();
            KBySgRBMI = new JRadioButtonMenuItem ("KBytes/sg");
            KBySgRBMI.addActionListener(myMenuListener);
            BWBGroup.add(KBySgRBMI);
            BWSubmenu.add(KBySgRBMI);

            KbiSgRBMI = new JRadioButtonMenuItem ("Kbits/sg");
            KbiSgRBMI.setSelected(true);
            KbiSgRBMI.addActionListener(myMenuListener);
            BWBGroup.add(KbiSgRBMI);
            BWSubmenu.add(KbiSgRBMI);

        FRSubmenu = new JMenu("Frame Rate (FR)...");
        FRSubmenu.setMnemonic(KeyEvent.VK_F);
        FRBGroup = new ButtonGroup();
            FRSgRBMI = new JRadioButtonMenuItem ("Frames/sg");
            FRSgRBMI.setSelected(true);
            FRSgRBMI.addActionListener(myMenuListener);
            FRBGroup.add(FRSgRBMI);
            FRSubmenu.add(FRSgRBMI);

        unitsMenu.add(BWSubmenu);
        unitsMenu.addSeparator();
        unitsMenu.add(FRSubmenu);

        // 2.2.- Add the units menu to the bar:
        menuBar.add(unitsMenu);

        // 3.- Create the help-menu:
        // -------------------------
        helpMenu = new JMenu ("Help");
        helpMenu.setMnemonic(KeyEvent.VK_H);
        menuBar.add(helpMenu);

        // 3.1.- Create the items in the help-menu and add them to the menu:
        JMenuItem helpMItem = new JMenuItem("User Manual...");
        helpMItem.setMnemonic(KeyEvent.VK_U);
        helpMItem.addActionListener(myMenuListener);
        JMenuItem aboutMItem = new JMenuItem("About...");
        aboutMItem.setMnemonic(KeyEvent.VK_A);
        aboutMItem.addActionListener(myMenuListener);

        helpMenu.add(helpMItem);
        helpMenu.add(aboutMItem);

        // 3.2.-Add the help-menu to the menu bar:
        menuBar.add(Box.createHorizontalGlue());
        menuBar.add(helpMenu);
    }

    //Function for the creation of all the elements in the central panel:
    //-------------------------------------------------------------------

    private  Component createComponents() {

        JPanel BottomPanel            = new JPanel();
        JPanel upperAudioPanel        = new JPanel();
        JPanel upperVideoPanel        = new JPanel();
        JPanel upperShDisplayPanel    = new JPanel();
        JPanel irtPanel               = new JPanel();
        JPanel RCVIrtPanel            = new JPanel();
        JPanel SNDIrtPanel            = new JPanel();
        upperTabbedPane               = new JTabbedPane();

        // 1.- Creation of the audio tabbed-panel:
        // ---------------------------------------
        ImageIcon audioIcon    = getJarImages("images/audioIcon.gif");
        upperAudioPanel.setLayout(new GridLayout(1,0));
        audioTModel            = new audioTableModel_t(DataBaseHandler);
        audioTable             = new audioJTable_t(audioTModel,this,"AUD",DataBaseHandler.getAudioDataBase());
        audioTable.initColumnSize();
        audioTable.initColumnRenderer();
        JScrollPane audioScrollPane = new JScrollPane(audioTable);
        upperAudioPanel.add(audioScrollPane);


        // 2.- Creation of the video tabbed-panel:
        // ---------------------------------------
        ImageIcon videoIcon    = getJarImages("images/videoIcon.gif");
        upperVideoPanel.setLayout(new GridLayout(1,0));
        videoTModel            = new videoTableModel_t(DataBaseHandler);
        videoTable             = new videoJTable_t(videoTModel,this,"VID",DataBaseHandler.getVideoDataBase());
        videoTable.initColumnSize();
        videoTable.initColumnRenderer();
        JScrollPane videoScrollPane = new JScrollPane(videoTable);
        upperVideoPanel.add(videoScrollPane);

        // 3.- Creation of the shared-display tabbed panel:
        // ------------------------------------------------
        ImageIcon shDisIcon    = getJarImages("images/shdisIcon.gif");
        upperShDisplayPanel.setLayout(new GridLayout(1,0));
        shDisplayTModel        = new shDisplayTableModel_t(DataBaseHandler);
        shDisplayTable         = new shDisplayJTable_t(shDisplayTModel,this,"SHD",DataBaseHandler.getShDisplayDataBase());
        shDisplayTable.initColumnSize();
        shDisplayTable.initColumnRenderer();
        JScrollPane shDisplayScrollPane = new JScrollPane(shDisplayTable);
        upperShDisplayPanel.add(shDisplayScrollPane);

        // 5.- Creating the irouter tabbed-Pannel:
        // ---------------------------------------
        // Quiza aqui en el metodo de seleccion deban
        // obtenerse los campos precisos.
        ImageIcon irtIcon = getJarImages("images/irouterIcon.gif");
        irtPanel.setLayout(new GridLayout(2,0));
            // 5.A.- Panel with the received table:
            // ------------------------------
            RCVIrtPanel.setLayout(new BoxLayout(RCVIrtPanel,BoxLayout.Y_AXIS));
            RCVIrtPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(new Color(0,0,0)),"Recibidos: "));


            irtRCVTModel = new irouterRCVTableModel_t(DataBaseHandler);
            irtRCVTable = new irouterRCVJTable_t(irtRCVTModel,this,"IRTRCV",DataBaseHandler.getIrouterDataBase());
            irtRCVTable.initColumnSize();
            irtRCVTable.initColumnRenderer();
            JScrollPane irtRCVScrollPane = new JScrollPane(irtRCVTable);
            //Prepare the RCVPanel:
            //---------------------
            JLabel RCVTotalLabel = new JLabel ("Total: ");
            RCVTotalValue = new JLabel ("");
            JPanel RCVTotalPanel = new JPanel();
            RCVTotalPanel.setLayout(new BoxLayout(RCVTotalPanel,BoxLayout.X_AXIS));
            RCVTotalPanel.add(RCVTotalLabel);
            RCVTotalPanel.add(RCVTotalValue);

            RCVIrtPanel.add(irtRCVScrollPane);
            RCVIrtPanel.add(RCVTotalPanel);

            // 5.B.- Panel with the send table:
            // --------------------------
            SNDIrtPanel.setLayout(new BoxLayout(SNDIrtPanel,BoxLayout.Y_AXIS));
            SNDIrtPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(new Color(0,0,0)),"Enviados: "));


            irtSNDTModel = new irouterSNDTableModel_t(DataBaseHandler);
            irtSNDTable = new irouterSNDJTable_t(irtSNDTModel,this,"IRTSND",DataBaseHandler.getIrouterDataBase());
            irtSNDTable.initColumnSize();
            irtSNDTable.initColumnRenderer();
            JScrollPane irtSNDScrollPane = new JScrollPane(irtSNDTable);
            //Prepare the RCVPanel:
            //---------------------
            JLabel SNDTotalLabel = new JLabel ("Total: ");
            SNDTotalValue = new JLabel ("");
            JPanel SNDTotalPanel = new JPanel();
            SNDTotalPanel.setLayout(new BoxLayout(SNDTotalPanel,BoxLayout.X_AXIS));
            SNDTotalPanel.add(SNDTotalLabel);
            SNDTotalPanel.add(SNDTotalValue);

            SNDIrtPanel.add(irtSNDScrollPane);
            SNDIrtPanel.add(SNDTotalPanel);

        //Include the panel:
        //------------------
        irtPanel.add(RCVIrtPanel);
        irtPanel.add(SNDIrtPanel);


        // 6.- Adding all the panels to the main panel:
        // --------------------------------------------
        upperTabbedPane.addTab("Audio", audioIcon, upperAudioPanel, "Audio Parameters and Measurements");
        upperTabbedPane.addTab("Video", videoIcon, upperVideoPanel, "Video Parameters and Measurements");
        upperTabbedPane.addTab("ShDisplay", shDisIcon, upperShDisplayPanel, "Shared Display Parameters and Measurements del sharedDispaly");
        upperTabbedPane.addTab("Irouter", irtIcon, irtPanel, "Irouter Parameters and Measurements");
        upperTabbedPane.setSelectedIndex(0);

        // 7.- Creating the bottom panel with all the buttons
        //     of the main screen of the monitor gui:
        // --------------------------------------------------

        // 7.1.- Graphics Button:
        //       this button let's draw a graphic of the evolution
        //       of one data selected from the tables.
        ImageIcon graphIcon = getJarImages("images/graphIcon.gif");
        JButton BGraphics = new JButton("Draw",graphIcon);
        BGraphics.setMnemonic(KeyEvent.VK_D);
        BGraphics.addActionListener(
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    //Code for manteining the count of frames opened:
                    if (selectedTable!=null) {
                        if (getUnits(selectedTable).equals("NA")) {
                            JOptionPane.showMessageDialog(upperTabbedPane,
                                             "Datum without temporal evolution",
                                             "Datum cannot be drawn",
                                             JOptionPane.ERROR_MESSAGE);
                            return;
                        }
                        graphicsFrame_t graphFrame =
                            new graphicsFrame_t(DataBaseHandler,
                                                threadsManager,
                                                getRequestedOid(selectedTable),
                                                getTitle(selectedTable),
                                                getConf(selectedTable),
                                                getUnits(selectedTable)
                                               );
                    }
                }
            }
        );

        // 7.2.- Exit GUI Button:
        //       this button closes the monitor GUI but not the program,
        //       so that the program continues receiving data from
        //       ISABEL and also serving this data to any SNMP manager
        //       that may ask for it.
        ImageIcon exitVisIcon = getJarImages("images/exitGUIIcon.gif");
        JButton BExitVis = new JButton ("Dismiss",exitVisIcon);
        BExitVis.setMnemonic(KeyEvent.VK_R);
        BExitVis.addActionListener(
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    threadsManager.deleteAllFrames();
                    setFinish(true);
                    threadsManager.MonitorGUIOff();
                }
            }
        );

        // 7.3.- Exit Button:
        //       this button closes the program as the exit
        //       option in the file menu.
        ImageIcon exitIcon = getJarImages("images/exitIcon.gif");
        JButton BExit = new JButton("Exit",exitIcon);
        BExit.setMnemonic(KeyEvent.VK_E);
        BExit.addActionListener(
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    System.exit(0);
                }
            }
        );

        // 7.4.- Adding all the buttons to the buttom panel:
        BottomPanel.setLayout( new GridLayout(1,0));
        BottomPanel.setBorder(BorderFactory.createEmptyBorder(30,30,10,30));
        BottomPanel.add(BGraphics);
        if (!threadsManager.noExitButton()) {
	    BottomPanel.add(BExitVis);
            BottomPanel.add(BExit);
        }
        BottomPanel.setBorder(BorderFactory.createBevelBorder(1));

        // 8.- Adding everything to the main panel:
        // ----------------------------------------
        JPanel pane = new JPanel();
        pane.setLayout(new BorderLayout());
        pane.add(upperTabbedPane,BorderLayout.CENTER);
        pane.add(BottomPanel,BorderLayout.SOUTH);

        return pane;
    }

    // run method:
    // -----------
    public void run () {
        try {
            while((threadsManager.doIkeepAlive())&&(!finish)) {
                updateAllData();
                thisThread.sleep(2000);
            }
            setVisible(false);
            dispose();
        } catch (Exception e) {
             System.out.println("Excepcion en RUNMonitorGUI:"+e);
        };
    }

    public ImageIcon getJarImages(String imageName) {
        return new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/"+imageName)));
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //FUNCTIONS THAT MUST BE CHANGED WHEN ADDING A NEW COMPONENT TO THE MONITOR:
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void    updateAllData () {
                audioTModel.updateTable();
                videoTModel.updateTable();
                shDisplayTModel.updateTable();
                irtRCVTModel.updateTable();
                irtSNDTModel.updateTable();
                try {
                    double value = Double.parseDouble(DataBaseHandler.getData(new sck.Oid("1.3.6.1.4.1.2781.1.2.2.0")).toString());
                    value=value/tableModel_t.getFactorBW();
                    RCVTotalValue.setText(new Double(value).toString());
                } catch (Exception exc) {
                    RCVTotalValue.setText("");
                }
                try {
                    double value = Double.parseDouble(DataBaseHandler.getData(new sck.Oid("1.3.6.1.4.1.2781.1.2.4.0")).toString());
                    value=value/tableModel_t.getFactorBW();
                    SNDTotalValue.setText(new Double(value).toString());
                } catch (Exception exc) {
                    SNDTotalValue.setText("");
                }

    }
    String getTitle (String st) {
        if (st.equals("AUD"))    return audioTable.getTitleForGraph();
        if (st.equals("VID"))    return videoTable.getTitleForGraph();
        if (st.equals("SHD"))    return shDisplayTable.getTitleForGraph();
        if (st.equals("IRTRCV")) return irtRCVTable.getTitleForGraph();
        if (st.equals("IRTSND")) return irtSNDTable.getTitleForGraph();

        return null;
    }

    sck.Oid getRequestedOid (String st) {
        if (st.equals("AUD"))    return audioTable.getOidForGraph();
        if (st.equals("VID"))    return videoTable.getOidForGraph();
        if (st.equals("SHD"))    return shDisplayTable.getOidForGraph();
        if (st.equals("IRTRCV")) return irtRCVTable.getOidForGraph();
        if (st.equals("IRTSND")) return irtSNDTable.getOidForGraph();

        return null;
    }

    String getUnits (String st) {
        if (st.equals("AUD"))    return audioTable.getColUnits();
        if (st.equals("VID"))    return videoTable.getColUnits();
        if (st.equals("SHD"))    return shDisplayTable.getColUnits();
        if (st.equals("IRTRCV")) return irtRCVTable.getColUnits();
        if (st.equals("IRTSND")) return irtSNDTable.getColUnits();

        return null;
    }

    String getConf (String st) {
        if (st.equals("AUD"))    return audioTable.getColConf();
        if (st.equals("VID"))    return videoTable.getColConf();
        if (st.equals("SHD"))    return shDisplayTable.getColConf();
        if (st.equals("IRTRCV")) return irtRCVTable.getColConf();
        if (st.equals("IRTSND")) return irtSNDTable.getColConf();

        return null;
    }

    //---------------------------------------------------------------------
    //Class that listen to the menu events:
    //---------------------------------------------------------------------
    class MenuListener implements ActionListener {
        FrameRef parentFrame;

        MenuListener (FrameRef parentFrame) {
            this.parentFrame = parentFrame;
        }

        public void actionPerformed(ActionEvent e) {
            JMenuItem source = (JMenuItem)(e.getSource());
            String sourceText = source.getText();

            //Events in the file-menu:
            //------------------------
            if (sourceText.equals("Exit")) {
                System.exit(0);
            }
            if (sourceText.equals("Dismiss")) {
                threadsManager.deleteAllFrames();
                setFinish(true);
                threadsManager.MonitorGUIOff();
                return;
            }
            if (sourceText.equals("Port configuration...")) {
                portConfFrame_t portConfFrame = new portConfFrame_t(parentFrame,threadsManager);
                return;
            }
            //Events in the units-menu:
            //-------------------------
            if (sourceText.equals("KBytes/sg"))
                tableModel_t.setFactorBW(100000*8);
            if (sourceText.equals("Kbits/sg"))
                tableModel_t.setFactorBW(100000);

            //Events in the help-menu:
            //------------------------
            if (sourceText.equals("About...")) {
                JOptionPane.showMessageDialog(parentFrame,
                    "ISABEL's SNMP Monitor\n"
                    +"\n"
                    +"The ISABEL's SNMP Monitor shows traffic data\n"
                    +"and delivers them to any SNMP manager\n"
                    +"\n"
                    +"Autor: ISABEL Development Team\n"
                    +"Copyright: Agora Systems S.A.\n"
                    +"http://www.agora-2000.com",
                    "About...",
                    JOptionPane.INFORMATION_MESSAGE,
                    new ImageIcon("images/monitorIcon.gif"));
                return;
            }
            if (sourceText.equals("User Manual...")) {
                helpFrame_t helpFrame = new helpFrame_t(threadsManager);
                threadsManager.insertFrame(helpFrame);
                return;
            }
        }
    }

}
