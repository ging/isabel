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

import java.text.*;
import javax.swing.table.*;

public abstract class tableModel_t extends AbstractTableModel {

    DataBaseHandler_t DataBaseHandler;

    // statsInfo must be a matrix, with as many rows than
    // values to be presented in the GUI. Columns have the
    // following meaning:
    // column 0: name
    // column 1: factor to be applied to value
    //     allowed values: "CTE", "BW", "INT", "2R"
    // column 2: units of the datum. 
    //     allowed values:
    //         "NA"    (No Appy)      "Kb/S"  (Kbits/sec)
    //         "FR/S"  (Frames/sec)   "AD"    (adimensional)
    //         "PKT/S" (Packets/sec)  "PKT"   (Packets)
    // column 3: width (relative size)
    //     allowed values: integer value as a String.
    final String[][] statsInfo;

    // the name of the component, "VID", "SHD", ...
    final String componentName;


    private static int factor2R;
    private static int factorBW;

    //Constructor:
    //------------
    tableModel_t (DataBaseHandler_t DataBaseHandler,
                  String [][]statsInfo,
                  String componentName
                 ) {
        this.DataBaseHandler = DataBaseHandler;
        this.statsInfo       = statsInfo;
        this.componentName   = componentName;

        factor2R = 100;
        factorBW = 100000;
    }

    // Functions that a table Model must define,
    // to let the JTable component attached
    // to this tableModel get all its data:
    public int getColumnCount() {
        return statsInfo.length;
    }

    abstract public int getRowCount();

    public String getColumnName (int col) {
        return statsInfo[col][0];
    }

    // gets the name of the component and thus
    // access the right data base
    public final String getComponentName () {
        return componentName;
    }

    // takes data from the right position in the dataBase.
    public Object getValueAt(int row, int col) {
        Object datum= DataBaseHandler.getData(getComponentName(), row, col+1);

        if(datum == null) {
            System.out.println(getComponentName()
                               + ": at " + row + "," + col
                               + " datum null"
                              );
            return "null";
        }

        if (statsInfo[col][1] == "CTE") {
            return datum;
        }
        if (statsInfo[col][1] == "INT") {
            int value = Integer.parseInt(datum.toString());
            return new Integer(value);
        }
        if (statsInfo[col][1] == "2R") {
            double value= Double.parseDouble(datum.toString());
            return new Double(value/factor2R);
        }
        if (statsInfo[col][1] == "BW") {
            double value= Double.parseDouble(datum.toString());
            return new Double(value/factorBW);
        }

        System.out.println(getComponentName()
                           + ": at " + row + "," + col
                           + " invalid type for data"
                          );
        return "null";
    }

    public void updateTable() {
        fireTableDataChanged();
    }

    public static int  getFactor2R () { return factor2R; };

    public static int  getFactorBW () { return factorBW; };

    public static void setFactor2R(int value) { factor2R= value; };

    public static void setFactorBW(int value) { factorBW= value; };

    public final String[][] getColumnDescr() {
        return statsInfo;
    };

}

