/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
#include "W32reg.h"

#include <shlwapi.h>

wchar_t*
lText(char * text){
	wchar_t auxChar[100];
	int n = mbstowcs(auxChar,text,strlen(text));
	auxChar[n]=0;
	return auxChar;
}

registry_t::registry_t(char * keyName)
{
	hKey = NULL;
	int j = 0;
	while(keyName[j]!=0)
	{
		char aux[100]="";
		char aux1[100]="";
		
		if (!hKey)
		{
			for (j = 0;keyName[j]!='/' && keyName[j]!=0;j++)
				aux1[j]=keyName[j];
			int k = 0;
			for (j+=1;keyName[j]!='/' && keyName[j]!=0;j++,k++)
				aux[k]=keyName[j];
			j++;
			HKEY auxKey;
			if (!strcmp(aux1,"HKEY_LOCAL_MACHINE"))
				auxKey = HKEY_LOCAL_MACHINE;
			if (!strcmp(aux1,"HKEY_CLASSES_ROOT"))
				auxKey = HKEY_CLASSES_ROOT;
			if (!strcmp(aux1,"HKEY_USERS"))
				auxKey = HKEY_USERS;
			if (!strcmp(aux1,"HKEY_CURRENT_USER"))
				auxKey = HKEY_CURRENT_USER;

			
			if(RegOpenKey(auxKey,aux,&hKey))
				return;
		
		}			
		memset(aux,0,100);	
		for (int i = 0;keyName[j]!='/' && keyName[j]!=0 ;i++,j++)
				aux[i]=keyName[j];
		j++;
		if (openSubKey(aux))
				return;
		
	}
}

long
registry_t::openSubKey(char * subKeyName)
{
	HKEY auxKey;
	if(RegOpenKey(hKey,subKeyName,&auxKey))
	   return E_FAIL;
	RegCloseKey(hKey);
	hKey=auxKey;
	return S_OK;
}

registry_t::~registry_t(void)
{
	RegCloseKey(hKey);
}

int
registry_t::read(char * varName,BYTE * data,DWORD len)
{
	DWORD auxLen = len;
	return RegQueryValueEx(hKey,varName,NULL,NULL,data,&auxLen);
}

long
registry_t::write(char * varName,BYTE * data,DWORD len,DWORD type)
{
	return RegSetValueEx(hKey,varName,0,type,data,len); 
}
