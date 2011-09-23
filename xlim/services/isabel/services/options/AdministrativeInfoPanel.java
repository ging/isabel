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
 * AdministartiveInfoPanel.java
 *
 * Created on 23 de septiembre de 2003, 11:05
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.util.Iterator;
import java.util.List;

import xedl.lib.jaxb.PersonalInformation;
import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SiteIdentification;
import xedl.lib.jaxb.SiteIdentification.ADMINISTRATIVEINFORMATION;

//import isabel.lim.HelpBrowser;

/**
 * Esta clase implementa un panel para introducir datos sobre la identidad
 * del usuario. Tiene un titulo, un icono, un panel para las etiquetas y
 * otro para los campos.
 */
public class AdministrativeInfoPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    /**
     * Crea un objeto AdminInfoPanel.
     */
    public AdministrativeInfoPanel() {
        initComponents();
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * En este caso comprueba que los caracteres de los nombres son letras,
     * numeros o espacio, punto, guion, o subrayado y que los emails son formalmente
     * validos.
     * @return True si los datos son correctos y false si no lo son.
     */
    public boolean validateData() {
        // Compruebo los direferentes campos. O bien estan vacios o bien cumplen la
        // expresion regular adecuada.
        if ((!getInstitutionName().equals("")) && (!getInstitutionName().matches(INSTITUTION_NAME_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid institution name",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getAdmin1Name().equals("")) && (!getAdmin1Name().matches(NAME_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid administrative Contact 1 name",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getAdmin2Name().equals("")) && (!getAdmin2Name().matches(NAME_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid administrative Contact 2 name",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getAdmin1Mail().equals("")) && (!getAdmin1Mail().matches(EMAIL_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Administrative Contact 1 e-mail",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getAdmin2Mail().equals("")) && (!getAdmin2Mail().matches(EMAIL_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Administrative Contact 2 e-mail",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getTech1Name().equals("")) && (!getTech1Name().matches(NAME_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Technical Contact 1 name",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getTech2Name().equals("")) && (!getTech2Name().matches(NAME_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Technical Contact 2 name",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getTech1Mail().equals("")) && (!getTech1Mail().matches(EMAIL_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Technical Contact 1 e-mail",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        if ((!getTech2Mail().equals("")) && (!getTech2Mail().matches(EMAIL_REGEX))) {
            JOptionPane.showMessageDialog(this, "Inavlid Technical Contact 2 e-mail",
            "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        return true;
    }
    
    /**
     * Carga los datos del sitio que se pasa como par�metro en el panel.
     * @param site Objeto xedl.Site que describe el sitio cuyos datos quieren cargarse.
     */
    public void loadData(SITE site) {
        setInstitutionName(site.getInstitution());
        
        // Contactos administrativos
        Iterator admins = site.getAdminInfoEmails().iterator();
        Iterator names = site.getAdminInfoNames().iterator();
        
        //ENRIQUE, si no hay next hay que fijar el valor a "", que si no se queda el de antes
        if (admins.hasNext()) {
            String mail = (String)admins.next();
            String name = (String)names.next();
            setAdmin1Mail(mail);
            setAdmin1Name(name);
        }
        else {
        	setAdmin1Mail("");
            setAdmin1Name("");
        }
        
        if (admins.hasNext()) {
            String mail = (String)admins.next();
            String name = (String)names.next();
            setAdmin2Mail(mail);
            setAdmin2Name(name);
        }
        else {
        	setAdmin2Mail("");
            setAdmin2Name("");
        }
        
        // Contactos tecnicos
        Iterator techs = site.getTechInfoEmails().iterator();
        Iterator namestechs = site.getTechInfoNames().iterator();
        if (techs.hasNext()) {
            String mail = (String)techs.next();
            String name = (String)namestechs.next();
            setTech1Mail(mail);
            setTech1Name(name);
        }
        else {
        	setTech1Mail("");
        	setTech1Name("");
        }
        
        if (techs.hasNext()) {
            String mail = (String)techs.next();
            String name = (String)namestechs.next();
            setTech2Mail(mail);
            setTech2Name(name);
        }
        else {
        	setTech2Mail("");
        	setTech2Name("");
        }
        
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setInstitutionName("");
       
        // Contactos administrativos
        setAdmin1Mail("");
        setAdmin1Name("");
        setAdmin2Mail("");
        setAdmin2Name("");
        
        // Contactos tecnicos
        setTech1Mail("");
        setTech1Name("");
        
        setTech2Mail("");
        setTech2Name("");
    }

    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.setInstitution(getInstitutionName());
        
        // Borro los contactos y pongo los nuevos
        List<PersonalInformation> lista_tech = null;
        List<PersonalInformation> lista_admins = null;
        if(site.getSiteIdentification()!=null && site.getSiteIdentification().getADMINISTRATIVEINFORMATION()!=null)
        {
        	lista_tech = site.getSiteIdentification().getADMINISTRATIVEINFORMATION().getTECHNICALCONTACT();
        	lista_tech.clear();
        	lista_admins = site.getSiteIdentification().getADMINISTRATIVEINFORMATION().getADMINISTRATIVECONTACT();
        	lista_admins.clear();
        }
        else
        {
        	if(site.getSiteIdentification()==null)
        		site.setSiteIdentification(new SiteIdentification());
        	if(site.getSiteIdentification().getADMINISTRATIVEINFORMATION()==null)
        		site.getSiteIdentification().setADMINISTRATIVEINFORMATION(new ADMINISTRATIVEINFORMATION());
        	lista_tech = site.getSiteIdentification().getADMINISTRATIVEINFORMATION().getTECHNICALCONTACT();
        	lista_admins = site.getSiteIdentification().getADMINISTRATIVEINFORMATION().getADMINISTRATIVECONTACT();
        	
        }
                
        PersonalInformation pi = null;
        if (!getAdmin1Mail().equals("")) {
            pi = new PersonalInformation();
            pi.setEMAIL(getAdmin1Mail());
            pi.setNAME(getAdmin1Name());
            lista_admins.add(pi);
        }
        if (!getAdmin2Mail().equals("")) {
        	pi = new PersonalInformation();
            pi.setEMAIL(getAdmin2Mail());
            pi.setNAME(getAdmin2Name());
            lista_admins.add(pi);
        }
        if (!getTech1Mail().equals("")) {
        	pi = new PersonalInformation();
            pi.setEMAIL(getTech1Mail());
            pi.setNAME(getTech1Name());
            lista_tech.add(pi);
        }
        if (!getTech2Mail().equals("")) {
        	pi = new PersonalInformation();
            pi.setEMAIL(getTech2Mail());
            pi.setNAME(getTech2Name());
            lista_tech.add(pi);
        }
    }
    
    /***************************************************************************
     *               METODOS SET Y GET DE DATOS DEL PANEL SITEID               *
     ***************************************************************************/
    
    public String getInstitutionName() {
        return institutionNameField.getText();
    }
    
    public void setInstitutionName(String name) {
        institutionNameField.setText(name);
    }
    
    public String getTech1Name() {
        return tech1NameField.getText();
    }
    
    public void setTech1Name(String name) {
        tech1NameField.setText(name);
    }
    
    public String getTech2Name() {
        return tech2NameField.getText();
    }
    
    public void setTech2Name(String name) {
        tech2NameField.setText(name);
    }
    
    public String getTech1Mail() {
        return tech1MailField.getText();
    }
    
    public void setTech1Mail(String mail) {
        tech1MailField.setText(mail);
    }
    
    public String getTech2Mail() {
        return tech2MailField.getText();
    }
    
    public void setTech2Mail(String mail) {
        tech2MailField.setText(mail);
    }
    
    public String getAdmin1Name() {
        return admin1NameField.getText();
    }
    
    public void setAdmin1Name(String name) {
        admin1NameField.setText(name);
    }
    
    public String getAdmin2Name() {
        return admin2NameField.getText();
    }
    
    public void setAdmin2Name(String name) {
        admin2NameField.setText(name);
    }
    
    public String getAdmin1Mail() {
        return admin1MailField.getText();
    }
    
    public void setAdmin1Mail(String mail) {
        admin1MailField.setText(mail);
    }
    
    public String getAdmin2Mail() {
        return admin2MailField.getText();
    }
    
    public void setAdmin2Mail(String mail) {
        admin2MailField.setText(mail);
    }
    
    /***************************************************************************
     **************************************************************************/
    
    //    /**
    //     * M�todo ejecutado cuando se pulsa el boton de ayuda.
    //     */
    //    private void helpButtonActionPerformed(ActionEvent evt) {
    //        if (helpBrowser == null)
    //            helpBrowser = new HelpBrowser(ADMIN_HELP_HOME);
    //        helpBrowser.show();
    //    }
    //
    /**
     * Este m�todo crea todos los componentes que forman el panel.
     */
    private void initComponents() {
        
        // Configuro el panel principal
        setLayout(new BorderLayout());
        setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EmptyBorder(new Insets(10,10,10,10))));
        
               
        // Etiqueta con el icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(ADMIN_INFO_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Un panel que contiene otros 3 paneles. Uno para el nombre
        // de la institucion, otro para los contactos tecnicos
        // y otro para los contactos administrativos.
        JPanel mainPane = new JPanel();
        mainPane.setLayout(new BoxLayout(mainPane, BoxLayout.Y_AXIS));
        mainPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        add(mainPane, BorderLayout.CENTER);
        
        //Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        mainPane.add(titlePane);
        
        // Etiqueta con el titulo
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(ADMIN_INFO_TITLE);
        titlePane.add(titleLabel);
        
        // Panel con el nombre de la institucion. Tiene una etiqueta y un campo.
        JPanel instituionNamePane = new JPanel();
        //instituionNamePane.setBorder(new EtchedBorder());
        instituionNamePane.setLayout(new BoxLayout(instituionNamePane, BoxLayout.X_AXIS));
        mainPane.add(instituionNamePane);
        
        // Espacio entre el borde y la etiqueta
        instituionNamePane.add(Box.createHorizontalStrut(10));
        
        // La Etiqueta y el campo
        JLabel instituionNameLabel = new JLabel();
        instituionNameLabel.setText("Institution Name");
        instituionNameLabel.setFont(new Font("Dialog", Font.BOLD, 14));
        instituionNamePane.add(instituionNameLabel);
        
        // Espacio entre la etiqueta y el campo
        instituionNamePane.add(Box.createHorizontalStrut(10));
        
        institutionNameField = new JTextField();
        institutionNameField.setColumns(13);
        institutionNameField.setMaximumSize(new Dimension(1000,25));
        institutionNameField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
       
        instituionNamePane.add(institutionNameField);
        
        // Espacio entre el campo y el final
        instituionNamePane.add(Box.createHorizontalStrut(10));
        
        // Panel de los contactos administrativos
        JPanel adminPane = new JPanel();
        //adminPane.setBorder(new EtchedBorder());
        adminPane.setLayout(new BoxLayout(adminPane, BoxLayout.X_AXIS));
        mainPane.add(adminPane);
        
        // Panel del contacto administrativo 1
        admin1MailField = new JTextField();
        admin1NameField = new JTextField();        
        admin1MailField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        admin1NameField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        JPanel admin1Pane = createInfoPanel(admin1NameField, admin1MailField,
        "Administrative Contact 1");
        adminPane.add(admin1Pane);
        
        // Separacion entre los paneles
        //adminPane.add(Box.createHorizontalGlue());
        
        // Panel del contacto administrativo 2
        admin2MailField = new JTextField();
        admin2NameField = new JTextField();
        admin2MailField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        admin2NameField.addKeyListener(new KeyListener(){
			public void keyPressed(KeyEvent arg0) {				
			}
			public void keyReleased(KeyEvent arg0) {				
			}
			public void keyTyped(KeyEvent arg0) {
				Options.setProfile_edited(true);
			}        	
        });
        JPanel admin2Pane = createInfoPanel(admin2NameField, admin2MailField,
        "Administrative Contact 2");
        adminPane.add(admin2Pane);
        
        
        // Panel de los contactos tecnicos
        JPanel techPane = new JPanel();
        //techPane.setBorder(new EtchedBorder());
        techPane.setLayout(new BoxLayout(techPane, BoxLayout.X_AXIS));
        mainPane.add(techPane);
        
        // Panel del contacto tecnico 1
        tech1MailField = new JTextField();
        tech1NameField = new JTextField();
        	tech1MailField.addKeyListener(new KeyListener(){
    			public void keyPressed(KeyEvent arg0) {				
    			}
    			public void keyReleased(KeyEvent arg0) {				
    			}
    			public void keyTyped(KeyEvent arg0) {
    				Options.setProfile_edited(true);
    			}        	
            });
        	tech1NameField.addKeyListener(new KeyListener(){
    			public void keyPressed(KeyEvent arg0) {				
    			}
    			public void keyReleased(KeyEvent arg0) {				
    			}
    			public void keyTyped(KeyEvent arg0) {
    				Options.setProfile_edited(true);
    			}        	
            });
        JPanel tech1Pane = createInfoPanel(tech1NameField, tech1MailField,
        "Technical Contact 1");
        techPane.add(tech1Pane);
        
        // Separacion entre los paneles
        //techPane.add(Box.createHorizontalGlue());
        
        // Panel del contacto tecnico 2
        tech2MailField = new JTextField();
        tech2NameField = new JTextField();
        tech2MailField.addKeyListener(new KeyListener(){
    			public void keyPressed(KeyEvent arg0) {				
    			}
    			public void keyReleased(KeyEvent arg0) {				
    			}
    			public void keyTyped(KeyEvent arg0) {
    				Options.setProfile_edited(true);
    			}        	
            });
        tech2NameField.addKeyListener(new KeyListener(){
    			public void keyPressed(KeyEvent arg0) {				
    			}
    			public void keyReleased(KeyEvent arg0) {				
    			}
    			public void keyTyped(KeyEvent arg0) {
    				Options.setProfile_edited(true);
    			}        	
            });
        JPanel tech2Pane = createInfoPanel(tech2NameField, tech2MailField,
        "Technical Contact 2");
        techPane.add(tech2Pane);
    }
    
    /**
     * Crea un panel con los elementos para meter el nombre y mail de un
     * contacto.
     */
    private JPanel createInfoPanel(JTextField nameField, JTextField mailField, String title) {
        // Panel que se devuelve como resultado
        JPanel pane = new JPanel();
        pane.setBorder(new CompoundBorder(null, new TitledBorder(title)));
        pane.setLayout(new GridLayout(0,1));
        
        // Panel con el campo de nombre
        JPanel namePane = new JPanel();
        namePane.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
        namePane.setBorder(new TitledBorder("Name"));
        pane.add(namePane);
        
        nameField.setColumns(11);
        namePane.add(nameField);
        
        // Panel con el campo para el mail
        JPanel mailPane = new JPanel();
        mailPane.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
        mailPane.setBorder(new TitledBorder("E-Mail"));
        pane.add(mailPane);
        
        mailField.setColumns(11);
        mailPane.add(mailField);
        
        return pane;
    }
    
    // Componentes visuales que guardan datos
    private JTextField institutionNameField;
    private JTextField tech1NameField;
    private JTextField tech2NameField;
    private JTextField admin1NameField;
    private JTextField admin2NameField;
    private JTextField tech1MailField;
    private JTextField tech2MailField;
    private JTextField admin1MailField;
    private JTextField admin2MailField;
    
    // Ventana para mostrar la ayuda
    //    private JFrame helpBrowser;
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new AdministrativeInfoPanel());
        f.pack();
        f.setVisible(true);
    }
    
} // Class AdminInfoPanel
