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
 * AudioConfigurer.java
 *
 * Created on 27 de noviembre de 2003, 12:47
 */

package isabel.sam;

import java.io.*;

import javax.swing.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;

import java.util.Map;
import java.util.Iterator;

/**
 * Pequeña aplicación para crear el fichero de configuracion de audio.
 * Muestra un panel que permite asociar los nombres virtuales de los controles
 * que se muestran en Isabel, con los nombres de los controles reales de la
 * tarjeta de sonido que maneja el programa amixer.
 * @author  Fernando Escribano
 */
public class AudioConfigurer extends JFrame {
    
    /**
     * Espacio entre campos.
     */
    private static final int V_GAP = 10;
    
    /**
     * Espacio entre paneles y bordes de la ventana.
     */
    private static final int BORDER_GAP = 15;
    
    /**
     * Tabla con los datos del fichero de configuración.
     */
    private AudioConfigFile configFile;
    
    /**
     * Nombre del fichero de configuracion.
     */
    private String fileName;
    
    /**
     * Array de nombres de los controles reales.
     */
    private String[] controls;

    /**
     * Numero de tarjeta a configurar.
     */
    private String cardNumber = "0";

    /**
     * Crea un nuevo objeto Audioconfigurer.
     */
    public AudioConfigurer(String fileName, String card) throws IOException {
        this.fileName = fileName;
        cardNumber = card;
        configFile = new AudioConfigFile(fileName);
        controls = getControlNames();
        initComponents();
    }
    
    private JPanel vpanel, rpanel;
    /**
     * Este metodo es llamado desde el contructor para inicializar los componentes
     * visuales.
     */
    private void initComponents() {
                
        // Title con los botones de salvar y cancelar
        getContentPane().add(createTitlePanel(),BorderLayout.NORTH);
        
        // Panel con los botones de salvar y cancelar
        getContentPane().add(createButtonsPane(), BorderLayout.SOUTH);
        
        // Panel para poner los nombre virtuales de los controles
        vpanel = createLabelsPane();
        getContentPane().add(vpanel, BorderLayout.CENTER);
       
        
        // Panel con los selectores.
        rpanel = createSelectorsPane();
        getContentPane().add(rpanel, BorderLayout.EAST);
        
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle("Isabel audio configurer");        
	setLocation(300,300);
        pack();
    }
    

    /** 
     *  Creates the title.
     */
    private JPanel createTitlePanel() {

        JPanel panel = new JPanel();
        panel.setBorder(new EmptyBorder(5,20,5,10));

        JLabel l = new JLabel("Sound Card Configuration:");
	panel.add(l);
	return panel;
    }


    /**
     * Crea el panel con los nombre virtuales de los controles.
     */
    private JPanel createLabelsPane() {
        JPanel panel = new JPanel();
        panel.setBorder(new EmptyBorder(BORDER_GAP, BORDER_GAP, BORDER_GAP, 0));
        panel.setLayout(new GridLayout(configFile.keySet().size(), 1, 0, V_GAP));
        
        // Etiquetas con los nombres virtuales de los controles
        Iterator it = configFile.keySet().iterator();
        JLabel label;
        while (it.hasNext()) {
            label = new JLabel((String)it.next());
            //label.setBorder(new CompoundBorder(new EtchedBorder(), new EmptyBorder(0, 5 ,0, 5)));
            panel.add(label);
        }
            
        return panel;
    }
    
    /**
     * Crea el panel que contiene los botones de salvar y cancelar.
     */
    private JPanel createButtonsPane() {
        JPanel panel = new JPanel();
        panel.setBorder(new EtchedBorder(EtchedBorder.RAISED));
        
        // Los botones alineados a la derecha del panel.
        // Boton de salvar
        JButton saveButton = new JButton("OK");
        saveButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                save();
            }
        });
        panel.add(saveButton);
        
        // Boton de cancelar
        JButton cancelButton = new JButton("CANCEL");
        cancelButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                cancel();
            }
        });        
        panel.add(cancelButton);
        
        //Hago los dos botones del mismo tamaño
        saveButton.setPreferredSize(cancelButton.getPreferredSize());
        
        return panel;
    }
    
    /**
     * Crea el panel con los selectores de control real.
     */
    private JPanel createSelectorsPane() {
        JPanel panel = new JPanel();
        panel.setBorder(new EmptyBorder(BORDER_GAP, BORDER_GAP, BORDER_GAP, BORDER_GAP));
        panel.setLayout(new GridLayout(configFile.keySet().size(), 1, 0, V_GAP));
        
        // Selectores de control real
        JComboBox box;
        
        Iterator it = configFile.keySet().iterator();
        while (it.hasNext()) {
            box = new JComboBox();
            box.setModel(new DefaultComboBoxModel(controls));
            box.setSelectedItem(configFile.get(it.next()));
            panel.add(box);
        }
        
        return panel;
    }
    
    private String[] getControlNames() {
        try {
            AlsaMixer amixer = new AlsaMixer(cardNumber);
            AudioControl[] ctrs = amixer.getAllControls();
            String[] result = new String[ctrs.length];
            for (int i = 0; i < ctrs.length; i++)
                result[i] = ctrs[i].getName();


            return result;        
        }catch (IOException e) {
            System.err.println("Error while executing amixer");
            String[] controls = {"'Master Digital',0", "'Line',0", "'CD',0", "'Mic',0", "'Mono Output',0", "'Analog Loopback',0","'Digital Loopback',0"};
            return controls;
        }
    }
    
    
    /**
     * Método que se ejecuta cuando se pulsa el boton de salvar.
     */
    public void save() {
        configFile.clear();
        for (int i = 0; i < vpanel.getComponentCount(); i++) {
            String virtual =((JLabel)vpanel.getComponent(i)).getText();
            String real = (String)((JComboBox)rpanel.getComponent(i)).getSelectedItem();
            configFile.put(virtual, real);
        }

        try {
            ((AudioConfigFile)configFile).save(fileName);
        }catch (IOException e) {
            System.err.println("Error while writing file " + fileName);
            System.err.println(e);
            System.exit(1);
        }
        System.exit(0);
    }
    
    /**
     * Método que se ejecuta cuando se pulsa el boton de cancelar
     */
    public void cancel() {
        System.exit(0);
    }
    
    
    /**
     * @param args Argumentos de la linea de comando. Solo acepta uno que debe ser
     * el nombre del fichero de configuración.
     */
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: AudioConfigurer <filename> [<cardnumber>]");
            System.exit(1);
        }
        
        // Establezco el Look and Feel y creo la ventana principal
        try {
            UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");            
        }catch (Exception e) {
        }
        try {
            String card = "0";
            if (args.length > 1)
                card = args[1]; 
            new AudioConfigurer(args[0], card).show();
        }catch (IOException e) {
            System.err.println("Error while loading file " + args[0]);
            System.err.println(e);
        }
    }
}
