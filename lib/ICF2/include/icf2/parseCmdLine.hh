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
/////////////////////////////////////////////////////////////////////////
//
// $Id: parseCmdLine.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__parse_cmd_line_hh__
#define __icf2__parse_cmd_line_hh__

#include <icf2/item.hh>
#include <icf2/ql.hh>


class optionDef_t: public virtual item_t
{
public:
    optionDef_t(const char *n, int id, const char *desc= NULL);
    virtual ~optionDef_t(void);

    mutable char *optName;
    mutable int optId;
    mutable int optHasArg;

    char    *optDesc;

    virtual const char *className(void) const { return "optionDef_t"; }
};

typedef ql_t<optionDef_t*> optionDefList_t;


class appParam_t: public virtual item_t
{
public:
    appParam_t(int id, const char *v);
    virtual ~appParam_t(void);

    mutable int  parId;
    mutable char *parValue;

    virtual const char *className(void) const { return "appParam_t"; }
};

typedef ql_t<appParam_t*> appParamList_t;



typedef char * /*const*/ *argv_t;


appParamList_t *getOpt(optionDefList_t &optList, int &argc, argv_t &argv);



#endif
