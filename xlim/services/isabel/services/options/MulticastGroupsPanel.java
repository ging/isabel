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
 * MulticastGroupsPanel.java
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
 * Esta clase implementa el panel que permite configurar las direcciones
 * de los grupos multicast utilizados cuando se emplea el modo de
 * conexion multicast.
 */
public class MulticastGroupsPanel extends JPanel implements OptionsStrings, ValidablePanel {

    private static final int GRID_GAP = 23;
    /**
     * Crea un objeto MulticastGroupsPanel.
     */
    public MulticastGroupsPanel() {
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
     * Este m�todo activa o desactiva los campos de este panel.
     */
    public void enableFields(boolean enable) {
        ttlSelector.setEnabled(enable);
        audioGroupField.setEnabled(enable);
        videoGroupField.setEnabled(enable);
        ftpGroupField.setEnabled(enable);
        pointerGroupField.setEnabled(enable);
        vumeterGroupField.setEnabled(enable);
        appSharingGroupField.setEnabled(enable);
    }

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL MULTICASTGROUPS          *
     ***************************************************************************/
    
    public int getTTL() {
        return ((Integer)ttlSelector.getValue()).intValue();
    }
    
    public void setTTL(int ttl) {
        ttlSelector.setValue(new Integer(ttl));
    }
    
    public String getAudioGroup() {
        return  audioGroupField.getText();
    }
    
    public void setAudioGroup(String group) {
         audioGroupField.setText(group);
    }
    
    public String getVideoGroup() {
        return  videoGroupField.getText();
    }
    
    public void setVideoGroup(String group) {
         videoGroupField.setText(group);
    }
    public String getFTPGroup() {
        return  ftpGroupField.getText();
    }
    
    public void setFTPGroup(String group) {
         ftpGroupField.setText(group);
    }
    public String getVumeterGroup() {
        return  vumeterGroupField.getText();
    }
    
    public void setVumeterGroup(String group) {
         vumeterGroupField.setText(group);
    }
    public String getPointerGroup() {
        return  pointerGroupField.getText();
    }
    
    public void setPointerGroup(String group) {
         pointerGroupField.setText(group);
    }
    public String getAppSharingGroup() {
        return  appSharingGroupField.getText();
    }
    
    public void setAppSharingGroup(String group) {
         appSharingGroupField.setText(group);
    }
    
//    /**
//     * M�todo ejecutado cuando se pulsa el boton de ayuda.
//     */
//    private void helpButtonActionPerformed(ActionEvent evt) {
//        if (helpBrowser == null)
//            helpBrowser = new HelpBrowser(MULTICAST_HELP_HOME);
//        helpBrowser.show();
//    }
    
    /**
     * Este m�todo crea todos los componentes que forman el panel.
     */
    private void initComponents() {

        // Configuro el panel principal
        setLayout(new BorderLayout());
        setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                     new EmptyBorder(new Insets(10,10,10,10))));

        
        
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


