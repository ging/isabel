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
 * RolePanel.java
 *
 * Created on 6 de octubre de 2003, 11:12
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
 * Esta clase implementa el panel que permite configurar el nombre
 * p�blico del usuario.
 */
public class RolePanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    // POSIBLES ROLES DEL USUARIO.

    /**
     * Crea un objeto RolePanel.
     */
    public RolePanel() {
        initComponents();
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * En este panel siempre es correcto.
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
        setRole(toPublicRole(site.getSITEROLE()));        
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.setSITEROLE(toPrivateRole(getRole()));        
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setRole(INTERACTIVE_ROLE);
    }
    

    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL DE ROLE                  *
     ***************************************************************************/

    /**
     * Obtiene el role elejido por el usuario.
     * @return String con el valor del campo.
     */
    public String getRole() {
        return roleGroup.getSelection().getActionCommand();
    }
    
    /**
     * Establece el role del usuario.
     * @param role El valor a establecer.
     */
    public void setRole(String role) {
        if (role.equalsIgnoreCase(INTERACTIVE_ROLE))
            interactiveRole.setSelected(true);
        /*
        else if (role.equalsIgnoreCase(ANTENNA_ROLE))
            antennaRole.setSelected(true);
        else if (role.equalsIgnoreCase(RECORDER_ROLE))
            recorderRole.setSelected(true);
        */
        else if (role.equalsIgnoreCase(FLOWSERVER_ROLE))
            flowserverRole.setSelected(true);        
        else if (role.equalsIgnoreCase(GATEWAY_ROLE))
            gatewayRole.setSelected(true);        
        else if (role.equalsIgnoreCase(FLASH_GATEWAY_ROLE))
            flashgatewayRole.setSelected(true);        

        /*
        else if (role.equalsIgnoreCase(MEDIASERVER_ROLE))
            mediaserverRole.setSelected(true);
        */
        else
        {
        	interactiveRole.setSelected(true);
        	Options.profile_edited = true;
        }
        
        roleSelected(new ActionEvent(this, 0, role));
    }

    /**************************************************************************
     **************************************************************************/
    
