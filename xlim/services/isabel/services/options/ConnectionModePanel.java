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
 * ConnectionModePanel.java
 *
 * Created on 12 de septiembre de 2003, 17:29
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;

//import isabel.lim.HelpBrowser;
/**
 * Esta clase un panel para introducir datos sobre el modo de conexion
 * a la sesion ISABEL
 */
public class ConnectionModePanel extends JPanel implements OptionsStrings, ValidablePanel {

    private ConnectionModePanelListener listener;
    
    /**
     * Crea un objeto ConnectionModePanel
     */
    public ConnectionModePanel() {
        initComponents();
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * @return True si los datos son correctos y false si no lo son.
     */
    public boolean validateData() {
        return true;
    }

    /**
     * Establece el listener de este panel. Si ya habia uno se sustituye.
     */
    public void setListener(ConnectionModePanelListener lis) {
        listener = lis;
    }
    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL CONNECTIONMODE           *
     ***************************************************************************/
    
    public boolean isUnicastMode() {
        return  unicastButton.isSelected();
    }
    
    public void setUnicastMode() {
         unicastButton.setSelected(true);
    }
    
    public boolean isMCUMode() {
        return  mcuButton.isSelected();
    }
    
    public void setMCUMode() {
         mcuButton.setSelected(true);
    }
    public boolean isMulticastMode() {
        return  multicastButton.isSelected();
    }
    
    public void setMulticastMode() {
         multicastButton.setSelected(true);
    }
    
    public String getPeerAddress() {
        return  peerAddressField.getText();
    }
    
    public void setPeerAddress(String address) {
         peerAddressField.setText(address);
    }
    
    public String getMCUAddress() {
        return  mcuAddressField.getText();
    }
    
    public void setMCUAddress(String address) {
         mcuAddressField.setText(address);
    }
    
    public String getMulticastTTL() {
        return  ttlField.getText();
    }
    
    public void setMulticastTTL(int ttl) {
         ttlField.setText("" + ttl);
    }
    
    public String getUPBandwidth() {
        return (String) upBWSelector.getSelectedItem();
    }
    
    public void setUPBandwidth(String bw) {
         upBWSelector.setSelectedItem(bw);
    }
    
    public String getDownBandwidth() {
        return (String) downBWSelector.getSelectedItem();
    }
    
    public void setDownBandwidth(String bw) {
         downBWSelector.setSelectedItem(bw);
    }

//    /**
//     * M�todo ejecutado cuando se pulsa el boton de ayuda.
//     */
//    private void helpButtonActionPerformed(ActionEvent evt) {
//        if (helpBrowser == null)
//            helpBrowser = new HelpBrowser(CMODE_HELP_HOME);
//        helpBrowser.show();
//    }
    
    /**
     * Este m�todo crea todos los componentes que forman el panel.
     */
    private void initComponents() {
        GridBagConstraints gridBagConstraints;

        // Grupo de botones para el modo de conexion
        ButtonGroup modeGroup = new ButtonGroup();

        // Configuracion del panel principal
        setLayout(new BorderLayout());
        setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                     new EmptyBorder(new Insets(10,10,10,10))));

        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        add(titlePane, BorderLayout.NORTH);
                
        // Etiqueta con el titulo
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(CONNECTION_MODE_TITLE);
        titlePane.add(titleLabel);

        // Separacion entre el titulo y el boton de ayuda
        titlePane.add(Box.createHorizontalGlue());
        
//        // Boton de ayuda        
//        JButton helpButton = new JButton();
//        helpButton.setIcon(new ImageIcon(getClass().getResource(HELP_ICON)));
//        helpButton.setToolTipText("Help");
//        helpButton.addActionListener(new ActionListener() {
//            public void actionPerformed(ActionEvent evt) {
//                helpButtonActionPerformed(evt);
//            }
//        });
//
//        titlePane.add(helpButton);
        
        
        // Etiqueta con el icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(CONNECTION_MODE_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);

        // Panel con los botones y un panel por cada modo
        JPanel modePane = new JPanel();
        modePane.setLayout(new BorderLayout());
        modePane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        add(modePane, BorderLayout.CENTER);

        // Panel con los botones para seleccionar el modo
        JPanel radioButtonsPane = new JPanel();
        radioButtonsPane.setLayout(new GridLayout(0, 1));
        radioButtonsPane.setBorder(new CompoundBorder(new EtchedBorder(),
                                   new TitledBorder("Choose One")));
        modePane.add(radioButtonsPane, BorderLayout.WEST);

        // Botones de seleccion de modo (unicast, mcu y multicast)           
        unicastButton = new JRadioButton();
        mcuButton = new JRadioButton();
        multicastButton = new JRadioButton();

