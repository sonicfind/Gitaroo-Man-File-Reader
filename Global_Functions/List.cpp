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

#include "..\Header\pch.h"
#include "List.h"

// ListNode<T>

template <class T>
ListNode<T>::ListNode() : data(), prev(nullptr), next(nullptr) {}
template <class T>
ListNode<T>::ListNode(const T& data, ListNode* pr, ListNode* nt) : data(), prev(pr), next(nt)
{
	this->data = data;
}
template <class T>
template <class... Args>
ListNode<T>::ListNode(ListNode* pr, ListNode* nt, Args&&... args) : prev(pr), next(nt), data(args...) {}

template <class T>
ListNode<T> ListNode<T>::operator=(const ListNode& node)
{
	data = node.data;
	return *this;
}
template <class T>
ListNode<T> ListNode<T>::operator=(const T& obj)
{
	data = obj;
	return *this;
}
template <class T>
bool ListNode<T>::operator==(ListNode& note) const { return data == note.data; }
template <class T>
bool ListNode<T>::operator!=(ListNode& note) const { return data != note.data; }
template <class T>
bool ListNode<T>::operator>=(ListNode& note) const { return data >= note.data; }
template <class T>
bool ListNode<T>::operator>(ListNode& note) const { return data > note.data; }
template <class T>
bool ListNode<T>::operator<=(ListNode& note) const { return data <= note.data; }
template <class T>
bool ListNode<T>::operator<(ListNode& note) const { return data < note.data; }

// ListNode<T*>

template <class T>
ListNode<T*>::ListNode() : data(nullptr), prev(nullptr), next(nullptr), emplaced(false) {}
template <class T>
ListNode<T*>::ListNode(T* data, ListNode* pr, ListNode* nt) : data(data), prev(pr), next(nt), emplaced(false) {}
template <class T>
template <class... Args>
ListNode<T*>::ListNode(ListNode* pr, ListNode* nt, Args&&... args) : prev(pr), next(nt), data(new T(args...)), emplaced(true) {}

template <class T>
ListNode<T*>::~ListNode()
{
	if (emplaced)
		delete data;
}
template <class T>
ListNode<T*> ListNode<T*>::operator=(const ListNode& node)
{
	*data = *node.data;
	return *this;
}
template <class T>
ListNode<T*> ListNode<T*>::operator=(T* ptr)
{
	data = ptr;
	emplaced = false;
	return *this;
}
template <class T>
bool ListNode<T*>::operator==(ListNode& note) const { return *data == *note.data; }
template <class T>
bool ListNode<T*>::operator!=(ListNode& note) const { return *data != *note.data; }
template <class T>
bool ListNode<T*>::operator>=(ListNode& note) const { return *data >= *note.data; }
template <class T>
bool ListNode<T*>::operator>(ListNode& note) const { return *data > * note.data; }
template <class T>
bool ListNode<T*>::operator<=(ListNode& note) const { return *data <= *note.data; }
template <class T>
bool ListNode<T*>::operator<(ListNode& note) const { return *data < *note.data; }

// ListNode<std::shared_ptr<T>>

template <class T>
ListNode<std::shared_ptr<T>>::ListNode() : data(nullptr), prev(nullptr), next(nullptr) {}
template <class T>
ListNode<std::shared_ptr<T>>::ListNode(T* data, ListNode* pr, ListNode* nt) : data(data), prev(pr), next(nt) {}
template <class T>
ListNode<std::shared_ptr<T>>::ListNode(const std::shared_ptr<T>& data, ListNode* pr, ListNode* nt) : data(data), prev(pr), next(nt) {}
template <class T>
template <class... Args>
ListNode<std::shared_ptr<T>>::ListNode(ListNode* pr, ListNode* nt, Args&&... args) : prev(pr), next(nt), data(new T(args...)){}
template <class T>
ListNode<std::shared_ptr<T>> ListNode<std::shared_ptr<T>>::operator=(const ListNode& node)
{
	*data = *node.data;
	return *this;
}
template <class T>
ListNode<std::shared_ptr<T>> ListNode<std::shared_ptr<T>>::operator=(T* ptr)
{
	data = std::make_shared<T>(ptr);
	return *this;
}
template <class T>
ListNode<std::shared_ptr<T>> ListNode<std::shared_ptr<T>>::operator=(const std::shared_ptr<T>& ptr)
{
	data = ptr;
	return *this;
}
template <class T>
bool ListNode<std::shared_ptr<T>>::operator==(ListNode& note) const { return *data == *note.data; }
template <class T>
bool ListNode<std::shared_ptr<T>>::operator!=(ListNode& note) const { return *data != *note.data; }
template <class T>
bool ListNode<std::shared_ptr<T>>::operator>=(ListNode& note) const { return *data >= *note.data; }
template <class T>
bool ListNode<std::shared_ptr<T>>::operator>(ListNode& note) const { return *data > * note.data; }
template <class T>
bool ListNode<std::shared_ptr<T>>::operator<=(ListNode& note) const { return *data <= *note.data; }
template <class T>
bool ListNode<std::shared_ptr<T>>::operator<(ListNode& note) const { return *data < *note.data; }

