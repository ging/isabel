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
 * Options.java
 *
 * Created on 16 de septiembre de 2003, 11:45
 */

package services.isabel.services.options;

import services.isabel.lib.Agenda;
import java.io.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;

import java.util.Iterator;

import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

/**
 * Esta clase implementa la aplicacion Options. Esta aplicacion crea un GUI
 * que permite al usuario introducir toda una serie de parametros
 * que posteriormente son guardados en un XEDL de options.
 * @author  Fernando Escribano
 */
public class Options extends JDialog implements OptionsPanelListener, OptionsStrings, ActionListener{
    
    /**
     * Identificador que se pone al sitio en el xedl que se crea.
     */
    static final String OPTIONS_SITE_ID = "";
    
    /**
     * Mensaje la primera vez que se abren las options
     */
    static final String ADVISE_LABEL = "*Please fill this mandatory fields";
    
    /**
     * Panel de etiquetas con los paneles de datos.
     */
    protected OptionsTabs tabs;
    
    /**
     * Los items del menu view.
     */
    private JCheckBoxMenuItem advancedItem, ipsecItem;
    
    /**
     * El panel para el JComboBox y el botón delete
     */
    private JPanel panelillo;
    
    /**
     * título que aparece antes de "Profile:"
     */
    private JLabel titulo_ventana;
    
        
    /**
     * label que indica "profile name" antes del combobox
     */
    private JLabel profileLabel;
    
        
    /**
     * El JComboBox de los profiles
     */
    private JComboBox profiles;
    
    /**
     * Perfil que se esta editando actualmente.
     * tiene que ser public porque luego las tabs comprueban con este si cambian las cosas
     */
    public static String currentProfile = "Default";
    
    /**
     * variable que dice si el perfil se ha editado o está como cuando se cargó
     * sirve para preguntar al usuario si guardar cambios o no
     */
    public static boolean profile_edited = false;
    
    /**
     * variable para evitar el evento que se genera al hacer un removeAllItems
     */
    private boolean esremove = false;