        unicastButton.setText("Unicast");
        modeGroup.add(unicastButton);
        unicastButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                unicastButtonActionPerformed(evt);
            }
        });
        radioButtonsPane.add(unicastButton);

        mcuButton.setText("MCU");
        modeGroup.add(mcuButton);
        mcuButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                mcuButtonActionPerformed(evt);
            }
        });
        radioButtonsPane.add(mcuButton);

        multicastButton.setText("Multicast");
        modeGroup.add(multicastButton);
        multicastButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                multicastButtonActionPerformed(evt);
            }
        });
        radioButtonsPane.add(multicastButton);            


        // Panel con los datos para cada modo
        // Contiene un panel por cada modo
        JPanel fieldsPane = new JPanel();
        fieldsPane.setLayout(new GridLayout(0, 1));
        modePane.add(fieldsPane, BorderLayout.CENTER);

        // Panel de unicast. contiene un campode texto para la direccion de peer.
        JPanel unicastPane = new JPanel();
        unicastPane.setLayout(new GridBagLayout());
        unicastPane.setBorder(new CompoundBorder(new EtchedBorder(),
                              new TitledBorder("Peer Address")));            
        fieldsPane.add(unicastPane);

        peerAddressField = new JTextField();
        peerAddressField.setColumns(10);
        peerAddressField.setToolTipText(PEER_ADDRESS_HELP);
        peerAddressField.setEnabled(false);
        unicastPane.add(peerAddressField);

        // Panel de MCU. Contiene un campo de texto para la direccion de la MCU
        JPanel mcuPanel = new JPanel();
        mcuPanel.setLayout(new GridBagLayout());
        mcuPanel.setBorder(new CompoundBorder(new EtchedBorder(),
                           new TitledBorder("MCU Address")));
        fieldsPane.add(mcuPanel);

        mcuAddressField = new JTextField();
        mcuAddressField.setColumns(10);
        mcuAddressField.setToolTipText(MCU_ADDRESS_HELP);
        mcuAddressField.setEnabled(false);
        mcuPanel.add(mcuAddressField);

        // Panel de multicast. Contiene dos paneles. Uno para seleccionar
        // el TTL y otro con un boton para configurar los grupos.
        JPanel multicastPane = new JPanel();
        multicastPane.setLayout(new BoxLayout(multicastPane, BoxLayout.X_AXIS));
        fieldsPane.add(multicastPane);

        // Panel con un campo con dos botones con flechas para elejir el TTL
        JPanel ttlPane = new JPanel();
        ttlPane.setLayout(new GridBagLayout());
        ttlPane.setBorder(new CompoundBorder(new EtchedBorder(),
                          new TitledBorder("TTL")));
        multicastPane.add(ttlPane);

        ttlField = new JTextField();
        ttlField.setColumns(3);
        ttlField.setHorizontalAlignment(JTextField.TRAILING);
        ttlField.setText("4");
        ttlField.setToolTipText(MULTICAST_TTL_HELP);
        ttlField.setEnabled(false);
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = GridBagConstraints.BOTH;
        ttlPane.add(ttlField, gridBagConstraints);

        // Botones UP y DOWN
        ttlUpButton = new JButton();
        ttlDownButton = new JButton();
        ttlUpButton.setIcon(new ImageIcon(getClass().getResource(SMALL_UP_ICON)));
        ttlUpButton.setPreferredSize(new Dimension(13, 13));
        ttlUpButton.setEnabled(false);
        ttlUpButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                ttlUpButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        ttlPane.add(ttlUpButton, gridBagConstraints);

        ttlDownButton.setIcon(new ImageIcon(getClass().getResource(SMALL_DOWN_ICON)));
        ttlDownButton.setPreferredSize(new Dimension(13, 13));
        ttlDownButton.setEnabled(false);
        ttlDownButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                ttlDownButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        ttlPane.add(ttlDownButton, gridBagConstraints);

        // Panel con el boton para configurar los grupos
        JPanel groupsPane = new JPanel();
        groupsPane.setLayout(new GridBagLayout());
        groupsPane.setBorder(new CompoundBorder(new EtchedBorder(),
                             new TitledBorder("Groups")));
        multicastPane.add(groupsPane);

        // Boton de configuracion de los grupos multicast
        configButton = new JButton();
        configButton.setText("Configure");
        configButton.setToolTipText(CONFIG_MULTICAST_HELP);
        configButton.setEnabled(false);
        configButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                configButtonActionPerformed(evt);
            }
        });
        groupsPane.add(configButton);


        // Panel para seleccionar el ancho de banda
        // Tiene dos selectores (dentro de paneles para que se vean bien)
        JPanel bandwidthPane = new JPanel();
        bandwidthPane.setLayout(new BoxLayout(bandwidthPane, BoxLayout.Y_AXIS));
        bandwidthPane.setBorder(new CompoundBorder(new EtchedBorder(),
                                new TitledBorder("Select Bandwidth")));
        modePane.add(bandwidthPane, BorderLayout.EAST);

        // UPLOAD BW
        JPanel upBWPane = new JPanel();
        upBWPane.setBorder(new TitledBorder(null, "Upload Bandwidth",
                                            TitledBorder.DEFAULT_JUSTIFICATION,
                                            TitledBorder.DEFAULT_POSITION,
                                            new Font("Dialog", 0, 10)));
        bandwidthPane.add(upBWPane);

        upBWSelector = new JComboBox();
        upBWSelector.setModel(new DefaultComboBoxModel(AVALAIBLE_BANDWIDTHS));
        upBWSelector.setSelectedItem(DEFAULT_UP_BW);
        upBWSelector.setPreferredSize(new Dimension(120, 25));
        upBWPane.add(upBWSelector);

        // 20 p�xeles entre un selector y otro
        bandwidthPane.add(Box.createVerticalGlue());

        // DOWNLOAD BW
        JPanel downBWPane = new JPanel();
        downBWPane.setBorder(new TitledBorder(null, "Download Bandwidth",
                                              TitledBorder.DEFAULT_JUSTIFICATION,
                                              TitledBorder.DEFAULT_POSITION,
                                              new Font("Dialog", 0, 10)));
        bandwidthPane.add(downBWPane);

        downBWSelector = new JComboBox();
        downBWSelector.setModel(new DefaultComboBoxModel(AVALAIBLE_BANDWIDTHS));
        downBWSelector.setSelectedItem(DEFAULT_DOWN_BW);
        downBWSelector.setPreferredSize(new Dimension(120, 25));
        downBWPane.add(downBWSelector);

        // Pongo los botones seleccionados por defecto
        mcuButton.setSelected(true);
        mcuAddressField.setEnabled(true);
    }

    /**
     * Metodo que se ejecuta cuando se pulsa el boton de configuracion.
     * Pasa al panel de multicast.
     */
    private void configButtonActionPerformed(ActionEvent evt) {
        if (listener != null)
            listener.configButtonPressed();
    }

    /**
     * Activa los campos de los paneles que se digan en los parametros.
     */
    private void enableFields(boolean uni, boolean mcu, boolean multi) {
        peerAddressField.setEnabled(uni);
        mcuAddressField.setEnabled(mcu);
        ttlField.setEnabled(multi);
        ttlUpButton.setEnabled(multi);
        ttlDownButton.setEnabled(multi);
        configButton.setEnabled(multi);
        if (listener != null)
            listener.multicastSelected(multi);
    }

    /**
     * M�todo que se ejecuta cuando se selecciona multicast.
     */
    private void multicastButtonActionPerformed(ActionEvent evt) {
        enableFields(false, false, true);
    }

    /**
     * M�todo que se ejecuta cuando se selecciona mcu.
     */
    private void mcuButtonActionPerformed(ActionEvent evt) {
        enableFields(false, true, false);
    }

    /**
     * M�todo que se ejecuta cuando se selecciona unicast.
     */
    private void unicastButtonActionPerformed(ActionEvent evt) {
        enableFields(true, false, false);
    }

    /**
     * Baja el campo TTL.
     */
    private void ttlDownButtonActionPerformed(ActionEvent evt) {
        int i = 4;
        try {
            i = Integer.parseInt(ttlField.getText());
            if (i == 0) i = 1;
        }catch (NumberFormatException e) {
        }
        ttlField.setText(--i + "");
    }

    /**
     * Sube el campo TTL.
     */
    private void ttlUpButtonActionPerformed(ActionEvent evt) {
        int i = 4;
        try {
            i = Integer.parseInt(ttlField.getText());
        }catch (NumberFormatException e) {
        }
        ttlField.setText(++i + "");
    }

    private JRadioButton unicastButton;
    private JRadioButton mcuButton;
    private JRadioButton multicastButton;

    private JTextField peerAddressField;
    private JTextField ttlField;
    private JTextField mcuAddressField;

    private JButton ttlUpButton;
    private JButton ttlDownButton;
    private JButton configButton;

    private JComboBox downBWSelector;
    private JComboBox upBWSelector;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new ConnectionModePanel());
        f.pack();
        f.setVisible(true);
    }

} // Class ConnectionModePanel
