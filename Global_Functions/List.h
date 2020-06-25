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
	A template class for holding a list of objects of any single type
*/
template <class T>
class List
{
private:
	class list_out_of_range : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "List index out of range";
		}
	} myex;
	struct Node
	{
		T data;
		Node* prev;
		Node* next;
		Node(Node* pr = nullptr, Node* nt = nullptr) : prev(pr), next(nt) {}
		Node(T newData, Node* pr = nullptr, Node* nt = nullptr) : data(newData), prev(pr), next(nt) {}
		template <typename... Args>
		Node(Node* pr, Node* nt, Args... args) : prev(pr), next(nt), data(args...) {}
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
	size_t* usedCount;
	//Returns the pointer of the node at the given index. Returns "nullptr" if index >= count.
	Node* find(size_t index)
	{
		if (index < *count)
		{
			Node* cur;
			if (index == 0) //Return *root
				cur = *root;
			else if (index == *count - 1) //Return *tail
				cur = *tail;
			else if (lastAccessed->prevNode == nullptr || signed(index) <= signed(lastAccessed->index) - index)
				//If *lastAccessed is empty, or if index is less than or equal to the difference between *lastAccessed and the given index
			{
				cur = *root;
				for (size_t i = 0; i < index; i++)
					cur = cur->next;
			}
			else if (index == lastAccessed->index) //Return *lastAccessed's node
				cur = lastAccessed->prevNode;
			else if (index > lastAccessed->index)
			{
				if (index - lastAccessed->index <= *count - 1) //Start from LA and go up
				{
					cur = lastAccessed->prevNode;
					for (size_t i = lastAccessed->index; i < index; i++)
						cur = cur->next;
				}
				else //Start from *tail and go down
				{
					cur = *tail;
					for (size_t i = *count - 1; i > index; i--)
						cur = cur->prev;
				}
			}
			else //Start from LA and go down
			{
				cur = lastAccessed->prevNode; 
				for (size_t i = lastAccessed->index; i > index; i--)
					cur = cur->prev;
			}
			*lastAccessed = placeSaver{ index, cur };
			return cur;
		}
		else
			return nullptr;
	}
public:
	//Create empty list
	List<T>()
	{
		root = new Node*(nullptr);
		tail = new Node*(nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
	}

	//Create list of specified size filled with default T objects (unless noted otherwise)
	List<T>(size_t size, T data = T())
	{
		root = new Node*(nullptr);
		tail = new Node*(nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
		for (unsigned i = 0; i < size; i++)
			push_back(data);
	}

	//Create a list based off the provided initializer list
	List<T>(const T* list, size_t listSize)
	{
		root = new Node * (nullptr);
		tail = new Node * (nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
		for (unsigned i = 0; i < listSize; i++)
			push_back(list[i]);
	}

	//Converts the provided initializer list/array into a template List
	List<T>(const std::initializer_list<T>& init)
	{
		root = new Node * (nullptr);
		tail = new Node * (nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
		for (unsigned i = 0; i < init.size(); i++)
			push_back(init.begin()[i]);
	}

	//Creates a list based off the provided initializer list
	//Each initializer element used as a parameter for a constructor
	//of object type 'T'
	template <typename Param>
	List<T>(const std::initializer_list<Param>& init)
	{
		root = new Node * (nullptr);
		tail = new Node * (nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
		for (unsigned i = 0; i < init.size(); i++)
			arg_emplace_back(init.begin()[i]);
	}

	//Create a list with cloned elements from the provided list
	List<T>(const List<T>& list)
	{
		root = new Node* (nullptr);
		tail = new Node* (nullptr);
		count = new size_t(0);
		usedCount = new size_t(1);
		lastAccessed = new placeSaver{ 0, nullptr };
		Node* cur = *list.root;
		for (unsigned i = 0; i < *list.count; i++)
		{
			push_back(cur->data);
			cur = cur->next;
		}
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List<T>& operator=(const std::initializer_list<T>& init)
	{
		if (*usedCount > 1)
			(*usedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete usedCount;
		}
		root = new Node* (nullptr);
		tail = new Node* (nullptr);
		lastAccessed = new placeSaver{ 0, nullptr };
		count = new size_t(0);
		usedCount = new size_t(1);
		for (unsigned i = 0; i < init.size(); i++)
			push_back(init.begin()[i]);
		return *this;
	}

	//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
	List<T>& operator=(const List<T>& list)
	{
		if (*usedCount > 1)
			(*usedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete usedCount;
		}
		root = list.root;
		tail = list.tail;
		count = list.count;
		lastAccessed = list.lastAccessed;
		usedCount = list.usedCount;
		(*usedCount)++;
		return *this;
	}

	//Calls clear, then deletes all pointers
	~List<T>()
	{
		if (*usedCount > 1)
			(*usedCount)--;
		else
		{
			clear();
			delete root;
			delete tail;
			delete count;
			delete lastAccessed;
			delete usedCount;
		}
	}

	//Returns count
	size_t& size() { return *count; }

	//Copies the element provided into a new *tail node
	void push_back(T data)
	{
		if (*count)
			*tail = (*tail)->next = new Node(data, *tail);
		else
			*root = *tail = new Node(data);
		*lastAccessed = placeSaver{ *count, *tail };
		(*count)++;
	}

	//Creates a new default element at the end
	void emplace_back()
	{
		if (*count)
			*tail = (*tail)->next = new Node(*tail);
		else
			*root = *tail = new Node();
		*lastAccessed = placeSaver{ *count, *tail };
		(*count)++;
	}

	//Creates a new element at the end using a constructor that takes the provided
	//list of parameters
	//
	//Ensure that there exists both a default constructor AND a specialized constructor
	//for the element type. The compiler will throw an error otherwise
	template <typename... Args>
	void arg_emplace_back(Args... args)
	{
		try
		{
			if (*count)
				*tail = (*tail)->next = new Node(*tail, nullptr, args...);
			else
				*root = *tail = new Node(nullptr, nullptr, args...);
			*lastAccessed = placeSaver{ *count, *tail };
			(*count)++;
		}
		catch (...)
		{
			throw "This object type does not have a constructor that takes the list of objects provided.";
		}
	}

	//Inserts a new node from the specified object at the given index, then returns the index
	size_t insert(size_t index, T data)
	{
		if (!*count)
		{
			*root = *tail = new Node(data);
			*lastAccessed = placeSaver{ 0, *tail };
			(*count)++;
			return 0;
		}
		else if (index == 0)
		{
			*root = (*root)->prev = new Node(data, nullptr, *root);
			*lastAccessed = placeSaver{ 0, *root };
			(*count)++;
			return 0;
		}
		else if (index >= *count)
		{
			*tail = (*tail)->next = new Node(data, *tail);
			*lastAccessed = placeSaver{ *count, *tail };
			(*count)++;
			return *count;
		}
		else
		{
			Node* cur = find(index);
			cur->prev = new Node(data, cur->prev, cur);
			cur->prev->prev->next = cur->prev;
			*lastAccessed = placeSaver{ index, cur->prev };
			(*count)++;
			return index;
		}
	}

	//Removes the *root element
	void pop_front()
	{
		if (*count)
		{
			Node* cur = *root;
			if (cur == lastAccessed->prevNode)
				*lastAccessed = placeSaver{ 0, nullptr };
			else
				lastAccessed->index--;
			*root = (*root)->next;
			delete cur;
			(*count)--;
			if (*count)
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
			Node* cur = *tail;
			if (cur == lastAccessed->prevNode)
				*lastAccessed = placeSaver{ 0, nullptr };
			*tail = (*tail)->prev;
			delete cur;
			(*count)--;
			if (*count)
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
		*count -= numElements;
		while (numElements > 0 && cur != nullptr)
		{
			if (cur->prev != nullptr)
				cur->prev->next = cur->next;
			else
				*root = cur->next;
			if (cur->next != nullptr)
				cur->next->prev = cur->prev;
			else
				*tail = cur->prev;
			Node* next = cur->next;
			delete cur;
			cur = next;
			numElements--;
			result = true;
		}
		if (cur != nullptr)
			lastAccessed->prevNode = cur;
		else
			*lastAccessed = placeSaver{ 0, nullptr };
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
		*lastAccessed = placeSaver{ 0, nullptr };
		*tail = nullptr;
	}

	//Adds new default elements to the end of the list to make it bigger or removes elements from the end to make it smaller
	void resize(size_t newSize)
	{
		if (newSize > *count)
		{
			if (!*count)
			{
				*root = *tail = new Node();
				(*count)++;
			}
			while (*count < newSize)
			{
				*tail = (*tail)->next = new Node(*tail);
				(*count)++;
			}
			*lastAccessed = placeSaver{ *count - 1, *tail };
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
				*lastAccessed = placeSaver{ *count - 1, *tail };
			}
			else
			{
				*root = nullptr;
				*lastAccessed = placeSaver{ 0, nullptr };
			}
		}
	}

	//Adds clones of specified object "data" to the end of the list to make it bigger or removes elements from the end to make it smaller
	void resize(size_t newSize, T& data)
	{
		if (newSize > *count)
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
			*lastAccessed = placeSaver{ *count - 1, *tail };
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
				*lastAccessed = placeSaver{ *count - 1, *tail };
			}
			else
			{
				*root = nullptr;
				*lastAccessed = placeSaver{ 0, nullptr };
			}
		}
	}

	//Returns element at *root if *count is at least 1
	T& front()
	{
		if (*count)
		{
			*lastAccessed = placeSaver{ 0, *root };
			return (*root)->data;
		}
		else
			throw myex;
	}

	//Returns element at *tail if *count is at least 1
	T& back()
	{
		if (*count)
		{
			*lastAccessed = placeSaver{ *count - 1, *tail };
			return (*tail)->data;
		}
		else
			throw myex;
	}

	//Returns element at index if *count is at least 1
	T& operator[](size_t index)
	{
		if (index < *count)
			return find(index)->data;
		else
			throw myex;
	}

	//Returns the index with an element that matches the provided object.
	//Starts the search from the provided index
	//If searching fails, returns -1.
	int search(T& compare, size_t startIndex = 0)
	{
		Node* cur = find(startIndex);
		if (cur != nullptr)
		{
			int index = -1;
			for (size_t i = 0; i < *count; i++)
			{
				if (cur->data == compare)
				{
					*lastAccessed = placeSaver{ i, cur };
					index = (int)i;
					break;
				}
				cur = cur->next;
			}
			return index;
		}
		else
			return -1;
	}

	//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
	bool moveElements(size_t index, size_t newPosition, size_t numElements = 1)
	{
		if (index < *count && newPosition <= *count)
		{
			if (index + numElements > *count)
				numElements = *count - index;
			else if (numElements == 0)
				numElements = 1;
			if (numElements <= *count)
			{
				if (newPosition < index)
				{
					Node* beg = find(index);
					Node* end = numElements > 1 ? find(index + numElements - 1) : beg;
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
					*lastAccessed = placeSaver{ newPosition, beg };
					return true;
				}
				else if (newPosition > index + numElements)
				{
					Node* beg = find(index);
					Node* end = numElements > 1 ? find(index + numElements - 1) : beg;
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
					*lastAccessed = placeSaver{ newPosition, beg };
					return true;
				}
			}
		}
		return false;
	}

	//Swaps the contents of the two lists.
	void swap(List<T>& other)
	{
		Node** rootbuf = root, tailbuf = tail;
		size_t* countbuf = count, usedBuf = usedCount;
		placeSaver* placebuf = lastAccessed;
		root = other.root;
		tail = other.tail;
		count = other.count;
		lastAccessed = other.lastAccessed;
		usedCount = other.usedCount;
		other.root = rootbuf;
		other.tail = tailbuf;
		other.*count = countbuf;
		other.lastAccessed = placebuf;
		other.usedCount = usedBuf;
	}
};