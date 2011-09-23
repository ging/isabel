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

public class DataBaseEntry_t {

    int ttl;
    String OID;

    DataBaseEntry_t(String OID) {
        ttl=0;
        try  {
            DataBase_t.AddToOids(new sck.Oid (OID) );
            this.OID = OID;
            DataBase_t.isSorted = false;
        } catch(Exception e) {}
    }

    int getTTL() { return ttl; }

    void incrementTTL() { ttl++; }

    void takeOut() {
        try {
            DataBase_t.DelFromOids(new sck.Oid(OID));
        } catch(Exception e) {}
        DataBase_t.isSorted= false;
    }
}

