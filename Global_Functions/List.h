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
namespace LinkedList
{
	namespace
	{
		template <class T>
		struct Type
		{
			static constexpr bool s_allowCloning = true;
			T object;
			Type() : object() {}
			template <class... Args>
			Type(Args&&... args) : object(args...) {}
			bool operator==(const Type& type) const { return object == type.object; }
			bool operator!=(const Type& type) const { return object != type.object; }
			bool operator>=(const Type& type) const { return object >= type.object; }
			bool operator> (const Type& type) const { return object >  type.object; }
			bool operator<=(const Type& type) const { return object <= type.object; }
			bool operator< (const Type& type) const { return object <  type.object; }
			bool operator==(const T& obj) const { return object == obj; }
			bool operator!=(const T& obj) const { return object != obj; }
			bool operator>=(const T& obj) const { return object >= obj; }
			bool operator> (const T& obj) const { return object >  obj; }
			bool operator<=(const T& obj) const { return object <= obj; }
			bool operator< (const T& obj) const { return object <  obj; }
		};

		template <class T>
		struct Type<T*>
		{
		private:
			bool emplaced;
		public:
			static constexpr bool s_allowCloning = false;
			T* object;
			Type() : object(nullptr), emplaced(false) {}
			Type(T* ptr) : object(ptr), emplaced(false) {}
			template <class... Args>
			Type(Args&&... args) : object(new T(args...)), emplaced(true) {}
			~Type()
			{
				if (emplaced)
					delete object;
			}
			Type& operator=(const Type& type)
			{
				if (emplaced)
					delete object;
				object = type.object;
				emplaced = type.emplaced;
				type.emplaced = false;
				return *this;
			}
			Type& operator=(const T*& ptr)
			{
				if (emplaced)
				{
					delete object;
					emplaced = false;
				}
				object = ptr;
				return *this;
			}
			bool operator==(const Type& type) const { return *object == *type.object; }
			bool operator!=(const Type& type) const { return *object != *type.object; }
			bool operator>=(const Type& type) const { return *object >= *type.object; }
			bool operator> (const Type& type) const { return *object > * type.object; }
			bool operator<=(const Type& type) const { return *object <= *type.object; }
			bool operator< (const Type& type) const { return *object < *type.object; }
			bool operator==(const T* ptr) const { return *object == *ptr; }
			bool operator!=(const T* ptr) const { return *object != *ptr; }
			bool operator>=(const T* ptr) const { return *object >= *ptr; }
			bool operator> (const T* ptr) const { return *object >  *ptr; }
			bool operator<=(const T* ptr) const { return *object <= *ptr; }
			bool operator< (const T* ptr) const { return *object <  *ptr; }
		};

		template <class T>
		struct Type<std::shared_ptr<T>>
		{
			static constexpr bool s_allowCloning = true;
			std::shared_ptr<T> object;
			Type() {}
			Type(T*& ptr) : object(ptr) {}
			template <class... Args>
			Type(Args&&... args) : object(std::make_shared(args...)) {}
			Type& operator=(const T*& ptr)
			{
				object.reset(ptr);
				return *this;
			}
			bool operator==(const Type& type) const { return *object == *type.object; }
			bool operator!=(const Type& type) const { return *object != *type.object; }
			bool operator>=(const Type& type) const { return *object >= *type.object; }
			bool operator> (const Type& type) const { return *object >  *type.object; }
			bool operator<=(const Type& type) const { return *object <= *type.object; }
			bool operator< (const Type& type) const { return *object <  *type.object; }
			bool operator==(const std::shared_ptr<T> obj) const { return *object == *obj; }
			bool operator!=(const std::shared_ptr<T> obj) const { return *object != *obj; }
			bool operator>=(const std::shared_ptr<T> obj) const { return *object >= *obj; }
			bool operator> (const std::shared_ptr<T> obj) const { return *object >  *obj; }
			bool operator<=(const std::shared_ptr<T> obj) const { return *object <= *obj; }
			bool operator< (const std::shared_ptr<T> obj) const { return *object <  *obj; }
		};
	}