        // Etiqueta del icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(MULTICAST_GROUPS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        mainPane = new JPanel();
        mainPane.setLayout(new GridBagLayout());
        mainPane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                              new EtchedBorder()));
        //mainPane.setBackground(Color.red);
        add(mainPane, BorderLayout.CENTER);
        
        // Panel para la etiqueta del titulo y el boton de ayuda
        GridBagConstraints gbc = new GridBagConstraints();
        //gbc.gridy = 0;
        gbc.gridx = 0;
        //gbc.weighty = 0.0;
        gbc.weightx = 1.0;
        gbc.gridwidth = 2;
        
        gbc.fill = gbc.HORIZONTAL;
        //gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,30,0);
        //Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new GridBagLayout());
        //titlePane.setBackground(Color.BLUE);
        //mainPane.setBackground(Color.YELLOW);
        
        mainPane.add(titlePane,gbc);
        
        gbc = new GridBagConstraints();
        gbc.anchor = gbc.EAST;
        gbc.fill = gbc.HORIZONTAL;

        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(MULTICAST_GROUPS_TITLE);
        titlePane.add(titleLabel,gbc);
        
        /*
        // Etiqueta con el titulo           
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(MULTICAST_GROUPS_TITLE);
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titlePane.add(titleLabel);
        */

        // Separacion entre el titulo y el boton de ayuda
        titlePane.add(Box.createHorizontalGlue());

        /*
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(MEDIA_CONTROLS_TITLE);
        titlePane.add(titleLabel,gbc);
        */
        
        gbc = new GridBagConstraints();
        gbc.gridy = 1;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel ttlLabel = new JLabel();
        ttlLabel.setFont(new Font("Dialog", Font.BOLD, 12));
        ttlLabel.setText(" TTL ");
        ttlLabel.setToolTipText(MULTICAST_TTL_HELP);
        //defaultDisplayLabel.setBackground(Color.yellow);
        //defaultDisplayLabel.setBorder(new EtchedBorder());
        //publicNameLabel.setBorder(new CompoundBorder(new EtchedBorder(),
        //                                             new EmptyBorder(1,20,1,20)));
        mainPane.add(ttlLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        
        ttlSelector = new JSpinner();
        ttlSelector.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
                Options.profile_edited=true;
            }
        });
        ttlSelector.setModel(new SpinnerNumberModel(4,1,20,1));
        ttlSelector.setEnabled(false);
        mainPane.add(ttlSelector,gbc);
        
        /*
        // Etiqueta del ttl
        JLabel ttlLabel = new JLabel();
        ttlLabel.setText("TTL");
        ttlLabel.setToolTipText(MULTICAST_TTL_HELP);
        ttlLabel.setBorder(new EtchedBorder());
        mainPane.add(ttlLabel);
        */
        
        // Etiqueta del grupo de audio
        gbc.gridy = 2;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel audioGroupLabel = new JLabel();
        audioGroupLabel.setText(" Audio Group ");
        audioGroupLabel.setToolTipText(AUDIO_GROUP_HELP);
        //audioGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(audioGroupLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        audioGroupField = new JTextField();
        audioGroupField.setColumns(10);
        audioGroupField.setText("239.255.6.1");
        audioGroupField.setHorizontalAlignment(JTextField.TRAILING);
        audioGroupField.setEnabled(false);
        audioGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(audioGroupField,gbc);
        /*
        appSharingDisplayField = new JTextField();
        appSharingDisplayField.setColumns(15);
        appSharingDisplayField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(appSharingDisplayField, gbc);
        */
        
        
        // Etiqueta del grupo de video
        gbc.gridy = 3;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel videoGroupLabel = new JLabel();
        videoGroupLabel.setText(" Video Group ");
        videoGroupLabel.setToolTipText(VIDEO_GROUP_HELP);
        //videoGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(videoGroupLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        videoGroupField = new JTextField();
        videoGroupField.setColumns(10);
        videoGroupField.setText("239.255.6.1");
        videoGroupField.setHorizontalAlignment(JTextField.TRAILING);
        videoGroupField.setEnabled(false);
        videoGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(videoGroupField,gbc);


        // Etiqueta del grupo de ftp 
        
        gbc.gridy = 4;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel ftpGroupLabel = new JLabel();
        ftpGroupLabel.setText(" FTP Group ");
        ftpGroupLabel.setToolTipText(FTP_GROUP_HELP);
        //ftpGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(ftpGroupLabel,gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        ftpGroupField = new JTextField();
        ftpGroupField.setColumns(10);
        ftpGroupField.setText("239.255.6.1");
        ftpGroupField.setHorizontalAlignment(JTextField.TRAILING);
        ftpGroupField.setEnabled(false);
        ftpGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(ftpGroupField,gbc);

        // Etiqueta del grupo del puntero      
        gbc.gridy = 5;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel pointerGroupLabel = new JLabel();
        pointerGroupLabel.setText(" Pointer Group ");
        pointerGroupLabel.setToolTipText(POINTER_GROUP_HELP);
        //pointerGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(pointerGroupLabel,gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        pointerGroupField = new JTextField();
        pointerGroupField.setColumns(10);
        pointerGroupField.setText("239.255.6.1");
        pointerGroupField.setHorizontalAlignment(JTextField.TRAILING);
        pointerGroupField.setEnabled(false);
        pointerGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(pointerGroupField,gbc);
        

        // Etiqueta del grupo de control vumetros
        gbc.gridy = 6;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel vumeterGroupLabel = new JLabel();
        vumeterGroupLabel.setText(" Vumeter Group ");
        vumeterGroupLabel.setToolTipText(VUMETER_GROUP_HELP);
        //vumeterGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(vumeterGroupLabel,gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        vumeterGroupField = new JTextField();
        vumeterGroupField.setColumns(10);
        vumeterGroupField.setText("239.255.6.1");
        vumeterGroupField.setHorizontalAlignment(JTextField.TRAILING);
        vumeterGroupField.setEnabled(false);
        vumeterGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(vumeterGroupField,gbc);

        // Etiqueta del grupo de comparticion de aplicaiones
        gbc.gridy = 7;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel appSharingGroupLabel = new JLabel();
        appSharingGroupLabel.setText("App Sharing Group  ");
        appSharingGroupLabel.setToolTipText(APP_SHARING_GROUP_HELP);
        //appSharingGroupLabel.setBorder(new EtchedBorder());
        mainPane.add(appSharingGroupLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        appSharingGroupField = new JTextField();
        appSharingGroupField.setColumns(10);
        appSharingGroupField.setText("239.255.6.1");
        appSharingGroupField.setHorizontalAlignment(JTextField.TRAILING);
        appSharingGroupField.setEnabled(false);
        appSharingGroupField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(appSharingGroupField,gbc);

        
    }


    // Componentes visuales que contiene datos
    private JSpinner ttlSelector;
    private JTextField ftpGroupField;
    private JTextField vumeterGroupField;
    private JTextField videoGroupField;
    private JTextField appSharingGroupField;
    private JTextField pointerGroupField;
    private JTextField audioGroupField;
    private JPanel mainPane;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new MulticastGroupsPanel());
        f.pack();
        f.setVisible(true);
            
    }

}// Class MulticastGroupsPanel
