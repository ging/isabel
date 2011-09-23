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
// $Id: ql.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/ql.hh>
#include <icf2/smartReference.hh>


//
// preinstantiate some useful lists
//
template class ql_t<char >;
template class ql_t<short>;
template class ql_t<int  >;
template class ql_t<long >;

template class ql_t<unsigned char >;
template class ql_t<unsigned short>;
template class ql_t<unsigned int  >;
template class ql_t<unsigned long >;

template class ql_t<float>;
template class ql_t<double>;


template class ql_t<item_t>;




template class smartReference_t< ql_t<char > >;
template class smartReference_t< ql_t<short> >;
template class smartReference_t< ql_t<int  > >;
template class smartReference_t< ql_t<long > >;

template class smartReference_t< ql_t<unsigned char > >;
template class smartReference_t< ql_t<unsigned short> >;
template class smartReference_t< ql_t<unsigned int  > >;
template class smartReference_t< ql_t<unsigned long > >;

template class smartReference_t< ql_t<float> >;
template class smartReference_t< ql_t<double> >;


template class smartReference_t< ql_t<item_t> >;

