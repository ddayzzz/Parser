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
	
	auto &&node_left = lhs.getNodeInfo();
	auto &&node_right = rhs.getNodeInfo();
	// �����жϳ����Ƿ����
	if (node_left.right.length() != node_right.right.length())
		return false;
	// . ��λ���Ƿ����
	int p = lhs._pos;
	if (p != rhs._pos)
		return false;
	// �ж�ÿһ������ʽ�Ҳ������Ƿ����
	return node_left.right == node_right.right;

}

bool operator!=(const LRJizuItem &lhs, const LRJizuItem & rhs)
{
	return !(lhs == rhs);
}
bool operator<(const LRJizuItem & lhs, const LRJizuItem & rhs)
{
	std::stringstream left;
	std::stringstream right;
	left << lhs;
	right << rhs;
	// ����ַ���
	return left.str() < right.str();
}
LRJizuItem::~LRJizuItem()
{
}

JizuItemBase::~JizuItemBase()
{
}