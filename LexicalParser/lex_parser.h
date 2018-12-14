#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "id_identifier.h"
/*
������Ǵʷ��������Ļ�����Ϣ
*/
enum TokenType {
	Num=0,Id,
	Else, If,
	Assign, 
	Or, Xor, 
	And, Eq, Ne, Lt, Gt, Le, Ge, 
	Add, Sub, Mul, Div, Mod,
	Key // �ؼ���
};
// Token ���������ƶ�Ӧ��
static const char *token_strings[20] = {
	"Num", "Id", "Else", "If", "Assign", "Or", "Xor", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Key"
};
class LexItem;
// �ʷ�������
class LexParser
{

private:
	// ��ű���������Ԫ�ص�����
	std::vector<LexItem> _tokens;
	// ��� token ͬʱ������һ��Ҫʶ����ļ�
	void do_next(IdentifierTable &table, std::istream &_input_stream);
public:
	void parse(IdentifierTable &table, std::ostream &os, std::istream &_input_stream);
	const std::vector<LexItem> &getTokens() const;
};

// ���ڵ��ʷ���
class LexItem
{
public:
	// ���ʵ�����
	TokenType _token_type;
	// ���ű����
	IDEntry _id_entry;
public:
	LexItem(TokenType tokentype, IDEntry entry);
	LexItem(TokenType tokentype);
	LexItem() = default;
	friend std::ostream &operator<<(std::ostream &os, const LexItem &lhs);
};
