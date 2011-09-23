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

import java.util.Vector;

public class irouterMediaID
{
    static String mediaIDs[]=
        { "audio",
          "vumetr",
          "video",
          "shDisp",
          "pointr",
          "sldFtp",
          "rtcpVd",
          "rtcpSD"
        };

    static int getMediaID(String mediaStr)
    {
        for (int i= 0; i < mediaIDs.length; i++)
        {
            if (mediaIDs[i].equals(mediaStr))
            {
                return i;
            }
        }

        System.out.println("irouterMediaID: UNKNOWN media! (" + mediaStr + ")");
        return -1;
    }

    static String getMediaStr(int mediaID)
    {
        if (mediaID < 0 || mediaID >= mediaIDs.length)
        {
            return null;
        }

        return mediaIDs[mediaID];
    }
}

