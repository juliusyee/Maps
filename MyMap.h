// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#include <queue>
#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap(); //destructor; deletes all of the tree's nodes
	void clear(); //deletes all of the trees nodes producing an empty tree
	int size() const; //return the number of assocations in the map

	//	The	associate	method	associates	one	item	(key)	with	another	(value).
	//	If	no	association	currently	exists	with	that	key,	this	method	inserts
	//	a	new	association	into	the	tree	with	that	key/value	pair.		If	there	is
	//	already	an	association	with	that	key	in	the	tree,	then	the	item
	//	associated	with	that	key	is	replaced	by	the	second	parameter	(value).
	//	Thus,	the	tree	contains	no	duplicate	keys
	void associate(const KeyType& key, const ValueType& value);

	//	If	no	association	exists	with	the	given	key,	return	nullptr;	otherwise,
	//	return	a	pointer	to	the	value	associated	with	that	key.		This	pointer	can	be
	//	used	to	examine	that	value,	and	if	the	map	is	allowed	to	be	modified,	to
	//	modify	that	value	directly	within	the	map (the	second	overload	enables
	//	this).		Using	a	little	C++	magic,	we	have	implemented	it	in	terms	of	the
	//	first	overload,	which	you	must	implement.
	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node
	{
		Node(KeyType myKey, ValueType myVal)
		{
			m_value = myVal;
			m_key = myKey;
			m_left = nullptr;
			m_right = nullptr;
		}
		KeyType m_key;
		ValueType m_value;
		Node* m_left; 
		Node* m_right;
	};
	Node* m_root;
	int m_size;
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
	m_root = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	//if tree is already empty, no need to clear anything
	if (m_root == nullptr)
	{
		return;
	}
	//Create a queue to hold pointers to nodes
	queue<Node*> temp;
	temp.push(m_root);

	while (!temp.empty())
	{
		Node* current = temp.front();
		temp.pop();
		if (current->m_left != nullptr)
		{
			temp.push(current->m_left);
		}
		if (current->m_right != nullptr)
		{
			temp.push(current->m_right);
		}
		delete current;
	}
	m_size = 0;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	//If the tree is empty
	if (m_root == nullptr)
	{
		m_root = new Node(key, value);
		m_size++;
		return;
	}
	//Otherwise, search for existing key 
	Node* ptr = m_root;
	for (;;)
	{
		if (key == ptr->m_key) 
		{
			ptr->m_value = value;
			return;
		}
		if (key < ptr->m_key)
		{
			if (ptr->m_left != nullptr)
			{
				ptr = ptr->m_left;
			}
			else
			{
				ptr->m_left = new Node(key, value);
				m_size++;
				return;
			}
		}
		else if (key > ptr->m_key)
		{
			if (ptr->m_right != nullptr)
			{
				ptr = ptr->m_right;
			}
			else
			{
				ptr->m_right = new Node(key, value);
				m_size++;
				return;
			}
		}
	}
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
	return m_size;
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	Node* ptr = m_root;
	ValueType* val = nullptr;
	while (ptr != nullptr)
	{
		if (key == ptr->m_key)
		{
			val = &(ptr->m_value);
			return(val);
		}
		else if (key < ptr->m_key)
		{
			ptr = ptr->m_left;
		}
		else
		{
			ptr = ptr->m_right;
		}
	}
	return (val);
}