	/*
		A linked list template class
	*/
	template <class T>
	class List
	{
		struct Node
		{
			Type<T> data;
			Node* prev;
			Node* next;

			Node() : prev(nullptr), next(nullptr) {}
			// Creates a shallow copy of the provided object
			Node(const T& data, Node* pr = nullptr, Node* nt = nullptr) : data(), prev(pr), next(nt)
			{
				this->data.object = data;
			}

			// Uses the arguments provided in "args" to create a new object.
			// If args is an object of type T, it creates a deep copy.
			template <class... Args>
			Node(Node* pr, Node* nt, Args&&... args) : data(args...), prev(pr), next(nt) {}
			bool operator==(Node& node) const { return data == node.data; }
			bool operator!=(Node& node) const { return data != node.data; }
			bool operator>=(Node& node) const { return data >= node.data; }
			bool operator> (Node& node) const { return data >  node.data; }
			bool operator<=(Node& node) const { return data <= node.data; }
			bool operator< (Node& node) const { return data <  node.data; }
			bool operator==(T& obj) const { return data == obj; }
			bool operator!=(T& obj) const { return data != obj; }
			bool operator>=(T& obj) const { return data >= obj; }
			bool operator> (T& obj) const { return data >  obj; }
			bool operator<=(T& obj) const { return data <= obj; }
			bool operator< (T& obj) const { return data <  obj; }
		};

	public:
		class Iterator
		{
			friend class List;
			friend struct SharedValues;
		public:
			Iterator() noexcept : m_currentNode(nullptr), m_currentIndex(0) {}
			Iterator(Node* pNode, const size_t pIndex) noexcept : m_currentNode(pNode), m_currentIndex(pIndex) {}
			Iterator& operator++()
			{
				if (m_currentNode)
				{
					m_currentNode = m_currentNode->next;
					++m_currentIndex;
				}
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator iterator = *this;
				++* this;
				return iterator;
			}

			Iterator& operator--()
			{
				if (m_currentNode)
				{
					m_currentNode = m_currentNode->prev;
					--m_currentIndex;
				}
				return *this;
			}

			Iterator operator--(int)
			{
				Iterator iterator = *this;
				--* this;
				return iterator;
			}

			bool operator!=(const Iterator& iterator)
			{
				return m_currentNode != iterator.m_currentNode;
			}

			bool operator!=(const size_t index)
			{
				return m_currentIndex != index;
			}

			T& operator*()
			{
				return m_currentNode->data.object;
			}
		private:
			size_t m_currentIndex;
			Node* m_currentNode;
		};

	private:
		struct SharedValues
		{
			Node* root = nullptr;
			Node* tail = nullptr;
			size_t count = 0;
			Iterator iterator;
			void clear()
			{
				iterator = { root, 0 };
				root = tail = nullptr;
				count = 0;
				while (iterator.m_currentNode)
					delete (iterator++).m_currentNode;
			}
			~SharedValues()
			{
				clear();
			}
		};
		std::shared_ptr<SharedValues> shared;

		/* Sets the shared iterator to the node at the given index.
		* @param index - the index to iterate to
		* @return The pointer to the node at that index. If index was invalid, returns nullptr.
		*/
		Node* setIterator(size_t index)
		{
			if (index < shared->count)
			{
				if (shared->iterator != index)
				{
					if (index == 0)
						shared->iterator = { shared->root, 0 };
					else if (index == (shared->count) - 1)
						shared->iterator = { shared->tail, shared->count - 1 };
					else if (shared->iterator.m_currentNode == nullptr || index << 1 <= shared->iterator.m_currentIndex)
					{
						// Start from root and go up
						shared->iterator = { shared->root->next, 1 };
						while (shared->iterator.m_currentIndex < index)
							++shared->iterator;
					}
					else if (index <= shared->iterator.m_currentIndex)
					{
						// Start from current iterator and go down
						do --shared->iterator;
						while (shared->iterator.m_currentIndex > index);
					}
					else
					{
						if (index - shared->iterator.m_currentIndex <= shared->count - index)
						{
							// Start from current iterator and go up
							do ++shared->iterator;
							while (shared->iterator.m_currentIndex < index);
						}
						else // Start from tail and go down
						{
							shared->iterator = { shared->tail->prev, shared->count - 2 };
							while (shared->iterator.m_currentIndex > index)
								--shared->iterator;
						}
					}
				}
				return shared->iterator.m_currentNode;
			}
			else
				return nullptr;
		}

