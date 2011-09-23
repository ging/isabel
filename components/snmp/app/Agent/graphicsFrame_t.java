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
// ********************************************************************
//
// FILE:    graphicsFrame_t.java
//
// DESCRIPTION:
//     This file contains all the class needed when drawing graphics
// of the evolution of some data.
//

import java.util.*;
import java.text.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import javax.swing.JFrame;
import javax.swing.JMenu.*;

public class graphicsFrame_t extends FrameRef {

    // Attributes:
    // -----------
    DataBaseHandler_t DataBaseHandler;
    threadsManager_t  threadsManager;
    String            dataName;
    sck.Oid           requestedOid;
    boolean           showLastValue = true;

    // Attributes concerning with the graph:
    // -------------------------------------
    graph.Graph2D     graphic;
    graph.DataSet     data;
    graph.Axis        yaxis_right;
    graph.Axis        xaxis_down;
    int               period = 500;
    int               maximum = 25;
    drawData          drawDataThread;
    double            factor;

    // Graphic elements:
    // -----------------
    JLabel            lastValueLabel;
    JLabel            lastValueUnits;
    JLabel            lastValueTitle;

    // Constructor:
    // ------------
    graphicsFrame_t(DataBaseHandler_t DataBaseHandler,
                    threadsManager_t  threadsManager,
                    sck.Oid           rqOid,
                    String            title,
                    String            conf,
                    String            units
                   ) {
        super (title);

        this.DataBaseHandler = DataBaseHandler;
        this.threadsManager  = threadsManager;
        this.requestedOid    = rqOid;
        initFactor(conf);

        // This lets the threadsManager know that exits a new frame.
        threadsManager.insertFrame(this);

        try {
            data = new graph.DataSet();
        } catch (Exception exc) {
            System.out.println("Excepcion capturada:"+exc);
        };

        // A.- Graphics Initializations:
        // -----------------------------

        // 1.- Panel that contains the last value in the graphic:
        // ------------------------------------------------------------
        lastValueTitle = new JLabel(title + " last value: ");
        lastValueLabel = new JLabel("",10);
        lastValueUnits = new JLabel();
        if (units.equals("Kb/S")) {
            lastValueUnits.setText(" Kbits/sg");
        }
        if (units.equals("FR/S")) {
            lastValueUnits.setText(" Frames/sg");
        }
        if (units.equals("PKT/S")) {
            lastValueUnits.setText(" Packets/sg");
        }
        if (units.equals("PKT")) {
            lastValueUnits.setText(" Packets");
        }
        JPanel labelPanel = new JPanel();
        labelPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
        labelPanel.setLayout(new BoxLayout(labelPanel, BoxLayout.X_AXIS));
        labelPanel.add(lastValueTitle);
        labelPanel.add(lastValueLabel);
        labelPanel.add(lastValueUnits);
        getContentPane().add(labelPanel,BorderLayout.NORTH);

        // 3.- Center panel containing the graphic:
        // ---------------------------------------
        Component contents = createComponents();
        getContentPane().add(contents,BorderLayout.CENTER);

        // 4.- Panel that contains the button bar
        // -------------------------------------------------------------------
        JPanel buttonPanel = createButtonPanel();
        getContentPane().add(buttonPanel,BorderLayout.SOUTH);

        // 5.- Start of the thread that draws the data in the graph2D component:
        // -----------------------------------------------------------------
        drawDataThread = new drawData(DataBaseHandler,
                                      threadsManager,
                                      data,
                                      graphic,
                                      requestedOid,
                                      this
                                     );

        // 6.-Draw this frame:
        // -------------------
        pack();
        setSize(400,400);
        show();

        addWindowListener(
            new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    drawDataThread.finish();
                }
            }
        );
    }

    // Function to get images from the jar file.
    // -----------------------------------------
    public ImageIcon getJarImages(String imageName) {
        return new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/"+imageName)));
    }

    // Function that eliminates a graphicsFrame and all the resources used by it:
    // --------------------------------------------------------------------------
    public void setFinish(boolean value) {
        if (value) {
            drawDataThread.finish();
            setVisible(false);
            dispose();
        }
    }

    // -------------------------------------------------------------------
    // Function that creates the button panel
    // -------------------------------------------------------------------
    protected JPanel createButtonPanel() {
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new GridLayout(0,3));

        ButtonListener myButtonListener = new ButtonListener(this);

        ImageIcon colorConfIcon = getJarImages("images/colorIcon.gif");
        JButton BColor = new JButton("Color",colorConfIcon);
        BColor.addActionListener(myButtonListener);

        ImageIcon monitorIcon = getJarImages("images/monitorIcon.gif");
        JButton BConf = new JButton("Conf",monitorIcon);
        BConf.addActionListener(myButtonListener);

        JButton BClose = new JButton("Close");
        BClose.addActionListener(myButtonListener);

        buttonPanel.add(BColor);
        buttonPanel.add(BConf);
        buttonPanel.add(BClose);

        return buttonPanel;
    }

    // --------------------------------------------------------------------
    // Function that creates the graphic and add it to the frame
    // --------------------------------------------------------------------
    protected Component createComponents() {
        JPanel graphPanel = new JPanel();
        graphPanel.setLayout(new GridLayout(1,0));
        graphPanel.setBorder(BorderFactory.createLineBorder(new Color(0,0,0)));

        // Graphics;
        graphic = new graph.Graph2D();
        graphic.zerocolor    = new Color(0,255,0); // Zero-axis color.
        graphic.borderTop    = 25;
        graphic.borderBottom = 25;
        graphPanel.add(graphic);

        data.linecolor  = new Color(0,0,255); // Data color.
        data.marker     = 1;
        data.markercolor= new Color(0,0,0);// Subdivision color.

        yaxis_right = graphic.createAxis(graph.Axis.RIGHT);
        yaxis_right.attachDataSet(data);
        yaxis_right.setManualRange(false);

        xaxis_down = graphic.createAxis(graph.Axis.BOTTOM);
        xaxis_down.attachDataSet(data);
        xaxis_down.setManualRange(false);

        graphic.attachDataSet(data);

        return graphPanel;
    }

    // Function that gives a initial value to the unit factors:
    // --------------------------------------------------------
    private void initFactor (String conf) {
        if (conf.equals("BW"))  factor=0.00001;
        if (conf.equals("2R"))  factor=0.01;
        if (conf.equals("INT")) factor=1;
    }

    // Function that return the thread that draws the data in this frame:
    // ------------------------------------------------------------------
    public drawData getdrawDataThread () {
        return drawDataThread;
    }

    // Functions that let change the graphic options:
    // ----------------------------------------------

    // 2.- Show or not the last painted value:
    // ---------------------------------------
    public boolean getShowLastValue() { return lastValueLabel.isVisible();};
    public void setShowLastValue(boolean value) {
        lastValueTitle.setVisible(value);
        lastValueLabel.setVisible(value);
        lastValueUnits.setVisible(value);
    };

    // 3.- Show or not the graphic axis:
    // ---------------------------------
    public boolean getShowYAxis() { return yaxis_right.redraw;};
    public void setShowYAxis (boolean value) { yaxis_right.redraw= value;};

    public boolean getShowXAxis() { return xaxis_down.redraw;};
    public void setShowXAxis (boolean value) { xaxis_down.redraw= value;};

    // 4.- Change the numbre of points on screen:
    // ------------------------------------------
    public int getPointsOnScreen() { return drawDataThread.getPointsOnScreen();};
    public void setPointsOnScreen(int value) {
        drawDataThread.setPointsOnScreen(value);
    }

    // 5.- Fix Y Axis minimum point to zero:
    // -------------------------------------
    public boolean getZeroYAxis () { return drawDataThread.getZeroYAxis();};
    public void setZeroYAxis(boolean value) {
        drawDataThread.setZeroYAxis(value);
    }

    // ------------------------------------------------
    // Inner class that listen to the button events:
    // ------------------------------------------------
    class ButtonListener implements ActionListener {
        graphicsFrame_t gFrame;

        ButtonListener (graphicsFrame_t gFrame) {
            this.gFrame = gFrame;
        }

        public void actionPerformed(ActionEvent e) {
            JButton source = (JButton)(e.getSource());
            if (source.getText().equals("Color")) {
                Color newColor =
                    JColorChooser.showDialog(
                        null,
                        "Choose color for the graphic",
                        drawDataThread.getDataSet().linecolor
                    );
                if (newColor!=null) {
                    drawDataThread.getDataSet().linecolor = newColor;
                }
                return;
            }
            if (source.getText().equals("Conf")) {
                graphConfigFrame_t confPanel = new graphConfigFrame_t(gFrame);
                return;
            }
            if (source.getText().equals("Close")) {
                threadsManager.deleteFrame(gFrame);
                gFrame.setFinish(true);
                return;
            }
        }
    }

}// End of graphicsFrame_t


