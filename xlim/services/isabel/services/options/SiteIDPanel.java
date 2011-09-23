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
 * SiteIDPanel.java
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
 * Esta clase implementa un panel para introducir datos sobre la identidad
 * del usuario. Tiene un titulo, un icono, un panel para las etiquetas y
 * otro para los campos.
 */
public class SiteIDPanel extends JPanel implements OptionsStrings, ValidablePanel {

    private static final int GRID_GAP = 11;
    
    /** 
     * Crea un objeto SiteIdPanel.
     */
    public SiteIDPanel() {
        initComponents();
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * @return True si los datos son correctos y false si no lo son.
     */
    public boolean validateData() {
        if (getUserID().length() > 10) {
            JOptionPane.showMessageDialog(this, "Bad user ID", "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        return true;
    }

    /***************************************************************************
     *               METODOS SET Y GET DE DATOS DEL PANEL SITEID               *
     ***************************************************************************/
    
    public String getUserID() {
        return userIDField.getText();
    }
    
    public void setUserID(String userID) {
        userIDField.setText(userID);
    }
    
    public String getEmail() {
        return emailField.getText();
    }
    
    public void setEmail(String email) {
        emailField.setText(email);
    }
    
    public String getSiteAddress() {
        return addressField.getText();
    }
    
    public void setSiteAddress(String address) {
        addressField.setText(address);
    }
    
    public String getURL() {
        return urlField.getText();
    }
    
    public void setURL(String url) {
        urlField.setText(url);
    }
    
    public String getSiteID() {
        return siteIDField.getText();
    }
    
    public void setSiteID(String siteID) {
        siteIDField.setText(siteID);
    }
    
    public String getAccessControl() {
        return accessControlField.getText();
    }
    
    public void setAccessControl(String accessControl) {
        accessControlField.setText(accessControl);
    }
    
    public String getPublicName() {
        return publicNameField.getText();
    }
    
    public void setPublicName(String publicName) {
        publicNameField.setText(publicName);
    }
        
    public String getRole() {
        return (String)roleSelector.getSelectedItem();
    }
    
    public void setRole(String role) {
        roleSelector.setSelectedItem(role);
    }

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

        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        add(titlePane, BorderLayout.NORTH);
        
        // Etiqueta con el titulo
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(SITE_ID_TITLE);
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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(SITE_ID_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);

        // Panel donde se ponen las etiquetas con los nombres de los campos
        JPanel labelsPane = new JPanel();
        labelsPane.setLayout(new GridLayout(0, 1, 0, GRID_GAP));
        labelsPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        add(labelsPane, BorderLayout.CENTER);

        // Panel con los campos para meter datos
        JPanel fieldsPane = new JPanel();
        fieldsPane.setLayout(new GridLayout(0, 1, 0, GRID_GAP));
        fieldsPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        add(fieldsPane, BorderLayout.EAST);

        // Etiqueta y campo de Site ID
        JLabel siteLabel = new JLabel();
        siteLabel.setText("Site ID");
        siteLabel.setToolTipText(SITE_ID_HELP);
        siteLabel.setBorder(new EtchedBorder());
        labelsPane.add(siteLabel);

        siteIDField = new JTextField();
        siteIDField.setColumns(10);
        fieldsPane.add(siteIDField);

        // Etiqueta y campo de Access control
        JLabel accesControlLabel = new JLabel();
        accesControlLabel.setText("Access Control");
        accesControlLabel.setToolTipText(ACCESS_CONTROL_HELP);
        accesControlLabel.setBorder(new EtchedBorder());
        labelsPane.add(accesControlLabel);

        accessControlField = new JPasswordField();
        accessControlField.setColumns(10);
        fieldsPane.add(accessControlField);


        // Etiqueta y campo de Public Name
        JLabel publicNameLabel = new JLabel();
        publicNameLabel.setText("Public name");
        publicNameLabel.setToolTipText(PUBLIC_NAME_HELP);
        publicNameLabel.setBorder(new EtchedBorder());
        labelsPane.add(publicNameLabel);

        publicNameField = new JTextField();
        publicNameField.setColumns(10);
        fieldsPane.add(publicNameField);

        // Etiqueta y campo de User ID
        JLabel userIDLabel = new JLabel();
        userIDLabel.setText("User ID");
        userIDLabel.setToolTipText(USER_ID_HELP);
        userIDLabel.setBorder(new EtchedBorder());
        labelsPane.add(userIDLabel);

        userIDField = new JTextField();
        userIDField.setColumns(10);
        fieldsPane.add(userIDField);            

        // Etiqueta y campo de e-mail
        JLabel emailLabel = new JLabel();
        emailLabel.setText("E-mail");
        emailLabel.setToolTipText(E_MAIL_HELP);
        emailLabel.setBorder(new EtchedBorder());
        labelsPane.add(emailLabel);

        emailField = new JTextField();
        emailField.setColumns(10);
        fieldsPane.add(emailField);

        // Etiqueta y campo de URL
        JLabel urlLabel = new JLabel();
        urlLabel.setText("URL");
        urlLabel.setToolTipText(URL_HELP);
        urlLabel.setBorder(new EtchedBorder());
        labelsPane.add(urlLabel);

        urlField = new JTextField();
        urlField.setColumns(10);
        fieldsPane.add(urlField);

        // Etiqueta y campo de Site Address
        JLabel addressLabel = new JLabel();
        addressLabel.setText("Site Address");
        addressLabel.setToolTipText(SITE_ADDRESS_HELP);
        addressLabel.setBorder(new EtchedBorder());
        labelsPane.add(addressLabel);

        addressField = new JTextField();
        addressField.setColumns(10);
        fieldsPane.add(addressField);            

        // Etiqueta y selector de Site Role
        JLabel roleLabel = new JLabel();
        roleLabel.setText("Site Role");
        roleLabel.setToolTipText(SITE_ROLE_HELP);
        roleLabel.setBorder(new EtchedBorder());
        labelsPane.add(roleLabel);

        roleSelector = new JComboBox();
        roleSelector.setModel(new DefaultComboBoxModel(ROLE_NAMES));
        fieldsPane.add(roleSelector);
    }
    
    // Componentes visuales que guardan datos
    private JTextField userIDField;
    private JTextField emailField;
    private JTextField addressField;
    private JTextField urlField;
    private JTextField siteIDField;
    private JTextField accessControlField;
    private JTextField publicNameField;        
    private JComboBox roleSelector;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;

} // Class SiteIDPanel
