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
 * ParityPanel.java
 *
 * Created on 1 de abril de 2004, 11:56
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import xedl.lib.jaxb.SITE;

import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Enumeration;

/**
 * Panel para introducir los datos sobre proteccion de errores.
 * @author  Fernando Escribano
 */
public class ParityPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    /**
     * Lista de los nombres del fec.
     */
	
    private static final String[] fecname = {" None", " 10 %", " 25 %", " 50 %", " 100 %"};
    private static final Point[] fecnk = {new Point(0,0),new Point(11,10),new Point(5,4),
                                         new Point(3,2),new Point(2,1)};
    
    /**
     * Crea un objeto ErrorProtectionPanel.
     */
    public ParityPanel() {
        initComponents();
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
        int [] parity = site.getParityProtectionModeParams();
        if (parity[0] == 0) {
            setSelection(fecname[0]);
        }
        else {
            Point p = new Point(parity[0], parity[1]);
            for (int i = 1; i < fecnk.length; i++) {
                if(p.equals(fecnk[i])) {
                    setSelection(fecname[i]);
                    return;
                }
            }
        }
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.delProtectionMode();
        String selected = modeGroup.getSelection().getActionCommand();
        for (int i = 1; i < fecname.length; i++) {
            if(fecname[i].equals(selected)) {
                site.setParityProtectionMode(fecnk[i].x, fecnk[i].y);
                return;
            }
        }
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setSelection(fecname[0]);
    }

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL ERRORPROTECTION          *
     ***************************************************************************/
    public void setOverhead(int overhead) {
        
    }
    
    public int getOverhead() {
        return 0;
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

        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        add(titlePane, BorderLayout.NORTH);
        
        // Etiqueta con el titulo           
        //JLabel titleLabel = new JLabel();
        //titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        //titleLabel.setForeground(TITLE_COLOR);
        //titleLabel.setText(PARITY_TITLE);
        //titlePane.add(titleLabel);

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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(FEC_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para poner los diferentes overheads
        JPanel modePanel = new JPanel();
        modePanel.setLayout(new GridBagLayout());
        modePanel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(modePanel, BorderLayout.CENTER);
        
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.insets = new Insets(60, 0, 10, 5);
        
        // Panel para el título
        JPanel titlePanel = new JPanel();
        titlePanel.setLayout(new BoxLayout(titlePanel, BoxLayout.X_AXIS));
        modePanel.add(titlePanel, gbc);
        
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(PARITY_TITLE);
        titleLabel.setHorizontalTextPosition(SwingConstants.LEFT);
        modePanel.add(titleLabel);

        
        modeGroup = new ButtonGroup();
        gbc = new GridBagConstraints();
        gbc.gridx = 1;
        gbc.fill = gbc.BOTH;
        gbc.weighty = 1;
        gbc.insets =  new Insets(10,0,10,0);
        for (int i = 0; i < fecname.length; i++)
        {
            JRadioButton temp = createRadioButton(fecname[i]);
            temp.addItemListener(new ItemListener() {
            	public void itemStateChanged(ItemEvent e) {
            		Options.setProfile_edited(true);
                 }
            });
            
        	modePanel.add(temp , gbc);
        }
           
        resetData();
    }
    
    /**
     * Selecciona el boton que tiene como texto el String que se
     * pasa como parametro.
     * @param s Texto del boton a seleccionar.
     */
    private void setSelection(String s) {
        Enumeration e = modeGroup.getElements();
        JRadioButton rb;
        while(e.hasMoreElements()) {
            if ((rb =(JRadioButton)e.nextElement()).getActionCommand().equals(s)) {
                rb.setSelected(true);
                return;
            }
        }        
    }
    
    /**
     * Crea un RadioButton y lo annade al ButtonGroup.
     */
    private JRadioButton createRadioButton(String s) {
        JRadioButton rb = new JRadioButton(s);
        rb.setActionCommand(s);
        modeGroup.add(rb);
        return rb;
    }
    
    // Componentes que almacenan datos
    private ButtonGroup modeGroup;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);        
        f.getContentPane().add(new ParityPanel());
        f.pack();
        f.setVisible(true);
            
    }
    
}// Class PublicNamePanel