// List<T>

template <class T>
ListNode<T>* List<T>::find(size_t index)
{
	if (index < shared->count)
	{
		ListNode<T>* cur;
		if (index == shared->lastAccessed.index) //Return shared->lastAccessed's node
			cur = shared->lastAccessed.prevNode;
		else if (index == 0) //Return shared->root
			cur = shared->root;
		else if (index == (shared->count) - 1) //Return shared->tail
			cur = shared->tail;
		else if (shared->lastAccessed.prevNode == nullptr || signed(index) <= signed(shared->lastAccessed.index) - index)
			//If shared->lastAccessed is empty, or if index is less than or equal to the difference between shared->lastAccessed and the given index
		{
			cur = shared->root;
			for (size_t i = 0; i < index; i++)
				cur = cur->next;
		}
		else if (index > shared->lastAccessed.index)
		{
			if (index - shared->lastAccessed.index <= (shared->count) - 1) //Start from LA and go up
			{
				cur = shared->lastAccessed.prevNode;
				for (size_t i = shared->lastAccessed.index; i < index; i++)
					cur = cur->next;
			}
			else //Start from shared->tail and go down
			{
				cur = shared->tail;
				for (size_t i = (shared->count) - 1; i > index; i--)
					cur = cur->prev;
			}
		}
		else //Start from LA and go down
		{
			cur = shared->lastAccessed.prevNode;
			for (size_t i = shared->lastAccessed.index; i > index; i--)
				cur = cur->prev;
		}
		shared->lastAccessed = { index, cur };
		return cur;
	}
	else
		return nullptr;
}

template <class T>
size_t List<T>::findOrderedPosition(ListNode<T>* newNode)
{
	size_t index = shared->lastAccessed.index;
	if (*newNode == *newNode->next)
	{
		*newNode->next = *newNode;
		delete newNode;
		return index;
	}
	else if (*newNode > * newNode->next)
	{
		newNode->prev = newNode->next;
		newNode->next = newNode->next->next;
		index++;
		while (newNode->next != nullptr)
		{
			if (*newNode == *newNode->next)
			{
				*newNode->next = *newNode;
				delete newNode;
				return index;
			}
			else if (*newNode < *newNode->next)
				break;
			else
			{
				newNode->prev = newNode->next;
				newNode->next = newNode->next->next;
				index++;
			}
		}
	}
	else
	{
		newNode->next = newNode->prev;
		newNode->prev = newNode->prev->prev;
		index--;
		while (newNode->prev != nullptr)
		{
			if (*newNode == *newNode->prev)
			{
				*newNode->prev = *newNode;
				delete newNode;
				return index;
			}
			else if (*newNode > * newNode->prev)
				break;
			else
			{
				newNode->next = newNode->prev;
				newNode->prev = newNode->prev->prev;
				index--;
			}
		}
	}
	if (newNode->next == nullptr)
		shared->tail = newNode;
	else
		newNode->next->prev = newNode;
	if (newNode->prev == nullptr)
		shared->root = newNode;
	else
		newNode->prev->next = newNode;
	shared->count++;
	shared->lastAccessed = { index, newNode };
	return index;
}

template <class T>
List<T>::List() : shared(std::make_shared<SharedValues>()) {}

template <class T>
template <class... Args>
List<T>::List(size_t size, Args&&... args) : shared(std::make_shared<SharedValues>())
{
	for (unsigned i = 0; i < size; i++)
		emplace_back(args...);
}

template <class T>
List<T>::List(const std::initializer_list<T>& init) : shared(std::make_shared<SharedValues>())
{
	for (unsigned i = 0; i < init.size(); i++)
		push_back(init.begin()[i]);
}

template <class T>
template <class... Args>
List<T>::List(const std::initializer_list<Args...>& init) : shared(std::make_shared<SharedValues>())
{
	for (unsigned i = 0; i < init.size(); i++)
		emplace_back(init.begin()[i]);
}

