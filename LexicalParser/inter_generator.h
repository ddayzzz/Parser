#pragma once
#include <string>
#include <vector>
#include <queue>
#include "lr_parser.h"

#include "id_identifier.h"
#include "lex_parser.h"
// ��Ԫʽ����
class QuadTuple
{
private:
	std::string _op;
	IDEntry _left_oprand;
	IDEntry _right_operand;
	IDEntry _temp;
public:
	QuadTuple(const std::string &op, IDEntry left, IDEntry right, IDEntry temp);
};
// �м��������, Ŀǰ֧�ֵ��Ǽ�˵����� �������ʽ ��ֵ��� �������ʽ �򵥿������
class BaseInterGenerator
{
private:
	std::vector<QuadTuple> _tuples;
	int _temp_var_avaiable_id=0;
public:
	void generate(const std::string &op, IDEntry left, IDEntry right, IDEntry temp);
	// �����м����
	void parse(IdentifierTable &idtable, const LexParser &lex_parser);
	// ������ʱ�������, ���Ҳ��뵽���ű��С�������ʱ��������ڵ�ַ
	IDEntry generateTempVariable(IdentifierTable &idtable);
};