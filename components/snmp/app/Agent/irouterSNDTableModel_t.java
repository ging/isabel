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

final public class irouterSNDTableModel_t extends tableModel_t {

    static final String[][] IROUTER_SND_STATS =
        { { "ChId",         "CTE",    "NA",    "70" },
          { "Media",        "CTE",    "NA",    "50" },
          { "DestinyIP",    "CTE",    "NA",   "120" },
          { "Pkts",         "INT",    "PKT",   "50" },
          { "DataBW",       "BW",     "Kb/S",  "70" },
          { "FecPkts",      "INT",    "PKT",   "50" },
          { "FecBW",        "BW",     "Kb/S",  "70" },
          { "Enqueued",     "INT",    "PKT",   "50" },
          { "Discarded",    "INT",    "PKT",   "50" }
        };

    irouterSNDTableModel_t (DataBaseHandler_t DataBaseHandler) {
        super(DataBaseHandler, IROUTER_SND_STATS, "IRTSND");
    }

    public int getRowCount() {
        return DataBaseHandler.getCountOfIrouterSNDEntries();
    }
}

