#pragma once
#include "lr_base.h"
#include "lr_jizu.hpp"
#include "base.h"
#include <list>
#include <queue>
#include <map>
#include <string>
#include <set>
#include <ostream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stack>
#include <cstdlib>
#include <cstring>


// 保存 LRxxx 分析表中 Action 和 Goto 的项目
class TableItem
{
private:
	// 对应表的符号
	char _column_symbol;
	// 对应的列号
	int _column_index;
	// 存储的数据信息, 一般是产生式编号 状态号
	int _column_data;
	// 类型信息
	TableItemType _item;
public:
	TableItem(TableItemType item_type, int column_index, char column_symbol, int column_data);
	TableItem();
	TableItemType getItemType() const;
	int getItemData() const;
};




// LR(0) 解析器. 如果存在冲突, 转换为 SLR(1) 以期解决大多数不能被 LR0 解决的冲突.
class LR0SLR1Parser : public Base
{
protected:

	std::map<int, Jizu<LRJizuItem>> _jizu;  // 集族编号-> 集族对象
	char _jizu_dfa[100][100]; // _jizu_dfa[1][2] = 'A' 表从 I1->I2 识别符号 A, = '\0' 表示不识别. 最多100个状态
	std::set<LRJizuItem> _jizu_item_marker;
	int next_avaiable_id; // 集族的可用的编号(也可以表示最后一个集族的编号)
	std::vector<std::vector<TableItem>> _table;  // Action 和 Goto [x][u] = xx 表示 状态 x 接收 u 的值 xxx = {S_xxx, r_xxx, Accept, Number}
	/* 求某一个集族, 来自于符号 fromSymbol, 新建的集族可能的编号的 jizuId, 使用 initialItems 作为初始生成式,  可供转移的符号包括 symbols_reached, 关联的产生式是 p_to_transfer
	返回集族对象, 同时修改几个那几个 转移的内容 */
	Jizu<LRJizuItem> do_generateJizu(int jizuId, const std::list<LRJizuItem> &initialItems);
	// 帮助函数, 用于总控程序处理状态等信息
	// 将数字表示的状态压入栈中
	void do_push_status(int number, std::string &stack);
	// 获取栈中的数字表示，保持栈的不变，返回的是数字以及这个数字在仿的栈的起始位置（按照顺序）。
	std::pair<int, int> do_check_status(const std::string &stack);
	// 检查是否完成 也就是 ACTION[栈顶的状态][#] = ACCEPT 的 ITEM 对象, 由于技术限制， 需要对原栈进行弹出操作， 但最后可保持栈的不变
	bool do_check_complete(const std::string &stack, char input_top);
	// 检查当前的集族是否存在问题
	CoflictType do_check_placable(const Jizu<LRJizuItem> &jizu, const node &accept) const;
	// 按照 拓扑排序求 FIRST 集合的排列顺序, 一般队列的头部是 最好求的. 但是这个算法并不是最优甚至无法避免诸如 E->T T->E 这种情况, 待改进
	void do_toplogically_get_first_order(std::queue<char> &order);
public:
	LR0SLR1Parser();
	// 生成集族
	void getJizu(char target);
	// 
	void demo();
	virtual ~LR0SLR1Parser() override;
	// 构造识别活前缀的 DFA
	void getDFAForActivePrefix();
	// LR0 分析表
	void getTable();
	// 打印分析表
	void printTable();
	// 分析(LRxx 总控程序), 支持输入一个符号串, 以 # 结尾
	void analyse();
};