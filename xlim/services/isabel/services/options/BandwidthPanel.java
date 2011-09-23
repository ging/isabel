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
 * BandwidthPanel.java
 *
 * Created on 10 de moviembre de 2003, 12:32
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
 * Esta clase implementa el panel que permite configurar el ancho de banda
 * que se empleara en l�a sesi�n.
 */
public class BandwidthPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    /**
     * Crea un objeto BandwidthPanel
     */
    public BandwidthPanel() {
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
        setUPBandwidth(toStr(site.getUpBandwidth()));
        setDownBandwidth(toStr(site.getDownBandwidth()));        
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        int ubw = toInt(getUPBandwidth());
        int dbw = toInt(getDownBandwidth());

        site.setDownBandwidth(dbw);
        site.setUpBandwidth(ubw);        
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setUPBandwidth(toStr(1000));
        setDownBandwidth(toStr(1000));
    }
    

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL BANDWIDTHPANEL           *
     ***************************************************************************/

    public String getUPBandwidth() {
        return (String) upBWSelector.getSelectedItem();
    }
    
    public void setUPBandwidth(String bw) {
         upBWSelector.setSelectedItem(bw);
    }
    
    public String getDownBandwidth() {
        return (String) downBWSelector.getSelectedItem();
    }
    
    public void setDownBandwidth(String bw) {
         downBWSelector.setSelectedItem(bw);
    }

    /**************************************************************************
     **************************************************************************/
    
//    /**
//     * M�todo ejecutado cuando se pulsa el boton de ayuda.
//     */
//    private void helpButtonActionPerformed(ActionEvent evt) {
//        if (helpBrowser == null)
//            helpBrowser = new HelpBrowser(BANDWIDTH_HELP_HOME);
//        helpBrowser.show();
//    }
    
    /**
     * Convierte un valor entero a una cadena de texto. Poniendo K, M,..
     */
    private String toStr(int i) {
        String result;
        int mega = 1000;
        if (i >= mega)
            result = String.valueOf(i/mega) + " M";
        else
            result = String.valueOf(i) + " K";
        
        return result;
    }
    
    /**
     * Convierte el valor que da el panel de bandwidth a un entero.
     */
    private int toInt(String s) {
        String[] ss = s.split(" ");
        int n = Integer.parseInt(ss[0]);
        if (ss[1].equals("M"))
            n = n * 1000;
        return n;
    }
    
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
        titleLabel.setText(BANDWIDTH_TITLE);
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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(BANDWIDTH_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para seleccionar el ancho de banda
        // Tiene dos selectores (dentro de paneles para que se vean bien)
        JPanel bandwidthPane = new JPanel();
        bandwidthPane.setLayout(new BoxLayout(bandwidthPane, BoxLayout.Y_AXIS));
        bandwidthPane.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(bandwidthPane, BorderLayout.CENTER);

        // UPLOAD BW
        JPanel upBWPane = new JPanel();
        upBWPane.setBorder(new CompoundBorder(new EmptyBorder(new Insets(20, 20, 20, 20)),
                                              new TitledBorder(null, "Upload Bandwidth",
                                              TitledBorder.DEFAULT_JUSTIFICATION,
                                              TitledBorder.DEFAULT_POSITION,
                                              new Font("Dialog", 0, 10))));
        bandwidthPane.add(upBWPane);

        upBWSelector = new JComboBox();
        upBWSelector.setModel(new DefaultComboBoxModel(AVALAIBLE_BANDWIDTHS));
        upBWSelector.setSelectedItem(DEFAULT_UP_BW);
        upBWSelector.setPreferredSize(new Dimension(120, 25));
        upBWSelector.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                Options.profile_edited = true;
            }
          });
        upBWPane.add(upBWSelector);

        // 20 p�xeles entre un selector y otro
        bandwidthPane.add(Box.createVerticalGlue());

        // DOWNLOAD BW
        JPanel downBWPane = new JPanel();
        downBWPane.setBorder(new CompoundBorder(new EmptyBorder(new Insets(20, 20, 20, 20)),
                                                new TitledBorder(null, "Download Bandwidth",
                                                TitledBorder.DEFAULT_JUSTIFICATION,
                                                TitledBorder.DEFAULT_POSITION,
                                                new Font("Dialog", 0, 10))));
        bandwidthPane.add(downBWPane);

        downBWSelector = new JComboBox();
        downBWSelector.setModel(new DefaultComboBoxModel(AVALAIBLE_BANDWIDTHS));
        downBWSelector.setSelectedItem(DEFAULT_DOWN_BW);
        downBWSelector.setPreferredSize(new Dimension(120, 25));
        downBWSelector.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                Options.profile_edited = true;
            }
          });
        downBWPane.add(downBWSelector);        
    }
        
    // Componentes visuales que contiene datos
    private JComboBox downBWSelector;
    private JComboBox upBWSelector;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    
}// Class BandwidthPanelPanel
