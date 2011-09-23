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
 * NetworkAccessPanel.java
 *
 * Created on 12 de septiembre de 2003, 17:29
 */

package services.isabel.services.options;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.net.URL;

import xedl.lib.jaxb.ISDN;
import xedl.lib.jaxb.ISDNCalled;
import xedl.lib.jaxb.ISDNCaller;
import xedl.lib.jaxb.ISDNParameters;
import xedl.lib.jaxb.NETWORKACCESS;
import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

//import isabel.lim.HelpBrowser;
/**
 * Esta clase un panel para introducir datos sobre el tipo de acceso de red
 * que dispone el usuario.
 */
public class NetworkAccessPanel extends JPanel implements OptionsStrings, ValidablePanel, XEDLSiteDataPanel {

    /**
     * Numero de canales maximos de ISDN que se pueden configurar.
     */
    private static final int MAX_ISDN_CHANNELS = 8;
    
    /** 
     * Crea un objeto NetworkAccessPanel.
     */
    public NetworkAccessPanel() {
        initComponents();
        setEnabledIsdnFields(1);
    }
    
    /** Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
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
        // Se mira el network access que esta puesto
        if (site.getNETWORKACCESS().isEthernet()) {
            setEthernetAccess();
        }
        else {
            setISDNAccess();
            ISDNParameters isdn = site.getNETWORKACCESS().getISDN().getISDNParameters();
            setISDNActiveChannels(isdn.getISDNChannels());
            if (isdn.isCaller()) {
                setCallerISDNRole();
                setMSN(isdn.getISDNCaller().getISDNPHONEMSN().toArray(new String[0]));
                setPhoneNumbers(isdn.getISDNCaller().getISDNPHONENUMBER().toArray(new String[0]));                
            }
            else {
                setCalledISDNRole();
            }            
        }
    }
    
    /**
     * Guarda los datos del panel en el objeto site que se pasa como par�metro.
     * @param site Objeto xedl.Site donde se almacenar�n los datos.
     */    
    public void saveData(SITE site) {
        site.setNETWORKACCESS(null);
        
        if (isEthernetAccess()) {
            site.setNetworkAccessEthernet();
        }
        else {
        	ISDN isdn = new ISDN();
        	ISDNParameters isdnParams = new ISDNParameters();
        	
            if (isCalledISDNRole()) {
            	ISDNCalled called = new ISDNCalled();
            	called.setISDNCHANNELS(Integer.parseInt(getISDNActiveChannels()));
                isdnParams.setISDNCalled(called);
            }
            else {
            	ISDNCaller caller = new ISDNCaller();
                String[] msn = getMSN();
                for (int i = 0; i < msn.length; i++)
                    if(!msn[i].equals(""))
                    	caller.getISDNPHONEMSN().add(msn[i]);
                String[] phone = getPhoneNumbers();
                for (int i = 0; i < phone.length; i++)
                    if(!phone[i].equals(""))
                    	caller.getISDNPHONENUMBER().add(phone[i]);
                
                caller.setISDNCHANNELS(Integer.parseInt(getISDNActiveChannels()));
                isdnParams.setISDNCaller(caller);
            }
            isdn.setISDNParameters(isdnParams);
            NETWORKACCESS net = new NETWORKACCESS();
            net.setISDN(isdn);
            site.setNETWORKACCESS(net);
        }
    }
    
    /**
     * Devuelve el panel a sus valores por defecto.
     */
    public void resetData() {
        setEthernetAccess();
    }
    
    
    /***************************************************************************
     *           METODOS SET Y GET DE DATOS DEL PANEL NETWORKSACCESS           *
     ***************************************************************************/
    

    public boolean isEthernetAccess() {
        return  ethernetButton.isSelected();
    }
    
    public void setEthernetAccess() {
        ethernetButton.doClick();
    }
    
    public boolean isISDNAccess() {
        return isdnButton.isSelected();
    }
    
