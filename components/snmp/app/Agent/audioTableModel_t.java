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

final public class audioTableModel_t extends tableModel_t {

    static final String [][] AUDIO_STATS =
        { { "ChId",           "CTE",   "NA",     "50"  },
          { "Codec",          "CTE",   "NA",    "100"  },
          { "SentBW",         "BW",    "Kb/S",   "70"  },
          { "RecvBW",         "BW",    "Kb/S",   "70"  },
          { "ReceivedPkts",   "INT",   "PKT",    "70"  },
          { "LostPkts",       "INT",   "PKT",    "50"  },
          { "DupPkts",        "INT",   "PKT",    "50"  },
          { "DisorderedPkts", "INT",   "PKT",    "50"  },
          { "ThrownPkts",     "INT",   "PKT",    "50"  },
          { "Jitter",         "2R",    "AD",     "70"  },
          { "Buffered",       "INT",   "AD",     "70"  },
          { "Buffering",      "INT",   "AD",     "70"  }
        };

    audioTableModel_t (DataBaseHandler_t DataBaseHandler) {
        super(DataBaseHandler, AUDIO_STATS, "AUD");
    }

    public int getRowCount() {
        return DataBaseHandler.getCountOfAudioEntries();
    }
}

