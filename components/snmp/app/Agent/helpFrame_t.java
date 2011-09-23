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
//***********************************************************************
//
//FILE:  helpFrame_t.java
//
//DESCRIPTION:
//    This class is a frame that implements a very rudimentary html browser.
//    It's used to see the help documentation that is on html.
//

import java.awt.*;
import javax.swing.*;
import java.net.*;

import javax.swing.event.*;
import java.awt.event.*;
import javax.swing.JMenu.*;
public class helpFrame_t  extends FrameRef {

    //Attributes:
    //-----------
    URL               helpURL;
    JMenu             fileMenu;
    JMenuBar          menuBar;
    threadsManager_t  threadsManager;

    //Constructor:
    //------------
    helpFrame_t (threadsManager_t threadsManager) {
        super ("User's Manual");

        this.threadsManager = threadsManager;

        //Create the menu:
        //----------------
         createMenu();
        setJMenuBar(menuBar);

        //Create the browser:
        //-------------------
        JEditorPane editorPane         = new JEditorPane();
        editorPane.setEditable(false);
        JScrollPane editorScrollPane   = new JScrollPane(editorPane);
        editorScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        editorScrollPane.setPreferredSize(new Dimension(680,720));

        //Load the URL:
        myHyperlinkListener_t myHyperlinkListener = new myHyperlinkListener_t(editorPane);
        editorPane.addHyperlinkListener(myHyperlinkListener);

        String file = null;
        try {
            file = ""+getClass().getResource("/help/helpIndex.html");
            helpURL= getClass().getResource("/help/helpIndex.html");
        }catch (Exception e) {
            System.out.println("No pude obtener el URL con la ayuda:"+file);
        }
        try {
            editorPane.setPage(helpURL);
        }catch (java.io.IOException e) {
            System.out.println("Intento de lectura de un url defectuoso:"+helpURL);
        }
        getContentPane().add(editorScrollPane);
        pack();
        setSize(new Dimension(640,680));
        show();
    }

    //Function to get
    //--------------------------------------------------------------------------
    //Class that listen for any attemp of using a hyperlink in the help document
    //--------------------------------------------------------------------------
    class myHyperlinkListener_t implements HyperlinkListener {

        JEditorPane thisEditorPane;
        URL nextURL;

        //Constructor:
        myHyperlinkListener_t(JEditorPane thisEditorPane) {
            this.thisEditorPane = thisEditorPane;
        }

        public void hyperlinkUpdate(HyperlinkEvent e) {
            if (e.getEventType().toString().equals("ACTIVATED")) {
            try {
                nextURL = e.getURL();
                thisEditorPane.setPage(nextURL);
                }catch (java.io.IOException exc) {
                System.out.println("Intento de lectura de un url defectuoso:"+nextURL);
                }
            }
        }
    }

    //Function that creates the upper menu:
    //-------------------------------------
    protected void createMenu() {
        menuBar = new JMenuBar();
        MenuListener myMenuListener = new MenuListener(this);

        //1.- Create the file-menu:
        //-------------------------
        fileMenu = new JMenu ("Options");
        fileMenu.setMnemonic(KeyEvent.VK_O);
        menuBar.add(fileMenu);

            //1.1.- Create the file-menu items:
            //---------------------------------
            JMenuItem closeMItem = new JMenuItem ("Close");
            closeMItem.setMnemonic(KeyEvent.VK_C);
            closeMItem.addActionListener(myMenuListener);

            JMenuItem noVisMItem= new JMenuItem("Dismiss");
            noVisMItem.setMnemonic(KeyEvent.VK_R);
            noVisMItem.addActionListener(myMenuListener);

            JMenuItem salirMItem=new JMenuItem("Exit");
            salirMItem.setMnemonic(KeyEvent.VK_E);
            salirMItem.addActionListener(myMenuListener);

            fileMenu.add(closeMItem);
            if(!threadsManager.noExitButton()) {
		fileMenu.addSeparator ();
		fileMenu.add(noVisMItem);
                fileMenu.add(salirMItem);
            }

            //1.2.-Add the file-menu to the menu bar:
            //---------------------------------------
            menuBar.add(fileMenu);
    }

    //Function that eliminates a graphicsFrame and all the resources used by it:
    //--------------------------------------------------------------------------
    public void setFinish(boolean value) {
        if (value) {
            setVisible(false);
            dispose();
        }
    }

    //----------------------------------------------------
    //Inner class that listen to the menu events:
    //----------------------------------------------------
    class MenuListener implements ActionListener {
        FrameRef parentFrame;

        MenuListener(FrameRef parentFrame) {
            this.parentFrame = parentFrame;
        }

        public void actionPerformed(ActionEvent e) {
            JMenuItem source = (JMenuItem)(e.getSource());
            String sourceText = source.getText();
            if (sourceText.equals("Close")) {
               threadsManager.deleteFrame(parentFrame);
               parentFrame.setFinish(true);
            }
            if (sourceText.equals("Exit")) {
               System.exit(0);
            }
            if (sourceText.equals("Dismiss")) {
               threadsManager.deleteAllFrames();
               threadsManager.getMonitorGUI().setFinish(true);
               threadsManager.MonitorGUIOff();
               return;
            }
        }
    }
}
