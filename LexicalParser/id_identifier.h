#pragma once
#include <string>
#include <vector>
// IDEntry ���ű�ĳ����ʶ�����������
using IDEntry = int;
// ���ű����
enum IDType {
	Unknown,
	Int,
	Float
};
class Identifier
{
public:
	// ��ʶ������
	std::string _name;
	// ��ʶ��������
	IDType _idtype;
	// ֵ
	int _value;
	Identifier(const std::string &name, IDType type, int value);
};

// ���ű����
class IdentifierTable
{
public:
	std::vector<Identifier> _identifiers;
	// ����ĳ������, ���ڴʷ������׶�
	IDEntry insert(const std::string &name);
	// ����ĳ���Ѿ�����ķ���
	IDEntry insert(const Identifier &id_obj);
	// ������ŵ�ĳЩ��Ϣ
	void fill(const IDEntry &id, IDType type);
	// ���ҷ��ţ���ֹ�޸�
	const Identifier &find(const IDEntry &id) const;
	// ���ҷ��ţ������޸�
	Identifier &find(const IDEntry &id);
	// �Ƿ����
	IDEntry existed(const std::string &name);
};