    public void setISDNAccess() {
        isdnButton.doClick();
    }

//    public boolean isModemAccess() {
//        return  modemButton.isSelected();
//    }
//    
//    public void setModemAccess() {
//         modemButton.setSelected(true);
//    }
    
    public boolean isCallerISDNRole() {
        return callerButton.isSelected();
    }
    
    public void setCallerISDNRole() {
        callerButton.doClick();
    }

    public boolean isCalledISDNRole() {
        return calledButton.isSelected();
    }
    
    public void setCalledISDNRole() {
        calledButton.doClick();
    }
    
    public String getISDNActiveChannels() {
        return  activeChannelsField.getText();
    }
    
    public void setISDNActiveChannels(int number) {
         activeChannelsField.setText("" + number);
         setEnabledIsdnFields(number);
    }

    public String[] getMSN() {
        String [] result = new String[MAX_ISDN_CHANNELS];
        for (int i = 0; i < result.length; i++)
            result[i] =  msnFields[i].getText();
        return result;
    }
    
    public void setMSN(String[] msns) {
        for (int i = 0; i < msns.length; i++)
             msnFields[i].setText(msns[i]);
    }
         
    public String[] getPhoneNumbers() {
        String [] result = new String[MAX_ISDN_CHANNELS];
        for (int i = 0; i < result.length; i++)
            result[i] =  phoneFields[i].getText();
        return result;
    }
    
    public void setPhoneNumbers(String[] phones) {
        for (int i = 0; i < phones.length; i++)
             phoneFields[i].setText(phones[i]);
    }

    /**
     *  Crea un JTextField y lo configura tal y como se necesita
     *  para el panel de ISDN.
     */
    private JTextField createISDNField() {
        JTextField field = new JTextField();
        field.setColumns(9);
        return field;
    }
    
//    /**
//     * M�todo ejecutado cuando se pulsa el boton de ayuda.
//     */
//    private void helpButtonActionPerformed(ActionEvent evt) {
//        if (helpBrowser == null)
//            helpBrowser = new HelpBrowser(NETWORK_HELP_HOME);
//        helpBrowser.show();
//    }
    
    /**
     * Este m�todo crea todos los componentes que forman el panel.
     */
    private void initComponents() {

        GridBagConstraints gridBagConstraints;

        // Grupos de botones para hacer que solo uno pueda seleccionarse
        ButtonGroup modeGroup = new ButtonGroup();            
        ButtonGroup isdnGroup = new ButtonGroup();

        // Configuracion del panel principal
        setLayout(new BorderLayout());
        setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                     new EmptyBorder(new Insets(10,10,10,10))));

        // Panel para la etiqueta del titulo y el boton de ayuda
        JPanel titlePane = new JPanel();
        titlePane.setLayout(new BoxLayout(titlePane, BoxLayout.X_AXIS));
        add(titlePane, BorderLayout.NORTH);

        // Etiqueta de titulo
        JLabel titleLabel = new JLabel();
        titleLabel.setFont(new Font(TITLE_FONT, Font.BOLD, 24));
        titleLabel.setForeground(TITLE_COLOR);
        titleLabel.setText(NETWORK_ACCESS_TITLE);
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
        
        // Etiqueta con el icono
        JLabel iconLabel = new JLabel();
        iconLabel.setIcon(new ImageIcon(getClass().getResource(NETWORK_ACCESS_ICON)));
        iconLabel.setBorder(new CompoundBorder(new EtchedBorder(EtchedBorder.RAISED),
                                               new EtchedBorder()));
        add(iconLabel, BorderLayout.WEST);

        // Panel que contiene los botones para seleccionar el tipo de acceso
        // y la configuracion de ISDN
        JPanel modePane = new JPanel();
        modePane.setLayout(new BorderLayout());
        modePane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        add(modePane, BorderLayout.CENTER);

        // Panel con los botones            
        JPanel radioButtonsPane = new JPanel();
        radioButtonsPane.setLayout(new GridLayout(0, 1));
        radioButtonsPane.setBorder(new CompoundBorder(new EtchedBorder(),
                                                      new TitledBorder("Choose One")));
        modePane.add(radioButtonsPane, BorderLayout.WEST);

        // Radio Buttons para seleccionar el tipo de acceso            
        ethernetButton = new JRadioButton();           
        isdnButton = new JRadioButton();
        //modemButton = new JRadioButton();

        ethernetButton.setText("Ethernet  ");
        modeGroup.add(ethernetButton);
        ethernetButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                ethernetButtonActionPerformed(evt);
            }
        });
        radioButtonsPane.add(ethernetButton);

        isdnButton.setText("ISDN");
        modeGroup.add(isdnButton);
        isdnButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                isdnButtonActionPerformed(evt);
            }
        });
        radioButtonsPane.add(isdnButton);