template <class T>
List<T>::List(const List<T>& list) : shared(std::make_shared<SharedValues>())
{
	ListNode<T>* cur = list.shared->root;
	for (unsigned i = 0; i < list.shared->count; i++)
	{
		emplace_back(cur->data);
		cur = cur->next;
	}
}

template <class T>
List<T>& List<T>::clone(const List<T>& list)
{
	if (shared == list.shared)
		return *this;

	if (shared.use_count() == 1)
		clear();
	shared = std::make_shared<SharedValues>();
	ListNode<T>* cur = list.shared->root;
	for (unsigned i = 0; i < list.shared->count; i++)
	{
		emplace_back(cur->data);
		cur = cur->next;
	}
	return *this;
}

template <class T>
List<T>& List<T>::operator=(const std::initializer_list<T>& init)
{
	if (shared.use_count() == 1)
		clear();
	shared = std::make_shared<SharedValues>();
	for (unsigned i = 0; i < init.size(); i++)
		push_back(init.begin()[i]);
	return *this;
}

template <class T>
template <class... Args>
List<T>& List<T>::operator=(const std::initializer_list<std::initializer_list<Args...>>& init)
{
	if (shared.use_count() == 1)
		clear();
	shared = std::make_shared<SharedValues>();
	for (unsigned i = 0; i < init.size(); i++)
		emplace_back(init.begin()[i].begin()...);
	return *this;
}

template <class T>
List<T>& List<T>::operator=(const List<T>& list)
{
	if (shared != list.shared)
	{
		if (shared.use_count() == 1)
			clear();
		shared = list.shared;
	}
	return *this;
}

template <class T>
List<T>::~List()
{
	if (shared.use_count() == 1)
		clear();
}

template <class T>
size_t& List<T>::size() const { return shared->count; }

template <class T>
void List<T>::push_back(const T& data)
{
	if (shared->count)
		shared->tail = shared->tail->next = new ListNode<T>(data, shared->tail);
	else
		shared->root = shared->tail = new ListNode<T>(data);
	shared->lastAccessed = { shared->count, shared->tail };
	shared->count++;
}

template <class T>
void List<T>::push_front(const T& data)
{
	if (shared->count)
		shared->root = shared->root->prev = new ListNode<T>(data, nullptr, shared->root);
	else
		shared->root = shared->tail = new ListNode<T>(data);
	shared->lastAccessed = { 0, shared->root };
	shared->count++;
}

template <class T>
template <class... Args>
T& List<T>::emplace_back(Args&&... args)
{
	if (shared->count)
		shared->tail = shared->tail->next = new ListNode<T>(shared->tail, nullptr, args...);
	else
		shared->root = shared->tail = new ListNode<T>(nullptr, nullptr, args...);
	shared->lastAccessed = { shared->count, shared->tail };
	shared->count++;
	return shared->tail->data;
}

template <class T>
template <class... Args>
T& List<T>::emplace_front(Args&&... args)
{
	if (shared->count)
		shared->root = shared->root->prev = new ListNode<T>(nullptr, shared->root, args...);
	else
		shared->root = shared->tail = new ListNode<T>(nullptr, nullptr, args...);
	shared->lastAccessed = { 0, shared->root };
	shared->count++;
	return shared->root->data;
}

template <class T>
size_t List<T>::insert(size_t index, size_t numElements, const T& data)
{
	ListNode<T>* cur;
	size_t curIndex;
	if (!shared->count)
	{
		cur = shared->root = shared->tail = new ListNode<T>(data);
		curIndex = 1;
		shared->count++;
	}
	else if (index == 0)
	{
		cur = shared->root = shared->root->prev = new ListNode<T>(data, nullptr, shared->root);
		curIndex = 1;
		shared->count++;
	}
	else if (index >= shared->count)
	{
		cur = shared->tail = shared->tail->next = new ListNode<T>(data, shared->tail, nullptr);
		curIndex = (shared->count) + 1;
		shared->count++;
	}
	else
	{
		cur = find(index);
		cur->prev = cur->prev->next = new ListNode<T>(data, cur->prev, cur);
		curIndex = index + 1;
		shared->count++;
	}
	while (curIndex < index + numElements)
	{
		cur->prev = cur->prev->next = new ListNode<T>(data, cur, cur->next);
		curIndex++;
		shared->count++;
	}
	shared->lastAccessed = { curIndex - 1, cur };
	return curIndex - 1;
}

