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

final public class videoTableModel_t extends tableModel_t {

    static final String [][] VIDEO_STATS =
        { { "ChId",      "CTE",    "NA",     "50" },
          { "UID",       "CTE",    "NA",     "70" },
          { "Codec",     "CTE",    "NA",    "100" },
          { "Q",         "INT",    "NA",     "30" },
          { "ImageSize", "CTE",    "NA",     "80" },
          { "SetFR",     "2R",     "FR/S",   "50" },
          { "DesBW",     "BW",     "Kb/S",   "70" },
          { "CodecBW",   "BW",     "Kb/S",   "70" },
          { "SentFR",    "2R",     "FR/S",   "50" },
          { "SentBW",    "BW",     "Kb/S",   "70" },
          { "RecvBW",    "BW",     "Kb/S",   "70" },
          { "EnsFR",     "2R",     "FR/S",   "50" },
          { "PaintFR",   "2R",     "FR/S",   "50" }
        };

    videoTableModel_t (DataBaseHandler_t DataBaseHandler) {
        super(DataBaseHandler, VIDEO_STATS, "VID");
    }

    public int getRowCount() {
        return DataBaseHandler.getCountOfVideoEntries();
    }
}

