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
//********************************************************************
//
//FILE:    graphConfigFrame_t.java
//
//DESCRIPTION:
//    This file is the frmae that lets us configurate different options
//of the graphics as number of points on screen reescale method...
//

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

class graphConfigFrame_t extends JDialog {

    //Attributes:
    //-----------
    graphicsFrame_t gFrame;
    boolean         showLastValue;
    boolean         showX_Axis,showY_Axis;
    boolean         zero_YAxis;
    int             pointsOnScreen;
    int             timeFactor;
    JCheckBox       ValueLabelCBox;
    JCheckBox       AxisYCBox,AxisXCBox;
    JCheckBox       ZeroYAxis;
    JSlider         pointsOnScreenSlider;

    //Constructor:
    //------------
    graphConfigFrame_t(graphicsFrame_t gFrame) {
        super(gFrame,"Configuration for the graphic:",true);
        this.gFrame = gFrame;
        this.showLastValue = gFrame.getShowLastValue();
        this.showY_Axis = gFrame.getShowYAxis();
        this.showX_Axis = gFrame.getShowXAxis();
        this.zero_YAxis = gFrame.getZeroYAxis();
        this.pointsOnScreen = pointsOnScreen;
        timeFactor = 5;
        Component contents = createComponents();
        getContentPane().add(contents,BorderLayout.CENTER);
        pack();
        show();
    }

    //Function that creates all the visual elements in the
    //the frame:
    //----------------------------------------------------
    protected Component createComponents() {
        JPanel panel = new JPanel();
        panel.setLayout( new BoxLayout(panel,BoxLayout.Y_AXIS));

        //1.- Rescale, Axis and Last Value Label:
        //---------------------------------------
        JPanel scalePanel = new JPanel();
        scalePanel.setLayout(new GridLayout(5,0));
        scalePanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(new Color(0,0,0)),"Options: "));
        ValueLabelCBox = new JCheckBox ("Show last value",showLastValue);
        AxisYCBox = new JCheckBox ("Show vertical axis",showY_Axis);
        AxisXCBox = new JCheckBox ("Show time axis",showX_Axis);
        ZeroYAxis = new JCheckBox ("Vertical axis always starts in zero",zero_YAxis);
        scalePanel.add(ValueLabelCBox);
        scalePanel.add(AxisYCBox);
        scalePanel.add(AxisXCBox);
        scalePanel.add(ZeroYAxis);
        //2.- Points on screen:
        //---------------------
        JPanel pointsPanel = new JPanel();
        pointsPanel.setLayout(new BoxLayout(pointsPanel,BoxLayout.Y_AXIS));
        pointsOnScreenSlider = new JSlider (JSlider.HORIZONTAL,0,100,gFrame.getPointsOnScreen());
        Hashtable labels = createLabelForSlider(timeFactor);
        pointsOnScreenSlider.setLabelTable(labels);
        pointsOnScreenSlider.setMajorTickSpacing(10);
        pointsOnScreenSlider.setMinorTickSpacing(5);
        pointsOnScreenSlider.setPaintTicks(true);
        pointsOnScreenSlider.setPaintLabels(true);
        pointsOnScreenSlider.setBorder(BorderFactory.createEmptyBorder(0,0,10,0));
        pointsPanel.add(pointsOnScreenSlider);
        pointsPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(new Color(0,0,0))," Time interval to show (last secs.): "));

        //3.- OK and Cancel buttons:
        //--------------------------
        JPanel buttonsPanel = new JPanel();
        ButtonListener myButtonListener = new ButtonListener(this);
        JButton BOK = new JButton("OK");
        BOK.addActionListener(myButtonListener);
        JButton BCancel = new JButton ("Cancel");
        BCancel.addActionListener(myButtonListener);
        buttonsPanel.setBorder(BorderFactory.createLineBorder(new Color(0,0,0)));
        buttonsPanel.setLayout(new FlowLayout(FlowLayout.CENTER));
        buttonsPanel.add(BOK);
        buttonsPanel.add(BCancel);

        panel.add(scalePanel);
        panel.add(pointsPanel);
        panel.add(buttonsPanel);
        return panel;
    }
    Hashtable createLabelForSlider (int tFactor) {
        Hashtable result = new Hashtable();
        for (int i =0 ; i<=100 ; i=i+10) {
            Integer value = new Integer(i*tFactor);
            result.put (new Integer (i),new JLabel(value.toString()));
        }
        return result;
    }
    // Inner class that listen for the events on the OK and
    //Cancel buttons:
    //-----------------------------------------------------
    class ButtonListener implements ActionListener {
        graphConfigFrame_t confPanel;
        ButtonListener (graphConfigFrame_t confPanel) {
            this.confPanel = confPanel;
        }
        public void actionPerformed (ActionEvent e) {
            JButton source = (JButton)e.getSource();
            if (source.getText().equals("OK")) {
                gFrame.setShowLastValue(confPanel.ValueLabelCBox.isSelected());
                gFrame.setShowYAxis(confPanel.AxisYCBox.isSelected());
                gFrame.setShowXAxis(confPanel.AxisXCBox.isSelected());
                gFrame.setZeroYAxis(confPanel.ZeroYAxis.isSelected());
                if (confPanel.pointsOnScreenSlider.getValue()<1) gFrame.setPointsOnScreen(1);
                    else gFrame.setPointsOnScreen(confPanel.pointsOnScreenSlider.getValue());
                confPanel.setVisible(false);
                confPanel.dispose();
            }
            if (source.getText().equals("Cancel")) {
                confPanel.setVisible(false);
                confPanel.dispose();
            }
        }
    }
}//EndOfConfigurationFrame