template <class T>
size_t List<T>::insert(size_t index, const T& data)
{
	return insert(index, 1, data);
}

template <class T>
template <class... Args>
size_t List<T>::emplace(size_t index, size_t numElements, Args&&... args)
{
	ListNode<T>* cur;
	size_t curIndex;
	if (!shared->count)
	{
		cur = shared->root = shared->tail = new ListNode<T>(nullptr, nullptr, args...);
		curIndex = 1;
		shared->count++;
	}
	else if (index == 0)
	{
		cur = shared->root = shared->root->prev = new ListNode<T>(nullptr, shared->root, args...);
		curIndex = 1;
		shared->count++;
	}
	else if (index >= shared->count)
	{
		cur = shared->tail = shared->tail->next = new ListNode<T>(shared->tail, nullptr, args...);
		curIndex = (shared->count) + 1;
		shared->count++;
	}
	else
	{
		cur = find(index);
		cur = cur->prev = cur->prev->next = new ListNode<T>(cur->prev, cur, args...);
		curIndex = index + 1;
		shared->count++;
	}
	while (curIndex < index + numElements)
	{
		cur->prev = cur->prev->next = new ListNode<T>(cur, cur->next, args...);
		curIndex++;
		shared->count++;
	}
	shared->lastAccessed = { curIndex - 1, cur };
	return curIndex - 1;
}

template <class T>
size_t List<T>::insert_ordered(const T& data)
{
	if (shared->lastAccessed.prevNode != nullptr)
		return findOrderedPosition(new ListNode<T>(data, shared->lastAccessed.prevNode->prev, shared->lastAccessed.prevNode));
	else
	{
		push_back(data);
		return 0;
	}
}

template <class T>
template <class... Args>
size_t List<T>::emplace_ordered(Args&&... args)
{
	if (shared->lastAccessed.prevNode != nullptr)
		return findOrderedPosition(new ListNode<T>(shared->lastAccessed.prevNode->prev, shared->lastAccessed.prevNode, args...));
	else
	{
		emplace_back(args...);
		return 0;
	}
}

template <class T>
template <class... Args>
void List<T>::resize(size_t newSize, Args&&... args)
{
	if (newSize > shared->count)
	{
		if (!shared->count)
		{
			shared->root = shared->tail = new ListNode<T>(nullptr, nullptr, args...);
			shared->count++;
		}
		while (shared->count < newSize)
		{
			shared->tail = shared->tail->next = new ListNode<T>(shared->tail, nullptr, args...);
			shared->count++;
		}
		shared->lastAccessed = { shared->count - 1, shared->tail };
	}
	else if (newSize < shared->count)
	{
		while (shared->count > newSize)
		{
			ListNode<T>* cur = shared->tail;
			shared->tail = shared->tail->prev;
			delete cur;
			shared->count--;
		}
		if (shared->count)
		{
			shared->tail->next = nullptr;
			shared->lastAccessed = { shared->count - 1, shared->tail };
		}
		else
		{
			shared->root = nullptr;
			shared->lastAccessed = { 0, nullptr };
		}
	}
}

template <class T>
void List<T>::resize(const T& data, size_t newSize)
{
	if (newSize > shared->count)
	{
		if (!shared->count)
		{
			shared->root = shared->tail = new ListNode<T>(data);
			shared->count++;
		}
		while (shared->count < newSize)
		{
			shared->tail = shared->tail->next = new ListNode<T>(data, shared->tail);
			shared->count++;
		}
		shared->lastAccessed = { shared->count - 1, shared->tail };
	}
	else if (newSize < shared->count)
	{
		while (shared->count > newSize)
		{
			ListNode<T>* cur = shared->tail;
			shared->tail = shared->tail->prev;
			delete cur;
			shared->count--;
		}
		if (shared->count)
		{
			shared->tail->next = nullptr;
			shared->lastAccessed = { shared->count - 1, shared->tail };
		}
		else
		{
			shared->root = nullptr;
			shared->lastAccessed = { 0, nullptr };
		}
	}
}

template <class T>
void List<T>::pop_front()
{
	if (shared->count)
	{
		shared->count--;
		if (shared->root == shared->lastAccessed.prevNode)
			shared->lastAccessed.prevNode = shared->lastAccessed.prevNode->next;
		else
			shared->lastAccessed.index--;
		ListNode<T>* cur = shared->root;
		shared->root = shared->root->next;
		delete cur;
		if (shared->root != nullptr)
			shared->root->prev = nullptr;
		else
			shared->tail = nullptr;
	}
}

