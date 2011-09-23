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
 * MulticastPanel.java
 *
 * Created on 30 de marzo de 2004, 13:05
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import services.isabel.lib.Constants;

import java.awt.*;
import java.awt.event.*;
import java.util.List;

import xedl.lib.jaxb.CONNECTIONMODE;
import xedl.lib.jaxb.MulticastParameters;
import xedl.lib.jaxb.SITE;

//import isabel.lim.HelpBrowser;

/**
 * Panel de configuracion de multicast
 * @author  Fernando Escribano
 */
public class MulticastPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
	/**
     * String para ponerlo como root_node en el caso de que NO sea multicast gateway pero si se radie multicast
     */
    public static final String ROOT_NODE = "0.0.0.0";
    
    /**
     * String para ponerlo como root_node en el caso de que SI sea multicast gateway (y si se radie multicast obviamente)
     */
    public static final String GATEWAY_NODE = "%ROOT_NODE%";
    
    public MulticastPanel() {
        initComponents();
        
        groups = new MulticastGroupsPanel();
        groups.enableFields(true);
        
        setUseDefaultValues(true);
        enableThings();
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
    	CONNECTIONMODE conn = site.getCONNECTIONMODE();
    	if(conn==null) conn = new CONNECTIONMODE();
    	boolean mcast = conn.getMulticast()!=null;
    	setMulticastEnabled(mcast);
    	setConnectToUnicastPeer(conn.getMulticast()!=null && conn.getMcu()!=null);
    	if (mcast) {
            MulticastParameters mparams = site.getCONNECTIONMODE().getMulticast().getMulticastParameters();
            if (mparams.getMCASTAUDIOGROUP() == null) {
                setUseDefaultValues(true);
            }
            else {
                setUseDefaultValues(false);
                groups.setTTL(mparams.getMCASTTTL());
                groups.setAudioGroup((String)mparams.getMCASTAUDIOGROUP());
                groups.setAppSharingGroup((String)mparams.getMCASTAPPSHGROUP());
                groups.setVideoGroup((String)mparams.getMCASTVIDEOGROUP());
                groups.setVumeterGroup((String)mparams.getMCASTVUMETERGROUP());
                groups.setFTPGroup((String)mparams.getMCASTFTPGROUP());
                groups.setPointerGroup((String)mparams.getMCASTPOINTERGROUP());
            }
            //vemos si es el gateway (si multicastroot es igual a GATEWAY_NODE)
            if(mparams.getMCASTROOT()!=null && mparams.getMCASTROOT().equals(GATEWAY_NODE))
            {
            	setConnectToUnicastPeer(true);
            }
            else
            {
            	setConnectToUnicastPeer(false);
            }
        }        
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.deleteMulticastConnectionMode();
        //el nombre que se añade en MCU_ADDRESS lo he puesto en el fichero constants con
        //lo que hay ahora hecho no puede ser cadena vacía, da nullpointerException
        //así que de momento pongo localhost
        //site.deleteMCUAddress(Constants.MCU_ADDRESS_MULTICAST);

        if(getMulticastEnabled()) {
            MulticastParameters mparams = site.setConnectionModeMulticast();
            if (!getUseDefaultValues()) {
                mparams.setMCASTTTL(new Integer(groups.getTTL()).byteValue());
                mparams.setMCASTAPPSHGROUP(groups.getAppSharingGroup());
                mparams.setMCASTAUDIOGROUP(groups.getAudioGroup());
                mparams.setMCASTVUMETERGROUP(groups.getVumeterGroup());                    
                mparams.setMCASTFTPGROUP(groups.getFTPGroup());                    
                mparams.setMCASTPOINTERGROUP(groups.getPointerGroup());
                mparams.setMCASTVIDEOGROUP(groups.getVideoGroup());
            }

            if (getConnectToUnicastPeer())
            {
                mparams.setMCASTROOT(GATEWAY_NODE);
                if(site.getMCUAddress()==null)
                {
                	site.insertMCUAddress(Constants.MCU_ADDRESS_MULTICAST,true);
                }
            }
            else
            {
            	mparams.setMCASTROOT(ROOT_NODE);
            	site.deleteMCUAddress(Constants.MCU_ADDRESS_MULTICAST);
            }
                //site.insertMCUAddress(Constants.MCU_ADDRESS_MULTICAST,true);
        }        
        
        	
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setMulticastEnabled(false);
    }
    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL MULTICASTPANEL           *
     ***************************************************************************/
    
    public void setMulticastEnabled(boolean enabled) {
        enableMulticastCB.setSelected(enabled);
    }
    
    public boolean getMulticastEnabled() {
        return enableMulticastCB.isSelected();
    }
    
    public void setUseDefaultValues(boolean enabled) {
        useDefaultCB.setSelected(enabled);
    }
    
    public boolean getUseDefaultValues() {
        return useDefaultCB.isSelected();
    }
    
    public void setConnectToUnicastPeer(boolean enabled) {
        unicastPeerCB.setSelected(enabled);
    }
    
    public boolean getConnectToUnicastPeer() {
        return unicastPeerCB.isSelected();
    }
    
    
    /**************************************************************************
     **************************************************************************/
    
    //    /**
    //     * M�todo ejecutado cuando se pulsa el boton de ayuda.
    //     */
    //    private void helpButtonActionPerformed(ActionEvent evt) {
    //        if (helpBrowser == null)
    //            helpBrowser = new HelpBrowser(SITE_ID_HELP_HOME);
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
        

        // Etiqueta del icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(MULTICAST_GROUPS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para datos
        JPanel mainPane = new JPanel();
        mainPane.setLayout(new GridBagLayout());
        mainPane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(mainPane, BorderLayout.CENTER);
        
        
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.anchor = GridBagConstraints.CENTER;
        //gbc.weighty = 1.0;
        //gbc.weightx = 1.0;
        gbc.insets = new Insets(5, 10, 0, 5);
        
        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        mainPane.add(titlePane, gbc);
        
        // Etiqueta con el titulo
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(MULTICAST_TITLE);
        titlePane.add(titleLabel);
        
        
        enableMulticastCB = new JCheckBox();
        enableMulticastCB.setText("Radiate multicast");
        
        enableMulticastCB.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
            	//si el botón ha cambiado pongo que se ha editado el profile
            	if(enableMulticastCB.isSelected() && !enableMulticastCB_selected)
            	{
            		Options.profile_edited = true;
            		//actualizo el estado anterior a selected
            		enableMulticastCB_selected = true;
            	}
            	else if (!enableMulticastCB.isSelected() && enableMulticastCB_selected)
            	{
            		Options.profile_edited = true;
//            		actualizo el estado anterior a no selected
            		enableMulticastCB_selected = false;
            	}
                enableThings();
            }
        });

        gbc.gridx = 0;
        gbc.gridy = 1;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        mainPane.add(enableMulticastCB, gbc);
        
        gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 2;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.weighty = 1.0;
        gbc.weightx = 1.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(5, 10, 0, 5);
        
        JPanel configPane = new JPanel();
        configPane.setLayout(new GridBagLayout());
        configPane.setBorder(new TitledBorder("Multicast Configuration"));
        mainPane.add(configPane, gbc);
        
        gbc = new GridBagConstraints();
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 1.0;
        gbc.insets = new Insets(0, 5, 0, 5);
        
        useDefaultCB = new JCheckBox();
        useDefaultCB.setText("Use default values");
        useDefaultCB.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
            	if(useDefaultCB.isSelected() && !useDefaultCB_selected)
            	{
            		Options.profile_edited = true;
            		//actualizo el estado anterior a selected
            		useDefaultCB_selected = true;
            	}
            	else if (!useDefaultCB.isSelected() && useDefaultCB_selected)
            	{
            		Options.profile_edited = true;
//            		actualizo el estado anterior a no selected
            		useDefaultCB_selected = false;
            	}
                enableThings();
            }
        });
        configPane.add(useDefaultCB, gbc);
        
        configGroupsButton = new JButton();
        configGroupsButton.setText("Configure");
        configGroupsButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
            	Options.profile_edited = true;
                configButtonPressed();
            }
        });
        configPane.add(configGroupsButton, gbc);
        
        gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 3;
        gbc.weighty = 1.0;
        gbc.weightx = 1.0;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.insets = new Insets(0, 10, 5, 0);
        
        unicastPeerCB = new JCheckBox();
        unicastPeerCB.setText("Act as multicast gateway");        
        unicastPeerCB.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
            	if(unicastPeerCB.isSelected() && !unicastPeerCB_selected)
            	{
            		Options.profile_edited = true;
            		//actualizo el estado anterior a selected
            		unicastPeerCB_selected = true;
            	}
            	else if (!unicastPeerCB.isSelected() && unicastPeerCB_selected)
            	{
            		Options.profile_edited = true;
//            		actualizo el estado anterior a no selected
            		unicastPeerCB_selected = false;
            	}
            	Options.profile_edited = true;
            }
        });
        mainPane.add(unicastPeerCB, gbc);
    }
    
    /**
     * Muestra una ventana con informacion sobre isabel cuando se pulsa el menu
     * de about.
     */
    private void configButtonPressed() {
        // Guargo los valores antiguos
        int ttl = groups.getTTL();
        String audio = groups.getAudioGroup();
        String apps = groups.getAppSharingGroup();
        String video = groups.getVideoGroup();
        String vumeter = groups.getVumeterGroup();
        String ftp = groups.getFTPGroup();
        String pointer = groups.getPointerGroup();
        int i = JOptionPane.showConfirmDialog(this,
        groups,
        "Multicast Groups",
        JOptionPane.OK_CANCEL_OPTION,
        JOptionPane.INFORMATION_MESSAGE,
        new ImageIcon());
        
        // Si la opcion elegida es cancelar restauro los valores antiguos
        if (i == JOptionPane.CANCEL_OPTION) {
            groups.setTTL(ttl);
            groups.setAudioGroup(audio);
            groups.setAppSharingGroup(apps);
            groups.setVideoGroup(video);
            groups.setVumeterGroup(vumeter);
            groups.setFTPGroup(ftp);
            groups.setPointerGroup(pointer);
        }
    }
    
    /**
     * Habilita los botones segun esten elegibdas las checkboxes.
     */
    private void enableThings() {
        useDefaultCB.setEnabled(enableMulticastCB.isSelected());
        configGroupsButton.setEnabled(enableMulticastCB.isSelected() && !useDefaultCB.isSelected());
        boolean selected = enableMulticastCB.isSelected();
        unicastPeerCB.setEnabled(selected);
    }
    
    // Componentes visuales que contiene datos
    private JCheckBox enableMulticastCB;
    private boolean enableMulticastCB_selected = false;
    private JCheckBox useDefaultCB;
    private boolean useDefaultCB_selected = false;
    private JSpinner ttlSelector;
    private JButton configGroupsButton;
    private JCheckBox unicastPeerCB;
    private boolean unicastPeerCB_selected = false;
    
    /**
     * Panel con los datos de los grupos.
     */
    public MulticastGroupsPanel groups;
    
    // Ventana para mostrar la ayuda
    //    private JFrame helpBrowser;
    
    public static void main(String[] args) throws Exception{
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        MulticastPanel mp = new MulticastPanel();
        f.getContentPane().add(mp);
        f.pack();
        f.setVisible(true);
    }
    
}// Class MulticastPanel