// -------------------------------------------------
//  This thread is the one that repaint the graphic
//  each 5 seconds adding a new point to the graphic
// -------------------------------------------------
class drawData extends java.lang.Thread {

    // Attributes:
    // -----------
    DataBaseHandler_t DataBaseHandler;
    threadsManager_t  threadsManager;
    graph.DataSet     data;
    graph.Graph2D     graphic;
    sck.Oid           requestedOid;
    boolean           finish;
    double            factor;
    int               pointsOnScreen= 50;
    long              count;
    boolean           changePointsOnScreen= false;
    boolean           zeroYAxis           = false;
    graphicsFrame_t   gFrame;

    // Number format
    // -----------------
    DecimalFormat df;

    // new elements for faster graphics:
    Image             osi    = null;
    Graphics          osg    = null;
    int               iwidth = 0;
    int               iheight= 0;

    // Constructor:
    // ------------
    drawData(DataBaseHandler_t DataBaseHandler,
             threadsManager_t threadsManager,
             graph.DataSet data,
             graph.Graph2D graphic,
             sck.Oid rqOid,
             graphicsFrame_t gFrame
            ) {
        this.DataBaseHandler = DataBaseHandler;
        this.threadsManager  = threadsManager;
        this.graphic         = graphic;
        this.requestedOid    = rqOid;
        this.gFrame          = gFrame;
        this.data            = data;
        this.finish          = false;
        this.factor          = gFrame.factor;

        // Number format:
        // -----------------------------
        df= new DecimalFormat();
        df.setMinimumIntegerDigits(1);
        df.setMaximumFractionDigits(2);
        df.setMinimumFractionDigits(2);

        this.start();
    }