		// Returns the index for where, in proper order, the new node was placed
		size_t createOrderedNode(Node* node)
		{
			while (shared->iterator.m_currentNode != nullptr)
			{
				if (*shared->iterator.m_currentNode == *node)
				{
					node->prev = shared->iterator.m_currentNode->prev;
					node->next = shared->iterator.m_currentNode->next;
					node->prev->next = node;
					node->next->prev = node;
					delete shared->iterator.m_currentNode;
					shared->iterator.m_currentNode = node;
					return shared->iterator.m_currentIndex;
				}
				else if (*shared->iterator.m_currentNode > * node)
					break;
				else
					++shared->iterator;
			}

			if (shared->iterator.m_currentNode == nullptr)
			{
				node->prev = shared->tail;
				shared->iterator.m_currentNode = shared->tail = node;

				if (node->prev)
					node->prev->next = node;
				else
					shared->root = node;
			}
			else
			{
				--shared->iterator;
				while (shared->iterator.m_currentNode != nullptr)
				{
					if (*shared->iterator.m_currentNode == *node)
					{
						node->prev = shared->iterator.m_currentNode->prev;
						node->next = shared->iterator.m_currentNode->next;
						node->prev->next = node;
						node->next->prev = node;
						delete shared->iterator.m_currentNode;
						shared->iterator.m_currentNode = node;
						return shared->iterator.m_currentIndex;
					}
					else if (*shared->iterator.m_currentNode < *node)
						break;
					else
						--shared->iterator;
				}

				if (shared->iterator.m_currentNode == nullptr)
				{
					node->next = shared->root;
					shared->iterator.m_currentNode = shared->root = shared->root->prev = node;
				}
				else
				{
					node->prev = shared->iterator.m_currentNode;
					node->next = shared->iterator.m_currentNode->next;
					node->prev->next = node;
					node->next->prev = node;
				}
			}
			++shared->count;
			return shared->iterator.m_currentIndex;
		}

	public:
		// Root wrapped in Iterator type
		Iterator begin() const
		{
			return Iterator(shared->root, 0);
		}

		// Tail wrapped in Iterator type
		Iterator end() const
		{
			return Iterator(shared->tail, shared->count - 1);
		}

		// Creates an empty list
		List() : shared(std::make_shared<SharedValues>()) {}

