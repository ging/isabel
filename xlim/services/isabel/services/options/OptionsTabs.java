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
 * OptionsTabs.java
 *
 * Created on 31 de marzo de 2004, 11:47
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

/**
 * Esta clase implementa un panel con tabs en el que estan puesto los
 * diferentes paneles de configuracion. Tiene ademas botones de atras y adelante
 * y botones de done y cancel. Permite mostrar solo un subconjunto de dichos
 * paneles.
 * @author  Fernando Escribano
 */
public class OptionsTabs extends JPanel implements OptionsStrings {
	//ENRIQUE quitado las tabs que tienen que ver con ipsec
	
    // Paneles para introducir los distintos tipos de datos
    public PublicNamePanel publicNamePane;
    public AdministrativeInfoPanel adminInfoPane;
    public RolePanel rolePane;
    public BandwidthPanel bandwidthPane;
    public MulticastPanel multicastPane;
    public MediaControlsPanel mediaControlsPane;
    public AditionalParametersPanel parametersPane;
    //public IPSecPanel ipsecPane;
    //public ErrorProtectionPanel fecPane;
    //public CertifiedIdentityPanel certPane;
    //public NetworkAccessPanel networkPane;
    public ParityPanel parityPane;

    // Listener que responde a los botones de cancel y done
    private OptionsPanelListener listener;
    
    // Panel con etiquetas donde se colocan los componentes
    private JTabbedPane tabbedPane;
    
    // Botones de la aplicacion
    private JButton nextButton;
    private JButton backButton;
    private JButton cancelButton;
    private JButton doneButton;
    private JButton resetButton;
    
    private Map basics;
    private Map advanced;
    //private Map ipsec;
    private Map all;
    
    /** Creates a new instance of OptionsTabs */
    public OptionsTabs() {
        initComponents();
        enableButtons();        

        basics = new TreeMap();
        basics.put("aSite ID", publicNamePane);
        basics.put("bAdmin. Info", adminInfoPane);
        
        advanced = new TreeMap();
        advanced.put("aSite ID", publicNamePane);
        advanced.put("bRole", rolePane);
        //advanced.put("cBW", bandwidthPane);
        //advanced.put("dISDN", networkPane);
        advanced.put("cMulticast", multicastPane);
        advanced.put("fMedia Ctrls", mediaControlsPane);
        advanced.put("gParameters", parametersPane);
        //advanced.put("8IPSec", ipsecPane);
        //advanced.put("9Cert. Id", certPane);
        //advanced.put("iF.E.C.", fecPane);
        advanced.put("hF.E.C.", parityPane);
        advanced.put("iAdmin. Info", adminInfoPane);
        /*
        ipsec = new TreeMap();
        ipsec.put("aSite ID", publicNamePane);
        ipsec.put("bAdmin. Info", adminInfoPane);        
        ipsec.put("cCert. Id", certPane);
        ipsec.put("dIPSec", ipsecPane);
        */
        
        all = new TreeMap();
        all.put("aSite ID", publicNamePane);
        all.put("bRole", rolePane);
        //all.put("dBW", bandwidthPane);
        //all.put("eISDN", networkPane);        
        all.put("cMulticast", multicastPane);
        all.put("fMedia Ctrls", mediaControlsPane);
        all.put("gParameters", parametersPane);
        //all.put("iF.E.C.", fecPane);        
        all.put("hF.E.C.", parityPane);
        all.put("iAdmin. Info", adminInfoPane);
        //all.put("jCert. Id", certPane);        
        //all.put("kIPSec", ipsecPane);
        
    }
    
    public void showBasics() {
        showTabs(basics);
    }
    
    public void showAdvanced() {
        showTabs(advanced);
    }
    
    /*
    public void showIpsec() {
        showTabs(ipsec);
    }
    */
    
    public void showAll() {
        showTabs(all);
    }
    
