#include "id_identifier.h"

Identifier::Identifier(const std::string & name, IDType type, int value):_name(name), _idtype(type), _value(value)
{
}

IDEntry IdentifierTable::insert(const std::string & name)
{
	for (int i = 0; i < _identifiers.size(); ++i)
	{
		if (_identifiers[i]._name == name)
			return i;
	}
	// ²»´æÔÚ
	_identifiers.push_back(Identifier(name, IDType::Unknown, 0));
	return _identifiers.size() - 1;
}

IDEntry IdentifierTable::insert(const Identifier & id_obj)
{
	_identifiers.push_back(id_obj);
	return _identifiers.size() - 1;
}

void IdentifierTable::fill(const IDEntry & id, IDType type)
{
	_identifiers[id]._idtype = type;

}

const Identifier & IdentifierTable::find(const IDEntry & id) const
{
	return _identifiers[id];
}

Identifier & IdentifierTable::find(const IDEntry & id)
{
	return _identifiers[id];
}

IDEntry IdentifierTable::existed(const std::string & name)
{
	for (int i = 0; i < _identifiers.size(); ++i)
	{
		if (_identifiers[i]._name == name)
			return i;
	}
	return -1;
}
