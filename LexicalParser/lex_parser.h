#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "id_identifier.h"
/*
定义的是词法解析器的基本信息
*/
enum TokenType {
	Num=0,Id,
	Else, If,
	Assign, 
	Or, Xor, 
	And, Eq, Ne, Lt, Gt, Le, Ge, 
	Add, Sub, Mul, Div, Mod,
	Key // 关键字
};
// Token 类型与名称对应表
static const char *token_strings[20] = {
	"Num", "Id", "Else", "If", "Assign", "Or", "Xor", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Key"
};
class LexItem;
// 词法解析器
class LexParser
{

private:
	// 存放保存解析后的元素的数组
	std::vector<LexItem> _tokens;
	// 存放 token 同时设置下一个要识别的文件
	void do_next(IdentifierTable &table, std::istream &_input_stream);
public:
	void parse(IdentifierTable &table, std::ostream &os, std::istream &_input_stream);
	const std::vector<LexItem> &getTokens() const;
};

// 机内单词分类
class LexItem
{
public:
	// 单词的类型
	TokenType _token_type;
	// 符号表入口
	IDEntry _id_entry;
public:
	LexItem(TokenType tokentype, IDEntry entry);
	LexItem(TokenType tokentype);
	LexItem() = default;
	friend std::ostream &operator<<(std::ostream &os, const LexItem &lhs);
};
