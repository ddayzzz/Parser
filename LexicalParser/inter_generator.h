#pragma once
#include <string>
#include <vector>
#include <queue>
#include "lr_parser.h"

#include "id_identifier.h"
#include "lex_parser.h"
// 四元式对象
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
// 中间代码生成, 目前支持的是简单说明语句 算术表达式 赋值语句 布尔表达式 简单控制语句
class BaseInterGenerator
{
private:
	std::vector<QuadTuple> _tuples;
	int _temp_var_avaiable_id=0;
public:
	void generate(const std::string &op, IDEntry left, IDEntry right, IDEntry temp);
	// 生成中间代码
	void parse(IdentifierTable &idtable, const LexParser &lex_parser);
	// 生成临时变量编号, 并且插入到符号表中。返回临时变量的入口地址
	IDEntry generateTempVariable(IdentifierTable &idtable);
};