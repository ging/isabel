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
 * AditionalParametersPanel.java
 *
 * Created on 12 de moviembre de 2003, 13:37
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import java.awt.*;
import java.awt.event.*;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.regex.Pattern;

import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SITEADDITIONALPARAMS;

//import isabel.lim.HelpBrowser;

/**
 * Esta clase implementa el panel que permite configurar el ancho de banda
 * que se empleara en l�a sesi�n.
 */
public class AditionalParametersPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    /**
     * Crea un objeto BandwidthPanel
     */
    public AditionalParametersPanel() {
        initComponents();
        enableButtons();
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
        SITEADDITIONALPARAMS ap = site.getSITEADDITIONALPARAMS();
        if (ap != null) {
        	int tamano = 3;
        	if(ap.getAny()!=null)
        		tamano += ap.getAny().size();
        	
        	int orden = 0;
        	String[] params = new String[tamano];
        	if(ap.getISABELVNCSERVERDISPLAY()!=null)
        	{        		
        		params [orden] = "ISABEL_VNC_SERVER_DISPLAY=" +  ap.getISABELVNCSERVERDISPLAY();
        		orden++;
        	}
        	if(ap.getISABELGATEWAY()!=null)
        	{
        		org.w3c.dom.Element mielem = (org.w3c.dom.Element)ap.getISABELGATEWAY();
        		params[orden] = "ISABEL_GATEWAY=" + mielem.getTextContent();
        		orden++;
        	}
        	if(ap.isISABELAUDIOMIXER())
        	{
        		params[orden] = "ISABEL_AUDIOMIXER=" + true;
        		orden++;
        	}
        	else
        	{
        		params[orden] = "ISABEL_AUDIOMIXER=" + false;
        		orden++;
        	}
        	if(ap.getAny()!=null)
        	{
        		List<Element> lista = ap.getAny();
        		for(int index=0; index<lista.size();index++)
        		{
        			org.w3c.dom.Element mielem = lista.get(index);
        			params[orden] = mielem.getTagName()+"="+mielem.getTextContent();
        			orden++;
        		}
        	}        
            setAditionalParameters(params);
        }
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        // Borro todos los que hay y pongo los nuevos
        site.setSITEADDITIONALPARAMS(null);
    	SITEADDITIONALPARAMS site_params = new SITEADDITIONALPARAMS();
        String[] params = getAditionalParameters();
        for (int i = 0; i< params.length; i++) {
            if(params[i]!=null)
            {
	        	String name = "";
	            String value = "";
	            StringTokenizer st = new StringTokenizer(params[i], "=");
	            name = st.nextToken();
	            if (st.hasMoreTokens())
	                value = st.nextToken();
	            
	            if(name.equals("ISABEL_VNC_SERVER_DISPLAY"))
	            {
	            	site_params.setISABELVNCSERVERDISPLAY(value);
	            }
	            else if(name.equals("ISABEL_AUDIOMIXER"))
	            {
	            	site_params.setISABELAUDIOMIXER(value.equals("true"));
	            }
	            else if(name.equals("ISABEL_GATEWAY"))
	            {
	            	DocumentBuilder db = null;
	        		try {
	        			db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
	        		} catch (ParserConfigurationException e) {			
	        			e.printStackTrace();
	        		}
	            	Document doc = db.newDocument();
	                org.w3c.dom.Element mielem = doc.createElement("ISABEL_GATEWAY");
	                mielem.setTextContent(value);
	            	site_params.setISABELGATEWAY(mielem);
	            }
	            else
	            {
	            	//si comienza por numero no es un nombre permitido
	            	if(!startsWithDigit(name))
	            	{
		            	//esto es lo del getAny
		            	DocumentBuilder db = null;
		        		try {
		        			db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
		        		} catch (ParserConfigurationException e) {			
		        			e.printStackTrace();
		        		}
		            	Document doc = db.newDocument();
		                org.w3c.dom.Element mielem = doc.createElement(name);
		                mielem.setTextContent(value);
		                
		                List<Element> lista = site_params.getAny();
		            	lista.add(mielem);
	            	}
	            }
            }
        }    
        site.setSITEADDITIONALPARAMS(site_params);
    }
    
    
    boolean startsWithDigit(String s) {
        return Pattern.compile("^[0-9]").matcher(s).find();
    }
    
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setAditionalParameters(DEFAULT_AD_PARAMS);        
    }

    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL ADITIONALPARAMETER       *
     ***************************************************************************/
    
    /**
     * Devuelve una lista que contiener Strings con formato param=value
     * donde estan todos los aditional parameters definidos.
     * @return String[] con los parametros adicionales.
     */
    public String[] getAditionalParameters() {
        String[] result = new String[model.getSize()];
        for (int i = 0; i < result.length; i++)
            result[i] = (String)model.get(i);
        
        return result;
    }
    
    /**
     * Establece la lista de variables locales.
     * @param params String[] en los que cada String tiene formato param=value.
     */
    public void setAditionalParameters(String[] params) {
        model.removeAllElements();
        for (int i = 0; i < params.length; i++)
            model.addElement(params[i]);
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
        titlePane.setBackground(Color.YELLOW);
        add(titlePane, BorderLayout.NORTH);
        
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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(AD_PARAMS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
               
        // Panel con la lista de parametros
        JPanel mainPanel = new JPanel();
        mainPanel.setBorder(new CompoundBorder(
        new CompoundBorder(
        new EtchedBorder(EtchedBorder.RAISED),
        new EtchedBorder()),
        new EmptyBorder(new Insets(20,20,20,20))));
        mainPanel.setLayout(new GridBagLayout());     
        add(mainPanel, BorderLayout.CENTER);
        
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.anchor = GridBagConstraints.CENTER;
        
        gbc.gridwidth = 2;
        gbc.fill = gbc.HORIZONTAL;
        gbc.insets = new Insets(0, 10, 10, 5);
        
        // Panel para el título
        JPanel titlePanel = new JPanel();
        titlePane.setLayout(new GridBagLayout());
        mainPanel.add(titlePanel, gbc);
        
        gbc = new GridBagConstraints();
        gbc.anchor = gbc.EAST;
        gbc.fill = gbc.HORIZONTAL;
        // Etiqueta para el título
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        titleLabel.setText(AD_PARAMS_TITLE);
        titlePanel.add(titleLabel, gbc);
        
        gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 1;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.weighty = 1.0;
        gbc.weightx = 1.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(5, 10, 0, 5);
        
        JScrollPane jsp = new JScrollPane();
        // Para que si el contenido es mas grande
        // ponga barras en lugar de crecer
        jsp.setPreferredSize(new Dimension(200,200));
        mainPanel.add(jsp, gbc);
        list = new JList();
        list.addListSelectionListener(new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent evt) {
            	Options.profile_edited=true;
            	enableButtons();
            }
        });
        
        jsp.setViewportView(list);
        
        model = new DefaultListModel();
        // Pongo los parametros por defecto
        
        setAditionalParameters(DEFAULT_AD_PARAMS);
        list.setModel(model);
        
        // Panel con los botones de manejo de la lista
        JPanel buttonPane = new JPanel();
        GridBagConstraints gridBagConstraints;
        buttonPane.setLayout(new GridBagLayout());
        
        // Boton para annadir
        addButton = new JButton();
        addButton.setText("New");
        addButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
            	Options.profile_edited=true;
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
            	Options.profile_edited=true;
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
            	Options.profile_edited=true;
                del();
            }
        });
        
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.anchor = GridBagConstraints.NORTH; // Para que los botones
        gridBagConstraints.weighty = 1;                       // queden arriba del panel
        buttonPane.add(delButton, gridBagConstraints);
        
        gbc = new GridBagConstraints();
        gbc.gridx = 1;
        gbc.gridy = 1;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.weighty = 2.0;
        gbc.weightx = 0.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(5, 10, 0, 5);
        mainPanel.add(buttonPane, gbc);
        
        
        
        //add(buttonPane, BorderLayout.EAST);
    }
    
    public void edit() {
        InputParameterPanel ip = new InputParameterPanel();
        int index = list.getSelectedIndex();
        String [] p = ((String)model.get(index)).split("=");
        String param = p[0];
        String value = "";
        if(p.length > 1)
            value = p[1];
        ip.setParameterName(param);
        ip.setParameterValue(value);
        Object[] message = {ip};
        int i = JOptionPane.showConfirmDialog(null, message, "Local Parameter", JOptionPane.OK_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE);
        param = ip.getParameterName();
        value = ip.getParameterValue();
        if (i == JOptionPane.OK_OPTION && !param.equals("")) {
            model.remove(index);
            model.add(index, param + "=" + value);
            enableButtons();
        }
    }
    
    public void add() {
        InputParameterPanel ip = new InputParameterPanel();
        Object[] message = {ip};
        int i = JOptionPane.showConfirmDialog(null, message, "Local Parameter", JOptionPane.OK_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE);
        String param = ip.getParameterName();
        String value = ip.getParameterValue();
        if (i == JOptionPane.OK_OPTION && !param.equals("") && !value.equals(""))
            model.addElement(param + "=" + value);
    }
    
    public void del() {
        model.remove(list.getSelectedIndex());
        enableButtons();
    }
    
    public void enableButtons() {
        boolean enable = !(list.getSelectedIndex() == -1);
        editButton.setEnabled(enable);
        delButton.setEnabled(enable);
    }
    
    // Componentes que contienen datos
    JList list;
    DefaultListModel model;
    
    // Botones
    JButton editButton, addButton, delButton;
    
    // Ventana para mostrar la ayuda
    //    private JFrame helpBrowser;
    
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new AditionalParametersPanel());
        f.pack();
        f.setVisible(true);
        
    }
    
    
}// Class AditionalParametersPanel