    /**
     * Devuelve un conjunto con todos los paneles de datos del xedl.
     * @return Conjunto de objetos que implementan XEDLSiteDataPanel
     */
    public Collection getXEDLSiteDataPanels() {
        return all.values();
    }
    
    private void showTabs(Map tabs) {
        tabbedPane.removeAll();
        Iterator it = tabs.keySet().iterator();
        while(it.hasNext()) {
            String title = (String)it.next();
            tabbedPane.addTab(title.substring(1), (JComponent)tabs.get(title));
        }
        enableButtons();        
    }
    
    /**
     * M�todo para establecer el listener que atiende a los eventos de los
     * botones de cancel y done. Sustiuye al anterior listener establecido si
     * hubiera alguno.
     * @param lis Listener que se quiere establecer.
     */
    public void setListener(OptionsPanelListener lis) {
        listener = lis;
    }
    
    /**************************************************************************
     *                        METODOS DE ACCION DE LOS BOTONES                *
     **************************************************************************/
    
    /**
     * Metodo que se ejecuta cuando es pulsado el boton de atras (back)
     * Cambia la tab seleccionada.
     */
    private void backButtonActionPerformed(ActionEvent evt) {
        nextButton.setEnabled(true);
        tabbedPane.setSelectedIndex(tabbedPane.getSelectedIndex() - 1);
    }
    
    /**
     * Metodo que se ejecuta cuando es pulsado el boton de atras (back)
     * Cambia la tab seleccionada.
     */
    private void nextButtonActionPerformed(ActionEvent evt) {
        // Valido los datos
        JPanel tab = (JPanel)tabbedPane.getSelectedComponent();
        ValidablePanel pane = (ValidablePanel)tab;
        
        // Si son validos paso al siguiente panel
        if (pane.validateData()) {        
            backButton.setEnabled(true);
            tabbedPane.setSelectedIndex(tabbedPane.getSelectedIndex() + 1);
        }        
    }

