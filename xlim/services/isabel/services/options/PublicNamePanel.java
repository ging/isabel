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
 * PublicNamePanel.java
 *
 * Created on 22 de septiembre de 2003, 17:32
 */

package services.isabel.services.options;
    
import services.isabel.lib.*;
import xedl.lib.jaxb.SITE;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;


//import isabel.lim.HelpBrowser;

/**
 * Esta clase implementa el panel que permite configurar el nombre
 * p�blico del usuario.
 */
public class PublicNamePanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {

    /**
     * Crea un objeto PublicNamePanel.
     */
    public PublicNamePanel() {
        initComponents();
        setNickname(Options.OPTIONS_SITE_ID);
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * En este caso comprueba que los caracteres del public name son letras,
     * numeros o espacio, punto, guion, o subrayado.
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
        setNickname(site.getID());
        if ((site.getPUBLICNAME() != null) && (site.getPUBLICNAME().length() > getNickname().length()))
            setPublicName(site.getPUBLICNAME().substring(getNickname().length() + 1));
        else
            setPublicName("");
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
//        setNickname("CHANGEME");
//        setPublicName("");
    }

    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.setPUBLICNAME(getNickname() + "." + getPublicName());
        site.setID(getNickname());
    }

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL PUBLICNAME               *
     ***************************************************************************/

    /**
     * Obtiene el contenido del campo Public Name del panel.
     * @return String con el valor del campo.
     */
    public String getPublicName() {
        return publicNameField.getText();
    }
    
    /**
     * Establece el campo public name del panel.
     * @param publicName El valor a establecer.
     */
    public void setPublicName(String publicName) {
        publicNameField.setText(publicName);
    }

    /**
     * Obtiene el contenido del campo NickName del panel.
     * @return String con el valor del campo.
     */
    public String getNickname() {
        return nickField.getText();
    }
    
    /**
     * Establece el campo nickname del panel.
     * @param nickname El valor a establecer.
     */
    public void setNickname(String nickname) {
        nickField.setText(nickname);
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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(SITE_ID_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para poner el campo y su etiqueta
        JPanel namePane = new JPanel();
        namePane.setLayout(new GridBagLayout());
        namePane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                              new EtchedBorder()));
        add(namePane, BorderLayout.CENTER);
        
        // Etiqueta y campo de Public Name
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridy = 0;
        gbc.gridx = 0;
        gbc.weighty = 1.0;
        gbc.weightx = 1.0;
        gbc.gridwidth = 2;
        
        gbc.fill = gbc.HORIZONTAL;
        //gbc.anchor = gbc.WEST;
        gbc.insets = new Insets(0,0,0,0);
        //Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new GridBagLayout());
        //titlePane.setBackground(Color.BLUE);
        //namePane.setBackground(Color.YELLOW);
          
        namePane.add(titlePane,gbc);
       
        // Etiqueta con el titulo    
        
        gbc = new GridBagConstraints();
        gbc.anchor = gbc.EAST;
        gbc.fill = gbc.HORIZONTAL;

        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(SITE_ID_TITLE);
        titlePane.add(titleLabel,gbc);
        
        //------------------ NICKNAME

        // Etiqueta y campo de nick
        gbc = new GridBagConstraints();
        gbc.gridy = 1;
        gbc.gridx = 0;
        gbc.weightx = gbc.EAST;
        gbc.anchor = gbc.EAST;
        gbc.insets = new Insets(0,0,0,0);
        JLabel nickLabel = new JLabel();
        nickLabel.setText("Nickname  ");
        nickLabel.setFont(new Font("Dialog", Font.BOLD, 14));
        nickLabel.setToolTipText(OptionsStrings.SITE_ID_HELP);
        //publicNameLabel.setBorder(new CompoundBorder(new EtchedBorder(),
        //                                             new EmptyBorder(1,20,1,20)));
        namePane.add(nickLabel, gbc);
      
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        nickField = new JTextField();
        nickField.setColumns(15);
        nickField.addKeyListener(new KeyAdapter(){
			public void keyTyped(KeyEvent evt) {
				if(!Servutilities.estapermitido_nick(evt.getKeyChar()))
				{
					//si no es un código permitido consumo el evento
					evt.consume();
				}				
				Options.setProfile_edited(true);
			}
        });        
        namePane.add(nickField, gbc);    
        
        
        //------------------ DIALOG
        
        gbc.gridy = 2;
        gbc.gridx = 0;
        gbc.weightx = gbc.EAST;
        gbc.anchor = gbc.EAST;
        gbc.insets = new Insets(0,0,0,0);
        JLabel publicNameLabel = new JLabel();
        publicNameLabel.setText("Location  ");
        publicNameLabel.setFont(new Font("Dialog", Font.BOLD, 14));
        publicNameLabel.setToolTipText(PUBLIC_NAME_HELP);
        namePane.add(publicNameLabel, gbc);
        
        gbc.gridx = 1;
        gbc.weightx = gbc.WEST;
        gbc.anchor = gbc.WEST;
        publicNameField = new JTextField();
        publicNameField.setColumns(15);
        publicNameField.addKeyListener(new KeyAdapter(){
        	public void keyTyped(KeyEvent evt) {
				if(!Servutilities.estapermitido_location(evt.getKeyChar()))
				{
					//si no es un código permitido consumo el evento
					evt.consume();
				}				
				Options.setProfile_edited(true);
			}
        });
        namePane.add(publicNameField, gbc);
        
        Agenda IsabelAgenda = null;
		try {
			IsabelAgenda = new Agenda (Constants.ISABEL_USER_DIR + Constants.FILE_SEPARATOR);
		} catch (IOException e) {
			e.printStackTrace();
		}   
        File optionsFile = IsabelAgenda.getOptionsFile();
        if (!optionsFile.exists()) {
        	gbc.gridx = 1;
        	gbc.gridy = 3;
        	gbc.weightx = gbc.WEST;
        	gbc.anchor = gbc.WEST;
        	adviseLabel = new JLabel();
        	adviseLabel.setFont(new java.awt.Font("MS Sans Serif", Font.ITALIC, 12));
        	adviseLabel.setText(Options.ADVISE_LABEL);
        	namePane.add(adviseLabel, gbc);
        }
        
//      Jpanel de relleno
        gbc.gridy = 3;
        gbc.anchor = gbc.NORTHWEST;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.weighty = 1.0;
        gbc.insets = new Insets(10,0,0,0);        
        //Panel para la etiqueta del titulo y el boton de ayuda
        JPanel emptyPane = new JPanel();
        emptyPane.setLayout(new BoxLayout(emptyPane, BoxLayout.X_AXIS));
        //emptyPane.setBackground(Color.BLUE);        
        namePane.add(emptyPane,gbc);
        
        
    }

    
    
    
    // Componentes visuales que contiene datos
    private JTextField publicNameField;
    private JTextField nickField;
    /**
     * label que indica "Please fill this mandatory fields" antes del combobox
     */
    private JLabel adviseLabel;
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    
}// Class PublicNamePanel