     /**
     * Crea un objeto options. Se encarga de crear la ventana principal de la aplicacion,
     * mostrarla y configurar el objeto OptionsPanel para que cuando los botones de
     * Save y Cancel sean pulsados se guarden los datos en un fichero con el primero
     * y se salga de la aplicacion con el segundo. Para obtener otro comportamiento se puede
     * crear una clase que extienda a esta sobreescribiendo los metodos:
     * public void cancelButtonPressed() y public void doneButtonPressed()
     */
    public Options(boolean setValues) {
    	// Configuro la ventana
    	this.setMinimumSize(new Dimension(500,400));
        this.setWindowTitle();
        this.setResizable(false);
        this.setModal(true);
        GridBagLayout gridbag = new GridBagLayout();        
        this.getContentPane().setLayout(gridbag);
        GridBagConstraints restric= new GridBagConstraints();
        restric.anchor = java.awt.GridBagConstraints.WEST;
        restric.fill = GridBagConstraints.HORIZONTAL;
        restric.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(this.getContentPane(),restric);
        
        //this.setIconImage(new ImageIcon(this.getClass().getResource(WINDOW_ICON)).getImage());
        this.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent evt) {
                //System.exit(0);
            	dispose();
            }
        });
        
        //Barra de menu
        //ENRIQUE quito el menu desplegable de profile, ahora será un combobox
        //this.setJMenuBar(createMenuBar());
        panelillo= new JPanel();
        GridBagLayout gb = new GridBagLayout();
        GridBagConstraints constr= new GridBagConstraints();
        constr.anchor = java.awt.GridBagConstraints.WEST;
        constr.insets = new Insets(5,10,5,10);
        //constr.fill = GridBagConstraints.HORIZONTAL;
        //constr.gridwidth = GridBagConstraints.REMAINDER;
        //gb.setConstraints(this.getContentPane(),restric);
        
        panelillo.setLayout(gb);
        titulo_ventana = new JLabel();
        titulo_ventana.setText(OPTIONS_TITLE);
        titulo_ventana.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        panelillo.add(titulo_ventana,constr);
        profileLabel = new JLabel();
        profileLabel.setText(PROFILE_LABEL_TEXT);
        panelillo.add(profileLabel, constr);
        
        profiles = new JComboBox();
        profiles.setEditable(true);
        rellenaCombo();
        profiles.addActionListener(this);
              
        panelillo.add(profiles, constr);
        
        //en vez de una jmenubar añado un button
        JButton deleteItem = new JButton(DELETE_PROFILE_ITEM_NAME);
        deleteItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                deleteProfile();
            }
        });        
        panelillo.add(deleteItem, constr);
        
        this.getContentPane().add(panelillo, restric);
                
        tabs = new OptionsTabs();
        tabs.setListener(this);
        tabs.showAll();
        
        //c.weightx = 2.0;
        getContentPane().add(tabs,restric);
        pack();
        
        //tabs.showBasics();        
        // Establezco los campos a los valores contenidos en el xedl si existe
        if (setValues)
            setInitialValues();
        
        // La coloco en el centro de la pantalla
        this.setBounds(this.getGraphicsConfiguration().getBounds().width/2 - this.getBounds().width/2,
        this.getGraphicsConfiguration().getBounds().height/2 - this.getBounds().height/2,
        this.getBounds().width, this.getBounds().height);
        setProfile_edited(false);
    }
    
    
    /**
	 * rellena el JComboBox de los profiles
	 */
	private void rellenaCombo() {
		//antes de añadir elementos tengo que vaciar profiles
		esremove = true;   //no atiendo a eventos
		profiles.removeAllItems();
		esremove = false;
		
		try {
			String[] profs = new Agenda().getProfileNames(true);
			profiles.addItem("Default");
			int i;
			for (i = 0; i < profs.length; i++) {
				if(profs[i].equals("Default"))
					continue;
				profiles.addItem(profs[i]);
			}
			profiles.setSelectedItem(currentProfile);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void actionPerformed(ActionEvent accion) {
		//System.out.println(accion.getActionCommand());
		//si el perfil que tenemos antes de cambiar está editado tenemos que preguntar si salvarlo
		if(profile_edited & accion.getActionCommand().equals("comboBoxChanged"))
		{			
			JFrame parentFrame = new JFrame();
            int resultPanel = JOptionPane.showConfirmDialog(parentFrame, "Profile " + currentProfile + " has been edited, save changes?", "Profiles", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE);
            parentFrame.dispose();
            if (resultPanel == JOptionPane.NO_OPTION) {
               //el usuario hace clic en NO, cargamos el nuevo perfil si ha sido un change 
               //o creamos uno nuevo con el nombre introducido si ha sido editado el combobox            	
               carga_perfil();            	
            } else if(resultPanel == JOptionPane.YES_OPTION){
            	//usuario hace clic en yes, guardamos el perfil actual 
            	//y cargamos el nuevo perfil si ha sido un change 
                //o creamos uno nuevo con el nombre introducido si ha sido editado el combobox
            	try {
					saveData();
				} catch (Exception e) {
					e.printStackTrace();
				}
				//despues de salvar cargamos el perfil
				carga_perfil();				
            }
            else {
            	//clic en cancel, volvemos pero antes de volver hay que vovler a poner como seleccionado
            	//el current porque el usuario puede haber escrito en el jcombobox
            	profiles.setSelectedItem(currentProfile);
            	return;
            }
		}
		else if(accion.getActionCommand().equals("comboBoxChanged"))
		{
			//el perfil no se ha editado, sólo cargamos lo nuevo
			carga_perfil();
		}
		else {
			
		}		
	}
	
	
	private void carga_perfil(){
		//para diferenciar si el evento es de seleccion o edición veo si el perfil seleccionado existe
		//si es así es una selección, si no se ha editado
		if (esremove)
			return;
		if( existe_profile((String)profiles.getSelectedItem()) )
		{
			//lo pongo como actual
			currentProfile=(String)profiles.getSelectedItem();
			setInitialValues();
            setWindowTitle();
            setProfile_edited(false);
		}
		else
		{
			currentProfile=(String)profiles.getSelectedItem();
			resetButtonPressed();
            setWindowTitle();
            //es un profile nuevo, no esta guardado
            setProfile_edited(true);
		}
		rellenaCombo();
	
	}
	
	/**
	 * método para ver si el perfil esta entre los que existen
	 * @param name nombre del profile que se quiere saber si existe
	 * @return si existe o no
	 */
	private boolean existe_profile(String name)
	{
		String[] profs;
		try {
			profs = new Agenda().getProfileNames(true);
		
		int i;
		for (i = 0; i < profs.length; i++) {
			if(profs[i].equals(name))
				return true;
		}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return false;
	}
	
	
	/**
	 * Crea la barra de menus.
	 * 
	 * @return JMenuBar de la aplicacion.
	 */
    private JMenuBar createMenuBar() {
        JMenuBar bar = new JMenuBar();
        
        JMenu profileMenu =  new JMenu();
        profileMenu.setText(PROFILE_MENU_NAME);
        bar.add(profileMenu);
        
        JMenuItem newItem = new JMenuItem();
        newItem.setText(NEW_PROFILE_ITEM_NAME);
        newItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                newProfile();
            }
        });
        profileMenu.add(newItem);
        profileMenu.addSeparator();
        
        JMenuItem loadItem = new JMenuItem();
        loadItem.setText(LOAD_PROFILE_ITEM_NAME);
        loadItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                loadProfile();
            }
        });
        profileMenu.add(loadItem);
        profileMenu.addSeparator();
        
        JMenuItem saveItem = new JMenuItem();
        saveItem.setText(SAVE_PROFILE_ITEM_NAME);
        saveItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                try {
                    saveData();
                }catch(Exception e) {
                    e.printStackTrace();
                }
            }
        });
        profileMenu.add(saveItem);
        
        JMenuItem saveAsItem = new JMenuItem();
        saveAsItem.setText(SAVEAS_PROFILE_ITEM_NAME);
        saveAsItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                saveProfileAs();
            }
        });
        profileMenu.add(saveAsItem);
        profileMenu.addSeparator();
        
        JMenuItem deleteItem = new JMenuItem();
        deleteItem.setText(DELETE_PROFILE_ITEM_NAME);
        deleteItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                deleteProfile();
            }
        });
        profileMenu.add(deleteItem);
        
        
        JMenu viewMenu = new JMenu();
        viewMenu.setText(VIEW_MENU_NAME);
        bar.add(viewMenu);
        
        advancedItem = new JCheckBoxMenuItem();
        advancedItem.setText(VIEW_AD_ITEM_NAME);
        advancedItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                showTabs();
            }
        });
        viewMenu.add(advancedItem);
        
        ipsecItem = new JCheckBoxMenuItem();
        ipsecItem.setText(VIEW_IPSEC_ITEM_NAME);
        ipsecItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                showTabs();
            }
        });
        viewMenu.add(ipsecItem);
        
        JMenu aboutMenu = new JMenu();
        aboutMenu.setText(ABOUT_MENU_NAME);
        bar.add(aboutMenu);
        
        JMenuItem aboutItem = new JMenuItem();
        aboutItem.setText(ABOUT_ISABEL_ITEM_NAME);
        aboutItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                aboutItemActionPerformed(evt);
            }
        });
        aboutMenu.add(aboutItem);
        
        
        return bar;
    }
    
    /**
     * Establece el titulo de la ventana seg�n el profile seleccionado.
     */
    private void setWindowTitle() {
        if (currentProfile == null)
            this.setTitle(OPTIONS_TITLE + " : Default");
        else
            this.setTitle(OPTIONS_TITLE + " : " + currentProfile);
            
    }
     
    /**
     * M�todo que se ejecuta cuando se selecciona nuevo perfil en el menu.
     */
    private void newProfile() {
        // Prgunto el nombre del profile
        String s = JOptionPane.showInputDialog(this, "Profile Name", "Profiles",
                                               JOptionPane.QUESTION_MESSAGE);
        
        // Si es valido lo guardo como profile actual y reseteo los paneles
        if (s != null && !s.trim().equals("")) {
            currentProfile = s;            
            resetButtonPressed();
            setWindowTitle();
        }
    }
        
    /**
     * M�todo que se ejecuta cuando se selecciona cargar perfil en el menu.
     */
    private void saveProfileAs() {
        try {
            // Prgunto el nombre del profile
            String name = JOptionPane.showInputDialog(this, "Profile Name", "Profiles",
                                                      JOptionPane.QUESTION_MESSAGE);

            // Si es valido lo guardo como profile actual y guardo los datos
            if (name != null && !name.trim().equals("")) {
                // Si el perfil ya existe pregunto antes de sobreescribir
                if (new Agenda().getOptionsFile(name).exists()) {
                    int confirmed = JOptionPane.showConfirmDialog(this,
                                    "Profile " + name + " already defined. Overwrite profile definition?",
                                    "Confirm profile overwrite",
                                    JOptionPane.YES_NO_OPTION);
                                
                    if (confirmed != JOptionPane.YES_OPTION)
                        return;                    
                }
                
                currentProfile = name;                    
                saveData();
                setWindowTitle();
            }
        }catch(Exception e) {
            e.printStackTrace();
        }            
    }
    
    /**
     * M�todo que se ejecuta cuando se selecciona cargar perfil en el menu.
     */
    private void loadProfile() {
        try {
            // Prgunto el nombre del profile
            String s = (String)JOptionPane.showInputDialog(this, "Profile Name", "Profiles",
                                                   JOptionPane.QUESTION_MESSAGE,
                                                   null, new Agenda().getProfileNames(true), currentProfile);

            // Si es valido lo guardo como profile actual y actualizo los paneles
            if (s != null && !s.equals("")) {
                currentProfile = s;
                setInitialValues();
                setWindowTitle();
            }
        }catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    /**
     * M�todo que se ejecuta cuando se selecciona borrar perfil en el menu.
     */
    private void deleteProfile() {
        try {
			Agenda a = new Agenda();
			// Prgunto el nombre del profile
			if (currentProfile.equals("Default")) {
				// si es el default no se puede borrar
				JOptionPane.showMessageDialog(this,
						"Profile Default can't be erased", "Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			} else {
				int seleccion = JOptionPane.showConfirmDialog(this, "Delete Profile " + currentProfile + " ?", 
													"Delete Profile", JOptionPane.YES_NO_OPTION);
				// Si es yes borro el profile actual y actualizo los paneles
				if (seleccion == JOptionPane.YES_OPTION) {
					a.getOptionsFile(currentProfile).delete();
					currentProfile = "Default";
					rellenaCombo();
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
        
    }
    
    
    /**
	 * Muestra una ventana con informacion sobre isabel cuando se pulsa el menu
	 * de about.
	 */
    private void aboutItemActionPerformed(ActionEvent evt) {
        JOptionPane.showMessageDialog(this,
        new ImageIcon(getClass().getResource(ABOUT_ISABEL_IMAGE)),
        "About Isabel", JOptionPane.INFORMATION_MESSAGE,
        new ImageIcon());
    }
    
    /**
     * Muestra u oculta los controles avanzados.
     */
    private void showTabs() {
        boolean showIpsec = ipsecItem.isSelected();
        boolean showAdvanced = advancedItem.isSelected();
        if (showIpsec && showAdvanced) {
            tabs.showAll();
        }/*
        else if (showIpsec){
            tabs.showIpsec();
        }*/
        else if (showAdvanced){
            tabs.showAdvanced();
        }
        else {
            tabs.showBasics();
        }
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de cancel en
     * el panel de opciones.
     */
    public void cancelButtonPressed() {
    	//si el perfil ha sido editado hay que preguntar si salvar los cambios
    	if(profile_edited)
    	{
    		int seleccion = JOptionPane.showConfirmDialog(this, "Profile " + currentProfile + " has been edited, save ?", 
					"Save Profile", JOptionPane.YES_NO_CANCEL_OPTION);
    		if(seleccion == JOptionPane.YES_OPTION)
    		{
    			try {
					saveData();
					dispose();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    		else if (seleccion == JOptionPane.NO_OPTION) {
                //el usuario hace clic en NO,
             	dispose();
             }
    		else 
    		{
    			//no hago nada si da a cancel o la X
    		}		
    	}
    	else
    	{
    		dispose();
    	}   	
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de reset en
     * el panel de opciones.
     */
    public void resetButtonPressed() {
        Iterator dataPanels = tabs.getXEDLSiteDataPanels().iterator();
        while (dataPanels.hasNext())
            ((XEDLSiteDataPanel)dataPanels.next()).resetData();
        
        setProfile_edited(true);
    }
    
    /** M�todo que se ejecuta cuando se pulsa el boton de done en
     * el panel de opciones.
     */
    public void doneButtonPressed() {
        // Guardo toda la informacion de los paneles en un XEDL
        if(profile_edited)
        {     
        	JFrame parentFrame = new JFrame();
            int resultPanel = JOptionPane.showConfirmDialog(parentFrame, "Profile " + currentProfile + " has been edited, save changes?", "Profiles", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE);
            parentFrame.dispose();
            if (resultPanel == JOptionPane.NO_OPTION) {
               //el usuario hace clic en NO,
            	dispose();
            }
            else if(resultPanel == JOptionPane.YES_OPTION){
            	//usuario hace clic en yes, guardamos el perfil actual 
            	try {
					saveData();
					dispose();
				} catch (Exception e) {
					e.printStackTrace();
				}
            }
            else{
            	//opcion cancel o cerrar la ventana que salió
            	//no hago nada
            }
     }
     else
     {
    	 //no está editado
    	 dispose();
     }
    }    
    
    /**
     * Guarda los datos de los paneles en el fichero de optiopns adecuado.
     */
    private void saveData() throws Exception {
        File optionsFile = new Agenda().getOptionsFile(currentProfile);
        XEDL optionsXEDL;
        if (optionsFile.exists())
            optionsXEDL = new XEDL(optionsFile.getAbsolutePath());
        else
            optionsXEDL = new XEDL();
        
        SiteList lista = new SiteList(optionsXEDL);
        String sitename = lista.getSiteIds().isEmpty()?OPTIONS_SITE_ID:
                          (String)lista.getSiteIds().getFirst();                        
        SITE optionsSite = lista.getSite(sitename);
        if (optionsSite == null)
            optionsSite = lista.createSite(OPTIONS_SITE_ID);

        // VALORES QUE PONGO POR DEFECTO
        optionsSite.setNetworkAccessEthernet();
        optionsSite.setControlCapabilities("full");

        // Recorremos la lista de paneles y se les dice que guarden loas datos
        Iterator dataPanels = tabs.getXEDLSiteDataPanels().iterator();
        while (dataPanels.hasNext())
            ((XEDLSiteDataPanel)dataPanels.next()).saveData(optionsSite);
        
        lista.addSite(optionsSite);
        // Lo salvo en el fichero y salgo
        optionsXEDL.save(optionsFile.getAbsolutePath());
    }
            
    /**
     * Pone el valor de los paneles al contenido en el XEDL.
     */
    private void setInitialValues() {
        try {
        	profile_edited = false;
            File optionsFile = new Agenda().getOptionsFile(currentProfile);
            if (!optionsFile.exists())
                return;
            
            XEDL optionsXEDL = new XEDL(optionsFile.getAbsolutePath());
            SiteList lista = new SiteList(optionsXEDL);
            String sitename = (String)lista.getSiteIds().getFirst();
            SITE optionsSite = lista.getSite(sitename);
            
            // Recorremos la lista de paneles y se les dice que carguen los datos
            Iterator dataPanels = tabs.getXEDLSiteDataPanels().iterator();
            while (dataPanels.hasNext())
                ((XEDLSiteDataPanel)dataPanels.next()).loadData(optionsSite);
            
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    /**
     * Metodo principal de la aplicacion. Crea la ventana principal
     * del GUI y los objetos que implementan la logica de la aplicacion.
     * @param args Argumentos de la linea de comandos.
     */
    public static void main(String [] args) {
        //JFrame.setDefaultLookAndFeelDecorated(true);
        //JDialog.setDefaultLookAndFeelDecorated(true);
        try {
            if (args.length > 0) {
                if (args[0].equals("system"))
                    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                else if (args[0].equals("motif"))
                    UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
                else if (args[0].equals("gtk"))
                    UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
            }
        }catch (Exception e) {
        }
        
        Options opt = new Options(true);
        opt.pack();
        opt.setVisible(true);
    }


	public static boolean isProfile_edited() {
		return profile_edited;
	}


	public static void setProfile_edited(boolean profile_edited) {
		Options.profile_edited = profile_edited;
	}

	
}
