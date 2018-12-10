#include "lr_jizu.hpp"

LRJizuItem::LRJizuItem(const node &node, int pos, int pid)
{
	this->_node.left = node.left;
	this->_node.right = node.right;
	this->_pos = pos;
	this->_pId = pid;
}


const char & LRJizuItem::charForClosure() const
{
	// TODO: �ڴ˴����� return ���
	if (this->isFinal())
		return '\0';
	else
		return this->_node.right[_pos];
}

bool LRJizuItem::isFinal() const
{
	if (_pos >= this->_node.right.size())
		return true;
	else
		return false;
}

LRJizuItem & LRJizuItem::operator++()
{
	// TODO: �ڴ˴����� return ���
	if (this->isFinal())
	{
		return *this; // ����������
	}
	++this->_pos;
	return *this;
}

const node & LRJizuItem::getNodeInfo() const
{
	return this->_node;
}

int LRJizuItem::getPId() const
{
	return _pId;
}

bool operator==(const LRJizuItem &lhs, const LRJizuItem & rhs)
{
	if (lhs._pos == rhs._pos && lhs._node.left == rhs._node.left && lhs._node.right == rhs._node.right)
		return true;
	else
		return false;
}

bool operator!=(const LRJizuItem &lhs, const LRJizuItem & rhs)
{
	return !(lhs == rhs);
}

bool operator<(const LRJizuItem &lhs, const LRJizuItem & rhs)
{
	// ʹ���ַ������бȽ�
	std::ostringstream l, r;
	l << lhs;
	r << rhs;
	return l.str() < r.str();
}
LRJizuItem::~LRJizuItem()
{
}

JizuItemBase::~JizuItemBase()
{
}