    // Functions to let configurate the points on screen:
    // --------------------------------------------------
    public int getPointsOnScreen() { return pointsOnScreen;};
    public void setPointsOnScreen(int value) {
        pointsOnScreen = value;
        changePointsOnScreen=true;
        // if (pointsOnScreen > value) changePointsOnScreen=true;
        // pointsOnScreen = value;
    };

    // Function to configurate the minimum Y axis point:
    // -------------------------------------------------
    public boolean getZeroYAxis() { return zeroYAxis;};
    public void setZeroYAxis (boolean value) {
        zeroYAxis = value;
    }

    // run method:
    // -----------
    public void run () {

        // Initializations before going inside the loop:
        // ---------------------------------------------
        double maxValue = 0;
        double minValue = -5;
        int period      = 5000;
        count           = 0;
        double [] actualData = new double[2];
        // double [] actualTime = new double[2];
        Graphics g;

        try { sleep(2000); } catch (Exception e) {};

        graphic.gridcolor=new Color(0,0,0);
        data.xaxis.setTitleText("Time (sec)");

        // Loop that paints and updates all the graphic elements:
        // ------------------------------------------------------------
        while ((threadsManager.doIkeepAlive())&&(!finish)) {
            // If I have more data that points on screen I delete the first one:
            // -----------------------------------------------------------------
            if (count>(100*5)) data.delete(0,0);
            // Actualizate the data:
            // ---------------------
            try {
                actualData[1] = Double.parseDouble(DataBaseHandler.getData(requestedOid).toString());
                actualData[1] = actualData[1]*factor;
                actualData[0] = count;
            } catch (NullPointerException e) {
                actualData[1]=0;
                actualData[0]=count;
            }
            try {
                data.append(actualData,1);
            } catch (Exception e) {
                System.out.println("Error appending data to graphic: "+e);
            };

            // Rescale the axis:
            // -----------------
            int initial = (data.dataPoints()-pointsOnScreen<0)?0:data.dataPoints()-pointsOnScreen;
            minValue = (data.getPoint(initial))[1]*0.9;
            for (int i=initial; i<data.dataPoints(); i++) {
                if (maxValue<((data.getPoint(i))[1])*1.1)
                    maxValue=((data.getPoint(i))[1]*1.1);
                if (minValue>((data.getPoint(i))[1])*0.9)
                    minValue=((data.getPoint(i))[1]*0.9);
            }
            data.yaxis.maximum = maxValue;
            maxValue=0;
            if (!zeroYAxis)
                data.yaxis.minimum = minValue;
            else
                data.yaxis.minimum = 0;
            if (count<(pointsOnScreen*5)) {
                data.xaxis.minimum = 0;
                data.xaxis.maximum = pointsOnScreen*5;
            } else {
                data.xaxis.minimum = count - (pointsOnScreen*5);
                data.xaxis.maximum = count;
            }

            //  Update the last value label:
            // ---------------------------------
            gFrame.lastValueLabel.setText(df.format(actualData[1]));

            // Actualizate the graphic:
            // ------------------------
             g = graphic.getGraphics();
            if (   (osi == null)
                || (iwidth != graphic.size().width)
                || (iheight != graphic.size().height)) {
                iwidth = graphic.size().width;
                iheight = graphic.size().height;
                osi = graphic.createImage(iwidth,iheight);
                osg = osi.getGraphics();
            }
            osg.setColor(gFrame.getBackground());
            osg.fillRect(0,0,iwidth,iheight);
            osg.setColor(g.getColor());

            osg.clipRect(0,0,iwidth,iheight);

            graphic.update(osg);

            g.drawImage(osi,0,0,graphic);

            try {
                sleep(period);
            } catch(Exception e) {};

            count=count+5;
        }// End of the loop

    }// End of the run method

    // Function that returns the data set attached to this thread:
    // -----------------------------------------------------------
    graph.DataSet getDataSet() {
        return data;
    }

    // Function that returns the unit factor:
    // --------------------------------------
    double getFactor () {
        return factor;
    }

    // Function that sets the unit factor:
    // -----------------------------------
    void setFactor (double factor) {
        this.factor=factor;
    }

    // This function lets the thread arrive to the end of the run method and then stop:
    // --------------------------------------------------------------------------------
    void finish() {
        finish=true;
    }

}// End of drawData
