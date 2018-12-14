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
	// ����������ļ�飬ֻ�ܽ��������� i=x op y ����ʽ
	std::queue<IDEntry> operands;
	// ��ȡ���еĵ���
	auto &&tokens = lex_parser.getTokens();
	int i = 0;
	while (i < tokens.size())
	{
		auto &&token = tokens[i];
		// ���ű��λ��
		auto &&identry = token._id_entry;
		// ��¼���ŵ�����
		std::string id_type;
		switch (token._token_type)
		{
		case TokenType::Key:
			// �ؼ��֣� Ŀǰֻ֧�� int float ����������
			id_type = idtable.find(identry)._name;
			break;
		case TokenType::Id:
			// ��Ҫ���������ʲô����
			if (id_type == "int")
				idtable.find(identry)._idtype = IDType::Int;
			else if (id_type == "float")
				idtable.find(identry)._idtype = IDType::Float;
			else
				idtable.find(identry)._idtype = IDType::Unknown;
			operands.push(identry);
			break;

		case TokenType::Assign:
			// ��ֵ���㣬ȡ�� ������� op �Ҳ�����
			left = tokens[++i]._id_entry;
			op = tokens[++i];
			right = tokens[++i]._id_entry;
			// д����Ԫʽ
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
