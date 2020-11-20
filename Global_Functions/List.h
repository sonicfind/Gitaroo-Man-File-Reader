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
			Iterator operator+(size_t val)
			{
				Iterator iterator = *this;
				for (unsigned index = 0; index < val && iterator.m_currentNode; ++index)
					++iterator;
				return iterator;
			}

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

			Iterator operator-(size_t val)
			{
				Iterator iterator = *this;
				for (unsigned index = 0; index < val && iterator.m_currentNode; ++index)
					--iterator;
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

			bool operator<(const Iterator& iterator)
			{
				return m_currentIndex < iterator.m_currentIndex;
			}

			bool operator>(const Iterator& iterator)
			{
				return m_currentIndex > iterator.m_currentIndex;
			}

			bool operator!=(const size_t index)
			{
				return m_currentIndex != index;
			}

			T& operator*()
			{
				return m_currentNode->data.object;
			}

			size_t getIndex() { return m_currentIndex; }
		private:
			size_t m_currentIndex;
			Node* m_currentNode;
		};

	private:
		struct SharedValues
		{
			Node* m_root = nullptr;
			Node* m_tail = nullptr;
			size_t m_count = 0;
			Iterator m_iterator;
			void clear()
			{
				m_iterator = { m_root, 0 };
				m_root = m_tail = nullptr;
				m_count = 0;
				while (m_iterator.m_currentNode)
					delete (m_iterator++).m_currentNode;
			}
			~SharedValues()
			{
				clear();
			}
		};
		std::shared_ptr<SharedValues> s_shared;

		/* Sets the shared iterator to the node at the given index.
		* @param index - the index to iterate to
		* @return The pointer to the node at that index. If index was invalid, returns nullptr.
		*/
		Node* setIterator(size_t index)
		{
			if (index < s_shared->m_count)
			{
				if (s_shared->m_iterator != index)
				{
					if (index == 0)
						s_shared->m_iterator = { s_shared->m_root, 0 };
					else if (index == (s_shared->m_count) - 1)
						s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
					else if (s_shared->m_iterator.m_currentNode == nullptr || index << 1 <= s_shared->m_iterator.m_currentIndex)
					{
						// Start from root and go up
						s_shared->m_iterator = { s_shared->m_root->next, 1 };
						while (s_shared->m_iterator.m_currentIndex < index)
							++s_shared->m_iterator;
					}
					else if (index <= s_shared->m_iterator.m_currentIndex)
					{
						// Start from current iterator and go down
						do --s_shared->m_iterator;
						while (s_shared->m_iterator.m_currentIndex > index);
					}
					else
					{
						if (index - s_shared->m_iterator.m_currentIndex <= s_shared->m_count - index)
						{
							// Start from current iterator and go up
							do ++s_shared->m_iterator;
							while (s_shared->m_iterator.m_currentIndex < index);
						}
						else // Start from tail and go down
						{
							s_shared->m_iterator = { s_shared->m_tail->prev, s_shared->m_count - 2 };
							while (s_shared->m_iterator.m_currentIndex > index)
								--s_shared->m_iterator;
						}
					}
				}
				return s_shared->m_iterator.m_currentNode;
			}
			else
				return nullptr;
		}

		// Returns the index for where, in proper order, the new node was placed
		size_t createOrderedNode(Node* node)
		{
			while (s_shared->m_iterator.m_currentNode != nullptr)
			{
				if (*s_shared->m_iterator.m_currentNode == *node)
				{
					node->prev = s_shared->m_iterator.m_currentNode->prev;
					node->next = s_shared->m_iterator.m_currentNode->next;
					if (node->prev)
						node->prev->next = node;
					else
						s_shared->m_root = node;
					if (node->next)
						node->next->prev = node;
					else
						s_shared->m_tail = node;
					delete s_shared->m_iterator.m_currentNode;
					s_shared->m_iterator.m_currentNode = node;
					return s_shared->m_iterator.m_currentIndex;
				}
				else if (*node < *s_shared->m_iterator.m_currentNode)
					break;
				else
					++s_shared->m_iterator;
			}

			if (s_shared->m_iterator.m_currentNode == nullptr)
			{
				node->prev = s_shared->m_tail;
				s_shared->m_iterator.m_currentNode = s_shared->m_tail = node;

				if (node->prev)
					node->prev->next = node;
				else
					s_shared->m_root = node;
			}
			else
			{
				--s_shared->m_iterator;
				while (s_shared->m_iterator.m_currentNode != nullptr)
				{
					if (*s_shared->m_iterator.m_currentNode == *node)
					{
						node->prev = s_shared->m_iterator.m_currentNode->prev;
						node->next = s_shared->m_iterator.m_currentNode->next;
						if (node->prev)
							node->prev->next = node;
						else
							s_shared->m_root = node;
						node->next->prev = node;
						delete s_shared->m_iterator.m_currentNode;
						s_shared->m_iterator.m_currentNode = node;
						return s_shared->m_iterator.m_currentIndex;
					}
					else if (*s_shared->m_iterator.m_currentNode < *node)
						break;
					else
						--s_shared->m_iterator;
				}

				if (s_shared->m_iterator.m_currentNode == nullptr)
				{
					node->next = s_shared->m_root;
					s_shared->m_iterator.m_currentNode = s_shared->m_root = s_shared->m_root->prev = node;
				}
				else
				{
					node->prev = s_shared->m_iterator.m_currentNode;
					node->next = s_shared->m_iterator.m_currentNode->next;
					node->prev->next = node;
					node->next->prev = node;
				}
			}
			++s_shared->m_count;
			return s_shared->m_iterator.m_currentIndex;
		}

	public:
		// Root wrapped in Iterator type
		Iterator begin() const
		{
			return Iterator(s_shared->m_root, 0);
		}

		// Return current iterator state
		Iterator current() const
		{
			return s_shared->m_iterator;
		}

		// Navigates to the given index, if possible, and returns the resulting iterator
		Iterator current(const size_t index)
		{
			setIterator(index);
			return s_shared->m_iterator;
		}

		// Tail wrapped in Iterator type
		Iterator end() const
		{
			return Iterator(nullptr, s_shared->m_count);
		}

		// Creates an empty list
		List() : s_shared(std::make_shared<SharedValues>()) {}

		// Creates a list of specified size
		// Every parameter after "size" is used in the construction of every new node
		template <class... Args>
		List(size_t size, Args&&... args) : s_shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < size; i++)
				emplace_back(args...);
		}

		// Creates a list based off the provided initializer list
		List(const std::initializer_list<T>& init) : s_shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < init.size(); i++)
				push_back(init.begin()[i]);
		}

		// Creates a list based off the provided initializer list
		// Uses each sublist as the basis for constructing a new object of the specified type
		template <class... Args>
		List(const std::initializer_list<Args...>& init) : s_shared(std::make_shared<SharedValues>())
		{
			for (unsigned i = 0; i < init.size(); i++)
				emplace_back(init.begin()[i]);
		}

		// Create a list with cloned elements from the provided list
		List(const List& list) : s_shared(std::make_shared<SharedValues>())
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
				s_shared = std::make_shared<SharedValues>();
				for (T& obj : list)
					emplace_back(obj);
			}
			else
				s_shared = list.s_shared;
			return *this;
		}

		//Assigns the curent linked list to hold the values presented by the initialized list
		List& operator=(const std::initializer_list<T>& init)
		{
			s_shared = std::make_shared<SharedValues>();
			for (unsigned i = 0; i < init.size(); i++)
				push_back(init.begin()[i]);
			return *this;
		}

		//Assigns the curent linked list to hold the values presented by the initialized list
		//
		template <class... Args>
		List& operator=(const std::initializer_list<std::initializer_list<Args...>>& init)
		{
			s_shared = std::make_shared<SharedValues>();
			for (unsigned i = 0; i < init.size(); i++)
				emplace_back(init.begin()[i].begin()...);
			return *this;
		}

		// Shares data between the two lists
		// They will both point to the same locations
		List& operator=(const List& list)
		{
			s_shared = list.s_shared;
			return *this;
		}

		//Returns the size of the list
		size_t& size() const { return s_shared->m_count; }

		//Creates a new element at the end of the list using a copy of the provided object
		//
		//--CODING NOTE: Ensure that this object type has a operator= function.
		//The compiler will throw an error if that is not the case.
		void push_back(const T& data)
		{
			if (s_shared->m_tail)
				s_shared->m_tail = s_shared->m_tail->next = new Node(data, s_shared->m_tail);
			else
				s_shared->m_root = s_shared->m_tail = new Node(data);
			s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count };
			s_shared->m_count++;
		}

		//Creates a new element at the beginning of the list using a copy of the provided object
		//
		//--CODING NOTE: Ensure that this object type has a operator= function.
		//The compiler will throw an error if that is not the case.
		void push_front(const T& data)
		{
			if (s_shared->m_root)
				s_shared->m_root = s_shared->m_root->prev = new Node(data, nullptr, s_shared->m_root);
			else
				s_shared->m_root = s_shared->m_tail = new Node(data);
			s_shared->m_iterator = { s_shared->m_root, 0 };
			s_shared->m_count++;
		}

		//Creates a new element at the end of the list using a constructor that 
		//takes all the parameters provided to the function
		//
		//--CODING NOTE: Ensure that this specialized constructor for the element type
		//exist. The compiler will throw an error if that is not the case.
		template <class... Args>
		T& emplace_back(Args&&... args)
		{
			if (s_shared->m_tail)
				s_shared->m_tail = s_shared->m_tail->next = new Node(s_shared->m_tail, nullptr, args...);
			else
				s_shared->m_root = s_shared->m_tail = new Node(nullptr, nullptr, args...);
			s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count++ };
			return s_shared->m_tail->data.object;
		}

		//Creates a new element at the beginning of the list using a constructor that 
		//takes all the parameters provided to the function
		//
		//--CODING NOTE: Ensure that this specialized constructor for the element type
		//exist. The compiler will throw an error if that is not the case.
		template <class... Args>
		T& emplace_front(Args&&... args)
		{
			if (s_shared->m_root)
				s_shared->m_root = s_shared->m_root->prev = new Node(nullptr, s_shared->m_root, args...);
			else
				s_shared->m_root = s_shared->m_tail = new Node(nullptr, nullptr, args...);
			s_shared->m_iterator = { s_shared->m_root, 0 };
			s_shared->m_count++;
			return s_shared->m_root->data.object;
		}

		//Maneuvers to the specified index and then creates numElements number of new elements
		//as shallow copies of T object "data"
		size_t insert(size_t index, size_t numElements, const T& data)
		{
			if (!s_shared->m_count)
			{
				s_shared->m_root = new Node(data);
				s_shared->m_iterator = { s_shared->m_root, 0 };
				s_shared->m_count++;
			}
			else if (index == 0)
			{
				s_shared->m_root = s_shared->m_root->prev = new Node(data, nullptr, s_shared->m_root);
				s_shared->m_iterator = { s_shared->m_root, 0 };
				s_shared->m_count++;
			}
			else if (setIterator(index))
			{
				s_shared->m_iterator.m_currentNode =
					s_shared->m_iterator.m_currentNode->prev =
					s_shared->m_iterator.m_currentNode->prev->next =
					new Node(data, s_shared->m_iterator.m_currentNode->prev, s_shared->m_iterator.m_currentNode);
				s_shared->m_count++;
			}
			else
			{
				s_shared->m_tail->next = new Node(data, s_shared->m_tail, nullptr);
				s_shared->m_iterator = { s_shared->m_tail->next, s_shared->m_count };
				s_shared->m_count++;
			}

			while (s_shared->m_iterator.m_currentIndex < index + numElements - 1)
			{
				s_shared->m_iterator.m_currentNode->next = new Node(data, s_shared->m_iterator.m_currentNode, s_shared->m_iterator.m_currentNode->next);
				++s_shared->m_iterator;
				s_shared->m_count++;
			}

			if (s_shared->m_iterator.m_currentIndex == s_shared->m_count - 1)
				s_shared->m_tail = s_shared->m_iterator.m_currentNode;
			return s_shared->m_iterator.m_currentIndex;
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
			if (!s_shared->m_root)
			{
				s_shared->m_root = new Node(nullptr, nullptr, args...);
				s_shared->m_iterator = { s_shared->m_root, 0 };
				s_shared->m_count++;
			}
			else if (index == 0)
			{
				s_shared->m_root = s_shared->m_root->prev = new Node(nullptr, s_shared->m_root, args...);
				s_shared->m_iterator = { s_shared->m_root, 0 };
				s_shared->m_count++;
			}
			else if (setIterator(index))
			{
				s_shared->m_iterator.m_currentNode =
					s_shared->m_iterator.m_currentNode->prev =
					s_shared->m_iterator.m_currentNode->prev->next =
					new Node(s_shared->m_iterator.m_currentNode->prev, s_shared->m_iterator.m_currentNode, args...);
				s_shared->m_count++;
			}
			else
			{
				s_shared->m_tail->next = new Node(s_shared->m_tail, nullptr, args...);
				s_shared->m_iterator = { s_shared->m_tail->next, s_shared->m_count };
				s_shared->m_count++;
			}

			while (s_shared->m_iterator.m_currentIndex < index + numElements - 1)
			{
				s_shared->m_iterator.m_currentNode->next = new Node(s_shared->m_iterator.m_currentNode, s_shared->m_iterator.m_currentNode->next, args...);
				++s_shared->m_iterator;
				s_shared->m_count++;
			}

			if (s_shared->m_iterator.m_currentIndex == s_shared->m_count - 1)
				s_shared->m_tail = s_shared->m_iterator.m_currentNode;
			return s_shared->m_iterator.m_currentIndex;
		}

		//Uses "data" to find the proper position in the list then constructs
		//a new element in that position with a shallow copy of "data"
		size_t insert_ordered(const T& data)
		{
			if (s_shared->m_count)
			{
				createOrderedNode(new Node(data));
				return s_shared->m_iterator.m_currentIndex;
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
			if (s_shared->m_count)
			{
				createOrderedNode(new Node(nullptr, nullptr, args...));
				return s_shared->m_iterator.m_currentIndex;
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
			if (newSize > s_shared->m_count)
			{
				if (!s_shared->m_count)
				{
					s_shared->m_root = s_shared->m_tail = new Node(nullptr, nullptr, args...);
					++s_shared->m_count;
				}
				while (s_shared->m_count < newSize)
				{
					s_shared->m_tail = s_shared->m_tail->next = new Node(s_shared->m_tail, nullptr, args...);
					++s_shared->m_count;
				}
				s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
			}
			else if (newSize < s_shared->m_count)
			{
				while (s_shared->m_count > newSize)
				{
					Node* cur = s_shared->m_tail;
					s_shared->m_tail = s_shared->m_tail->prev;
					s_shared->m_count--;
					delete cur;
				}
				if (s_shared->m_count)
				{
					s_shared->m_tail->next = nullptr;
					s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
				}
				else
				{
					s_shared->m_root = nullptr;
					s_shared->m_iterator = { nullptr, 0 };
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
			if (newSize > s_shared->m_count)
			{
				if (!s_shared->m_count)
				{
					s_shared->m_root = s_shared->m_tail = new Node(data);
					s_shared->m_count++;
				}
				while (s_shared->m_count < newSize)
				{
					s_shared->m_tail = s_shared->m_tail->next = new Node(data, s_shared->m_tail);
					s_shared->m_count++;
				}
				s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
			}
			else if (newSize < s_shared->m_count)
			{
				while (s_shared->m_count > newSize)
				{
					Node* cur = s_shared->m_tail;
					s_shared->m_tail = s_shared->m_tail->prev;
					s_shared->m_count--;
					delete cur;
				}
				if (s_shared->m_count)
				{
					s_shared->m_tail->next = nullptr;
					s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
				}
				else
				{
					s_shared->m_root = nullptr;
					s_shared->m_iterator = { nullptr, 0 };
				}
			}
		}

		//Removes the first element
		void pop_front()
		{
			if (s_shared->m_count)
			{
				Node* cur = s_shared->m_root;
				s_shared->m_root = s_shared->m_root->next;
				delete cur;
				if (s_shared->m_root != nullptr)
					s_shared->m_root->prev = nullptr;
				else
					s_shared->m_tail = nullptr;
				s_shared->m_iterator = { s_shared->m_root , 0 };
				--s_shared->m_count;
			}
		}

		//Removes the last element
		void pop_back()
		{
			if (s_shared->m_count)
			{
				Node* cur = s_shared->m_tail;
				s_shared->m_tail = s_shared->m_tail->prev;
				delete cur;
				if (s_shared->m_tail != nullptr)
					s_shared->m_tail->next = nullptr;
				else
					s_shared->m_root = nullptr;
				s_shared->m_count--;
				s_shared->m_iterator = { s_shared->m_tail , s_shared->m_count - 1 };
				--s_shared->m_count;
			}
		}

		//Deletes "numElements" number of elements from the list starting at the provided index
		bool erase(size_t index, size_t numElements = 1)
		{
			if (index < s_shared->m_count)
			{
				Node* beg = setIterator(index)->prev;
				while (numElements > 0 && s_shared->m_iterator.m_currentNode != nullptr)
				{
					delete (s_shared->m_iterator++).m_currentNode;
					--numElements;
					--s_shared->m_count;
					--s_shared->m_iterator.m_currentIndex;
				}
				if (beg)
				{
					beg->next = s_shared->m_iterator.m_currentNode;
					if (beg->next)
						s_shared->m_iterator.m_currentNode->prev = beg;
					else
					{
						s_shared->m_tail = beg;
						s_shared->m_iterator = { s_shared->m_tail , s_shared->m_count - 1 };
					}
				}
				else if (s_shared->m_iterator.m_currentNode)
				{
					s_shared->m_iterator.m_currentNode->prev = beg;
					s_shared->m_root = s_shared->m_iterator.m_currentNode;
				}
				else
					s_shared->m_root = s_shared->m_tail = nullptr;
				return true;
			}
			return false;
		}

		//Deletes all elements
		void clear()
		{
			s_shared->clear();
		}

		//Returns first element if one exists
		T& front()
		{
			if (s_shared->m_root)
			{
				s_shared->m_iterator = { s_shared->m_root, 0 };
				return s_shared->m_root->data.object;
			}
			else
				throw "Error: list index out of range";
		}

		//Returns last element if one exists
		T& back()
		{
			if (s_shared->m_tail)
			{
				s_shared->m_iterator = { s_shared->m_tail, s_shared->m_count - 1 };
				return s_shared->m_tail->data.object;
			}
			else
				throw "Error: list index out of range";
		}

		//Returns element at index if one exists
		T& operator[](size_t index)
		{
			if (index < s_shared->m_count)
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
				while (s_shared->m_iterator.m_currentNode)
				{
					if (*s_shared->m_iterator.m_currentNode == compare)
						return (signed)s_shared->m_iterator.m_currentIndex;
					++s_shared->m_iterator;
				}
			}
			return -1;
		}

		//Moves numElements number of elements starting at position index to position newPosition if both positions are valid.
		void moveElements(size_t index, size_t newPosition, size_t numElements = 1)
		{
			if (index >= s_shared->m_count)
				throw "Error: list index out of range";
			else if (newPosition > s_shared->m_count)
				throw "Error: list newPosition index out of range";
			else
			{
				if (index + numElements > s_shared->m_count)
					numElements = s_shared->m_count - index;
				else if (numElements == 0)
					numElements = 1;
				if (newPosition < index)
				{
					Node* end = setIterator(index + numElements - 1);
					Node* beg = setIterator(index);
					Node* pos = setIterator(newPosition);
					beg->prev->next = end->next;
					if (end->next)
						end->next->prev = beg->prev;
					else
						s_shared->m_tail = beg->prev;
					end->next = pos;
					beg->prev = pos->prev;
					pos->prev = end;
					if (beg->prev)
						beg->prev->next = beg;
					else
						s_shared->m_root = beg;
					s_shared->m_iterator.m_currentNode = beg;
				}
				else if (newPosition > index + numElements)
				{
					Node* prePos = setIterator(newPosition - 1);
					Node* end = setIterator(index + numElements - 1);
					Node* beg = setIterator(index);
					end->next->prev = beg->prev;
					if (beg->prev)
						beg->prev->next = end->next;
					else
						s_shared->m_root = end->next;
					beg->prev = prePos;
					end->next = prePos->next;
					prePos->next = beg;
					if (end->next)
						end->next->prev = end;
					else
						s_shared->m_tail = end;
					s_shared->m_iterator.m_currentIndex = newPosition - numElements;
				}
			}
		}

		//Swaps the contents of the two lists.
		List<T>& swap(List<T>& other)
		{
			s_shared.swap(other.s_shared);
			return *this;
		}
	};
}
