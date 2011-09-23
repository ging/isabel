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
/////////////////////////////////////////////////////////////////////////
//
// $Id: XmlFilter.java 22731 2011-06-30 13:26:45Z santiago $
//
/////////////////////////////////////////////////////////////////////////
package services.isabel.lib;

import java.io.File;
import javax.swing.*;
import javax.swing.filechooser.*;


import services.isabel.lib.*;

/**
 * Esta clase define un filtro de ficheros xml para el explorador de ficheros.
 */
public class XmlFilter extends FileFilter {

    //Accept all directories and all xml files.
    public boolean accept(File f) {
        if (f.isDirectory()) {
            return true;
        }

        String extension = Servutilities.getExtension(f);
        if (extension != null) {
            if (extension.equals(Constants.XEDL_EXTENSION)) {
                    return true;
            } else {
                return false;
            }
        }

        return false;
    }

    //The description of this filter
    public String getDescription() {
        return "XEDL files";
    }
}
