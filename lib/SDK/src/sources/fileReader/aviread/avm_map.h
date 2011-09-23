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
#ifndef AVIFILE_AVM_MAP_H
#define AVIFILE_AVM_MAP_H

#ifndef AVM_BEGIN_NAMESPACE
#define AVM_BEGIN_NAMESPACE namespace avm {
#define AVM_END_NAMESPACE   }
#endif

#include <assert.h>

/**********
 * WARNING - this file is meant to be used by internal avifile application
 * DO NOT USE in your own project!
 * the API here could change in any minute
 */

AVM_BEGIN_NAMESPACE;

template <class Key> class less
{
public:
    bool operator()(const Key& k1, const Key& k2) const { return k1<k2; }
};

template <class Key> class equal
{
public:
    bool operator()(const Key& k1, const Key& k2) const { return k1==k2; }
};

// do not use this container in time critical context
// search time is between log(N) and N, depending on order of element insertion
template <class Key, class Value, class Compare = less<Key>, class equal = equal<Key> > class avm_map
{
protected:
    template <class Key1, class Value1> struct pair
    {
	Key1 key;
	Value1 value;
	pair() : key(Key()), value(Value()) {}
	pair(Key1 k, Value1 v) : key(k), value(v) {}
	pair(const pair<Key1, Value1>& p) : key(p.key), value(p.value) {}
    };
    typedef pair<Key, Value> _Tpair;

    template <class Key1, class Value1> struct binary_tree_node
    {
	pair<Key1, Value1>* entry;
	binary_tree_node<Key1, Value1>* left;
	Key1 minval;
	binary_tree_node<Key1, Value1>* right;
	binary_tree_node<Key1, Value1>* parent;
	Key1 maxval;
	binary_tree_node(binary_tree_node<Key1, Value1>* ptr=0) : entry(0), left(0), right(0), parent(ptr), weight(1) {}
	int weight;
	void destroy()
	{
	    if(left)
	    {
		left->destroy();
		delete left;
	    }
	    if(right)
	    {
		right->destroy();
		delete right;
	    }
	    delete entry;
	}
    };
    typedef binary_tree_node<Key, Value> _Tnode;

    _Tnode* m_pTree;
    Value* m_pDefaultValue;
    Compare m_sC;
    equal m_sE;
    // this funny trick makes old compiler work with this pointer
    static void update_min_max_weights(void* node);
    _Tnode* find_private(const Key& key) const;
public:
#if 1
    class const_iterator
    {
	_Tnode* p;
    public:
	const_iterator(_Tnode* p1=0) : p(p1) {}
	const_iterator(const const_iterator& c) : p(c.p) {}
	bool operator==(const const_iterator& c) const { return p==c.p; }
	operator const _Tpair*() const { return p ? p->entry : 0 ; }
	_Tpair* operator->() { return p ? p->entry : 0 ; }
	const_iterator operator++(int)
	{
	    if(p==0)
		return *this;
	    _Tnode* p1 = p;
	    while(p1)
	    {
		if(p1->parent && (p1==p1->parent->left) && p1->parent->right)
		{
		    p1=p1->parent->right;
		    break;
		}
		if(!p1->parent)
		{
		    p=0;
		    return *this;
		}
		p1=p1->parent;
	    }
	    while(p1->left || p1->right)
		if(p1->left)
		    p1=p1->left;
		else
		    p1=p1->right;
	    p=p1;
	    return *this;
	}
    };
#endif
    avm_map();
    ~avm_map() { m_pTree->destroy(); delete m_pTree; }
#if 1
    const_iterator begin()
    {
	_Tnode* p1=m_pTree;
	    while(p1->left || p1->right)
		if(p1->left)
		    p1=p1->left;
		else
		    p1=p1->right;
	return const_iterator(p1);
    }

    const_iterator end()
    {
	return 0;
    }
#endif
    // inserts a new entry into the map
    Value* insert(const Key& key, Value value);

    // searches for the entry that corresponds to the key. If the search fails, returns 0.
    Value* find(const Key& key) const
    {
	_Tnode* tree_node=find_private(key);
	if(!tree_node)
	    return 0;
	else
	{
	    assert(tree_node->entry);
	    return &(tree_node->entry->value);
	}
    }

    // searches for the entry that corresponds to the key. If the search fails, inserts a new entry into the map
    // and returns it ( default STL map behavior ).
    Value* find_insert(const Key& key)
    {
	_Tnode* tree_node=find_private(key);
	if(!tree_node)
	    return insert(key, *m_pDefaultValue);
	else
	{
	    assert(tree_node->entry);
	    return &(tree_node->entry->value);
	}
    }

    // searches for the entry that corresponds to the key. If the search fails, returns pointer to the default entry
    // ( entry that corresponds to Key=0 ).
    Value* find_default(const Key& key = Key()) const
    {
	if(m_sE(key, Key()))
	    return m_pDefaultValue;
	_Tnode* tree_node=find_private(key);
	if(!tree_node)
	    return m_pDefaultValue;
	else
	{
	    assert(tree_node->entry);
	    return &(tree_node->entry->value);
	}
    }

    // erases entry that corresponds to the key if such entry is present. Returns true on success.
    bool erase(const Key& key);
};


template <class Key, class Value, class Compare, class Equal> avm_map<Key, Value, Compare, Equal>::avm_map()
{
    m_pTree=new _Tnode;
    m_pTree->entry = new pair<Key, Value>;
    m_pDefaultValue=&(m_pTree->entry->value);
}