//        modemButton.setText("Modem");
//        modeGroup.add(modemButton);
//        modemButton.addActionListener(new ActionListener() {
//            public void actionPerformed(ActionEvent evt) {
//                modemButtonActionPerformed(evt);
//            }
//        });
//        radioButtonsPane.add(modemButton);


        // Panel de configuracion de ISDN
        // Contiene un panel para los botones de rol
        // y otro para los numeros
        JPanel isdnPane = new JPanel();
        isdnPane.setLayout(new BorderLayout());
        isdnPane.setBorder(new CompoundBorder(new EtchedBorder(),
                                              new TitledBorder("ISDN Configuration")));
        modePane.add(isdnPane, BorderLayout.CENTER);

        // Panel para los botones del rol ISDN y una etiqueta
        JPanel isdnRolePane = new JPanel();
        isdnRolePane.setBorder(new EtchedBorder());
        isdnPane.add(isdnRolePane, BorderLayout.NORTH);


        // Etiqueta de rol
        JLabel roleLabel = new JLabel();
        roleLabel.setText("ISDN Role:   ");
        isdnRolePane.add(roleLabel);

        // Botones de rol ISDN
        callerButton = new JRadioButton();
        calledButton = new JRadioButton();

        callerButton.setText("Caller");
        isdnGroup.add(callerButton);
        callerButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                callerButtonActionPerformed(evt);
            }
        });            
        isdnRolePane.add(callerButton);

        calledButton.setText("Called");
        isdnGroup.add(calledButton);
        calledButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                calledButtonActionPerformed(evt);
            }
        });
        isdnRolePane.add(calledButton);

        // Panel para los numeros a marcar
        // Contiene tres paneles 1 pana los MSN/EAZ otro par los
        // phone# y otro para elejir el numero de canales activos
        JPanel isdnNumbersPane = new JPanel();
        isdnNumbersPane.setLayout(new BorderLayout());
        isdnNumbersPane.setBorder(new EtchedBorder());
        isdnPane.add(isdnNumbersPane, BorderLayout.WEST);

        // Panel de MSN
        JPanel msnPane = new JPanel();
        msnPane.setLayout(new GridLayout(0, 1));
        isdnNumbersPane.add(msnPane, BorderLayout.CENTER);

        // Etiqueta que pone MSN/EAZ
        JLabel msnLabel = new JLabel();
        msnLabel.setText("MSN/EAZ");
        msnPane.add(msnLabel);

        // Campos para introducir el MSN/EAZ
        msnFields = new JTextField[MAX_ISDN_CHANNELS];
        for (int i = 0; i < MAX_ISDN_CHANNELS; i++) {
            msnFields[i] = createISDNField();
            msnPane.add(msnFields[i]);
        }

        // Panel de phone #
        JPanel phonePane = new JPanel();
        phonePane.setLayout(new GridLayout(0, 1));
        isdnNumbersPane.add(phonePane, BorderLayout.EAST);

        // Etiqueta que phone phone #
        JLabel phoneLabel = new JLabel();
        phoneLabel.setText("Phone #");
        phonePane.add(phoneLabel);

        // Campos para introducir los phone #
        phoneFields = new JTextField[MAX_ISDN_CHANNELS];
        for (int i = 0; i < MAX_ISDN_CHANNELS; i++) {
            phoneFields[i] = createISDNField();
            phonePane.add(phoneFields[i]);
        }

        // Panel para elejir el numero de canales activos
        // Contiene un campo con el numero
        // dos botones para subirlo y bajarlo
        // y una etiqueta con el ancho de banda
        JPanel activeChannelsPane = new JPanel();
        activeChannelsPane.setLayout(new GridBagLayout());
        activeChannelsPane.setBorder(new EtchedBorder());
        isdnPane.add(activeChannelsPane, BorderLayout.CENTER);

        // Campo con el numero de canales activos
        activeChannelsField = new JTextField();
        activeChannelsField.setColumns(3);        
        activeChannelsField.setHorizontalAlignment(JTextField.TRAILING);
        activeChannelsField.setText("1");
        activeChannelsField.setEditable(false);
        activeChannelsField.setForeground(Color.BLACK);
        activeChannelsField.setToolTipText(ACTIVE_ISDN_CHANNELS_HELP);
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = GridBagConstraints.BOTH;
        activeChannelsPane.add(activeChannelsField, gridBagConstraints);

        // Botones para subir y bajar el numero de canales activos
        activeChannelsUpButton = new JButton();
        activeChannelsDownButton = new JButton();
        Image up = new ImageIcon(getClass().getResource(SMALL_UP_ICON)).getImage()
                   .getScaledInstance(11, 11, Image.SCALE_SMOOTH);
        
        activeChannelsUpButton.setIcon(new ImageIcon(up));
        activeChannelsUpButton.setMargin(new Insets(0,0,0,0));
        activeChannelsUpButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                activeChannelsUpButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        activeChannelsPane.add(activeChannelsUpButton, gridBagConstraints);

        Image down = new ImageIcon(getClass().getResource(SMALL_DOWN_ICON)).getImage()
                   .getScaledInstance(11, 11, Image.SCALE_SMOOTH);        
        activeChannelsDownButton.setIcon(new ImageIcon(down));
        activeChannelsDownButton.setMargin(new Insets(0,0,0,0));
        activeChannelsDownButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                activeChannelsDownButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        activeChannelsPane.add(activeChannelsDownButton, gridBagConstraints);
        
        // Etiqueta que pone channels #
        JLabel activeChannelsLabel = new JLabel();
        activeChannelsLabel.setFont(new Font("Dialog", 1, 10));
        activeChannelsLabel.setText("Channels #");
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        activeChannelsPane.add(activeChannelsLabel, gridBagConstraints);
        
        // Etiqueta con el ancho de banda
        bitRateLabel = new JLabel();
        bitRateLabel.setText("64 Kbps");
        bitRateLabel.setBorder(new EmptyBorder(new Insets(5, 5, 5, 5)));
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = GridBagConstraints.BOTH;
        activeChannelsPane.add(bitRateLabel, gridBagConstraints);

        // Pongo los botones seleccionados por defecto
        ethernetButton.setSelected(true);        
        callerButton.setSelected(true);
        enableISDN(false);
    }

    /**
     * M�todo que se ejecuta cuando se pulsa el boton de bajar el numero
     * de canales ISDN activos.
     * Baja el numero que aparece en el panel y actualiza los campos que
     * se muestran.
     */
    private void activeChannelsDownButtonActionPerformed(ActionEvent evt) {
        int i = 2;
        try {
            i = Integer.parseInt(activeChannelsField.getText());
            if (i == 1) i = 2;
        }catch (NumberFormatException e) {
        }
        activeChannelsField.setText(--i + "");
        bitRateLabel.setText(i * 64 + " Kbps");
        if (isCallerISDNRole())
            setEnabledIsdnFields(i);
        else
            setEnabledIsdnFields(0);
    }

    /**
     * M�todo que se ejecuta cuando se pulsa el boton de subir el numero
     * de canales ISDN activos.
     * Sube el numero que aparece en el panel y actualiza los campos que
     * se muestran.
     */
    private void activeChannelsUpButtonActionPerformed(ActionEvent evt) {
        int i = 1;
        try {
            i = Integer.parseInt(activeChannelsField.getText());
            if (i == MAX_ISDN_CHANNELS) i = MAX_ISDN_CHANNELS - 1;
        }catch (NumberFormatException e) {
        }
        activeChannelsField.setText(++i + "");
        bitRateLabel.setText(i * 64 + " Kbps");
        if (isCallerISDNRole())
            setEnabledIsdnFields(i);
        else
            setEnabledIsdnFields(0);
    }
    
    /**
     * Este metodo establece el numero de campos de numeros de ISDN
     * que se muestran.
     */
    private void setEnabledIsdnFields(int n) {
        for (int i = 0; i < MAX_ISDN_CHANNELS; i++) {
            msnFields[i].setVisible(false);
            phoneFields[i].setVisible(false);
        }

        for (int i = 0; i < n; i++) {
            msnFields[i].setVisible(true);
            phoneFields[i].setVisible(true);
        }
        
        for (int i = n; i < MAX_ISDN_CHANNELS; i++) {
            msnFields[i].setText("");
            phoneFields[i].setText("");
        }
    }

