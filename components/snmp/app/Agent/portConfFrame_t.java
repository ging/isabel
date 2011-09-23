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
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
public class portConfFrame_t extends JDialog {
    threadsManager_t threadsManager;
    FrameRef parentFrame;
    portConfFrame_t(FrameRef parentFrame, threadsManager_t threadsManager) {
        super (parentFrame,"Port configuration",true);
        this.threadsManager = threadsManager;
        this.parentFrame    = parentFrame;
        Component contents  = createComponents();
        getContentPane().add(contents, BorderLayout.CENTER);
        pack();
        show();
    }

    protected Component createComponents() {
        JPanel panel = new JPanel();
        panel.setLayout ( new BoxLayout(panel, BoxLayout.Y_AXIS));

        //1.- Options Panel:
        JPanel optionsPanel = new JPanel();
        optionsPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(new Color(0,0,0)),"Servers ports:"));
        optionsPanel.setLayout(new GridLayout(3,2));
        JLabel SNMPSL = new JLabel("SNMP server port:");
        JLabel CtrlSL = new JLabel("Control port:");
        JLabel IntrSL = new JLabel("ISABEL data port:");
        JTextField SNMPSTF = new JTextField(new Integer(threadsManager.getSNMPServer().SNMPPort).toString(),5);
        String CtrlText = (threadsManager.getCtrlServer().shellControl)?
                            "shell":
                            new Integer(threadsManager.getCtrlServer().ctrlPort).toString();
        JTextField CtrlSTF = new JTextField(CtrlText,5);
        CtrlSTF.setEditable(false);
        JTextField IntrSTF = new JTextField(new Integer(threadsManager.getInternalServer().internalPort).toString(),5);
        IntrSTF.setEditable(false);
        optionsPanel.add(SNMPSL);
        optionsPanel.add(SNMPSTF);
        optionsPanel.add(CtrlSL);
        optionsPanel.add(CtrlSTF);
        optionsPanel.add(IntrSL);
        optionsPanel.add(IntrSTF);

        //2.- Bottom buttons:
        JPanel buttonsPanel = new JPanel();
      ButtonListener myButtonListener = new ButtonListener(this, SNMPSTF, CtrlSTF, IntrSTF);
      JButton BOK = new JButton("OK");
      BOK.addActionListener(myButtonListener);
      JButton BCancel = new JButton ("Cancel");
      BCancel.addActionListener(myButtonListener);
      buttonsPanel.setBorder(BorderFactory.createLineBorder(new Color(0,0,0)));
      buttonsPanel.setLayout(new FlowLayout(FlowLayout.CENTER));
      buttonsPanel.add(BOK);
      buttonsPanel.add(BCancel);

        //3.- Add each of the panels to the return panel:
        panel.add(optionsPanel);
        panel.add(buttonsPanel);
        return panel;
    }

    class ButtonListener implements ActionListener {
      portConfFrame_t optFrame;
        JTextField SNMPSTF, CtrlSTF, IntrSTF;
        ButtonListener (portConfFrame_t optFrame, JTextField SNMPSTF, JTextField CtrlSTF, JTextField IntrSTF) {
         this.optFrame = optFrame;
          this.SNMPSTF = SNMPSTF;
            this.CtrlSTF = CtrlSTF;
            this.IntrSTF = IntrSTF;
        }
      public void actionPerformed (ActionEvent e) {
         JButton source = (JButton)e.getSource();
         if (source.getText().equals("OK")) {
            //Check for any possible changes:
                //1.- SNMPPort Change:
                int newSNMPPort;
                try {
                    newSNMPPort = Integer.parseInt(SNMPSTF.getText());
                }catch (NullPointerException exc) {
                    JOptionPane.showMessageDialog(parentFrame,
                                                            "No se ha introducido ningún valor para el puerto SNMP",
                                                            "Valor no válido",
                                                            JOptionPane.ERROR_MESSAGE);
                    return;
                }catch (NumberFormatException exc) {
                    JOptionPane.showMessageDialog(parentFrame,
                                                            "El valor introducido para el puero SNMP no es un número",
                                                            "Valor no válido",
                                                            JOptionPane.ERROR_MESSAGE);
                    return;
                }
                if (threadsManager.getSNMPServer().SNMPPort!=newSNMPPort) {
                    Component contents = createTextPanel("Dejará de atender peticiones SNMP en el puerto: "
                                                                     +threadsManager.getSNMPServer().SNMPPort+". ¿Está seguro?");
                    int response = JOptionPane.showConfirmDialog(parentFrame,
                    contents,
                    "Cambio del puerto SNMPPort:",
                    JOptionPane.YES_NO_OPTION);
                    if ( response == JOptionPane.NO_OPTION ) return;
                    if ( response == JOptionPane.YES_OPTION ) {
                            try {
                                threadsManager.SNMPServerOnAgain(newSNMPPort);
                            }catch (java.net.SocketException exc) {
                                 JOptionPane.showMessageDialog(parentFrame,
                                                    "El puerto que desea usar ya está ocupado.Debe dar otro valor",
                                       "Error en el puerto SNMP",
                                       JOptionPane.ERROR_MESSAGE);
                                return;
                            }
                        }
                    }
                //2.- CtrlPort change:

                //3.- Internal change:
                /*int newIntrPort;
            try {
               newIntrPort = Integer.parseInt(IntrSTF.getText());
            }catch (NullPointerException exc) {
               JOptionPane.showMessageDialog(parentFrame,
                                             "No se ha introducido ningún valor para el puerto de comunicación con ISABEL",
                                             "Valor no válido",
                                             JOptionPane.ERROR_MESSAGE);
               return;
            }catch (NumberFormatException exc) {
               JOptionPane.showMessageDialog(parentFrame,
                                             "El valor introducido para el puero de comunicación con ISABEL no es un número",
                                             "Valor no válido",
                                             JOptionPane.ERROR_MESSAGE);
               return;
            }
            if (threadsManager.getInternalServer().internalPort!=newIntrPort) {
               Component contents = createTextPanel("Dejará de recibir datos de ISABEL en el puerto: "
                                                    +threadsManager.getInternalServer().internalPort+".También se borraran todos los datos disponibles en este momento. ¿Está seguro?");
               int response = JOptionPane.showConfirmDialog(parentFrame,
               contents,
               "Cambio del puerto de comunicac. con ISABEL:",
               JOptionPane.YES_NO_OPTION);
               if ( response == JOptionPane.NO_OPTION ) return;
               if ( response == JOptionPane.YES_OPTION ) {
                     try {
                        threadsManager.InternalServerOnAgain(newIntrPort);
                     }catch (java.net.SocketException exc) {
                        JOptionPane.showMessageDialog(parentFrame,
                                       "El puerto que desea usar ya está ocupado.Debe dar otro valor",
                                       "Error en el puerto de comunic. con ISABEL:",
                                       JOptionPane.ERROR_MESSAGE);
                        return;
                     }
                  }
                } */
                optFrame.setVisible(false);
            optFrame.dispose();
             return;
            }
         if (source.getText().equals("Cancel")) {
            optFrame.setVisible(false);
            optFrame.dispose();
         }
      }
    }
    Component createTextPanel(String text) {
        JPanel panel = new JPanel();
        panel.setSize(new Dimension (600,50));
        JLabel textLabel = new JLabel(text);
        textLabel.setForeground(new Color (0,0,0));
        panel.add(textLabel);
        return panel;

    }
}//End of OpcionesFrame

