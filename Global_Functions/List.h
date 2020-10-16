#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020 Gitaroo Pals
 *
 *  Gitaroo Man File Reader is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  Gitaroo Man File Reader is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Gitaroo Man File Reader.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#include <memory>
template <class T>
class ListNode;
template <class T>
class ListNode<T*>;
template <class T>
class ListNode<std::shared_ptr<T>>;

/*
	A linked list template class
*/
template <class T>
class List
{
private:
	struct placeSaver
	{
		size_t index = 0;
		ListNode<T>* prevNode = nullptr;
	};

	struct SharedValues
	{
		ListNode<T>* root = nullptr;
		ListNode<T>* tail = nullptr;
		size_t count = 0;
		placeSaver lastAccessed;
	};
	std::shared_ptr<SharedValues> shared;

	//Returns the pointer of the node at the given index. Returns "nullptr" if index >= count.
	ListNode<T>* find(size_t index);
	//Returns the index for where, in order, the new node was placed
	size_t findOrderedPosition(ListNode<T>* newNode);
public:
	//Create empty list
	List();
	//Create list of specified size
	//Every parameter after "size" is used in
	//the construction of every new node
	template <class... Args>
	List(size_t size, Args&&... args);
	//Creates a list based off the provided initializer list
	List(const std::initializer_list<T>& init);
	//Creates a list based off the provided initializer list
	//Uses t
	template <class... Args>
	List(const std::initializer_list<Args...>& init);
	//Create a list with cloned elements from the provided list
	List(const List& list);
	List& clone(const List& list);
	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List& operator=(const std::initializer_list<T>& init);
	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	template <class... Args>
	List& operator=(const std::initializer_list<std::initializer_list<Args...>>& init);
	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List& operator=(const List& list);
	//Calls clear, then deletes all pointers
	~List();
	//Returns count
	size_t& size() const;
	//Creates a new element at the end of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_back(const T& data);
	//Creates a new element at the beginning of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_front(const T& data);
	//Creates a new element at the end of the list using a constructor that 
	//takes all the parameters provided to the function
	//
	//--CODING NOTE: Ensure that this specialized constructor for the element type
	//exist. The compiler will throw an error if that is not the case.
	template <class... Args>
	T& emplace_back(Args&&... args);
	//Creates a new element at the beginning of the list using a constructor that 
	//takes all the parameters provided to the function
	//
	//--CODING NOTE: Ensure that this specialized constructor for the element type
	//exist. The compiler will throw an error if that is not the case.
	template <class... Args>
	T& emplace_front(Args&&... args);
	//Maneuvers to the specified index and then creates numElements number of new elements
	//as shallow copies of T object "data"
	size_t insert(size_t index, size_t numElements, const T& data);
	//Maneuvers to the specified index and then creates a new element 
	//with a shallow copy of T object "data"
	size_t insert(size_t index, const T& data);
	//Maneuvers to the specified index and then creates numElements number of new elements
	//using the args provided as the parameters for object T's constructor
	template <class... Args>
	size_t emplace(size_t index, size_t numElements, Args&&... args);
	//Uses "data" to find the proper position in the list then constructs
	//a new element in that position with a shallow copy of "data"
	size_t insert_ordered(const T& data);
	//Constructs a new element using the values provided in parameter pack "args"
	//and then finds that element's ordered placement in the list
	template <class... Args>
	size_t emplace_ordered(Args&&... args);
	//Changes the size of a list
	//
	//--If newSize is bigger, the values inside parameter pack args
	//are used to construct new elements at the end of the list. If
	//args is simply an object of type T, create a deep copy.
	//--If newSize is smaller, elements are removed starting from the end
	template <class... Args>
	void resize(size_t newSize, Args&&... args);
	//Changes the size of a list
	//
	//--If newSize is bigger, new elements are created at the end of the list
	//as shallow copies of object "data"
	//--If newSize is smaller, elements are removed starting from the end
	void resize(const T& data, size_t newSize);
	//Removes the shared->root element
	void pop_front();
	//Removes the shared->tail element
	void pop_back();
	//Deletes "numElements" number of elements from the list starting at the provided index
	bool erase(size_t index, size_t numElements = 1);
	//Deletes all elements
	void clear();
	//Returns first element if one exists
	T& front();
	//Returns last element if one exists
	T& back();
	//Returns element at index if one exists
	T& operator[](size_t index);
	//Returns the index with an element that matches the provided object.
	//Starts the search from the provided index
	//If searching fails, returns -1.
	int search(T& compare, size_t startIndex = 0);
	//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
	void moveElements(size_t index, size_t newPosition, size_t numElements = 1);
	//Swaps the contents of the two lists.
	List<T>& swap(List<T>& other);
};

template <class T>
class ListNode
{
	friend List<T>;
	T data;
	ListNode* prev;
	ListNode* next;
	//Calls default constructor for data
	ListNode();
	//Creates a shallow copy of the provided object
	ListNode(const T& data, ListNode* pr = nullptr, ListNode* nt = nullptr);
	//Uses the arguments provided in "args" to create a new object.
	//If args is an object of type T, it creates a deep copy.
	template <class... Args>
	ListNode(ListNode* pr, ListNode* nt, Args&&... args);
	ListNode operator=(const ListNode& node);
	ListNode operator=(const T& obj);
	bool operator==(ListNode& note) const;
	bool operator!=(ListNode& note) const;
	bool operator>=(ListNode& note) const;
	bool operator>(ListNode& note) const;
	bool operator<=(ListNode& note) const;
	bool operator<(ListNode& note) const;
};

template <class T>
class ListNode<T*>
{
	friend List<T*>;
	T* data;
	ListNode* prev;
	ListNode* next;
	bool emplaced;
	//Calls default constructor for data
	ListNode();
	ListNode(T* data, ListNode* pr = nullptr, ListNode* nt = nullptr);
	//Uses the arguments provided in "args" to create a new object.
	//If args is an object of type T, it creates a deep copy.
	template <class... Args>
	ListNode(ListNode* pr, ListNode* nt, Args&&... args);
	~ListNode();
	ListNode operator=(const ListNode& node);
	ListNode operator=(T* ptr);
	bool operator==(ListNode& note) const;
	bool operator!=(ListNode& note) const;
	bool operator>=(ListNode& note) const;
	bool operator>(ListNode& note) const;
	bool operator<=(ListNode& note) const;
	bool operator<(ListNode& note) const;
};

template <class T>
class ListNode<std::shared_ptr<T>>
{
	friend List<std::shared_ptr<T>>;
	std::shared_ptr<T> data;
	ListNode* prev;
	ListNode* next;
	//Calls default constructor for data
	ListNode();
	ListNode(T* data, ListNode* pr = nullptr, ListNode* nt = nullptr);
	ListNode(const std::shared_ptr<T>& data, ListNode* pr = nullptr, ListNode* nt = nullptr);
	//Uses the arguments provided in "args" to create a new object.
	//If args is an object of type T, it creates a deep copy.
	template <class... Args>
	ListNode(ListNode* pr, ListNode* nt, Args&&... args);
	ListNode operator=(const ListNode& node);
	ListNode operator=(T* node);
	ListNode operator=(const std::shared_ptr<T>& ptr);
	bool operator==(ListNode& note) const;
	bool operator!=(ListNode& note) const;
	bool operator>=(ListNode& note) const;
	bool operator>(ListNode& note) const;
	bool operator<=(ListNode& note) const;
	bool operator<(ListNode& note) const;
};