template <class T>
void List<T>::pop_back()
{
	if (shared->count)
	{
		shared->count--;
		if (shared->tail == shared->lastAccessed.prevNode)
		{
			//Overflow if index is 0 doesn't matter as it would mean count is also 0
			shared->lastAccessed.index--;
			shared->lastAccessed.prevNode = shared->lastAccessed.prevNode->prev;
		}
		ListNode<T>* cur = shared->tail;
		shared->tail = shared->tail->prev;
		delete cur;
		if (shared->tail != nullptr)
			shared->tail->next = nullptr;
		else
			shared->root = nullptr;
	}
}

template <class T>
bool List<T>::erase(size_t index, size_t numElements)
{
	bool result = false;
	ListNode<T>* cur = find(index);
	while (numElements > 0 && cur != nullptr)
	{
		ListNode<T>* next = cur->next;
		if (cur->prev != nullptr)
			cur->prev->next = next;
		else
			shared->root = next;
		if (next != nullptr)
			next->prev = cur->prev;
		else
			shared->tail = cur->prev;
		delete cur;
		cur = next;
		numElements--;
		shared->count--;
		result = true;
	}
	if (cur != nullptr)
		shared->lastAccessed = { index, cur };
	else if (shared->count)
		shared->lastAccessed = { (shared->count) - 1, shared->tail };
	else
		shared->lastAccessed = { 0, nullptr };
	return result;
}

template <class T>
void List<T>::clear()
{
	while (shared->count)
	{
		ListNode<T>* next = shared->root->next;
		delete shared->root;
		shared->root = next;
		shared->count--;
	}
	shared->lastAccessed = { 0, nullptr };
	shared->tail = nullptr;
}

template <class T>
T& List<T>::front()
{
	if (shared->count)
	{
		shared->lastAccessed = { 0, shared->root };
		return shared->root->data;
	}
	else
		throw "Error: list index out of range";
}

template <class T>
T& List<T>::back()
{
	if (shared->count)
	{
		shared->lastAccessed = { (shared->count) - 1, shared->tail };
		return shared->tail->data;
	}
	else
		throw "Error: list index out of range";
}

template <class T>
T& List<T>::operator[](size_t index)
{
	if (index < shared->count)
		return find(index)->data;
	else
		throw "Error: list index out of range";
}

template <class T>
int List<T>::search(T& compare, size_t startIndex)
{
	ListNode<T>* cur = find(startIndex);
	while (cur != nullptr)
	{
		if (cur->data == compare)
		{
			shared->lastAccessed = { startIndex, cur };
			return (int)startIndex;
		}
		else
		{
			startIndex++;
			cur = cur->next;
		}
	}
	//Only reaches here if searching fails
	return -1;
}

template <class T>
void List<T>::moveElements(size_t index, size_t newPosition, size_t numElements)
{
	if (index >= shared->count)
		throw "Error: list index out of range";
	else if (newPosition > shared->count)
		throw "Error: list newPosition index out of range";
	else
	{
		if (index + numElements > shared->count)
			numElements = shared->count - index;
		else if (numElements == 0)
			numElements = 1;
		ListNode<T>* beg = find(index);
		if (newPosition < index)
		{
			ListNode<T>* end = find(index + numElements - 1);
			ListNode<T>* pos = find(newPosition);
			beg->prev->next = end->next;
			if (end->next != nullptr)
				end->next->prev = beg->prev;
			else
				shared->tail = beg->prev;
			if (pos->prev != nullptr)
				pos->prev->next = beg;
			else
				shared->root = beg;
			beg->prev = pos->prev;
			end->next = pos;
			pos->prev = end;
			shared->lastAccessed = { newPosition, beg };
		}
		else if (newPosition > index + numElements)
		{
			ListNode<T>* end = find(index + numElements - 1);
			ListNode<T>* pos = find(newPosition - 1);
			end->next->prev = beg->prev;
			if (beg->prev != nullptr)
				beg->prev->next = end->next;
			else
				shared->root = end->next;
			if (pos->next != nullptr)
				pos->next->prev = end;
			else
				shared->tail = end;
			end->next = pos->next;
			beg->prev = pos;
			pos->next = beg;
			shared->lastAccessed = { newPosition, beg };
		}
	}
}

template <class T>
List<T>& List<T>::swap(List<T>& other)
{
	if (shared == other.shared)
		return *this;
	SharedValues* buffer = shared.get();
	shared = other.shared;
	other.shared = buffer;
	return *this;
}
