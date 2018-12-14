#pragma once
#include <string>
#include <vector>
// IDEntry 符号表某个标识符的入口类型
using IDEntry = int;
// 符号表管理
enum IDType {
	Unknown,
	Int,
	Float
};
class Identifier
{
public:
	// 标识符名称
	std::string _name;
	// 标识符的类型
	IDType _idtype;
	// 值
	int _value;
	Identifier(const std::string &name, IDType type, int value);
};

// 符号表管理
class IdentifierTable
{
public:
	std::vector<Identifier> _identifiers;
	// 插入某个符号, 用于词法分析阶段
	IDEntry insert(const std::string &name);
	// 插入某个已经构造的符号
	IDEntry insert(const Identifier &id_obj);
	// 填入符号的某些信息
	void fill(const IDEntry &id, IDType type);
	// 查找符号，禁止修改
	const Identifier &find(const IDEntry &id) const;
	// 查找符号，允许修改
	Identifier &find(const IDEntry &id);
	// 是否存在
	IDEntry existed(const std::string &name);
};