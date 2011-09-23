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

#ifndef __ICF2_W32_REG_H
#define __ICF2_W32_REG_H

#include "general.h"
#include <winreg.h>

class registry_t
{

private:

	HKEY hKey;
	long openSubKey(char * subKeyName);

public:

	registry_t(char * keyName);
	virtual ~registry_t(void);

	int read(char * varName,BYTE * data,DWORD len);
	long write(char * varName,BYTE * data,DWORD len,DWORD type);
};

#endif
