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
 * MediaControlsPanel.java
 *
 * Created on 12 de septiembre de 2003, 17:29
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import xedl.lib.jaxb.SITE;

import java.awt.*;
import java.awt.event.*;


//import isabel.lim.HelpBrowser;

/**
 * Esta clase implementa el panel donde se introducen los datos sobre
 * los displays utilizados por los distintos paneles de control
 * de la aplicacion ISABEL.
 */
public class MediaControlsPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    private static final int GRID_GAP = 1;
    
    /**
     * Crea un objeto MulticastGroupsPanel.
     */
    public MediaControlsPanel() {
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
     * Carga los datos del sitio que se pasa como par�metro en el panel.
     * @param site Objeto xedl.Site que describe el sitio cuyos datos quieren cargarse.
     */
    public void loadData(SITE site) {
        setAppSharingDisplay(site.getAppSharingCtrl());
        setAudioDisplay(site.getAudioCtrl());
        setDataDisplay(site.getDataCtrl());
        setDefaultDisplay(site.getDefaultCtrl());
        setVideoDisplay(site.getVideoCtrl());        
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.setDefaultCtrl(getDefaultDisplay());
        site.setAppSharingCtrl(getAppSharingDisplay());
        site.setAudioCtrl(getAudioDisplay());
        site.setDataCtrl(getDataDisplay());
        site.setVideoCtrl(getVideoDisplay());        
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setAppSharingDisplay("");
        setAudioDisplay("");
        setDataDisplay("");
        setDefaultDisplay("");
        setVideoDisplay("");
    }
    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL MEDIACONTROLS            *
     ***************************************************************************/
    
    public String getAudioDisplay() {
        return  audioDisplayField.getText();
    }
    
    public void setAudioDisplay(String display) {
        audioDisplayField.setText(display);
    }
    
    public String getVideoDisplay() {
        return  videoDisplayField.getText();
    }
    
    public void setVideoDisplay(String display) {
        videoDisplayField.setText(display);
    }
    public String getDefaultDisplay() {
        return  defaultDisplayField.getText();
    }
    
    public void setDefaultDisplay(String display) {
        defaultDisplayField.setText(display);
    }
    public String getAppSharingDisplay() {
        return  appSharingDisplayField.getText();
    }
    
    public void setAppSharingDisplay(String display) {
        appSharingDisplayField.setText(display);
    }
    public String getDataDisplay() {
        return  dataDisplayField.getText();
    }
    
    public void setDataDisplay(String display) {
        dataDisplayField.setText(display);
    }
    
    //    /**
    //     * M�todo ejecutado cuando se pulsa el boton de ayuda.
    //     */
    //    private void helpButtonActionPerformed(ActionEvent evt) {
    //        if (helpBrowser == null)
    //            helpBrowser = new HelpBrowser(MEDIA_CONTROLS_HELP_HOME);
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
         
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
       
        // Etiqueta del icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(MEDIA_CONTROLS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Ventana general
        mainPane = new JPanel();
        mainPane.setLayout(new GridBagLayout());
        mainPane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                              new EtchedBorder()));
        //mainPane.setBackground(Color.red);
        add(mainPane, BorderLayout.CENTER);
        
        // Panel del título de la ventana
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
       
        // Etiqueta con el titulo     
        
        gbc = new GridBagConstraints();
        gbc.anchor = gbc.EAST;
        gbc.fill = gbc.HORIZONTAL;

        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(MEDIA_CONTROLS_TITLE);
        titlePane.add(titleLabel,gbc);
       
        // Etiqueta y campo: Default control display:
        gbc = new GridBagConstraints();
        gbc.gridy = 1;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel defaultDisplayLabel = new JLabel();
        defaultDisplayLabel.setFont(new Font("Dialog", Font.BOLD, 12));
        defaultDisplayLabel.setText(" Default control display  ");
        defaultDisplayLabel.setToolTipText(DEFAULT_CONTROL_DISPLAY_HELP);
        //defaultDisplayLabel.setBackground(Color.yellow);
        //defaultDisplayLabel.setBorder(new EtchedBorder());
        //publicNameLabel.setBorder(new CompoundBorder(new EtchedBorder(),
        //                                             new EmptyBorder(1,20,1,20)));
        mainPane.add(defaultDisplayLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        
        defaultDisplayField = new JTextField();
        defaultDisplayField.setColumns(15);
        defaultDisplayField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(defaultDisplayField, gbc);
        
        // Etiqueta y campo: App sharing control display
        gbc.gridy = 2;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel appSharingDisplayLabel = new JLabel();
        appSharingDisplayLabel.setText(" App sharing control display ");
        appSharingDisplayLabel.setToolTipText(APP_SHARING_CONTROL_DISPLAY_HELP);
        //appSharingDisplayLabel.setBackground(Color.orange);
        //appSharingDisplayLabel.setBorder(new EtchedBorder());
        mainPane.add(appSharingDisplayLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
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
        
        // Etiqueta y campo: audio control display
        gbc.gridy = 3;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel audioDisplayLabel = new JLabel();
        audioDisplayLabel.setText(" Audio control display");
        audioDisplayLabel.setToolTipText(AUDIO_CONTROL_DISPLAY_HELP);
        // audioDisplayLabel.setBackground(Color.green);
        // audioDisplayLabel.setBorder(new EtchedBorder());
        mainPane.add(audioDisplayLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        audioDisplayField = new JTextField();
        audioDisplayField.setColumns(15);
        audioDisplayField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(audioDisplayField, gbc);
        
        // Etiqueta y campo: data control display
        gbc.gridy = 4;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel dataDisplayLabel = new JLabel();
        dataDisplayLabel.setText(" Data control display");
        dataDisplayLabel.setToolTipText(DATA_CONTROL_DISPLAY_HELP);
        //dataDisplayLabel.setBackground(Color.magenta);
        //dataDisplayLabel.setBorder(new EtchedBorder());
        mainPane.add(dataDisplayLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        dataDisplayField = new JTextField();
        dataDisplayField.setColumns(15);
        dataDisplayField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(dataDisplayField, gbc);
        
        // Etiqueta y campo: Video control display
        gbc.gridy = 5;
        gbc.gridx = 0;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,10,0);
        JLabel videoDisplayLabel = new JLabel();
        videoDisplayLabel.setText(" Video control display");
        videoDisplayLabel.setToolTipText(VIDEO_CONTROL_DISPLAY_HELP);
        //videoDisplayLabel.setBackground(Color.cyan);
        //videoDisplayLabel.setBorder(new EtchedBorder());
        mainPane.add(videoDisplayLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        videoDisplayField = new JTextField();
        videoDisplayField.setColumns(15);
        videoDisplayField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        mainPane.add(videoDisplayField, gbc);
                
        
        
    }
    
    // Componentes visuales que contiene datos
    private JTextField defaultDisplayField;
    private JTextField audioDisplayField;
    private JTextField appSharingDisplayField;
    private JTextField dataDisplayField;
    private JTextField videoDisplayField;
    private JPanel mainPane;
    
    // Ventana para mostrar la ayuda
    //    private JFrame helpBrowser;
    
}// Class MediaControlsPanel
