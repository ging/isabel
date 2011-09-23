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

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;

public abstract class ISJTable extends JTable {
    int        selectedRow;
    int        selectedCol;
    String     selectedCompId;
    sck.Oid    selectedOid;
    String     graphTitle;

    ISJTable (TableModel tm, String cId) {
        super (tm);
        this.selectedCompId = cId;
        setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        setPreferredScrollableViewportSize(new Dimension (580,200));
        setCellSelectionEnabled(true);

        //1.1.1.- row selection:
        ListSelectionModel rowSM = getSelectionModel();
        rowSM.addListSelectionListener(
            new ListSelectionListener() {
                public void valueChanged(ListSelectionEvent e) {
                    ListSelectionModel lsm= (ListSelectionModel)e.getSource();
                    if (lsm.isSelectionEmpty()) {
                        // System.out.println("No rows are selected.");
                    }
                    else {
                       // HA HABIDO UN CAMBIO
                       selectedRow = lsm.getMaxSelectionIndex();
                       lsm.clearSelection();
                       putSelectedTable();
                    }
                }
            }
        );

        //1.1.2.- column selection:
        ListSelectionModel colSM = getColumnModel().getSelectionModel();
        colSM.addListSelectionListener(
            new ListSelectionListener() {
                public void valueChanged(ListSelectionEvent e) {
                    ListSelectionModel lsm= (ListSelectionModel)e.getSource();
                    if (lsm.isSelectionEmpty()) {
                        //System.out.println("No columns are selected");
                    }
                    else  {
                        selectedCol= lsm.getMinSelectionIndex();
                        //System.out.println("SelectedCol="+selectedCol);
                    }
                }
            }
        );
    }

    void initColumnSize() {
        tableModel_t tm= (tableModel_t)getModel();
        String [][] COMP_STATS= tm.getColumnDescr();

        for (int i = 0; i < COMP_STATS.length; i++) {
            TableColumn column = getColumnModel().getColumn(i);
            String strW= COMP_STATS[i][3];
            int width= Integer.parseInt(strW);
            column.setPreferredWidth(width);
        }
    }

    void initColumnRenderer() {
        tableModel_t tm= (tableModel_t)getModel();
        String [][] COMP_STATS= tm.getColumnDescr();

        for (int i = 0; i < COMP_STATS.length; i++) {
            TableColumn column = getColumnModel().getColumn(i);
            if(    (COMP_STATS[i][1].equals("2R"))
                || (COMP_STATS[i][1].equals("BW")) ) {
                column.setCellRenderer(new FixedFractionDigitsRenderer(2));
            } else {
                DefaultTableCellRenderer dtcr= new DefaultTableCellRenderer();
                dtcr.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
                column.setCellRenderer(dtcr);
            }
        }
    }

    String getColUnits () {
        tableModel_t tm= (tableModel_t)getModel();
        String [][] COMP_STATS= tm.getColumnDescr();
        return COMP_STATS[selectedCol][2];
    }

    String getColConf () {
        tableModel_t tm= (tableModel_t)getModel();
        String [][] COMP_STATS= tm.getColumnDescr();
        return COMP_STATS[selectedCol][1];
    }

    abstract sck.Oid  getOidForGraph ();
    abstract String   getTitleForGraph ();
    abstract void     putSelectedTable();
}

