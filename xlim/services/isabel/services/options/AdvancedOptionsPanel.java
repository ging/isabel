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
 * AdvancedOptionsPanel.java
 *
 * Created on 12 de septiembre de 2003, 17:29
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;

/**
 * Panel principal para introducir todos los parametros opcionales de un
 * sitio de la aplicacion Isabel.
 * Consta de un panel principal que contiene 5 subpaneles seleccionables
 * mediante etiquetas donde se encuentran los distintos datos.
 * @author  Fernando Escribano
 */
public class AdvancedOptionsPanel extends JPanel implements OptionsStrings, ConnectionModePanelListener {    
        
    // Paneles para introducir los distintos tipos de datos
    public PublicNamePanel publicNamePane;
    public AdministrativeInfoPanel adminInfoPane;
    public SiteIDPanel siteIDPane;
    public RolePanel rolePane;
    public BandwidthPanel bandwidthPane;    
    public NetworkAccessPanel networkAccessPane;
    public ConnectionModePanel connectionModePane;
    public MulticastPanel multicastPane;
    public MediaControlsPanel mediaControlsPane;
    public AditionalParametersPanel parametersPane;

    // Listener que responde a los botones de cancel y done
    private OptionsPanelListener listener;
    
    // Panel con etiquetas donde se colocan los componentes
    private JTabbedPane tabbedPane;
    
    // Botones de la aplicacion
    private JButton nextButton;
    private JButton backButton;
    private JButton cancelButton;
    private JButton doneButton;
        
    /**
     * Contruye un objeto OptionsPanel.
     */
    public AdvancedOptionsPanel() {
        initComponents();
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
    /***************************************************************************
     *           METODOS DEL INTERFAZ ConnectionModePanelListener              *
     ***************************************************************************/
    /**
     * Este metodo se ejecuta cuando se selecciona el modo de conexion.
     * Activa los campos del panel de grupos multicast
     * @param multi True si es multicast y false en otro caso.
     */
    public void multicastSelected(boolean multi) {
        //multicastGroupsPane.enableFields(multi);
    }
    
    /**
     * Este m�todo se ejecuta cuando es pulsado el boton de configuracion
     * de los grupos multicast.
     * Selecciona la tab de los grupos multicast en el panel.
     */
    public void configButtonPressed() {
        //tabbedPane.setSelectedComponent(multicastGroupsPane);
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
        tabbedPane.addTab("P. Name", publicNamePane);
        
        // Panel para introducir los datos de informacion administrativa
        adminInfoPane = new AdministrativeInfoPanel();
        tabbedPane.addTab("Admin. Info", adminInfoPane);

//        // Panel para introducir los datos de Site Identification
//        siteIDPane = new SiteIDPanel();
//        tabbedPane.addTab("Site ID", siteIDPane);
        
        // Panel para introducir los datos del role
        rolePane = new RolePanel();
        tabbedPane.addTab("Role", rolePane);

//        // Panel para introducir los datos de Network Access
//        networkAccessPane = new NetworkAccessPanel();
//        tabbedPane.addTab("Network", networkAccessPane);
                
//        // Panel para introducir los datos de Connection Mode
//        connectionModePane = new ConnectionModePanel();
//        connectionModePane.setListener(this);
//        tabbedPane.addTab("Conn. Mode", connectionModePane);
        
        // Panel para introducir los datos de Bandwidth
        bandwidthPane = new BandwidthPanel();
        tabbedPane.addTab("BW", bandwidthPane);
        
        // Panel para introducir los datos de los grupos multicast
        multicastPane = new MulticastPanel();
        tabbedPane.addTab("Multicast", multicastPane);
        
        // Panel para introducir los datos de los displays de control
        mediaControlsPane = new MediaControlsPanel();
        tabbedPane.addTab("Media Ctrls", mediaControlsPane);
        
        // Panel para introducir los parametros adicionales
        parametersPane = new AditionalParametersPanel();
        tabbedPane.addTab("Ad. Params", parametersPane);

               
        // Panel con los botones control de la aplicacion
        JPanel buttonPane = new JPanel();       
        buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
        buttonPane.setBorder(new CompoundBorder(new SoftBevelBorder(BevelBorder.RAISED),
                                                new EmptyBorder(new Insets(1, 10, 1, 10))));
        add(buttonPane, BorderLayout.SOUTH);

        // Boton de atras
        backButton = new JButton();
        //backButton.setText("Back");
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
        //nextButton.setText("Next");
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
        
        // Boton de finalizar
        doneButton = new JButton();
        doneButton.setText("Done");
        //doneButton.setIcon(new ImageIcon(getClass().getResource(DONE_ICON)));
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
        //cancelButton.setIcon(new ImageIcon(getClass().getResource(CANCEL_ICON)));
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
        
        frame.getContentPane().add(new AdvancedOptionsPanel());
        frame.pack();
        frame.show();
    }
}