		// Creates a list of specified size
		// Every parameter after "size" is used in the construction of every new node
		template <class... Args>
		List(size_t size, Args&&... args) : shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < size; i++)
				emplace_back(args...);
		}

		// Creates a list based off the provided initializer list
		List(const std::initializer_list<T>& init) : shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < init.size(); i++)
				push_back(init.begin()[i]);
		}

		// Creates a list based off the provided initializer list
		// Uses each sublist as the basis for constructing a new object of the specified type
		template <class... Args>
		List(const std::initializer_list<Args...>& init) : shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < init.size(); i++)
				emplace_back(init.begin()[i]);
		}

		// Create a list with cloned elements from the provided list
		List(const List& list) : shared(std::make_shared<SharedValues>())
		{
			for (T& obj : list)
				emplace_back(obj);
		}

		// Clones the provided list into the current one as if the current list is newly created
		// If type T is a raw pointer type, then the list will instead become a shallow copy
		List& clone(const List& list)
		{
			if (Type<T>::s_allowCloning)
			{
				shared = std::make_shared<SharedValues>();
				for (T& obj : list)
					emplace_back(obj);
			}
			else
				shared = list.shared;
			return *this;
		}

		//Assigns the curent linked list to hold the values presented by the initialized list
		List& operator=(const std::initializer_list<T>& init)
		{
			shared = std::make_shared<SharedValues>();
			for (unsigned i = 0; i < init.size(); i++)
				push_back(init.begin()[i]);
			return *this;
		}

		//Assigns the curent linked list to hold the values presented by the initialized list
		//
		template <class... Args>
		List& operator=(const std::initializer_list<std::initializer_list<Args...>>& init)
		{
			shared = std::make_shared<SharedValues>();
			for (unsigned i = 0; i < init.size(); i++)
				emplace_back(init.begin()[i].begin()...);
			return *this;
		}

		//Copy root, tail, count, lastaccessed, and usedcount (which gets incremented)
		List& operator=(const List& list)
		{
			shared = list.shared;
			return *this;
		}

		//Returns count
		size_t& size() const { return shared->count; }

		//Creates a new element at the end of the list using a copy of the provided object
		//
		//--CODING NOTE: Ensure that this object type has a operator= function.
		//The compiler will throw an error if that is not the case.
		void push_back(const T& data)
		{
			if (shared->count)
				shared->tail = shared->tail->next = new Node(data, shared->tail);
			else
				shared->root = shared->tail = new Node(data);
			shared->iterator = { shared->tail, shared->count };
			shared->count++;
		}

		//Creates a new element at the beginning of the list using a copy of the provided object
		//
		//--CODING NOTE: Ensure that this object type has a operator= function.
		//The compiler will throw an error if that is not the case.
		void push_front(const T& data)
		{
			if (shared->count)
				shared->root = shared->root->prev = new Node(data, nullptr, shared->root);
			else
				shared->root = shared->tail = new Node(data);
			shared->iterator = { shared->root, 0 };
			shared->count++;
		}

		//Creates a new element at the end of the list using a constructor that 
		//takes all the parameters provided to the function
		//
		//--CODING NOTE: Ensure that this specialized constructor for the element type
		//exist. The compiler will throw an error if that is not the case.
		template <class... Args>
		T& emplace_back(Args&&... args)
		{
			if (shared->count)
				shared->tail = shared->tail->next = new Node(shared->tail, nullptr, args...);
			else
				shared->root = shared->tail = new Node(nullptr, nullptr, args...);
			shared->iterator = { shared->tail, shared->count++ };
			return shared->tail->data.object;
		}

		//Creates a new element at the beginning of the list using a constructor that 
		//takes all the parameters provided to the function
		//
		//--CODING NOTE: Ensure that this specialized constructor for the element type
		//exist. The compiler will throw an error if that is not the case.
		template <class... Args>
		T& emplace_front(Args&&... args)
		{
			if (shared->count)
				shared->root = shared->root->prev = new Node(nullptr, shared->root, args...);
			else
				shared->root = shared->tail = new Node(nullptr, nullptr, args...);
			shared->iterator = { shared->root, 0 };
			shared->count++;
			return shared->root->data.object;
		}

		//Maneuvers to the specified index and then creates numElements number of new elements
		//as shallow copies of T object "data"
		size_t insert(size_t index, size_t numElements, const T& data)
		{
			if (!shared->count)
			{
				shared->root = new Node(data);
				shared->iterator = { shared->root, 0 };
				shared->count++;
			}
			else if (index == 0)
			{
				shared->root = shared->root->prev = new Node(data, nullptr, shared->root);
				shared->iterator = { shared->root, 0 };
				shared->count++;
			}
			else if (setIterator(index))
			{
				shared->iterator.m_currentNode->prev =
					shared->iterator.m_currentNode->prev->next =
					new Node(data, shared->iterator.m_currentNode->prev, shared->iterator.m_currentNode);
				shared->count++;
			}
			else
			{
				shared->tail->next = new Node(data, shared->tail, nullptr);
				shared->iterator = { shared->tail->next, shared->count };
				shared->count++;
			}

			while (shared->iterator.m_currentIndex < index + numElements)
			{
				++shared->iterator;
				shared->iterator.m_currentNode = new Node(data, shared->iterator.m_currentNode, shared->iterator.m_currentNode->next);
				shared->count++;
			}

			if (shared->iterator.m_currentIndex == shared->count - 1)
				shared->tail = shared->iterator.m_currentNode;
			return shared->iterator.m_currentIndex;
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
			if (!shared->count)
			{
				shared->root = new Node(nullptr, nullptr, args...);
				shared->iterator = { shared->root, 0 };
				shared->count++;
			}
			else if (index == 0)
			{
				shared->root = shared->root->prev = new Node(nullptr, shared->root, args...);
				shared->iterator = { shared->root, 0 };
				shared->count++;
			}
			else if (setIterator(index))
			{
				shared->iterator.m_currentNode->prev =
					shared->iterator.m_currentNode->prev->next =
					new Node(shared->iterator.m_currentNode->prev, shared->iterator.m_currentNode, args...);
				shared->count++;
			}
			else
			{
				shared->tail->next = new Node(shared->tail, nullptr, args...);
				shared->iterator = { shared->tail->next, shared->count };
				shared->count++;
			}

			while (shared->iterator.m_currentIndex < index + numElements)
			{
				++shared->iterator;
				shared->iterator.m_currentNode = new Node(shared->iterator.m_currentNode, shared->iterator.m_currentNode->next, args...);
				shared->count++;
			}

			if (shared->iterator.m_currentIndex == shared->count - 1)
				shared->tail = shared->iterator.m_currentNode;
			return shared->iterator.m_currentIndex;
		}

		//Uses "data" to find the proper position in the list then constructs
		//a new element in that position with a shallow copy of "data"
		size_t insert_ordered(const T& data)
		{
			if (shared->count)
			{
				createOrderedNode(new Node(data));
				return shared->iterator.m_currentIndex;
			}
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
			if (shared->count)
			{
				createOrderedNode(new Node(nullptr, nullptr, args...));
				return shared->iterator.m_currentIndex;
			}
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
			if (newSize > shared->count)
			{
				if (!shared->count)
				{
					shared->root = shared->tail = new Node(nullptr, nullptr, args...);
					++shared->count;
				}
				while (shared->count < newSize)
				{
					shared->tail = shared->tail->next = new Node(shared->tail, nullptr, args...);
					++shared->count;
				}
				shared->iterator = { shared->tail, shared->count - 1 };
			}
			else if (newSize < shared->count)
			{
				while (shared->count > newSize)
				{
					List<T>::Node* cur = shared->tail;
					shared->tail = shared->tail->prev;
					shared->count--;
					delete cur;
				}
				if (shared->count)
				{
					shared->tail->next = nullptr;
					shared->iterator = { shared->tail, shared->count - 1 };
				}
				else
				{
					shared->root = nullptr;
					shared->iterator = { nullptr, 0 };
				}
			}
		}

		//Changes the size of a list
		//
		//--If newSize is bigger, new elements are created at the end of the list
		//as shallow copies of object "data"
		//--If newSize is smaller, elements are removed starting from the end
		void resize(T& data, size_t newSize)
		{
			if (newSize > shared->count)
			{
				if (!shared->count)
				{
					shared->root = shared->tail = new Node(data);
					shared->count++;
				}
				while (shared->count < newSize)
				{
					shared->tail = shared->tail->next = new Node(data, shared->tail);
					shared->count++;
				}
				shared->iterator = { shared->tail, shared->count - 1 };
			}
			else if (newSize < shared->count)
			{
				while (shared->count > newSize)
				{
					List<T>::Node* cur = shared->tail;
					shared->tail = shared->tail->prev;
					shared->count--;
					delete cur;
				}
				if (shared->count)
				{
					shared->tail->next = nullptr;
					shared->iterator = { shared->tail, shared->count - 1 };
				}
				else
				{
					shared->root = nullptr;
					shared->iterator = { nullptr, 0 };
				}
			}
		}

		//Removes the shared->root element
		void pop_front()
		{
			if (shared->count)
			{
				List<T>::Node* cur = shared->root;
				shared->root = shared->root->next;
				delete cur;
				if (shared->root != nullptr)
					shared->root->prev = nullptr;
				else
					shared->tail = nullptr;
				shared->iterator = { shared->root , 0 };
			}
		}

		//Removes the shared->tail element
		void pop_back()
		{
			if (shared->count)
			{
				List<T>::Node* cur = shared->tail;
				shared->tail = shared->tail->prev;
				delete cur;
				if (shared->tail != nullptr)
					shared->tail->next = nullptr;
				else
					shared->root = nullptr;
				shared->count--;
				shared->iterator = { shared->tail , shared->count - 1 };
			}
		}

		//Deletes "numElements" number of elements from the list starting at the provided index
		bool erase(size_t index, size_t numElements = 1)
		{
			if (index < shared->count)
			{
				List<T>::Node* beg = setIterator(index)->prev;
				while (numElements > 0 && shared->iterator.m_currentNode != nullptr)
				{
					delete (shared->iterator++).m_currentNode;
					--numElements;
					--shared->count;
					--shared->iterator.m_currentIndex;
				}
				if (beg)
				{
					beg->next = shared->iterator.m_currentNode;
					if (beg->next)
						shared->iterator.m_currentNode->prev = beg;
					else
					{
						shared->tail = beg;
						shared->iterator = { shared->tail , shared->count - 1 };
					}
				}
				else if (shared->iterator.m_currentNode)
				{
					shared->iterator.m_currentNode->prev = beg;
					shared->root = shared->iterator.m_currentNode;
				}
				else
					shared->root = shared->tail = nullptr;
				return true;
			}
			return false;
		}

		//Deletes all elements
		void clear()
		{
			shared->clear();
		}

		//Returns first element if one exists
		T& front()
		{
			if (shared->count)
			{
				shared->iterator = { shared->root, 0 };
				return shared->root->data.object;
			}
			else
				throw "Error: list index out of range";
		}

		//Returns last element if one exists
		T& back()
		{
			if (shared->count)
			{
				shared->iterator = { shared->tail, shared->count - 1 };
				return shared->tail->data.object;
			}
			else
				throw "Error: list index out of range";
		}

		//Returns element at index if one exists
		T& operator[](size_t index)
		{
			if (index < shared->count)
				return setIterator(index)->data.object;
			else
				throw "Error: list index out of range";
		}

		//Returns the index with an element that matches the provided object.
		//Starts the search from the provided index
		//If searching fails, returns -1.
		int search(T& compare, size_t startIndex = 0)
		{
			if (setIterator(startIndex))
			{
				while (shared->iterator.m_currentNode)
				{
					if (*shared->iterator.m_currentNode == compare)
						return (signed)shared->iterator.m_currentIndex;
					++shared->iterator;
				}
			}
			return -1;
		}

		//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
		void moveElements(size_t index, size_t newPosition, size_t numElements = 1)
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
				if (newPosition < index || newPosition > index + numElements)
				{
					List<T>::Node* beg = setIterator(index);
					List<T>::Node* end = setIterator(index + numElements - 1);
					List<T>::Node* pos = setIterator(newPosition);
					if (beg->prev)
					{
						beg->prev->next = end->next;
						if (end->next)
							end->next->prev = beg->prev;
						else
							shared->tail = beg->prev;
					}
					else
					{
						shared->root = end->next;
						end->next->prev = beg->prev;
					}

					beg->prev = pos->prev;
					end->next = pos;
					pos->prev->next = beg;
					pos->prev = end;
					shared->iterator.m_currentNode = beg;
				}
			}
		}

		//Swaps the contents of the two lists.
		List<T>& swap(List<T>& other)
		{
			shared.swap(other.shared);
			return *this;
		}
	};
}
