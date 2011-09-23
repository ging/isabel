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
 * @Author lailoken
 */
public class StatusPanel extends JPanel {

  private JLabel msgLabel;
  private JProgressBar progressBar;
  private JTextArea msgArea;
  private JScrollPane msgScrollPane;
  private JButton exitButton;

  /**
   * Implenta el interfaz gr�fico para mostrar paneles informativos de los diferentes servicios
   * msg: Mensaje que se quiere mostrar en el panel.
   * progress: estado de la barra de progreso.
   * indet: indica si la barra de progreso es indeterminada o no.
   */
  public StatusPanel(String msg, int progress, boolean indet) {
    super(new BorderLayout());

    //this.setLayout(new GridBagLayout());
    //GridBagConstraints gc;
    //final JFileChooser fc = new JFileChooser();
    progressBar = new JProgressBar(0,100); //limites de la barra de progreso
    progressBar.setValue(progress);
    progressBar.setStringPainted(true);
    if (indet) {
      progressBar.setString(""); // para indeterminada
    } else{
      progressBar.setString(null); //display % string
    }
    progressBar.setIndeterminate(indet);
    //JPanel panel = new JPanel();
    //gc = new GridBagConstraints();
    //gc.gridx = 0;
    //gc.gridy = 0;
    //gc.anchor = gc.WEST;
    //add(progressBar,gc);

    // Mensaje
/*    msgLabel = new JLabel(msg);
    //gc = new GridBagConstraints();
    gc.gridx = 0;
    gc.gridy = 1;
    gc.anchor = gc.WEST;
    //add(msgLabel, gc);
    msgLabel.setText(msg);
    add(msgLabel,gc);
*/

    // Textpanel
    //gc.gridx = 0;
    //gc.gridy = 1;
    //gc.anchor = gc.WEST;
    //add(msgLabel, gc);
    msgArea = new JTextArea(5, 20);
    msgArea.setMargin(new Insets(5,5,5,5));
    msgArea.setEditable(false);
    msgArea.setCursor(null); //inherit the panel's cursor
                                //see bug 4851758
    //msgArea.setLineWrap(true);
    //msgArea.setWrapStyleWord(true);
    msgArea.append(msg);
    //add(msgArea,gc);
    
    // Metemos los elementos dentro del StatusPanel
    JPanel panel = new JPanel();
    panel.add(progressBar);
    add(panel, BorderLayout.PAGE_START);
    msgScrollPane = new JScrollPane(msgArea);

    add(msgScrollPane, BorderLayout.CENTER);
    setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

    exitButton = new javax.swing.JButton();
    exitButton.setFont(new java.awt.Font("MS Sans Serif", 1, 12));
    exitButton.setText("Cancel");
    exitButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        System.exit(0);
      }
    });
    
    add(exitButton,BorderLayout.PAGE_END);
    
    //add(panel);
//    add(panel, BorderLayout.PAGE_START);
//    add(new JScrollPane(taskOutput), BorderLayout.CENTER);
//    setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
  }
  
  public void appendText(String text) {
    msgArea.append(text);
    // Move the vertical scrollbar down to see changes in the text
    //msgScrollPane.getVerticalScrollBar().setValue(msgArea.getHeight() - msgArea.getVisibleRect().height );
    msgScrollPane.getVerticalScrollBar().setValue(msgScrollPane.getVerticalScrollBar().getMaximum());
  }
  
  public void changeProgress(int newProgress) {
    progressBar.setValue(newProgress);
  }
  
  public void setIndeterminate(boolean indet){
    progressBar.setIndeterminate(indet);
  }
}

