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

/*
	A linked list template class
*/
template <class T>
class List
{
private:
	struct Node
	{
		T data;
		Node* prev;
		Node* next;
		//Calls default constructor for data
		Node() : data(), prev(nullptr), next(nullptr) {}
		//Creates a shallow copy of the provided object
		Node(const T& data, Node* pr = nullptr, Node* nt = nullptr) : data(), prev(pr), next(nt)
		{
			this->data = data;
		}
		//Uses the arguments provided in "args" to create a new object.
		//If args is an object of type T, it creates a deep copy.
		template <class... Args>
		Node(Node* pr, Node* nt, Args&&... args) : prev(pr), next(nt), data(args...) {}
	};

	Node** root;
	Node** tail;
	struct placeSaver
	{
		size_t index = 0;
		Node* prevNode = nullptr;
	};
	placeSaver* lastAccessed;
	size_t* count;
	size_t* UsedCount;
	//Returns the pointer of the node at the given index. Returns "nullptr" if index >= count.
	Node* find(size_t index)
	{
		if (index < *count)
		{
			Node* cur;
			if (index == lastAccessed->index) //Return *lastAccessed's node
				cur = lastAccessed->prevNode;
			else if (index == 0) //Return *root
				cur = *root;
			else if (index == (*count) - 1) //Return *tail
				cur = *tail;
			else if (lastAccessed->prevNode == nullptr || signed(index) <= signed(lastAccessed->index) - index)
				//If *lastAccessed is empty, or if index is less than or equal to the difference between *lastAccessed and the given index
			{
				cur = *root;
				for (size_t i = 0; i < index; i++)
					cur = cur->next;
			}
			else if (index > lastAccessed->index)
			{
				if (index - lastAccessed->index <= (*count) - 1) //Start from LA and go up
				{
					cur = lastAccessed->prevNode;
					for (size_t i = lastAccessed->index; i < index; i++)
						cur = cur->next;
				}
				else //Start from *tail and go down
				{
					cur = *tail;
					for (size_t i = (*count) - 1; i > index; i--)
						cur = cur->prev;
				}
			}
			else //Start from LA and go down
			{
				cur = lastAccessed->prevNode; 
				for (size_t i = lastAccessed->index; i > index; i--)
					cur = cur->prev;
			}
			*lastAccessed = { index, cur };
			return cur;
		}
		else
			return nullptr;
	}
	//Returns the index for where, in order, the new node was placed
	size_t findOrderedPosition(Node* newNode)
	{
		size_t index = lastAccessed->index;
		if (newNode->data == newNode->next->data)
		{
			newNode->next->data = newNode->data;
			delete newNode;
			return index;
		}
		else if (newNode->data > newNode->next->data)
		{
			while (newNode->next != nullptr)
			{
				if (newNode->data == newNode->next->data)
				{
					newNode->next->data = newNode->data;
					delete newNode;
					return index;
				}
				else if (newNode->data < newNode->next->data)
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
			while (newNode->prev != nullptr)
			{
				if (newNode->data == newNode->prev->data)
				{
					newNode->prev->data = newNode->data;
					delete newNode;
					return index;
				}
				else if (newNode->data > newNode->prev->data)
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
			*tail = newNode;
		else
			newNode->next->prev = newNode;
		if (newNode->prev == nullptr)
			*root = newNode;
		else
			newNode->prev->next = newNode;
		(*count)++;
		*lastAccessed = { index, newNode };
		return index;
	}
public:
	//Create empty list
	List<T>()
	{
		root = new Node*(nullptr);
		tail = new Node*(nullptr);
		count = new size_t(0);
		UsedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
	}

	//Create list of specified size
	//Every parameter after "size" is used in
	//the construction of every new node
	template <class... Args>
	List<T>(size_t size, Args&&... args) : List()
	{
		for (unsigned i = 0; i < size; i++)
			emplace_back(args...);
	}

	//Creates a list based off the provided initializer list
	List<T>(const std::initializer_list<T>& init) : List()
	{
		for (unsigned i = 0; i < init.size(); i++)
			push_back(init.begin()[i]);
	}

	//Creates a list based off the provided initializer list
	//Uses t
	template <class... Args>
	List<T>(const std::initializer_list<Args...>& init) : List()
	{
		for (unsigned i = 0; i < init.size(); i++)
			emplace_back(init.begin()[i]);
	}

	//Create a list with cloned elements from the provided list
	List<T>(const List<T>& list) : List()
	{
		Node* cur = *list.root;
		for (unsigned i = 0; i < *list.count; i++)
		{
			emplace_back(cur->data);
			cur = cur->next;
		}
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List<T>& operator=(const std::initializer_list<T>& init)
	{
		if (*UsedCount > 1)
			(*UsedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete UsedCount;
		}
		root = new Node*(nullptr);
		tail = new Node*(nullptr);
		lastAccessed = new placeSaver{ 0, nullptr };
		count = new size_t(0);
		UsedCount = new size_t(1);
		for (unsigned i = 0; i < init.size(); i++)
			push_back(init.begin()[i]);
		return *this;
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	template <class... Args>
	List<T>& operator=(const std::initializer_list<std::initializer_list<Args...>>& init)
	{
		if (*UsedCount > 1)
			(*UsedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete UsedCount;
		}
		root = new Node*(nullptr);
		tail = new Node*(nullptr);
		lastAccessed = new placeSaver{ 0, nullptr };
		count = new size_t(0);
		UsedCount = new size_t(1);
		for (unsigned i = 0; i < init.size(); i++)
			emplace_back(init.begin()[i].begin()...);
		return *this;
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List<T>& operator=(const List<T>& list)
	{
		if (UsedCount != list.UsedCount)
		{
			if (*UsedCount > 1)
				(*UsedCount)--;
			else
			{
				clear();
				delete root;
				delete tail;
				delete count;
				delete lastAccessed;
				delete UsedCount;
			}
			root = list.root;
			tail = list.tail;
			count = list.count;
			lastAccessed = list.lastAccessed;
			UsedCount = list.UsedCount;
			(*UsedCount)++;
		}
		return *this;
	}

	//Calls clear, then deletes all pointers
	~List<T>()
	{
		if (*UsedCount > 1)
			(*UsedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete UsedCount;
		}
	}

	//Returns count
	size_t& size() const { return *count; }

	//Creates a new element at the end of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_back(const T& data)
	{
		if (*count)
			*tail = (*tail)->next = new Node(data, *tail);
		else
			*root = *tail = new Node(data);
		*lastAccessed = { *count, *tail };
		(*count)++;
	}

	//Creates a new element at the beginning of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_front(const T& data)
	{
		if (*count)
			*root = (*root)->prev = new Node(data, nullptr, *root);
		else
			*root = *tail = new Node(data);
		*lastAccessed = { 0, *root };
		(*count)++;
	}

	//Creates a new element at the end of the list using a constructor that 
	//takes all the parameters provided to the function
	//
	//--CODING NOTE: Ensure that this specialized constructor for the element type
	//exist. The compiler will throw an error if that is not the case.
	template <class... Args>
	T& emplace_back(Args&&... args)
	{
		if (*count)
			*tail = (*tail)->next = new Node(*tail, nullptr, args...);
		else
			*root = *tail = new Node(nullptr, nullptr, args...);
		*lastAccessed = { *count, *tail };
		(*count)++;
		return (*tail)->data;
	}

	//Creates a new element at the beginning of the list using a constructor that 
	//takes all the parameters provided to the function
	//
	//--CODING NOTE: Ensure that this specialized constructor for the element type
	//exist. The compiler will throw an error if that is not the case.
	template <class... Args>
	T& emplace_front(Args&&... args)
	{
		if (*count)
			*root = (*root)->prev = new Node(nullptr, *root, args...);
		else
			*root = *tail = new Node(nullptr, nullptr, args...);
		*lastAccessed = { 0, *root };
		(*count)++;
		return (*root)->data;
	}

	//Maneuvers to the specified index and then creates numElements number of new elements
	//as shallow copies of T object "data"
	size_t insert(size_t index, size_t numElements, const T& data)
	{
		Node* cur;
		size_t curIndex;
		if (!*count)
		{
			cur = *root = *tail = new Node(data);
			curIndex = 1;
			(*count)++;
		}
		else if (index == 0)
		{
			cur = *root = (*root)->prev = new Node(data, nullptr, *root);
			curIndex = 1;
			(*count)++;
		}
		else if (index >= *count)
		{
			cur = *tail = (*tail)->next = new Node(data, *tail, nullptr);
			curIndex = (*count) + 1;
			(*count)++;
		}
		else
		{
			cur = find(index);
			cur->prev = cur->prev->next = new Node(data, cur->prev, cur);
			curIndex = index + 1;
			(*count)++;
		}
		while (curIndex < index + numElements)
		{
			cur->prev = cur->prev->next = new Node(data, cur, cur->next);
			curIndex++;
			(*count)++;
		}
		*lastAccessed = { curIndex - 1, cur };
		return curIndex - 1;
	}

	//Maneuvers to the specified index and then creates a new element 
	//with a shallow copy of T object "data"
	size_t insert(size_t index, const T& data)
	{
		return insert(index, 1, data);
	}

	//Maneuvers to the specified index and then creates numElements number of new elements
	//using the args provided as the parameters for object T's constructor
	template <class... Args>
	size_t emplace(size_t index, size_t numElements, Args&&... args)
	{
		Node* cur;
		size_t curIndex;
		if (!*count)
		{
			cur = *root = *tail = new Node(nullptr, nullptr, args...);
			curIndex = 1;
			(*count)++;
		}
		else if (index == 0)
		{
			cur = *root = (*root)->prev = new Node(nullptr, *root, args...);
			curIndex = 1;
			(*count)++;
		}
		else if (index >= *count)
		{
			cur = *tail = (*tail)->next = new Node(*tail, nullptr, args...);
			curIndex = (*count) + 1;
			(*count)++;
		}
		else
		{
			cur = find(index);
			cur = cur->prev = cur->prev->next = new Node(cur->prev, cur, args...);
			curIndex = index + 1;
			(*count)++;
		}
		while (curIndex < index + numElements)
		{
			cur->prev = cur->prev->next = new Node(cur, cur->next, args...);
			curIndex++;
			(*count)++;
		}
		*lastAccessed = { curIndex - 1, cur };
		return curIndex - 1;
	}

	//Uses "data" to find the proper position in the list then constructs
	//a new element in that position with a shallow copy of "data"
	size_t insert_ordered(const T& data)
	{
		if (lastAccessed->prevNode != nullptr)
			return findOrderedPosition(new Node(data, lastAccessed->prevNode->prev, lastAccessed->prevNode));
		else
		{
			push_back(data);
			return 0;
		}
	}

	//Constructs a new element using the values provided in parameter pack "args"
	//and then finds that element's ordered placement in the list
	template <class... Args>
	size_t emplace_ordered(Args&&... args)
	{
		if (lastAccessed->prevNode != nullptr)
			return findOrderedPosition(new Node(lastAccessed->prevNode->prev, lastAccessed->prevNode, args...));
		else
		{
			emplace_back(args...);
			return 0;
		}
	}

	//Changes the size of a list
	//
	//--If newSize is bigger, the values inside parameter pack args
	//are used to construct new elements at the end of the list. If
	//args is simply an object of type T, create a deep copy.
	//--If newSize is smaller, elements are removed starting from the end
	template <class... Args>
	void resize(size_t newSize, Args&&... args)
	{
		if (newSize > * count)
		{
			if (!*count)
			{
				*root = *tail = new Node(nullptr, nullptr, args...);
				(*count)++;
			}
			while (*count < newSize)
			{
				*tail = (*tail)->next = new Node(*tail, nullptr, args...);
				(*count)++;
			}
			*lastAccessed = { (*count) - 1, *tail };
		}
		else if (newSize < *count)
		{
			while (*count > newSize)
			{
				Node* cur = *tail;
				*tail = (*tail)->prev;
				delete cur;
				(*count)--;
			}
			if (*count)
			{
				(*tail)->next = nullptr;
				*lastAccessed = { (*count) - 1, *tail };
			}
			else
			{
				*root = nullptr;
				*lastAccessed = { 0, nullptr };
			}
		}
	}

	//Changes the size of a list
	//
	//--If newSize is bigger, new elements are created at the end of the list
	//as shallow copies of object "data"
	//--If newSize is smaller, elements are removed starting from the end
	void resize(const T& data, size_t newSize)
	{
		if (newSize > * count)
		{
			if (!*count)
			{
				*root = *tail = new Node(data);
				(*count)++;
			}
			while (*count < newSize)
			{
				*tail = (*tail)->next = new Node(data, *tail);
				(*count)++;
			}
			*lastAccessed = { (*count) - 1, *tail };
		}
		else if (newSize < *count)
		{
			while (*count > newSize)
			{
				Node* cur = *tail;
				*tail = (*tail)->prev;
				delete cur;
				(*count)--;
			}
			if (*count)
			{
				(*tail)->next = nullptr;
				*lastAccessed = { (*count) - 1, *tail };
			}
			else
			{
				*root = nullptr;
				*lastAccessed = { 0, nullptr };
			}
		}
	}

	//Removes the *root element
	void pop_front()
	{
		if (*count)
		{
			(*count)--;
			if (*root == lastAccessed->prevNode)
				lastAccessed->prevNode = lastAccessed->prevNode->next;
			else
				lastAccessed->index--;
			Node* cur = *root;
			*root = (*root)->next;
			delete cur;
			if (*root != nullptr)
				(*root)->prev = nullptr;
			else
				*tail = nullptr;
		}
	}

	//Removes the *tail element
	void pop_back()
	{
		if (*count)
		{
			(*count)--;
			if (*tail == lastAccessed->prevNode)
			{
				//Overflow if index is 0 doesn't matter as it would mean count is also 0
				lastAccessed->index--;
				lastAccessed->prevNode = lastAccessed->prevNode->prev;
			}
			Node* cur = *tail;
			*tail = (*tail)->prev;
			delete cur;
			if (*tail != nullptr)
				(*tail)->next = nullptr;
			else
				*root = nullptr;
		}
	}

	//Deletes "numElements" number of elements from the list starting at the provided index
	bool erase(size_t index, size_t numElements = 1)
	{
		bool result = false;
		Node* cur = find(index);
		while (numElements > 0 && cur != nullptr)
		{
			Node* next = cur->next;
			if (cur->prev != nullptr)
				cur->prev->next = next;
			else
				*root = next;
			if (next != nullptr)
				next->prev = cur->prev;
			else
				*tail = cur->prev;
			delete cur;
			cur = next;
			numElements--;
			(*count)--;
			result = true;
		}
		if (cur != nullptr)
			*lastAccessed = { index, cur };
		else if (*count)
			*lastAccessed = { (*count) - 1, *tail };
		else
			*lastAccessed = { 0, nullptr };
		return result;
	}

	//Deletes all elements
	void clear()
	{
		while (*count)
		{
			Node* next = (*root)->next;
			delete *root;
			*root = next;
			(*count)--;
		}
		*lastAccessed = { 0, nullptr };
		*tail = nullptr;
	}

	//Returns first element if one exists
	T& front()
	{
		if (*count)
		{
			*lastAccessed = { 0, *root };
			return (*root)->data;
		}
		else
			throw "Error: list index out of range";
	}

	//Returns last element if one exists
	T& back()
	{
		if (*count)
		{
			*lastAccessed = { (*count) - 1, *tail };
			return (*tail)->data;
		}
		else
			throw "Error: list index out of range";
	}

	//Returns element at index if one exists
	T& operator[](size_t index)
	{
		if (index < *count)
			return find(index)->data;
		else
			throw "Error: list index out of range";
	}

	//Returns the index with an element that matches the provided object.
	//Starts the search from the provided index
	//If searching fails, returns -1.
	int search(T& compare, size_t startIndex = 0)
	{
		Node* cur = find(startIndex);
		while (cur != nullptr)
		{
			if (cur->data == compare)
			{
				*lastAccessed = { startIndex, cur };
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

	//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
	void moveElements(size_t index, size_t newPosition, size_t numElements = 1)
	{
		if (index >= *count)
			throw "Error: list index out of range";
		else if (newPosition > * count)
			throw "Error: list newPosition index out of range";
		else
		{
			if (index + numElements > *count)
				numElements = *count - index;
			else if (numElements == 0)
				numElements = 1;
			Node* beg = find(index);
			if (newPosition < index)
			{
				Node* end = find(index + numElements - 1);
				Node* pos = find(newPosition);
				beg->prev->next = end->next;
				if (end->next != nullptr)
					end->next->prev = beg->prev;
				else
					*tail = beg->prev;
				if (pos->prev != nullptr)
					pos->prev->next = beg;
				else
					*root = beg;
				beg->prev = pos->prev;
				end->next = pos;
				pos->prev = end;
				*lastAccessed = { newPosition, beg };
			}
			else if (newPosition > index + numElements)
			{
				Node* end = find(index + numElements - 1);
				Node* pos = find(newPosition - 1);
				end->next->prev = beg->prev;
				if (beg->prev != nullptr)
					beg->prev->next = end->next;
				else
					*root = end->next;
				if (pos->next != nullptr)
					pos->next->prev = end;
				else
					*tail = end;
				end->next = pos->next;
				beg->prev = pos;
				pos->next = beg;
				*lastAccessed = { newPosition, beg };
			}
		}
	}

	//Swaps the contents of the two lists.
	void swap(List<T>& other)
	{
		Node** rootbuf = root, **tailbuf = tail;
		size_t* countbuf = count, *usedBuf = UsedCount;
		placeSaver* placebuf = lastAccessed;
		root = other.root;
		tail = other.tail;
		count = other.count;
		lastAccessed = other.lastAccessed;
		UsedCount = other.UsedCount;
		other.root = rootbuf;
		other.tail = tailbuf;
		other.count = countbuf;
		other.lastAccessed = placebuf;
		other.UsedCount = usedBuf;
	}
};

/*
=================================================================================================================
=================================================================================================================
=================================================================================================================
*/

//Handles lists of pointer types
template <typename T>
class List<T*>
{
private:
	struct Node
	{
		T* data;
		Node* prev;
		Node* next;
		//Calls default constructor for data
		Node() : data(nullptr), prev(nullptr), next(nullptr) {}
		//Creates a shallow copy of the provided object
		Node(T* data, Node* pr = nullptr, Node* nt = nullptr) : data(nullptr), prev(pr), next(nt)
		{
			this->data = data;
		}
	};

	Node** root;
	Node** tail;
	struct placeSaver
	{
		size_t index = 0;
		Node* prevNode = nullptr;
	};
	placeSaver* lastAccessed;
	size_t* count;
	size_t* UsedCount;
	//Returns the pointer of the node at the given index. Returns "nullptr" if index >= count.
	Node* find(size_t index)
	{
		if (index < *count)
		{
			Node* cur;
			if (index == lastAccessed->index) //Return *lastAccessed's node
				cur = lastAccessed->prevNode;
			else if (index == 0) //Return *root
				cur = *root;
			else if (index == (*count) - 1) //Return *tail
				cur = *tail;
			else if (lastAccessed->prevNode == nullptr || signed(index) <= signed(lastAccessed->index) - index)
				//If *lastAccessed is empty, or if index is less than or equal to the difference between *lastAccessed and the given index
			{
				cur = *root;
				for (size_t i = 0; i < index; i++)
					cur = cur->next;
			}
			else if (index > lastAccessed->index)
			{
				if (index - lastAccessed->index <= (*count) - 1) //Start from LA and go up
				{
					cur = lastAccessed->prevNode;
					for (size_t i = lastAccessed->index; i < index; i++)
						cur = cur->next;
				}
				else //Start from *tail and go down
				{
					cur = *tail;
					for (size_t i = (*count) - 1; i > index; i--)
						cur = cur->prev;
				}
			}
			else //Start from LA and go down
			{
				cur = lastAccessed->prevNode;
				for (size_t i = lastAccessed->index; i > index; i--)
					cur = cur->prev;
			}
			*lastAccessed = { index, cur };
			return cur;
		}
		else
			return nullptr;
	}
	//Returns the index for where, in order, the new node was placed
	size_t findOrderedPosition(Node* newNode)
	{
		size_t index = lastAccessed->index;
		if (*newNode->data == *newNode->next->data)
		{
			*newNode->next->data = *newNode->data;
			delete newNode;
			return index;
		}
		else if (*newNode->data > *newNode->next->data)
		{
			while (newNode->next != nullptr)
			{
				if (*newNode->data == *newNode->next->data)
				{
					*newNode->next->data = *newNode->data;
					delete newNode;
					return index;
				}
				else if (*newNode->data < *newNode->next->data)
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
			while (newNode->prev != nullptr)
			{
				if (*newNode->data == *newNode->prev->data)
				{
					*newNode->prev->data = *newNode->data;
					delete newNode;
					return index;
				}
				else if (*newNode->data > *newNode->prev->data)
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
			*tail = newNode;
		else
			newNode->next->prev = newNode;
		if (newNode->prev == nullptr)
			*root = newNode;
		else
			newNode->prev->next = newNode;
		(*count)++;
		*lastAccessed = { index, newNode };
		return index;
	}
public:
	//Create empty list
	List<T*>()
	{
		root = new Node * (nullptr);
		tail = new Node * (nullptr);
		count = new size_t(0);
		UsedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
	}

	//Create a list with cloned elements from the provided list
	List<T*>(const List<T*>& list) : List()
	{
		Node* cur = *list.root;
		for (unsigned i = 0; i < *list.count; i++)
		{
			push_back(cur->data);
			cur = cur->next;
		}
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List<T*>& operator=(const List<T*>& list)
	{
		if (UsedCount != list.UsedCount)
		{
			if (*UsedCount > 1)
				(*UsedCount)--;
			else
			{
				clear();
				delete root;
				delete tail;
				delete count;
				delete lastAccessed;
				delete UsedCount;
			}
			root = list.root;
			tail = list.tail;
			count = list.count;
			lastAccessed = list.lastAccessed;
			UsedCount = list.UsedCount;
			(*UsedCount)++;
		}
		return *this;
	}

	//Calls clear, then deletes all pointers
	~List<T*>()
	{
		if (*UsedCount > 1)
			(*UsedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete UsedCount;
		}
	}

	//Returns count
	size_t& size() const { return *count; }

	//Creates a new element at the end of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_back(T* data)
	{
		if (*count)
			*tail = (*tail)->next = new Node(data, *tail);
		else
			*root = *tail = new Node(data);
		*lastAccessed = { *count, *tail };
		(*count)++;
	}

	//Creates a new element at the beginning of the list using a copy of the provided object
	//
	//--CODING NOTE: Ensure that this object type has a operator= function.
	//The compiler will throw an error if that is not the case.
	void push_front(T* data)
	{
		if (*count)
			*root = (*root)->prev = new Node(data, nullptr, *root);
		else
			*root = *tail = new Node(data);
		*lastAccessed = { 0, *root };
		(*count)++;
	}

	//Maneuvers to the specified index and then creates numElements number of new elements
	//as shallow copies of T object "data"
	size_t insert(size_t index, size_t numElements, T* data)
	{
		Node* cur;
		size_t curIndex;
		if (!*count)
		{
			cur = *root = *tail = new Node(data);
			curIndex = 1;
			(*count)++;
		}
		else if (index == 0)
		{
			cur = *root = (*root)->prev = new Node(data, nullptr, *root);
			curIndex = 1;
			(*count)++;
		}
		else if (index >= *count)
		{
			cur = *tail = (*tail)->next = new Node(data, *tail, nullptr);
			curIndex = (*count) + 1;
			(*count)++;
		}
		else
		{
			cur = find(index);
			cur->prev = cur->prev->next = new Node(data, cur->prev, cur);
			curIndex = index + 1;
			(*count)++;
		}
		while (curIndex < index + numElements)
		{
			cur->prev = cur->prev->next = new Node(data, cur, cur->next);
			curIndex++;
			(*count)++;
		}
		*lastAccessed = { curIndex - 1, cur };
		return curIndex - 1;
	}

	//Maneuvers to the specified index and then creates a new element 
	//with a shallow copy of T object "data"
	size_t insert(size_t index, T* data)
	{
		return insert(index, 1, data);
	}

	//Uses "data" to find the proper position in the list then constructs
	//a new element in that position with a shallow copy of "data"
	size_t insert_ordered(T* data)
	{
		if (lastAccessed->prevNode != nullptr)
			return findOrderedPosition(new Node(data, lastAccessed->prevNode->prev, lastAccessed->prevNode));
		else
		{
			push_back(data);
			return 0;
		}
	}

	//Changes the size of a list
	//
	//--If newSize is bigger, new elements are created at the end of the list
	//as shallow copies of object "data"
	//--If newSize is smaller, elements are removed starting from the end
	void resize(T* data, size_t newSize)
	{
		if (newSize > * count)
		{
			if (!*count)
			{
				*root = *tail = new Node(data);
				(*count)++;
			}
			while (*count < newSize)
			{
				*tail = (*tail)->next = new Node(data, *tail);
				(*count)++;
			}
			*lastAccessed = { (*count) - 1, *tail };
		}
		else if (newSize < *count)
		{
			while (*count > newSize)
			{
				Node* cur = *tail;
				*tail = (*tail)->prev;
				delete cur;
				(*count)--;
			}
			if (*count)
			{
				(*tail)->next = nullptr;
				*lastAccessed = { (*count) - 1, *tail };
			}
			else
			{
				*root = nullptr;
				*lastAccessed = { 0, nullptr };
			}
		}
	}

	//Removes the *root element
	void pop_front()
	{
		if (*count)
		{
			(*count)--;
			if (*root == lastAccessed->prevNode)
				lastAccessed->prevNode = lastAccessed->prevNode->next;
			else
				lastAccessed->index--;
			Node* cur = *root;
			*root = (*root)->next;
			delete cur;
			if (*root != nullptr)
				(*root)->prev = nullptr;
			else
				*tail = nullptr;
		}
	}

	//Removes the *tail element
	void pop_back()
	{
		if (*count)
		{
			(*count)--;
			if (*tail == lastAccessed->prevNode)
			{
				//Overflow if index is 0 doesn't matter as it would mean count is also 0
				lastAccessed->index--;
				lastAccessed->prevNode = lastAccessed->prevNode->prev;
			}
			Node* cur = *tail;
			*tail = (*tail)->prev;
			delete cur;
			if (*tail != nullptr)
				(*tail)->next = nullptr;
			else
				*root = nullptr;
		}
	}

	//Deletes "numElements" number of elements from the list starting at the provided index
	bool erase(size_t index, size_t numElements = 1)
	{
		bool result = false;
		Node* cur = find(index);
		while (numElements > 0 && cur != nullptr)
		{
			Node* next = cur->next;
			if (cur->prev != nullptr)
				cur->prev->next = next;
			else
				*root = next;
			if (next != nullptr)
				next->prev = cur->prev;
			else
				*tail = cur->prev;
			delete cur;
			cur = next;
			numElements--;
			(*count)--;
			result = true;
		}
		if (cur != nullptr)
			*lastAccessed = { index, cur };
		else if (*count)
			*lastAccessed = { (*count) - 1, *tail };
		else
			*lastAccessed = { 0, nullptr };
		return result;
	}

	//Deletes all elements
	void clear()
	{
		while (*count)
		{
			Node* next = (*root)->next;
			delete* root;
			*root = next;
			(*count)--;
		}
		*lastAccessed = { 0, nullptr };
		*tail = nullptr;
	}

	//Returns element at *root if *count is at least 1
	T* front()
	{
		if (*count)
		{
			*lastAccessed = { 0, *root };
			return (*root)->data;
		}
		else
			throw "Error: list index out of range";
	}

	//Returns element at *tail if *count is at least 1
	T* back()
	{
		if (*count)
		{
			*lastAccessed = { (*count) - 1, *tail };
			return (*tail)->data;
		}
		else
			throw "Error: list index out of range";
	}

	//Returns element at index if *count is at least 1
	T* operator[](size_t index)
	{
		if (index < *count)
			return find(index)->data;
		else
			throw "Error: list index out of range";
	}
	/*
	Returns the index with an element that matches the provided object.
	Starts the search from the provided index
	@param compare - a pointer to an object of type T
	@param startIndex - index to start the search from
	@return The index of the located object; -1 if not found.
	*/
	int search(T* compare, size_t startIndex = 0)
	{
		Node* cur = find(startIndex);
		while (cur != nullptr)
		{
			if (*cur->data == *compare)
			{
				*lastAccessed = { startIndex, cur };
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

	/*
	Returns the index with an element that matches the provided object.
	Starts the search from the provided index
	@param compare - an object of type T
	@param startIndex - index to start the search from
	@return The index of the located object; -1 if not found.
	*/
	int search(T& compare, size_t startIndex = 0)
	{
		Node* cur = find(startIndex);
		while (cur != nullptr)
		{
			if (*cur->data == compare)
			{
				*lastAccessed = { startIndex, cur };
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

	//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
	void moveElements(size_t index, size_t newPosition, size_t numElements = 1)
	{
		if (index >= *count)
			throw "Error: list index out of range";
		else if (newPosition > * count)
			throw "Error: list newPosition index out of range";
		else
		{
			if (index + numElements > * count)
				numElements = *count - index;
			else if (numElements == 0)
				numElements = 1;
			Node* beg = find(index);
			if (newPosition < index)
			{
				Node* end = find(index + numElements - 1);
				Node* pos = find(newPosition);
				beg->prev->next = end->next;
				if (end->next != nullptr)
					end->next->prev = beg->prev;
				else
					*tail = beg->prev;
				if (pos->prev != nullptr)
					pos->prev->next = beg;
				else
					*root = beg;
				beg->prev = pos->prev;
				end->next = pos;
				pos->prev = end;
				*lastAccessed = { newPosition, beg };
			}
			else if (newPosition > index + numElements)
			{
				Node* end = find(index + numElements - 1);
				Node* pos = find(newPosition - 1);
				end->next->prev = beg->prev;
				if (beg->prev != nullptr)
					beg->prev->next = end->next;
				else
					*root = end->next;
				if (pos->next != nullptr)
					pos->next->prev = end;
				else
					*tail = end;
				end->next = pos->next;
				beg->prev = pos;
				pos->next = beg;
				*lastAccessed = { newPosition, beg };
			}
		}
	}

	//Swaps the contents of the two lists.
	void swap(List<T>& other)
	{
		Node** rootbuf = root, **tailbuf = tail;
		size_t* countbuf = count, *usedBuf = UsedCount;
		placeSaver* placebuf = lastAccessed;
		root = other.root;
		tail = other.tail;
		count = other.count;
		lastAccessed = other.lastAccessed;
		UsedCount = other.UsedCount;
		other.root = rootbuf;
		other.tail = tailbuf;
		other.*count = countbuf;
		other.lastAccessed = placebuf;
		other.UsedCount = usedBuf;
	}
};