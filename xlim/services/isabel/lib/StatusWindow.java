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
package services.isabel.lib;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

// Para el manejo de la Agenda
//import isabel.xlim.services.Agenda;

import java.io.*;

// Para el manejo de XEDL
import xedl.lib.xedl.*;

import java.net.*;

/**
 * Implenta el interfaz gr�fico para mostrar paneles informativos de los diferentes servicios
 */
public class StatusWindow {

  private JComponent newContentPane;
  private JFrame frame;
  private static String newLine = "\r\n";
  private String historyMsg = "";
  private StatusPanel statusPanel;
  
  /**
   * Llamado desde el constructor para inicializar los componentes.
   */
  private void initComponents(String title, String msg) {
    //Make sure we have nice window decorations.
    //JFrame.setDefaultLookAndFeelDecorated(true);

    //Create and set up the window.
    frame = new JFrame(title);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    //frame.setResizable(false);

    // Updating History Message
    historyMsg = historyMsg + msg;

    //Create and set up the content pane.
    statusPanel = new StatusPanel(historyMsg,0,true);
    newContentPane = statusPanel;
    newContentPane.setOpaque(true); //content panes must be opaque
    frame.setContentPane(newContentPane);
    //Display the window.
    frame.pack();
    frame.setVisible(true);
  }


  public StatusWindow(String msg) {
    initComponents("ISABEL Connect To Status",msg);
  }

  public StatusWindow(String title, String msg) {
    initComponents(title,msg);
  }


  public StatusWindow() {
    initComponents("ISABEL Connect To Status","    Processing...    ");
  }


  /**
   * Establece un nuevo mensaje para ense�ar en el panel
   */
  public void setStatusMsg(String newMsg,int newValue) {
    // Updating history Message
    historyMsg = historyMsg + newLine + newMsg;

    //***newContentPane = new StatusPanel(historyMsg,newValue,false);
    statusPanel.appendText(newLine + newMsg);
    statusPanel.changeProgress(newValue);
    statusPanel.setIndeterminate(false);
    //newContentPane.setOpaque(true); //content panes must be opaque
    //frame.setContentPane(newContentPane);
    //Display the window.
    //frame.pack();
    //frame.setVisible(true);
  }


  /**
   * Oculta el panel
   */
  public void hideStatusWindow() {
    frame.setVisible(false);
    frame.dispose();
  }



  public static void main(String[] args) {
    try{
      // Necesitamos conocer todas las ips de la maquina....
      String address;
      // Primero sacamos el nombre de la maquina
      String host = InetAddress.getLocalHost().getHostName();
      address = host;
      // Ahora cogemos todas las ips
      InetAddress[] hostIps = InetAddress.getAllByName(host);
      System.out.println("Found " + hostIps.length + " ips.");
      for (int i=0;i<hostIps.length;i++){
        try {
          address = address + "/" + hostIps[i].toString().split("/")[1];
        } catch (Exception e) {
        }
        System.out.println("Address is" + address);
      }
    } catch (Exception e) {
    }
    
    // Creamos el objeto StatusWindow y lo ense�a.
    StatusWindow statusWin = new StatusWindow();
    
    int i;
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    for (i=0; i<1000000000;i++) {
      System.out.println("Contando... "+i);
    }
    
    // Cambiamos valores a ver si hay suerte...
    statusWin.setStatusMsg("pruebaaaaa",20);
    
    for (i=0; i<100000000;i++) {
      //System.out.println("Contando... "+i);
    }
    
    // Cambiamos valores a ver si hay suerte...
    statusWin.setStatusMsg("kkkkkkkkkkkkkkkk\r\nasdasdf\r\nasdfasdfadsf",50);
    
    for (i=0; i<100000000;i++) {
      //System.out.println("Contando... "+i);
    }
    
    // Cambiamos valores a ver si hay suerte...
    statusWin.setStatusMsg("ZZZZZZZZZ\r\nZZZZZZZZZZ",75);
    
    for (i=0; i<100000000;i++) {
      //System.out.println("Contando... "+i);
    }
    
    //Ocultamos la ventana
    statusWin.hideStatusWindow();
  }
}