//    /**
//     * M�todo ejecutado cuando se pulsa el boton de ayuda.
//     */
//    private void helpButtonActionPerformed(ActionEvent evt) {
//        if (helpBrowser == null)
//            helpBrowser = new HelpBrowser(ROLE_HELP_HOME);
//        helpBrowser.show();
//    }
    
    /**
     * Devuelve el String de rol que se muestra al usuario.
     * @param role Role tal y como ha de escribirse en el xedl.
     * @return Role tal y como se le muestra al usuario.
     */
    private String toPublicRole(String role) {
        for (int i = 0; i < ROLE_XEDL_NAMES.length; i++)
            if (ROLE_XEDL_NAMES[i].equals(role)) return ROLE_NAMES[i];
        
        // nunca deberia llegar aqui
        return null;
    }
    
    /**
     * Devuelve el String de rol que se escribe en el xedl.
     * @param role Role tal y como se muestra al usuario
     * @return Role tal y como se escribe en el xedl.
     */
    private String toPrivateRole(String role) {
        for (int i = 0; i < ROLE_XEDL_NAMES.length; i++)
            if (ROLE_NAMES[i].equals(role)) return ROLE_XEDL_NAMES[i];
        
        // nunca deberia llegar aqui
        return null;        
    }
    
    /**
     * Este m�todo crea todos los componentes que forman el panel.
     */
    private void initComponents() {

        // Configuro el panel principal
        setLayout(new BorderLayout());
        setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                     new EmptyBorder(new Insets(10,10,10,10))));


        // Etiqueta del icono
        iconLabel = new JLabel();
        //iconLabel.setIcon(new ImageIcon(getClass().getResource(ROLE_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para poner los botones de rol.
        JPanel rolePane = new JPanel();
        rolePane.setLayout(new GridBagLayout());
        rolePane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                              new EtchedBorder()));
        //rolePane.setBackground(Color.BLUE);
        add(rolePane, BorderLayout.CENTER);
        
        
        GridBagConstraints gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = GridBagConstraints.NORTH;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new Insets(15, 10, 10, 5);
        
        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        rolePane.add(titlePane, gridBagConstraints);        
        
        
        // Etiqueta con el titulo           
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(ROLE_TITLE);
        titlePane.add(titleLabel, gridBagConstraints);
        
        // Grupo de botones para que solo sea posible seleccionar uno
        roleGroup = new ButtonGroup();
                     
        // Botones para seleccionar el rol 
        interactiveRole = new JRadioButton();
        interactiveRole.setText(INTERACTIVE_ROLE);
        interactiveRole.setToolTipText(INTERACTIVE_ROLE_HELP);
        interactiveRole.setActionCommand(INTERACTIVE_ROLE);
        interactiveRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });


        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.fill = gridBagConstraints.BOTH;
        gridBagConstraints.weighty = 1;
        gridBagConstraints.insets =  new Insets(10,0,10,0);
        
        //gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(interactiveRole);
        rolePane.add(interactiveRole, gridBagConstraints);

        
        /*
        antennaRole = new JCheckBox();
        antennaRole.setText(ANTENNA_ROLE);
        antennaRole.setToolTipText(ANTENNA_ROLE_HELP);
        antennaRole.setActionCommand(ANTENNA_ROLE);
        antennaRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(antennaRole);
        rolePane.add(antennaRole, gridBagConstraints);
        */

        
        flowserverRole = new JRadioButton();
        flowserverRole.setText(FLOWSERVER_ROLE);
        flowserverRole.setToolTipText(FLOWSERVER_ROLE_HELP);
        flowserverRole.setActionCommand(FLOWSERVER_ROLE);
        flowserverRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });
        
        
        //flowserverRole.setEnabled(false);
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        //gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(flowserverRole);
        rolePane.add(flowserverRole, gridBagConstraints);

        
        gatewayRole = new JRadioButton();
        gatewayRole.setText(GATEWAY_ROLE);
        gatewayRole.setToolTipText(GATEWAY_ROLE_HELP);
        gatewayRole.setActionCommand(GATEWAY_ROLE);
        gatewayRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });        
        //gatewayRole.setEnabled(false);
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = gridBagConstraints.BOTH;
        //gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(gatewayRole);
        rolePane.add(gatewayRole, gridBagConstraints);

        flashgatewayRole = new JRadioButton();
        flashgatewayRole.setText(FLASH_GATEWAY_ROLE);
        flashgatewayRole.setToolTipText(GATEWAY_ROLE_HELP);
        flashgatewayRole.setActionCommand(FLASH_GATEWAY_ROLE);
        flashgatewayRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });        
        //gatewayRole.setEnabled(false);
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = gridBagConstraints.BOTH;
        //gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(flashgatewayRole);
        rolePane.add(flashgatewayRole, gridBagConstraints);
        
        
        
        /*
        mediaserverRole = new JRadioButton();
        mediaserverRole.setText(MEDIASERVER_ROLE);
        mediaserverRole.setToolTipText(MEDIASERVER_ROLE_HELP);
        mediaserverRole.setActionCommand(MEDIASERVER_ROLE);
        mediaserverRole.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	Options.profile_edited = true;
                roleSelected(e);
            }
        });      
         
        //gatewayRole.setEnabled(false);
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        //gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        roleGroup.add(mediaserverRole);
        rolePane.add(mediaserverRole, gridBagConstraints);
        */
        interactiveRole.setSelected(true);
        roleSelected(new ActionEvent(this, 0, INTERACTIVE_ROLE));
    }
    
    private void roleSelected(ActionEvent e) {
        String icon = ROLE_ICON_PATH + e.getActionCommand().toLowerCase().replace(' ', '_') + ROLE_ICON_EXT;
        icon = icon.replace('(','_');
        icon = icon.replace(')','_');    
        iconLabel.setIcon(new ImageIcon(getClass().getResource(icon)));
    }
        
    // Componentes visuales que contiene datos
    private JRadioButton interactiveRole;
    //private JRadioButton antennaRole;
    //private JRadioButton recorderRole;
    private JRadioButton flowserverRole;
    private JRadioButton gatewayRole;
    private JRadioButton flashgatewayRole;
    //private JRadioButton mediaserverRole;
    
    private JLabel iconLabel;
    
    ButtonGroup roleGroup;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    
}// Class RolePanel
