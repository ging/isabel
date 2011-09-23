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
public class IntegerDataBaseEntry_t extends DataBaseEntry_t {

    //Attributes
    //----------
    sck.Integer value;

    //Construtor:
    //-----------
    IntegerDataBaseEntry_t (int value, String Oid) {
        super (Oid);
        this.value = new sck.Integer (value);
    }

    void setValue (int value) {
        this.value = new sck.Integer (value);
        ttl = 0;
    }
    sck.Integer getValue () {
        return value;
    }


}//End of IntegerDataBaseEntry_t
