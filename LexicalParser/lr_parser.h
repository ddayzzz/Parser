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


// ���� LRxxx �������� Action �� Goto ����Ŀ
class TableItem
{
private:
	// ��Ӧ��ķ���
	char _column_symbol;
	// ��Ӧ���к�
	int _column_index;
	// �洢��������Ϣ, һ���ǲ���ʽ��� ״̬��
	int _column_data;
	// ������Ϣ
	TableItemType _item;
public:
	TableItem(TableItemType item_type, int column_index, char column_symbol, int column_data);
	TableItem();
	TableItemType getItemType() const;
	int getItemData() const;
};




// LR(0) ������. ������ڳ�ͻ, ת��Ϊ SLR(1) ���ڽ����������ܱ� LR0 ����ĳ�ͻ.
class LR0SLR1Parser : public Base
{
protected:

	std::map<int, Jizu<LRJizuItem>> _jizu;  // ������-> �������
	char _jizu_dfa[100][100]; // _jizu_dfa[1][2] = 'A' ��� I1->I2 ʶ����� A, = '\0' ��ʾ��ʶ��. ���100��״̬
	std::set<LRJizuItem> _jizu_item_marker;
	int next_avaiable_id; // ����Ŀ��õı��(Ҳ���Ա�ʾ���һ������ı��)
	std::vector<std::vector<TableItem>> _table;  // Action �� Goto [x][u] = xx ��ʾ ״̬ x ���� u ��ֵ xxx = {S_xxx, r_xxx, Accept, Number}
	/* ��ĳһ������, �����ڷ��� fromSymbol, �½��ļ�����ܵı�ŵ� jizuId, ʹ�� initialItems ��Ϊ��ʼ����ʽ,  �ɹ�ת�Ƶķ��Ű��� symbols_reached, �����Ĳ���ʽ�� p_to_transfer
	���ؼ������, ͬʱ�޸ļ����Ǽ��� ת�Ƶ����� */
	Jizu<LRJizuItem> do_generateJizu(int jizuId, const std::list<LRJizuItem> &initialItems);
	// ��������, �����ܿس�����״̬����Ϣ
	// �����ֱ�ʾ��״̬ѹ��ջ��
	void do_push_status(int number, std::string &stack);
	// ��ȡջ�е����ֱ�ʾ������ջ�Ĳ��䣬���ص��������Լ���������ڷµ�ջ����ʼλ�ã�����˳�򣩡�
	std::pair<int, int> do_check_status(const std::string &stack);
	// ����Ƿ���� Ҳ���� ACTION[ջ����״̬][#] = ACCEPT �� ITEM ����, ���ڼ������ƣ� ��Ҫ��ԭջ���е��������� �����ɱ���ջ�Ĳ���
	bool do_check_complete(const std::string &stack, char input_top);
	// ��鵱ǰ�ļ����Ƿ��������
	CoflictType do_check_placable(const Jizu<LRJizuItem> &jizu, const node &accept) const;
	// ���� ���������� FIRST ���ϵ�����˳��, һ����е�ͷ���� ������. ��������㷨���������������޷��������� E->T T->E �������, ���Ľ�
	void do_toplogically_get_first_order(std::queue<char> &order);
public:
	LR0SLR1Parser();
	// ���ɼ���
	void getJizu(char target);
	// 
	void demo();
	virtual ~LR0SLR1Parser() override;
	// ����ʶ���ǰ׺�� DFA
	void getDFAForActivePrefix();
	// LR0 ������
	void getTable();
	// ��ӡ������
	void printTable();
	// ����(LRxx �ܿس���), ֧������һ�����Ŵ�, �� # ��β
	void analyse();
};