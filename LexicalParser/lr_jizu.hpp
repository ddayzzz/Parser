#pragma once
#include "base.h"
#include <sstream>
#include <map>
#include <list>


/*���弯���в���ʽ�Ļ���*/
class JizuItemBase
{
public:
	virtual ~JizuItemBase() = 0; // ���鹹�캯��, ������ʹ�� ��̬�ַ��ļ������� new �ڴ����
};

// ���� LR(0) �� SLR(1) �ļ�����
class LRJizuItem : public JizuItemBase
{
private:
	node _node; // ��ͨ�ļ���Ĳ���ʽ, LR0 ��������
	int _pos; // ��¼�� . ��λ��
	int _pId; // ���� LR0�ķ��Ĳ���ʽ���
public:
	// �����µĶ���, ��Ҫ����ʽ �Լ� . ��λ��
	LRJizuItem(const node &node, int pos, int pid);

	// ���� . �������һ������ ��� isFinal == true, ���� ���ַ�
	const char &charForClosure() const;
	// �Ƿ�����ֹ״̬
	bool isFinal() const;
	// ��ǰ�ƽ� . (++pos)
	LRJizuItem &operator++();
	// ��ȡ������ص���Ϣ
	const node &getNodeInfo() const;
	// ��ȡ����ʽ���
	int getPId() const;
	// ��صıȽ������
	friend bool operator==(const LRJizuItem &lhs, const LRJizuItem &rhs);
	friend bool operator!=(const LRJizuItem &lhs, const LRJizuItem &rhs);
	friend bool operator<(const LRJizuItem &lhs, const LRJizuItem &rhs);
	// �������������Ŀ���ַ�����ʾ
	friend std::ostream &operator<<(std::ostream &os, const LRJizuItem &t)
	{
		// TODO: �ڴ˴����� return ���
		if (!t.isFinal())
		{
			os << t._node.left << "->";
			int sz = t._node.right.length();
			for (int i = 0; i < sz; ++i)
			{
				if (i == t._pos)
					os << '.';
				os << t._node.right[i];
			}
		}
		else
		{
			os << t._node.left << "->" << t._node.right << '.';
		}
		// �ǲ���������?
		return os;
	}
	// ��������, �����ɴ�����
	~LRJizuItem();
};
/*���ڼ���Ĳ���*/
template<typename JizuItemType>
class Jizu
{
private:
	std::set<JizuItemType> _items;  // �����������е����в���ʽ
	int _id;
public:
	Jizu(int id, const std::initializer_list<JizuItemType> &items) : _id(id), _items(items.begin(), items.end()) {}
	Jizu(int id, const std::list<JizuItemType> &items) : _id(id), _items(items.begin(), items.end()) {}
	Jizu() :_id(-1) {}
	// ������������
	// �жϼ����Ƿ���ͬ
	friend bool operator==(const Jizu &lhs, const Jizu &rhs)
	{
		return lhs._items == rhs._items;
	}
	friend bool operator!=(const Jizu &lhs, const Jizu &rhs)
	{
		return !(lhs == rhs);
	}
	// �����Ľӿں���
	// ��ȡ�����еĲ���ʽ, ���ص��ǰ����Ĳ���ʽ����
	const std::set<JizuItemType> getAllP() const;
	// �����µĶ���
	void insertNewP(const JizuItemType &rhs);
	// ��ȡ������
	int getId() const;
	// ��ȡ��صĿ�ת�Ƶķ��źͶ�Ӧ�Ĳ���ʽ p_to_transfer[i] = p ��ʾ����i�Ĳ���ʽ����. symbols_reached[i] ��ʾ����ͨ������ i ����ת��
	void getTransferSymbolsAndPs(std::multimap<char, LRJizuItem> &p_to_transfer, std::set<char> &symbols_reached) const;
};

template<typename JizuItemType>
inline const std::set<JizuItemType> Jizu<JizuItemType>::getAllP() const
{
	return this->_items;
}

template<typename JizuItemType>
inline void Jizu<JizuItemType>::insertNewP(const JizuItemType & rhs)
{
	this->_items.insert(rhs);
}

template<typename JizuItemType>
inline int Jizu<JizuItemType>::getId() const
{
	return _id;
}

template<typename JizuItemType>
inline void Jizu<JizuItemType>::getTransferSymbolsAndPs(std::multimap<char, LRJizuItem>& p_to_transfer, std::set<char>& symbols_reached) const
{
	// �����ڲ��Ĳ���ʽ�������Ϣ
	for (auto && p : _items)
	{
		// ����ʽ����һ������
		auto nxt_char = p.charForClosure();
		if (nxt_char != '\0')
		{
			// ת�Ƶ����״̬�Ĳ���ʽ
			p_to_transfer.insert(std::make_pair(nxt_char, p));
			// ����ӷ��ſɹ�ת�Ƶķ���
			symbols_reached.insert(nxt_char);
		}
	}
}