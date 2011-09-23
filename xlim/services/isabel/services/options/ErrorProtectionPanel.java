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
 * ErrorProtectionPanel.java
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
import java.util.Enumeration;

/**
 * Panel para introducir los datos sobre proteccion de errores.
 * @author  Fernando Escribano
 */
public class ErrorProtectionPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {
    
    /**
     * Crea un objeto ErrorProtectionPanel.
     */
    public ErrorProtectionPanel() {
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
        int [] hamming = site.getHammingProtectionModeParams();
        int [] parity = site.getParityProtectionModeParams();

        if(hamming[0]==0 && parity[0]==0) {
            setFecType("None");
            return;
        }
        
        if (parity[0] != 0) {
            setFecType("Parity");
            setParityN(parity[0]);
            setParityK(parity[1]);
        }
        else {
            setFecType("Hamming");
            setHammingN(hamming[0]);
            setHammingK(hamming[1]);
        }
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */
    public void saveData(SITE site) {
        site.delProtectionMode();
        String type = getFecType();
        if (type.equals("Hamming")) {
            site.setHammingProtectionMode(getHammingN(), getHammingK());
            return;
        }
        
        if (type.equals("Parity"))
            site.setParityProtectionMode(getParityN(), getParityK());
            
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setFecType("None");
    }

    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL ERRORPROTECTION          *
     ***************************************************************************/

    /**
     * Obtiene el tipo de proteccoin seleccionada.
     * @return String con el valor del campo.
     */
    public String getFecType() {
        return modeGroup.getSelection().getActionCommand();
    }
    
    /**
     * Establece el tipo de proteccion.
     * @param fec El valor a establecer.
     */
    public void setFecType(String fec) {
        Enumeration buttons = modeGroup.getElements();
        while(buttons.hasMoreElements()) {
            JRadioButton rb = (JRadioButton)buttons.nextElement();
            ButtonModel button = rb.getModel();
            modeGroup.setSelected(button, button.getActionCommand().equals(fec));
        }
    }
    
    public int getParityN() {
        return ((Integer)parityNSelector.getValue()).intValue();
    }
    
    public int getParityK() {
        return ((Integer)parityKSelector.getValue()).intValue();
    }

    public int getHammingN() {
        return ((Integer)hammingNSelector.getValue()).intValue();
    }
    
    public int getHammingK() {
        return ((Integer)hammingKSelector.getValue()).intValue();
    }
    
    public void setParityN(int i) {
        parityNSelector.setValue(new Integer(i));
    }
    
    public void setParityK(int i) {
        parityKSelector.setValue(new Integer(i));
    }
    public void setHammingN(int i) {
        hammingNSelector.setValue(new Integer(i));
    }
    public void setHammingK(int i) {
        hammingKSelector.setValue(new Integer(i));
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
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(FEC_TITLE);
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
        iconLabel.setIcon(new ImageIcon(getClass().getResource(FEC_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);
        
        // Panel para poner los diferentes modos.
        JPanel modePanel = new JPanel();
        modePanel.setLayout(new GridLayout(0, 1));
        modePanel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(modePanel, BorderLayout.CENTER);
        
        modeGroup = new ButtonGroup();
        // Panel de none
        JPanel nonePanel = new JPanel();
        nonePanel.setLayout(new GridBagLayout());        
        nonePanel.setBorder(new CompoundBorder(new EtchedBorder(), new EmptyBorder(new Insets(3, 3, 3, 3))));
        modePanel.add(nonePanel);
        
        GridBagConstraints gbc = new GridBagConstraints();
        
        JRadioButton noneButton = new JRadioButton();
        noneButton.setText("None");
        noneButton.setActionCommand("None");
        noneButton.setSelected(true);        
        modeGroup.add(noneButton);
        
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 1.0;
        gbc.insets = new Insets(10, 10, 0, 0);
        nonePanel.add(noneButton, gbc);
        
        parityNSelector = new JSpinner();
        parityKSelector = new JSpinner();
        modePanel.add(createTypePanel("Parity", parityNSelector, parityKSelector));
        
        hammingNSelector = new JSpinner();
        hammingKSelector = new JSpinner();
        modePanel.add(createTypePanel("Hamming", hammingNSelector, hammingKSelector));

    }
    
    
    private JPanel createTypePanel(String type, final JSpinner nSelector, final JSpinner kSelector) {
        JPanel parityPanel = new JPanel();
        parityPanel.setLayout(new GridBagLayout());
        parityPanel.setBorder(new CompoundBorder(new EtchedBorder(), new EmptyBorder(new Insets(3, 3, 3, 3))));
        
        final JRadioButton parityButton = new JRadioButton();
        parityButton.setText(type);
        parityButton.setActionCommand(type);
        parityButton.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
                nSelector.setEnabled(parityButton.isSelected());
                kSelector.setEnabled(parityButton.isSelected());
                Options.setProfile_edited(true);
            }
        });
        modeGroup.add(parityButton);
        
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.gridheight = 2;
        gbc.insets = new Insets(0, 10, 0, 10);
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 1.0;
        parityPanel.add(parityButton, gbc);

        
        JLabel pnl = new JLabel("n");
        gbc = new GridBagConstraints();
        gbc.gridx = 1;
        gbc.gridy = 0;
        gbc.anchor = GridBagConstraints.EAST;
        gbc.weightx = 1.0;
        gbc.weighty = 1.0;
        gbc.insets = new Insets(0, 0, 0, 10);
        parityPanel.add(pnl, gbc);
        
        JLabel pkl = new JLabel("k");
        gbc = new GridBagConstraints();
        gbc.gridx = 1;
        gbc.gridy = 1;
        gbc.anchor = GridBagConstraints.EAST;
        gbc.weightx = 1.0;
        gbc.weighty = 1.0;
        gbc.insets = new Insets(0, 0, 0, 10);
        parityPanel.add(pkl, gbc);

        nSelector.setModel(new SpinnerNumberModel(2, 2, 11, 1));
        nSelector.setEnabled(false);
        nSelector.setMaximumSize(new Dimension(1000, 20));
        nSelector.setPreferredSize(new Dimension(50, 20));
        
        gbc = new GridBagConstraints();
        gbc.gridx = 2;
        gbc.insets = new Insets(0, 0, 0, 10);
        
        parityPanel.add(nSelector, gbc);
        
        kSelector.setModel(new SpinnerNumberModel(1, 1, 10, 1));
        kSelector.setEnabled(false);
        kSelector.setMaximumSize(new Dimension(1000, 20));
        kSelector.setPreferredSize(new Dimension(50, 20));
        
        gbc = new GridBagConstraints();
        gbc.gridx = 2;
        gbc.insets = new Insets(0, 0, 0, 10);
        
        parityPanel.add(kSelector, gbc);
        
        return parityPanel;
    }

    // Componentes que almacenan datos
    private ButtonGroup modeGroup;
    private JSpinner parityNSelector;
    private JSpinner parityKSelector;
    private JSpinner hammingNSelector;
    private JSpinner hammingKSelector;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;
    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);        
        f.getContentPane().add(new ErrorProtectionPanel());
        f.pack();
        f.setVisible(true);
            
    }
    
}// Class PublicNamePanel