template <class Key, class Value, class Compare, class Equal> void avm_map<Key, Value, Compare, Equal>::update_min_max_weights(void* n)
{
    // cast to the needed type - used to prevent internal compiler error
    // for old egcc
    //avm_map<Key, Value, Compare, Equal>::_Tnode* node = (avm_map<Key, Value, Compare, Equal>::_Tnode*) n;
    _Tnode* node = (_Tnode*) n;

    if(node->entry)
    {
	node->weight=1;
	node->minval=node->maxval=node->entry->key;
	node=node->parent;
    }
    else
    {
	if(node->left && node->left->entry)
	{
	    node->left->weight=0;
	    node->left->minval=node->left->maxval=node->left->entry->key;
	}
	if(node->right && node->right->entry)
	{
	    node->right->weight=0;
	    node->right->minval=node->right->maxval=node->right->entry->key;
	}
    }
    while(node)
    {
	node->weight=0;
	if(node->left)
	{
	    node->minval=node->left->minval;
	    node->weight+=node->left->weight;
	}
	else
	    node->minval=node->right->minval;
	if(node->right)
	{
	    node->maxval=node->right->maxval;
	    node->weight+=node->right->weight;
	}
	else
	    node->maxval=node->left->maxval;
	node=node->parent;
//	while(1)
//	  {
//	    int diff=0;
//	    if(node->left)
//		diff+=node->left->weight;
//	    if(nod->right)
//		diff-=m_pTree->right->weight;
//	    if((diff>-2) && (diff<2))
//		break;
//	}
    }
}

template <class Key, class Value, class Compare, class Equal> Value* avm_map<Key, Value, Compare, Equal>::insert(const Key& key, Value value)
{
    _Tnode* ptr=m_pTree;
    while(1)
    {
	if(ptr->entry)
	{
	    if(m_sE(ptr->entry->key, key))
	    {
		ptr->entry->value=value;
		return &(ptr->entry->value);
	    }
	    ptr->left=new _Tnode(ptr);
	    ptr->right=new _Tnode(ptr);
	    if(m_sC(ptr->entry->key, key))
	    {
		ptr->left->entry=ptr->entry;
		ptr->right->entry=new pair<Key, Value>(key, value);
		ptr->entry=0;
		update_min_max_weights(ptr);
		return &(ptr->right->entry->value);
	    }
	    else
	    {
		ptr->left->entry=new pair<Key, Value>(key, value);
		ptr->right->entry=ptr->entry;
		ptr->entry=0;
		update_min_max_weights(ptr);
		return &(ptr->left->entry->value);
	    }
	}

	if(ptr->left && ((m_sC(key, ptr->left->maxval)) || m_sE(key, ptr->left->maxval)))
	{
	    ptr=ptr->left;
	    continue;
	}

	if(ptr->right && ((m_sC(ptr->right->minval, key)) || m_sE(ptr->right->minval, key)))
	{
	    ptr=ptr->right;
	    continue;
	}
	if(ptr->left && ptr->right)
	{
	    if(ptr->left->weight<ptr->right->weight)
		ptr=ptr->left;
	    else
		ptr=ptr->right;
	    continue;
	}
	if(!ptr->left)
	{
	    ptr->left=new _Tnode(ptr);
	    ptr->left->entry=new pair<Key, Value>(key, value);
	    update_min_max_weights(ptr);
	    return &(ptr->left->entry->value);
	}
	if(!ptr->right)
	{
	    ptr->right=new _Tnode(ptr);
	    ptr->right->entry=new pair<Key, Value>(key, value);
	    update_min_max_weights(ptr);
	    return &(ptr->right->entry->value);
	}
	//printf("Should not arrive here\n");
    }
}

template <class Key, class Value, class Compare, class Equal> typename avm_map<Key, Value, Compare, Equal>::_Tnode* avm_map<Key, Value, Compare, Equal>::find_private(const Key& key) const
{
    _Tnode* ptr=m_pTree;
    while(1)
    {
	if(ptr->entry)
	{
	    if (ptr->entry->key && m_sE(ptr->entry->key, key))
		return ptr;
	    return 0;
	}

	if(ptr->left && ((m_sC(key, ptr->left->maxval)) || m_sE(key, ptr->left->maxval)))
	{
	    ptr=ptr->left;
	    continue;
	}

	if(ptr->right && ((m_sC(ptr->right->minval, key)) || m_sE(ptr->right->minval, key)))
	{
	    ptr=ptr->right;
	    continue;
	}
	if(ptr->left && ptr->right)
	{
	    if(ptr->left->weight<ptr->right->weight)
		ptr=ptr->left;
	    else
		ptr=ptr->right;
	    continue;
	}
	return 0;
    }
}

template <class Key, class Value, class Compare, class Equal> bool avm_map<Key, Value, Compare, Equal>::erase(const Key& key)
{
    if(m_sE(key, Key()))
	return false; // you may not erase the default entry
    _Tnode* ptr=find_private(key);
    if(!ptr)
	return false;
    _Tnode* parent=ptr->parent;
    assert(parent); // since we have more than one tree node, this can't be root entry
    delete ptr->entry;
    if(parent->left==ptr)
	parent->left=0;
    else
	parent->right=0;
    delete ptr;
    while(!parent->left && !parent->right)
    {
	if(parent->parent->left==parent)
	    parent->parent->left=0;
	else
	    parent->parent->right=0;
	ptr=parent;
	parent=parent->parent;
	delete ptr;
    }
    if(parent->left && parent->left->entry && !parent->right)
    {
	parent->entry=parent->left->entry;
	delete parent->left;
	parent->left=0;
    }
    if(parent->right && parent->right->entry && !parent->left)
    {
	parent->entry=parent->right->entry;
	delete parent->right;
	parent->right=0;
    }
    update_min_max_weights(parent);
    return true;
}

AVM_END_NAMESPACE;

#endif
