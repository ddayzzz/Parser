#include "inter_generator.h"

QuadTuple::QuadTuple(const std::string &op, IDEntry left, IDEntry right, IDEntry temp):_op(op), _left_oprand(left), _right_operand(right), _temp(temp)
{
}

void BaseInterGenerator::generate(const std::string & op, IDEntry left, IDEntry right, IDEntry temp)
{
	_tuples.emplace_back(op, left, right, temp);
}

void BaseInterGenerator::parse(IdentifierTable & idtable, const LexParser & lex_parser)
{
	IDEntry left, right;
	LexItem op;
	IDEntry temp;
	// 不进行语义的检查，只能解析类似于 i=x op y 的形式
	std::queue<IDEntry> operands;
	// 获取所有的单词
	auto &&tokens = lex_parser.getTokens();
	int i = 0;
	while (i < tokens.size())
	{
		auto &&token = tokens[i];
		// 符号表的位置
		auto &&identry = token._id_entry;
		// 记录符号的类型
		std::string id_type;
		switch (token._token_type)
		{
		case TokenType::Key:
			// 关键字， 目前只支持 int float 的类型声明
			id_type = idtable.find(identry)._name;
			break;
		case TokenType::Id:
			// 需要设置这个是什么类型
			if (id_type == "int")
				idtable.find(identry)._idtype = IDType::Int;
			else if (id_type == "float")
				idtable.find(identry)._idtype = IDType::Float;
			else
				idtable.find(identry)._idtype = IDType::Unknown;
			operands.push(identry);
			break;

		case TokenType::Assign:
			// 赋值运算，取出 左操作数 op 右操作数
			left = tokens[++i]._id_entry;
			op = tokens[++i];
			right = tokens[++i]._id_entry;
			// 写入四元式
			temp = generateTempVariable(idtable);
			this->_tuples.emplace_back(token_strings[op._token_type], left, right, temp);
			break;
		default:
			break;
		}
	}
}

IDEntry BaseInterGenerator::generateTempVariable(IdentifierTable & idtable)
{
	int var_id = _temp_var_avaiable_id++;
	return idtable.insert("T" + std::to_string(var_id));
}
