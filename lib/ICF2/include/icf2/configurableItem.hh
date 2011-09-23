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
// $Id: conditionalVar.hh 13950 2008-07-04 15:57:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__configurable_item__hh__
#define __icf2__configurable_item__hh__

#include <map>
#include <vector>
#include <string>

#include <icf2/general.h>

class ConfigurableItem_t
{
public:
   enum AttrType_e
   {
       ATTR_BOOL,
       ATTR_CHAR,
       ATTR_SHORT,
       ATTR_INT,
       ATTR_FLOAT,
       ATTR_DOUBLE,
       ATTR_STRING,
       ATTR_PTR,
       ATTR_NOT_SUPPORTED
   };

private:

    class Attr_t
    {
    protected:
        std::string attr_exception;
        std::string name;
    public:
        Attr_t(std::string n)
        {
            name = n;
            attr_exception ="::Invalid argument type for this attribute\n";
        }
        virtual ~Attr_t(void){}

        virtual bool setValue(bool v){ throw (name + attr_exception).c_str(); return false;}
        virtual bool setValue(char v){ throw (name + attr_exception).c_str(); return false;}
        virtual bool setValue(short v){ throw (name + attr_exception).c_str();  return false; }
        virtual bool setValue(int v){ throw (name + attr_exception).c_str(); return false; }
        virtual bool setValue(float v){ throw (name + attr_exception).c_str(); return false; }
        virtual bool setValue(double v){ throw (name + attr_exception).c_str(); return false; }
        virtual bool setValue(std::string v){ throw (name + attr_exception).c_str(); return false; }
        virtual bool setValue(void* v){ throw (name + attr_exception).c_str(); return false; }
        virtual void getValue(bool * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(char * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(short * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(int * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(float * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(double * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(std::string * v){ throw (name + attr_exception).c_str(); }
        virtual void getValue(void ** v){ throw (name + attr_exception).c_str(); }
        const char * getName(void){ return name.c_str(); }

        const char *getTypeStr(void)
        {
            switch(getType())
            {
                case ATTR_BOOL:     return "bool";
                case ATTR_CHAR:     return "char";
                case ATTR_SHORT:    return "short";
                case ATTR_INT:      return "int";
                case ATTR_FLOAT:    return "float";
                case ATTR_DOUBLE:   return "double";
                case ATTR_STRING:   return "string";
                case ATTR_PTR:      return "void *";
                default:            return "void";
            }
            return "unknown";
        }

        virtual AttrType_e getType(void)= 0;

protected:
        AttrType_e guessType(void)
        {
            //Check attr type
            try
            {
                bool v;
                getValue(&v);
                return ATTR_BOOL;
            }catch(...){}
            try
            {
                char v;
                getValue(&v);
                return ATTR_CHAR;
            }catch(...){}
            try
            {
                short v;
                getValue(&v);
                return ATTR_SHORT;
            }catch(...){}
            try
            {
                int v;
                getValue(&v);
                return ATTR_INT;
            }catch(...){}
            try
            {
                float v;
                getValue(&v);
                return ATTR_FLOAT;
            }catch(...){}
            try
            {
                double v;
                getValue(&v);
                return ATTR_DOUBLE;
            }catch(...){}
            try
            {
                std::string v;
                getValue(&v);
                return ATTR_STRING;
            }catch(...){}
            try
            {
                void* v;
                getValue(&v);
                return ATTR_PTR;
            }catch(...){}
            throw "Invalid type attribute type";
            return ATTR_NOT_SUPPORTED;
        }
    };

private:

    template <class C,class T> class TAttr_t: public Attr_t
    {
    private:

        C * owner;
        T * value;
        AttrType_e type;
        bool (C::* setCB)(T newV, T oldV);
        T    (C::* getCB)(void);

    public:

        TAttr_t(const char * n,
                C* o,
                T  v,
                bool (C::* set_callback)(T newV, T oldV),
                T    (C::* get_callback)(void)
               )
        : Attr_t(n)
        {
            owner = o;
            value = new T;
            setCB = NULL;
            getCB = NULL;

            // FIRST, we try to guess the type BEFORE setting the callbacks
            // to avoid calling 'get_callback'
            type= Attr_t::guessType();

            // NOW, we can safely set the callbacks
            setCB = set_callback;
            getCB = get_callback;

            // initial value
            setValue(v);
        }

        virtual ~TAttr_t(void)
        {
            delete value;
            value = NULL;
        }

        AttrType_e getType(void)
        {
            return type;
        }

        bool setValue(T v)
        {
            bool ret = true;

            if (owner && setCB)
            {
                ret = (owner->*setCB)(v, *value);
            }
            if (ret)
            {
                *value = v;
            }

            return ret;
        }

        void getValue(T * val)
        {
            if (owner && getCB)
            {
                *value = (owner->*getCB)();
            }
            *val = *value;
        };
    };

private:

    std::map<std::string,Attr_t*> AttrMap;

protected:

    virtual ~ConfigurableItem_t(void)
    {
        for (std::map<std::string,Attr_t*>::iterator iter = AttrMap.begin();
             iter != AttrMap.end();
             ++iter
            )
        {
            delAttr(iter->first.c_str());
        }
    }

public:

    template <class C,class T> bool addAttr(const char * name,
        T val,
        bool (C::* set_cb)(T newV, T oldV),
        T    (C::* get_cb)(void)
        )
    {
        delAttr(name);
        Attr_t *attr = new TAttr_t<C,T>(name,static_cast<C*>(this),val,set_cb,get_cb);
        AttrMap[name] = attr;
        return true;
    }

    bool delAttr(const char * name)
    {
        if(AttrMap[name] != NULL)
        {
            delete AttrMap[name];
            AttrMap[name] = NULL;
        }
        return true;
    }

    Attr_t *getAttr(const char * name)
    {
        return AttrMap[name];
    }

    std::vector<const char *> getAttrNamesList(void)
    {
        std::vector<const char *> attrList;
        for (std::map<std::string,Attr_t*>::iterator iter = AttrMap.begin();
             iter != AttrMap.end();
             ++iter
            )
        {
            attrList.push_back(iter->first.c_str());
        }
        return attrList;
    }
};

#endif

