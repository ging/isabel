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
 * InputParameterPanel.java
 *
 * Created on 12 de noviembre de 2003, 16:33
 */

package services.isabel.services.options;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.TitledBorder;

import java.io.File;
import javax.swing.border.CompoundBorder;

/**
 * Panel para introducir los datos de una de las entradas
 * del menu o uno de los servicios del servidor del lim.
 * @author  Fernando Escribano
 */
public class InputParameterPanel extends JPanel {
    
    private JTextField parameterField;
    
    private JTextField valueField;
    
    
    /** Creates a new instance of InputProgramPanel */
    public InputParameterPanel() {
        initComponents();
    }
    
    /**
     * Devuelve el campo del nombre.
     */
    public String getParameterName() {
        return parameterField.getText();
    }
    
    /**
     * Devuelve el campo del programa
     */
    public String getParameterValue() {
        return valueField.getText();
    }
    
    /**
     * Establece el campo del nombre.
     */
    public void setParameterName(String name) {
        parameterField.setText(name);
    }
    
    /**
     * Establece el campo del programa
     */    
    public void setParameterValue(String program) {
        valueField.setText(program);
    }
    
    /**
     * Llamado desde el constructor para inicializar los componentes.
     */
    private void initComponents() {
        setLayout(new GridLayout(4, 1));
        
        JLabel nameLabel = new JLabel("Parameter");
        add(nameLabel);
        
        parameterField = new JTextField();
        add(parameterField);
        
        JLabel valueLabel = new JLabel("Value");
        add(valueLabel);
        
        valueField = new JTextField();
        add(valueField);                
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        InputParameterPanel ip = new InputParameterPanel();
        ip.setParameterName("Hola");
        ip.setParameterValue("Adios");
        Object[] message = {ip};
        int i = JOptionPane.showConfirmDialog(null, message, "Local Parameter", JOptionPane.OK_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE);
        System.out.println(ip.getParameterName());
        System.out.println(ip.getParameterValue());
        System.exit(0);
    }
}
