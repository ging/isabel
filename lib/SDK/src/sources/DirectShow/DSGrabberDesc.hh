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
#ifndef __DSGRABBERDESC_H__
#define __DSGRABBERDESC_H__

#include <string>

#include <Isabel_SDK/sourceDescriptor.hh>

#define DS_NAME "DirectShow Image Source"
#define DS_DESC "DirectShow module for Isabel Video"

class CDSGrabberDesc: public sourceDescriptor_t
{
private:
	
	std::string m_Device;
    std::string m_Name;


public:
    CDSGrabberDesc(const char *npath,
                    const char *nname);
	CDSGrabberDesc(const CDSGrabberDesc &other);
    virtual ~CDSGrabberDesc(void);

public:
   const char      *getID                 (void) const { return m_Device.c_str();      }
   const char      *getInputPorts         (void) const { return NULL;}
   bool				operator==(const CDSGrabberDesc &other) const;
   CDSGrabberDesc&	operator=(const CDSGrabberDesc &other);
   bool				operator<(const CDSGrabberDesc &other) const;
   bool				operator>(const CDSGrabberDesc &other) const;
};


#endif