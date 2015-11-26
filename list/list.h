#ifndef __LIST__
#define __LIST__
#ifndef __IOSTREAM__
#include <iostream>
#endif
#ifndef __ALLOCATOR__
#include "allocator.h"
#endif
#ifndef __DEFAULT_ALLOC_TEMPLATE__
#include "__default_alloc_template.h"
#endif
#ifndef __ITERATOR_TRAITS__
#include "__iterator_traits.h"
#endif
template<class T>
struct list_node{
	list_node *prev;
	list_node *next;
	T data;
};

template<class T,class Ref,class Diff>
struct list_iterator{
	typedef list_iterator<T, Ref, Diff> iterator;
	typedef list_iterator<T, T&,ptrdiff_t> self;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef ptrdiff_t different_type;
	typedef size_t size_type;
	typedef list_node<T> node_type;

	list_node<T>  *node;
	list_iterator() :node(0){}
	explicit list_iterator(list_node<T>* lhs) :node(lhs){}
	list_iterator &operator++(){
		node = node->next;
		return (*this);
	}
	list_iterator &operator++(int){
		node_type tmp = node;
		++(*this);
		return self(tmp);
	}
	list_iterator &operator--(int){
		node_type tmp = node;
		--(*this);
		return self(tmp);
	}

	list_iterator &operator--(){
		node = node->prev;
		return (*this);
	}
	list_iterator(const list_iterator& lhs){
		node = lhs->node;
	}
	list_iterator(list_iterator&& lhs){
		node = lhs->node;
		lhs->node = 0;
	}
	list_iterator(list_node<T>* lhs){
		node = lhs;
	}
	bool operator!=(self lhs){
		return lhs.node != node;
	}
	bool operator==(self lhs){
		return lhs.node == node;
	}
	reference operator*(){
		return node->data;
	}
	pointer operator->(){
		return &*(*this);
	}
};

template<class T,class Alloc = default_alloc>
class list{
public:
	typedef __random_access_iterator_tag catagory;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef ptrdiff_t different_type;
	typedef list_iterator<T, T&, ptrdiff_t> iterator;
	typedef typename __allocator<list_node<T>, Alloc> allocator_t;
	typedef list_node<T>* node_pointer;
	typedef size_t size_type;


private:
	node_pointer key;
public:

	iterator begin(){ return iterator(key->next); }
	iterator end(){ return iterator(key); }
	reference front(){ return (key->next->data); }
	reference back(){ return *(--iterator(key)) }
	node_pointer get_node(){ return node_pointer(allocator_t::allocate()); }

	/*create a node and assigned*/
	node_pointer create_node(value_type lhs){
		node_pointer tmp = get_node();
		construct(&tmp->data, lhs);
		return tmp;
	}
	/*here to release memory*/
	void put_node(node_pointer lhs){
		allocator_t::deallocate(lhs);
	}
	/*here to destory a node and release memory*/
	void destory_node(node_pointer lhs){
		destory(&lhs->data);
		put_node(lhs);
	}
	/*here to create a empty link*/
	void empty_initialize(){
		node_pointer tmp = get_node();
		tmp->next = &tmp;
		tmp->prev = &tmp;
	}
	list(){ empty_initialize(); }
	bool empty()const { return key->next == key; }
	size_type size(){

		node_pointer tmp = key->next;
		size_type n = 0;
		while (tmp != key){
			++n;
			tmp = tmp->next;
		}
		return n;
		/*
		the other implements is
		size_type result  = 0;
		distance(begin(),end(),result);
		return result;
		*/

	}
	/*here to */
	iterator list_link(node_pointer lhs, node_pointer rhs){
		/*here to assert to avoid that l*/
		static_assert(lhs || rhs);

		rhs->next = lhs->next;
		rhs->prev = lhs;
		lhs->next->prev = rhs;
		rhs->next = lhs;
		return iterator(rhs);
	}
	void push_back(value_type lhs){
		/*
		node_pointer tmp = create_node(lhs);
		list_link(key->prev, tmp);
		*/
		insert(end(), lhs);
	}
	void push_front(value_type lhs){
		insert(begin(), lhs);
	}
	iterator insert(iterator position, const value_type& lhs){
		return list_link((--position).node, create_node(lhs));
	}
	iterator erase(iterator position){
		iterator tmp(node_pointer(position.node))->next);
		(node_pointer(position.node))->prev->next = (node_pointer(position.node))->next;
		(node_pointer(position.node))->next->prev = (node_pointer(positon.node))->prev;
		destory_node(position.node);
		return tmp;
	}
	void pop_front(){
		erase(begin());
	}
	void pop_back(){
		iterator tmp = --end();
		erase(tmp);
	}
	void clear(){
		node_pointer cur = key->next;
		while (cur != key){
			node_pointer tmp = cur;
			cur = cur->next;
			destory_node(tmp);
		}
		key->next = key;
		key->prev = key;
	}
	void remove(const value_type& lhs){
		iterator _begin = begin();
		iterator _end = end();

		while (_begin != _end)
		{
			if (lhs == *_begin)
			{
				_begin = erase(_begin);
			}
			else
			{
				++_begin;
			}
		}

			/*here jjg who makes way is like that*/
			/*
			while(_begin != _end){
			iterator next = _begin;
			++next;
			if(lhs == *_begin){
			erase(_begin);
			_begin = next;
		
		}
		*/
	}

	void unique(){
		iterator _begin = begin();
		iterator _end = end();
		iterator _next = _begin;
		if (_begin == _end)return;
		while (++_next != _end){
			if (*_begin == *_next)
				erase(_next);
			else
				_begin = _next;
			_next = _begin;
		}
	}
	/*the function to make insert first-last to before position*/
	void transfer(iterator position, iterator first, iterator last){
		node_pointer _posi = position.node;
		node_pointer _fir = first.node;
		node_pointer _las = last.node;
		node_pointer _posi_pre = _posi->prev;
		node_pointer _fir_pre = _fir->prev;
		node_pointer _las_pre = _las->prev;
		_posi->prev = _las_pre;
		_las_pre->next = _posi;
		_posi_pre->next = _fir;
		_fir->prev = _posi_pre;
		_fir_pre->next = _las;
		_las->prev = _fir_pre;
		
	}
	void splice(iterator position, list &x){
		if (!x.empty()){
			transfer(position, x.begin(), x.end());
		}
	}
	void splice(iterator position, list &x, iterator i){
		iterator j = i;
		++j;
		if (position == j || position == i) return;
		transfer(position, i, j);
	}
	void splice(iterator position, list&, iterator first, iterator last){
		if (first != last)
			transfer(position, first, last);
	}
	void reserve(){
		if (key->next == key || key->next->next == key)	return;
		iterator first = begin();
		iterator end = (--end());
		while (first != end){
			(*first++) = (*end--);
		}
	}
};
#endif