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
 * IPAddressPanel.java
 *
 * Created on 12 de moviembre de 2003, 13:37
 */

package services.isabel.services.options;


import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.StringTokenizer;

import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.XEDLManager;

//import isabel.lim.HelpBrowser;

/**
 * Esta clase implementa un panel que permite a�adir cambiar o borrar
 * las direcciones IP del sitio.
 */
public class IPAddressPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
	//xedlmanager para ver que la ip que añado no se repita
	private XEDLManager xedl_manager=null;
    /**
     * Crea un objeto BandwidthPanel
     */
    public IPAddressPanel() {
        initComponents();
        enableButtons();
    }
    
    /**
     * Crea un objeto BandwidthPanel
     */
    public IPAddressPanel(XEDLManager xedl_manager) {
        initComponents();
        enableButtons();
        this.xedl_manager = xedl_manager;
    }
    

    public void setXEDLManager(XEDLManager xedlManager)
    {
    	this.xedl_manager = xedlManager;    	
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
        LinkedList alist = site.getSiteAddress();
        if (alist != null) {
            setAddressList(alist);
        }
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    
    public void saveData(SITE site) {
        // Borro todas las que habia
        LinkedList oldList = site.getSiteAddress();
        if (oldList != null) {
            for (int i = 0; i < oldList.size(); i++)
                site.deleteSiteAddress((String)oldList.get(i));
        }
        
        // Guardo las nuevas
        LinkedList newList = getAddressList();
        for (int i = 0; i < newList.size(); i++)
            site.insertSiteAddress((String)newList.get(i), false);
        
    }
      
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        
    }

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL IPADDRESS                *
     ***************************************************************************/
    
    /**
     * Devuelve una lista que contiene las direcciones del sitio
     * @return LinkedList con las direcciones
     */
    public LinkedList getAddressList() {
        LinkedList result = new LinkedList();
        for (int i = 0; i < model.size(); i++)
            result.add(i, model.get(i));
        
        return result;
    }
    
    /**
     * Establece la lista de direcciones
     * @param alist LinkedList en la que estan las direcciones.
     */
    public void setAddressList(LinkedList alist) {
        model.removeAllElements();
        for (int i = 0; i < alist.size(); i++)
            model.addElement(alist.get(i));
    }
    
    /**************************************************************************
     **************************************************************************/
    
    //    /**
    //     * M�todo ejecutado cuando se pulsa el boton de ayuda.
    //     */
    //    private void helpButtonActionPerformed(ActionEvent evt) {
    //        if (helpBrowser == null)
    //            helpBrowser = new HelpBrowser(AD_PARAMS_HELP_HOME);
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
        titleLabel.setText("Site Address");
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
        
        
        // Etiqueta del icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(NETWORK_ACCESS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel con la lista de direcciones
        JPanel mainPane = new JPanel();
        mainPane.setBorder(new CompoundBorder(
        new CompoundBorder(
        new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()),
        new EmptyBorder(new Insets(20,20,20,20))));
        mainPane.setLayout(new GridLayout(1,1));
        add(mainPane, BorderLayout.CENTER);
        
        JScrollPane jsp = new JScrollPane();
        // Para que si el contenido es mas grande
        // ponga barras en lugar de crecer
        jsp.setPreferredSize(new Dimension(200,200));
        mainPane.add(jsp);
        list = new JList();
        list.addListSelectionListener(new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent evt) {
                enableButtons();
            }
        });
        
        jsp.setViewportView(list);
        
        model = new DefaultListModel();
        list.setModel(model);
        
        // Panel con los botones de manejo de la lista
        JPanel buttonPane = new JPanel();
        GridBagConstraints gridBagConstraints;
        buttonPane.setLayout(new GridBagLayout());
        buttonPane.setBorder(new CompoundBorder(
        new CompoundBorder(
        new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()),
        new EmptyBorder(new Insets(2,20,2,20))));
        
        // Boton para a�adir
        addButton = new JButton();
        addButton.setText("New");
        addButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                add();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new Insets(10, 0, 10, 0);
        buttonPane.add(addButton, gridBagConstraints);
        
        
        // Boton para editar
        editButton = new JButton();
        editButton.setText("Edit");
        editButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                edit();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new Insets(0, 0, 10, 0);
        buttonPane.add(editButton, gridBagConstraints);
        
        // Boton para borrar
        delButton = new JButton();
        delButton.setText("Delete");
        delButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                del();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new Insets(0, 0, 10, 0);
        buttonPane.add(delButton, gridBagConstraints);
        
        // Boton para subir
        upButton = new JButton();
        upButton.setText("Up");
        upButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                up();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new Insets(0, 0, 10, 0);
        buttonPane.add(upButton, gridBagConstraints);
        
        // Boton para bajar
        downButton = new JButton();
        downButton.setText("Down");
        downButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                down();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = GridBagConstraints.NORTH; // Para que los botones
        gridBagConstraints.weighty = 1;                       // queden arriba del panel
        buttonPane.add(downButton, gridBagConstraints);
        
        
        add(buttonPane, BorderLayout.EAST);
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa edit.
     */
    private void edit() {
        int index = list.getSelectedIndex();
        String value = JOptionPane.showInputDialog(this, "Site Address or Domain Name", list.getSelectedValue());
        if (value != null) {
        	if(xedl_manager!=null)
        	{
        		//compruebo que la ip no está repetida en el xedl
        		String id_rep = xedl_manager.isAddressRepeated(value);
        		if(id_rep!=null)
        		{
        			JOptionPane.showMessageDialog(null, "Address already in use by " + id_rep, "Address already in use", JOptionPane.ERROR_MESSAGE);
        		}
        		else
        			model.set(index, value);
        	}
        	else
        	{
        		model.set(index, value);
        	}
        }
    }
    
    
    /**
     * M�todo que se ejecuta cuando se pulsa add.
     */
    private void add() {
        String value = JOptionPane.showInputDialog(this, "Site Address or Domain Name");
        if (value != null && !value.trim().equals(""))
        {
        	if(xedl_manager!=null)
        	{
        		//compruebo que la ip no está repetida en el xedl
        		String id_rep = xedl_manager.isAddressRepeated(value);
        		if(id_rep!=null)
        		{
        			JOptionPane.showMessageDialog(null, "Address already in use by " + id_rep, "Address already in use", JOptionPane.ERROR_MESSAGE);
        		}
        		else
        			model.addElement(value);
        	}
        	else
        	{
        		model.addElement(value);
        	}
        }       
        list.setSelectedIndex(model.getSize()-1);
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa del.
     */
    private void del() {
        model.remove(list.getSelectedIndex());
        enableButtons();
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa up.
     */
    private void up() {
        int i = list.getSelectedIndex();
        swap(i, i-1);
        list.setSelectedIndex(i-1);
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa down.
     */
    private void down() {
        int i = list.getSelectedIndex();
        swap(i, i+1);
        list.setSelectedIndex(i+1);
    }
    
    /**
     * Intercambia los valores de dos posiciones de la lista.
     * @param i Poscion de uno de los valores a intercambiar.
     * @param j Poscion de uno de los valores a intercambiar.
     */
    private void swap(int i, int j) {
        Object valuei = model.get(i);
        Object valuej = model.get(j);
        
        model.set(j, valuei);
        model.set(i, valuej);
    }
    
    /**
     * Habilita o deshabilita los botones dependiendo de la seleccion.
     */
    private void enableButtons() {
        int index = list.getSelectedIndex();
        editButton.setEnabled(index != -1);
        delButton.setEnabled(index != -1 && model.getSize()>1);
        
        upButton.setEnabled(index > 0);
        downButton.setEnabled((index != -1) && (index+1 < model.getSize()));
        
    }
    
    // Componentes que contienen datos
    private JList list;
    private DefaultListModel model;
    
    // Botones
    private JButton editButton, addButton, delButton, upButton, downButton;
    
    // Ventana para mostrar la ayuda
    //    private JFrame helpBrowser;
    
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new IPAddressPanel());        
        f.pack();
        f.setVisible(true);        
    }
    
}// Class IPAddressPanel