//    /**
//     * Metodo ejecutado cuando se elije modem.
//     */
//    private void modemButtonActionPerformed(ActionEvent evt) {
//        enableISDN(false);
//    }

    /**
     * Metodo ejecutado cuando se elije isdn.
     */
    private void isdnButtonActionPerformed(ActionEvent evt) {
        enableISDN(true);
    }

    /**
     * Metodo ejecutado cuando se elije ethernet.
     */
    private void ethernetButtonActionPerformed(ActionEvent evt) {
        enableISDN(false);
    }

    /**
     * Activa o desactiva los campos referentes al ISDN.
     */
    private void enableISDN(boolean enable) {
        for (int i = 0; i < MAX_ISDN_CHANNELS; i++) {
            msnFields[i].setEnabled(enable);
            phoneFields[i].setEnabled(enable);
        }
        
        activeChannelsField.setEnabled(enable);
        activeChannelsDownButton.setEnabled(enable);
        activeChannelsUpButton.setEnabled(enable);
        callerButton.setEnabled(enable);
        calledButton.setEnabled(enable);
    }
    
    /**
     * Metodo ejecutado cuando se elije called.
     */
    private void calledButtonActionPerformed(ActionEvent evt) {
        setEnabledIsdnFields(0);
    }

    /**
     * Metodo ejecutado cuando se elije caller.
     */
    private void callerButtonActionPerformed(ActionEvent evt) {
        setEnabledIsdnFields(Integer.parseInt(getISDNActiveChannels()));
    }

    public static void main(String[] args) {
        JFrame f = new JFrame();
        f.setDefaultCloseOperation(f.EXIT_ON_CLOSE);
        f.getContentPane().add(new NetworkAccessPanel());
        f.pack();
        f.setVisible(true);
    }
        
    // Componentes visuales que guardan datos
    private JTextField[] msnFields;
    private JTextField[] phoneFields;
    private JTextField activeChannelsField;
    private JLabel bitRateLabel;

    private JRadioButton ethernetButton;
    private JRadioButton isdnButton;
//    private JRadioButton modemButton;
    private JRadioButton callerButton;
    private JRadioButton calledButton;
    
    private JButton activeChannelsDownButton;
    private JButton activeChannelsUpButton;
    
    // Ventana para mostrar la ayuda
//    private JFrame helpBrowser;


} // Class NetworkAccessPanel
