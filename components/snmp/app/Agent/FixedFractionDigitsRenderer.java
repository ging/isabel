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
import java.text.NumberFormat;
import javax.swing.table.DefaultTableCellRenderer;

public class FixedFractionDigitsRenderer extends DefaultTableCellRenderer {

    static NumberFormat formatter;

    public FixedFractionDigitsRenderer(int numDigits) {
        super();
        setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);

formatter = NumberFormat.getInstance(Locale.getDefault());
//formatter = NumberFormat.getNumberInstance();
formatter.setMinimumIntegerDigits(1);
formatter.setMaximumFractionDigits(2);
formatter.setMinimumFractionDigits(2);
        //df.setMinimumIntegerDigits(1);
        //df.setMaximumFractionDigits(2);
        //df.setMinimumFractionDigits(2);

    }

    public void setValue(Object value) {
        if ((value != null) && (value instanceof Number)) {
            Number numberValue = (Number) value;
            value = formatter.format(numberValue.doubleValue());
        }
        super.setValue(value);
    }

  }