    /**
     * M�todo que se ejecuta cuando se pulsa el boton de done.
     * Simplemente avisa al listener si existe.
     */
    private void resetButtonActionPerformed(ActionEvent evt) {
        if (listener != null)
            listener.resetButtonPressed();        
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de done.
     * Simplemente avisa al listener si existe.
     */
    private void doneButtonActionPerformed(ActionEvent evt) {
        if (listener != null)
            listener.doneButtonPressed();
    }
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de cancel.
     * Simplemente avisa al listener si existe.
     */
    private void cancelButtonActionPerformed(ActionEvent evt) {
        if (listener != null)
            listener.cancelButtonPressed();
    }
    
    /**
     * M�todo que dependiendo de la tab seleccionda activa o desactiva los botones
     * de cambio de tab (back y next).
     */
    private void enableButtons() {
        if (tabbedPane.getSelectedIndex() == tabbedPane.getTabCount() - 1)
            nextButton.setEnabled(false);
        else
            nextButton.setEnabled(true);
        if (tabbedPane.getSelectedIndex() == 0)
            backButton.setEnabled(false);
        else
            backButton.setEnabled(true);
    }
    /***************************************************************************
     *                    METODOS PARA CONSTRUIR EL PANEL PRINCIPAL            *
     ***************************************************************************/
        
    /**
     * Este m�todo es llamado desde el constructor para inizializar los componentes
     * visuales que forman el panel.
     */
    private void initComponents() {
        
        // Pongo el Layout del panel
        setLayout(new BorderLayout());
        
        
        // TabbedPane que contiene los paneles para los datos
        tabbedPane = new JTabbedPane();
        tabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
        add(tabbedPane, BorderLayout.CENTER);
        
        // Panel para introducir el public name
        publicNamePane = new PublicNamePanel();
        
        // Panel para introducir los datos de informacion administrativa
        adminInfoPane = new AdministrativeInfoPanel();
        
        // Panel para introducir los datos del role
        rolePane = new RolePanel();
        
        // Panel para introducir los datos de Bandwidth
        bandwidthPane = new BandwidthPanel();
        
        // Panel para introducir los datos de los grupos multicast
        multicastPane = new MulticastPanel();
        
        // Panel para introducir los datos de los displays de control
        mediaControlsPane = new MediaControlsPanel();
        
        // Panel para introducir los parametros adicionales
        parametersPane = new AditionalParametersPanel();

        // Panel para introducir los parammetros de IPSEC
        //ipsecPane = new IPSecPanel();
                
        //Panel con los datos sobre fec
        //fecPane = new ErrorProtectionPanel();
        
        //Panel con los datos sobre parity
        parityPane = new ParityPanel();
        
        // Panel de Certified Identity
        //certPane = new CertifiedIdentityPanel();

        // Panel con loas datos de acceso de red
        //networkPane = new NetworkAccessPanel();
        
        // Panel con los botones control de la aplicacion
        JPanel buttonPane = new JPanel();       
        buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
        buttonPane.setBorder(new CompoundBorder(new SoftBevelBorder(BevelBorder.RAISED),
                                                new EmptyBorder(new Insets(1, 10, 1, 10))));
        add(buttonPane, BorderLayout.SOUTH);

        // Boton de atras
        backButton = new JButton();
        backButton.setIcon(new ImageIcon(getClass().getResource(BACK_ICON)));
        backButton.setToolTipText(BACK_HELP);
        backButton.setEnabled(false);
        backButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                backButtonActionPerformed(evt);
            }
        });
        buttonPane.add(backButton);

        // 10 Pixeles entre un boton y otro
        buttonPane.add(Box.createHorizontalStrut(10));
        
        // Boton de adelante
        nextButton = new JButton();
        nextButton.setIcon(new ImageIcon(getClass().getResource(FORWARD_ICON)));
        nextButton.setToolTipText(NEXT_HELP);
        nextButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                nextButtonActionPerformed(evt);
            }
        });
        buttonPane.add(nextButton);
        
        // Los otros dos botones en el otro extremo del panel
        buttonPane.add(Box.createHorizontalGlue());

        // Boton de reset
        resetButton = new JButton();
        resetButton.setText("Restore default values");
        resetButton.setToolTipText(RESET_HELP);
        resetButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                resetButtonActionPerformed(evt);
            }
        });
        buttonPane.add(resetButton);
        
        // 10 Pixeles entre un boton y otro
        buttonPane.add(Box.createHorizontalStrut(10));
        
        // Boton de finalizar
        doneButton = new JButton();
        doneButton.setText("Done");
        doneButton.setToolTipText(DONE_HELP);
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                doneButtonActionPerformed(evt);
            }
        });
        buttonPane.add(doneButton);
        
        // 10 Pixeles entre un boton y otro
        buttonPane.add(Box.createHorizontalStrut(10));
        
        // Boton de cancelar
        cancelButton = new JButton();
        cancelButton.setText("Cancel");
        cancelButton.setToolTipText(CANCEL_HELP);
        cancelButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                cancelButtonActionPerformed(evt);
            }
        });
        buttonPane.add(cancelButton);
        
        // A�ado el listener al TabbedPane para que responda a los cambios de tab
        // seleccionada
        tabbedPane.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
                enableButtons();
            }
        });
    }
    
    /**
     * M�todo de prueba del GUI.
     */
    public static void main (String [] args) {
        JFrame.setDefaultLookAndFeelDecorated(true);
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

        JFrame frame = new JFrame();
        frame.setTitle(OPTIONS_TITLE);
        frame.setResizable(false);
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent evt) {
                System.exit(0);
            }
        });
        
        OptionsTabs ot = new OptionsTabs();
        ot.showAll();
        frame.getContentPane().add(ot);
        frame.pack();
        frame.setVisible(true);
    }    
}
