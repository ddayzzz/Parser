#pragma once
#include "base.h"
#include <sstream>
#include <map>
#include <list>


/*定义集族中产生式的基类*/
class JizuItemBase
{
public:
	virtual ~JizuItemBase() = 0; // 纯虚构造函数, 在这里使用 静态分发的技术避免 new 内存对象
};

// 用于 LR(0) 和 SLR(1) 的集族项
class LRJizuItem : public JizuItemBase
{
private:
	node _node; // 普通的集族的产生式, LR0 集族类型
	int _pos; // 记录的 . 的位置
	int _pId; // 所在 LR0文法的产生式编号
public:
	// 构造新的对象, 需要产生式 以及 . 的位置
	LRJizuItem(const node &node, int pos, int pid);

	// 返回 . 后面的下一个符号 如果 isFinal == true, 返回 空字符
	const char &charForClosure() const;
	// 是否是终止状态
	bool isFinal() const;
	// 向前移进 . (++pos)
	LRJizuItem &operator++();
	// 获取产生相关的信息
	const node &getNodeInfo() const;
	// 获取产生式编号
	int getPId() const;
	// 相关的比较运算符
	friend bool operator==(const LRJizuItem &lhs, const LRJizuItem &rhs);
	friend bool operator!=(const LRJizuItem &lhs, const LRJizuItem &rhs);
	friend bool operator<(const LRJizuItem &lhs, const LRJizuItem &rhs);
	// 返回这个集族项目的字符串表示
	friend std::ostream &operator<<(std::ostream &os, const LRJizuItem &t)
	{
		// TODO: 在此处插入 return 语句
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
		// 是不是最后才有?
		return os;
	}
	// 析构函数, 避免变成纯虚类
	~LRJizuItem();
};
/*对于集族的操作*/
template<typename JizuItemType>
class Jizu
{
private:
	std::set<JizuItemType> _items;  // 存放这个集族中的所有产生式
	int _id;
public:
	Jizu(int id, const std::initializer_list<JizuItemType> &items) : _id(id), _items(items.begin(), items.end()) {}
	Jizu(int id, const std::list<JizuItemType> &items) : _id(id), _items(items.begin(), items.end()) {}
	Jizu() :_id(-1) {}
	// 定义各类运算符
	// 判断集族是否相同
	friend bool operator==(const Jizu &lhs, const Jizu &rhs)
	{
		return lhs._items == rhs._items;
	}
	friend bool operator!=(const Jizu &lhs, const Jizu &rhs)
	{
		return !(lhs == rhs);
	}
	// 其他的接口函数
	// 获取其所有的产生式, 返回的是包含的产生式对象
	const std::set<JizuItemType> getAllP() const;
	// 插入新的对象
	void insertNewP(const JizuItemType &rhs);
	// 获取集族编号
	int getId() const;
	// 获取相关的可转移的符号和对应的产生式 p_to_transfer[i] = p 表示符号i的产生式集合. symbols_reached[i] 表示可以通过符号 i 进行转移
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
	// 生成内部的产生式的相关信息
	for (auto && p : _items)
	{
		// 产生式的下一个符号
		auto nxt_char = p.charForClosure();
		if (nxt_char != '\0')
		{
			// 转移到这个状态的产生式
			p_to_transfer.insert(std::make_pair(nxt_char, p));
			// 仅添加符号可供转移的符号
			symbols_reached.insert(nxt_char);
		}
	}
}