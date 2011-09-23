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

import java.util.*;

public class DataBase_t {
    static Vector  Oids           = new Vector();
    static boolean isSorted       = false;

    static final String IROUTER   = "1.3.6.1.4.1.2781.1.2";
    static final String AUDIO     = "1.3.6.1.4.1.2781.1.4";
    static final String VIDEO     = "1.3.6.1.4.1.2781.1.5";
    static final String SLIDESFTP = "1.3.6.1.4.1.2781.1.6";
    static final String SHDISPLAY = "1.3.6.1.4.1.2781.1.7";

    DataBase_t () {
    }
    void AddOid (sck.Oid OID) {
        Oids.insertElementAt(OID,Oids.size());
    }
    void DelOid (sck.Oid OID) {
        Oids.removeElement(OID);
    }
    static void AddToOids (sck.Oid OID) {
        Oids.insertElementAt(OID,Oids.size());
    }
    static void DelFromOids (sck.Oid OID) {
        Oids.removeElement(OID);
    }
    static boolean contains(sck.Oid OID) {
        return Oids.contains(OID);
    }
    static void sort () {
        //long  sortInicio = System.currentTimeMillis();
        for (int i=0; i<Oids.size(); i++) {
            for (int j=0; j<Oids.size()-1; j++) {
                sck.Oid currentItem = (sck.Oid) Oids.get(j);
                sck.Oid nextItem = (sck.Oid) Oids.get(j+1);
                if (currentItem.greaterThan(nextItem)) {
                    Oids.setElementAt(nextItem, j);
                    Oids.setElementAt(currentItem, j+1);
                }
            }
        }
        //System.out.println("Sorted in ="+(System.currentTimeMillis()-sortInicio));
        //System.out.println("OidsSize="+Oids.size());
        //for (int i=0; i<Oids.size(); i++) {
        //    sck.Oid cOid = (sck.Oid)Oids.get(i);
        //    System.out.println("|"+cOid+"|");
        //}
        //System.out.println("| FIN MIB");
    }

    sck.Oid getOid(int i) {
        return (sck.Oid)Oids.get(i);
    }

    static sck.Oid getNext (sck.Oid Original) {
        if (!isSorted) {
            sort();
            isSorted=true;
        }
        return getNext(Original,0,Oids.size()-1);
    }


    static sck.Oid getNext(sck.Oid Original,int orig, int ending) {
        //System.out.println("BUSCO NEXT OID de "+Original+"-->"+orig+"-->"+ending);
        int midPosition = (orig+ending)/2;
        if (orig>ending) {
            if (orig<=(Oids.size()-1)) return (sck.Oid)Oids.get(orig);
            else return null;
        }
        if (Original.equals((sck.Oid)Oids.get(midPosition))) {
            if (midPosition<(Oids.size()-1)) return (sck.Oid)Oids.get(midPosition+1);
            else return null;
        }
        if (Original.greaterThan((sck.Oid)Oids.get(midPosition))) {
        //if (IsGreater(Original, (sck.Oid)Oids.get(midPosition))) {
            return getNext(Original,midPosition+1,ending);
        } else {
            if ( Original.equals( (sck.Oid)Oids.get(ending) ) ) {
                    if (ending==(Oids.size()-1)) return null;
                    else return (sck.Oid)Oids.get(ending+1);
            } else {
                return getNext(Original,orig,midPosition-1);
            }
        }
    }

}
