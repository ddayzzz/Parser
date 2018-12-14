#include "inter_grammerTree.h"

inline Parameter::Parameter(int id, const std::string & name, const std::string & des, const std::string & cond, const std::string error) :\
_paraID(id), _name(name), _description(des), _errorMsg(error), _condition(nullptr) {}

//��������ı��ʽ��Ϣ

inline Log::Log(LogType logtype, const std::string & info, const std::string & exp) :_logtype(logtype), _message(info), _exp(exp) {}

inline Log::Log(LogType logtype, const char * msg, const char * exp) : _logtype(logtype), _message(msg) { if (exp) _exp = exp; }

//����൱��Ĭ�ϵĹ��캯�� Ĭ��Ϊû�д���

inline Log::Log(LogType logtype) : _logtype(logtype) {}

//��̬ʶ�����ͣ�����RTTI�Ŀ���

inline ExpNodeBaseType ExpNodeBase::getType()
{
	return ExpNodeBaseType::FCN;
}


//ͬһ����ĵ�һ������

inline ExpNodeBase::ExpNodeBase() : pChild(nullptr), pBrother(nullptr) {}

inline ExpNodeBase::~ExpNodeBase()
{
	ExpNodeBase *pnode = pChild, *temp;
	while (pnode)
	{
		temp = pnode->pBrother;
		delete pnode;
		pnode = temp;
	}

}

//���ʽҶ�ӽ�㣬��ô���Լ̳���Щ�ӿڣ��������Ҫ���̳�Ĭ�ϵİ汾�Ϳ�����

inline std::pair<ValueType, Log> ValueTypeBase::asLongDouble() const { return { ValueType(),Log(LogType::ERROR,"��֧�ֽ�������ת��ΪLong Double����") }; }

//ת��Ϊ��ֵ����

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_add(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"���������Ͳ�֧�ֲ�����\'+\'") }; }

//�ӷ�

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_divide(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"���������Ͳ�֧�ֲ�����\'-\'") }; }

//����

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_subtract(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"���������Ͳ�֧�ֲ�����\'*\'") }; }

//�˷�

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_multiply(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"���������Ͳ�֧�ֲ�����\'/\'") }; }

//����

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_reverse() { return { nullptr,Log(LogType::ERROR,"���������Ͳ�֧��\'��Ԫ����\'") }; }

//����Ԫ

inline ValueTypeBase::~ValueTypeBase() {}

//δ֪���ĵ�����

inline FunctionInfo::FunctionInfo(const std::string & fcnname, const std::string & disname, const std::string & exp, std::initializer_list<Parameter> parainit, bool builtin, int prior, FcnCombine comb, int unkcount) :\
_priority(prior), _unkcount(unkcount), _expression(exp), _FcnName(fcnname), _displayname(disname), \
_isbuiltin(builtin), _combine(comb)
{
	for (auto i = parainit.begin(); i != parainit.end(); ++i)
	{
		_map_nameToInfo.emplace(i->_name, *i);//���ݲ��������ֽ���ӳ���
	}
}

//����ǿ��Դӱ��ʽ������ĳ�������ʽ �����Ļ�����Ҫ��������

inline Const::Const(const std::string & exp, const std::string & des) :_description(des), _exp(exp) {
	if (_exp.size() > 0)
	{
		auto res = Transfer_ExpToExpTree(_exp, 0, _exp.size() - 1);//�����ı��ʽ���ܰ�������
		if (res && res->eval())
		{
			_value = res->eval()->clone();
			delete res;
		}
	}
}

inline Const::Const(ValueTypeBase * restree, const std::string & des) :_value(restree->clone()), _exp(std::string()), _description(des)
{

}

inline Const::~Const()
{
	if (_value)
		delete _value;
	_value = nullptr;
}

inline ValueTypeBase * Const::getValueExp()
{
	if (_value)
		return _value;
	else
		return nullptr;
}

//һ��Ҫ���

inline Const::Const(const Const & rhs)
{
	_description = rhs._description;
	_exp = rhs._exp;
	_value = rhs._value->clone();
}

Const & Const::operator=(const Const & rhs)
{
	_description = rhs._description;
	_exp = rhs._exp;
	_value = rhs._value->clone();
	return *this;
}

inline void Number::strTonNum(const std::string & s)
{
	int TEN = 1;
	double DECMIAL = 0.1;
	bool nodec = true;
	int posofdec = s.find_first_of('.', 0);
	if (posofdec == std::string::npos)
		posofdec = s.size();
	int i;
	for (i = posofdec - 1; i >= 0; --i)
	{
		_val += (s[i] - '0')*TEN;
		TEN = TEN * 10;
	}
	for (posofdec = posofdec + 1; posofdec <= s.size() - 1; ++posofdec)
	{
		_val += (s[posofdec] - '0')*DECMIAL;
		DECMIAL /= 10;
	}

}

inline Number::Number(const std::string & s) { strTonNum(s); }

inline Number::Number(const ValueType & v) :_val(v) {}

inline ValueTypeBase * Number::eval(std::vector<Log>* log) { return this; }

inline ExpNodeBaseType Number::getType()
{
	return ExpNodeBaseType::NUM;
}

inline std::pair<ValueType, Log> Number::asLongDouble() const { return { _val,Log() }; }

inline std::pair<ValueTypeBase*, Log> Number::opt_add(ValueTypeBase * r)
{
	//return {new Number(_val + r.eval()->asLongDouble());
	auto p = r->eval()->asLongDouble();
	if (p.second._logtype == LogType::NONE)
	{
		return { new Number(_val + p.first),Log() };
	}
	else
	{
		return { nullptr,Log(LogType::ERROR,"�Ҳ�������֧��ת��ΪLong Double����") };
	}
}

inline std::pair<ValueTypeBase*, Log> Number::opt_subtract(ValueTypeBase * r)
{
	auto p = r->eval()->asLongDouble();
	if (p.second._logtype == LogType::NONE)
	{
		return { new Number(_val - p.first),Log() };
	}
	else
	{
		return { nullptr,Log(LogType::ERROR,"�Ҳ�������֧��ת��ΪLong Double����") };
	}
}

inline std::pair<ValueTypeBase*, Log> Number::opt_multiply(ValueTypeBase * r)
{
	//return {new Number(_val + r.eval()->asLongDouble());
	auto p = r->eval()->asLongDouble();
	if (p.second._logtype == LogType::NONE)
	{
		return { new Number(_val * p.first),Log() };
	}
	else
	{
		return { nullptr,Log(LogType::ERROR,"�Ҳ�������֧��ת��ΪLong Double����") };
	}
}

inline std::pair<ValueTypeBase*, Log> Number::opt_divide(ValueTypeBase * r)
{
	auto p = r->eval()->asLongDouble();
	if (p.second._logtype == LogType::NONE)
	{
		if (p.first == 0.0l)
		{
			return { nullptr,Log(LogType::ERROR,"��������Ϊ0") };
		}
		return { new Number(_val / p.first),Log() };
	}
	else
	{
		return { nullptr,Log(LogType::ERROR,"�Ҳ�������֧��ת��ΪLong Double����") };
	}
}

inline std::pair<ValueTypeBase*, Log> Number::opt_reverse()
{
	return { new Number(0.0l - _val),Log() };
}

void Number::print(std::ostream &os)
{
	os << this->_val;
}

//�������� ����û��ʲô�����ͷŵ�

inline Number::~Number() {}

inline Number * Number::clone()
{
	return new Number(*this);
}

//ָ����ָ��

inline Present::Present(PresentType type, ExpNodeBase * presentfor, int id) :_type(type), _id(id), _presentfor(presentfor) {}

inline ExpNodeBaseType Present::getType() { return ExpNodeBaseType::PRE; }

inline ValueTypeBase * Present::eval(std::vector<Log>* log) {
	if (_presentfor)
	{
		return _presentfor->eval();//���ر������ָ��
	}
	//error
	return nullptr;
}

inline Present::~Present()
{
	if (_type == PresentType::CONST)
		return;//���������ͷ�
	if (_presentfor)
	{
		delete _presentfor;
		_presentfor = nullptr;
	}
}

//���ú�������Ϣָ��

inline void Function::do_setFcnPtr()
{
	if (fcnID.size() > 0)
	{
		auto i = getFunctionInfo(fcnID);
		fcnPtr = i;
	}
}

inline ExpNodeBaseType Function::getType() { return ExpNodeBaseType::FCN; }

inline FunctionInfo * Function::getFcnExpPtr() { return fcnPtr; }

inline Function::Function(const std::string & fn) :fcnID(fn) { do_setFcnPtr(); }

inline Function::Function(const char c) { fcnID.push_back(c); do_setFcnPtr(); }

//��������Ҫ��� ʹ��ջģʽ��

inline Function::~Function() {}

//�������ĸ������������뺯���Ľ���ʽ�ı��ʽ�� �޸�present��δ֪�����͵�present

inline bool UserFunction::do_trasfer_presentRealParameter(std::vector<ExpNodeBase*>& real, ExpNodeBase * exptree, std::vector<Log>* log)
{
	ExpNodeBase* pchild = exptree->pChild;
	bool res = true;
	while (pchild)
	{
		if (pchild->getType() == ExpNodeBaseType::FCN)
		{
			//��Ȼʽ����ʽ
			res = res & do_trasfer_presentRealParameter(real, pchild, log);//��ô������һ��ĵݹ�
		}
		else
		{
			if (pchild->getType() == ExpNodeBaseType::NUM)
				;
			else
			{
				if (pchild->getType() == ExpNodeBaseType::PRE)
				{
					Present *pre = dynamic_cast<Present*>(pchild);
					if (pre)
					{
						if (pre->_type == PresentType::UNK)
						{
							if (pre->_id < real.size())
								pre->_presentfor = real[pre->_id];
							else
							{
								if (log) log->emplace_back(LogType::ERROR, "ָ���Ĳ�������");
								return false;
							}
						}
						else
						{
							if (pre->_type == PresentType::CONST)
								;//�����Ѿ��õ���ֵ ����ҾͲ�����
						}
					}
				}
				else
				{
					if (log) log->emplace_back(LogType::ERROR, "δ���������");
					return false;
				}
			}
		}
		pchild = pchild->pBrother;
	}
	return res;
}

inline bool UserFunction::setParameterPtr(std::stack<ExpNodeBase*>& sta, std::vector<Log>*) { return false; }

inline UserFunction::UserFunction(const std::string & fcnname) :Function(fcnname), fcnExpr(nullptr) {}

inline ValueTypeBase * UserFunction::eval(std::vector<Log>* log) {
	if (fcnExpr)
	{
		if (result == nullptr)
			result = fcnExpr->eval(log);
		return result;
	}
	else
	{
		//û��ָ������
		if (log)
			log->emplace_back(LogType::ERROR, "����û��ָ���ı��ʽ");
	}
	return nullptr;
}

inline bool UserFunction::setParameterPtr(std::vector<ExpNodeBase*>& real, std::vector<Log>* log)
{
	if (fcnExpr == nullptr)
	{
		fcnExpr = Transfer_ExpToExpTree(fcnPtr->_expression, 0, fcnPtr->_expression.size() - 1, &fcnPtr->_map_nameToInfo);
		if (fcnExpr)
		{
			//update result
			if (result)
				delete result;//����Ѿ����������Ϣ
			realUnknown = real.size();
			return do_trasfer_presentRealParameter(real, fcnExpr, log);
		}
		else
		{
			if (log)
				log->emplace_back(LogType::ERROR, "�޷��������ʽ��", fcnPtr->_expression);
			return false;
		}
	}
	else
	{
		return true;
	}
}

inline UserFunction::~UserFunction()
{
	if (fcnExpr)
	{
		delete fcnExpr;
		fcnExpr = nullptr;
	}
}

inline BuiltInFunction::BuiltInFunction(const std::string & fcnid) :Function(fcnid) {}

inline BuiltInFunction::BuiltInFunction(char c) : Function(c) {}

inline ValueTypeBase * BuiltInFunction::eval(std::vector<Log>* log) {
	if (result == nullptr)
	{
		result = BuiltInFuntionCalc(fcnPtr, realUnknown, pChild, log);
	}
	return result;

}

//�����Ҫ�ù�userfunction�Ľ���ʽ�Ĵ����βεĹ���

inline bool BuiltInFunction::setParameterPtr(std::vector<ExpNodeBase*>& real, std::vector<Log>* log) {
	int j;
	ExpNodeBase **pnode = &pChild;
	for (j = 0; j < real.size(); ++j)
	{
		*pnode = real[j];
		pnode = &((*pnode)->pBrother);
	}
	realUnknown = j;
	//��̬��������������
	return true;
}

//�����Ҫ���ڽ������ʽ��ʱ���õ���ջ

inline bool BuiltInFunction::setParameterPtr(std::stack<ExpNodeBase*>& sta, std::vector<Log>* log)
{
	auto fptr = getFunctionInfo(fcnID);
	int unk = fptr->_unkcount;
	if (fptr)
	{
		ExpNodeBase **pnode = &pChild;
		int j;
		if (fptr->_combine == FcnCombine::Left)
		{
			j = 0;
			std::stack<ExpNodeBase*> rever;
			for (; !sta.empty() && j < unk; ++j)
			{
				rever.push(sta.top());
				sta.pop();
			}
			while (!rever.empty())
			{
				*pnode = rever.top();
				rever.pop();
				pnode = &((*pnode)->pBrother);
			}
		}
		else
		{
			for (j = 0; j < unk && sta.size(); ++j)
			{
				*pnode = sta.top();
				sta.pop();
				pnode = &((*pnode)->pBrother);
			}
		}
		realUnknown = j;//����ʵ�ʵĲ���
		return true;
	}
	else
	{
		if (log)
			log->emplace_back(LogType::ERROR, "�޷�����������Ϣ��û����Ч�ĺ�����Ϣָ��");
		return false;
	}
}

inline BuiltInFunction::~BuiltInFunction()
{
	if (result)
	{
		delete result;
		result = nullptr;
	}
}

inline void BuiltInFunction::setResultPtr(ValueTypeBase * res) { result = res; }

//֧�ִ��в����Լ�ָ���������ı��ʽ

inline void Expression::do_specify_unknown(std::vector<ExpNodeBase*>* real_para, std::vector<Log>* log)
{
	if (_isfcn == false)
	{
		if (_isunk == false)
		{
			//�Ǻ��� �Ҳ������� ֱ�Ӽ��� ��������Ҫ���¼���
			if (_expTree == nullptr)
			{
				_expTree = Transfer_ExpToExpTree(_fcnname_or_expr, 0, _fcnname_or_expr.size() - 1, nullptr, log);
			}
			if (_result == nullptr && _expTree)
			{
				_result = _expTree->eval(log);
			}
		}
		else
		{
			//�Ǻ��� ���Ǵ����� �����ڣ�ÿһ������δ֪���ı��ʽ��������뺯���б���
		}
	}
	else
	{
		if (_fptr == nullptr)
		{
			_fptr = getFunctionInfo(_fcnname_or_expr);
		}
		//���� ���ܴ��л����ǲ������� ��������������Ͳ�Ҫ��μ���
		if (_fptr && _expTree && _result && _isunk == false)
			return;
		if (!_fptr)
			return;//function not fount
		if (_expTree == nullptr)
			_expTree = do_realUnkown_Function(_fcnname_or_expr, real_para, real_para->size(), log);
		if (_expTree)
		{
			_result = _expTree->eval(log);
		}
	}
}

//ָ�������������Ǳ��ʽ

Expression::Expression(const std::string & fcnname_or_expr, bool isunk, bool isfcn) :_fcnname_or_expr(fcnname_or_expr), _isfcn(isfcn), _isunk(isunk)
{
	//��Ҫ��ȡ�����ı��ʽ
}

//��ʼ����

ValueTypeBase * Expression::Eval(std::vector<ExpNodeBase*>* real_para, std::vector<Log>* log)
{
	do_specify_unknown(real_para, log);
	if (_result)
	{
		return _result;
	}
	return _result;
}

Expression::~Expression()
{
	if (_expTree)
	{
		delete _expTree;
		_expTree = nullptr;
	}
}
ValueTypeBase* BuiltInFuntionCalc(const FunctionInfo *fptr, int realunk, ExpNodeBase *paras, std::vector<Log> *log)
{
	if (!fptr)
	{
		if (log)
			log->emplace_back(LogType::ERROR, "û��ָ������ָ������");
		return nullptr;
	}
	if (realunk == 2)
	{
		if (!(paras && paras->pBrother))
		{
			if (log)
				log->emplace_back(LogType::ERROR, "�����Ĳ�����ʵ�ʵĲ��������", fptr->_FcnName);
			return nullptr;
		}
		//��Ԫ����
		std::pair<ValueTypeBase*, Log> calles;
		if (fptr->_FcnName == "+")
			calles = paras->eval()->opt_add(paras->pBrother->eval());
		else if (fptr->_FcnName == "-")
			calles = paras->eval()->opt_subtract(paras->pBrother->eval());
		else if (fptr->_FcnName == "/")
			calles = paras->eval()->opt_divide(paras->pBrother->eval());
		else if (fptr->_FcnName == "*")
			calles = paras->eval()->opt_multiply(paras->pBrother->eval());
		else
		{
			//unknown opts
			if (log)
				log->emplace_back(LogType::ERROR, std::string("���������\'").append(fptr->_FcnName).append("\'��ʵ�ʲ��������������������Ͳ�����"));
			return nullptr;
		}
		if (calles.second._logtype == LogType::NONE)
			return calles.first;
		if (log)
			log->push_back(calles.second);
		return nullptr;
	}
	else
	{
		if (realunk == 1)
		{
			if (!paras)
			{
				if (log)
					log->emplace_back(LogType::ERROR, "�����Ĳ�����ʵ�ʵĲ��������", fptr->_FcnName);
				return nullptr;
			}
			//һԪ����ע��ȡ�������� ʹ��number����Ԫ����
			std::pair<ValueTypeBase*, Log> calles;
			if (fptr->_FcnName == "-")
				calles = paras->eval()->opt_reverse();
			else if (fptr->_FcnName == "sin")
			{
				auto fcncalls = paras->eval()->asLongDouble();
				if (fcncalls.second._logtype == LogType::NONE)
					return new Number(sinl(fcncalls.first));
				//error
				if (log) log->push_back(fcncalls.second);
				return nullptr;
				//����Ҫ�����´ε��ж�
			}
			else
			{
				//unknown opts
				if (log)
					log->emplace_back(LogType::ERROR, std::string("���������\'").append(fptr->_FcnName).append("\'��ʵ�ʲ��������������������Ͳ�����"));
				return nullptr;
			}
			if (calles.second._logtype == LogType::NONE)
				return calles.first;
			if (log)
				log->push_back(calles.second);
			return nullptr;
		}
	}
}



//����ǳ�ʼ����ȫ�ֺ����б�
void init_functionlist()
{
	Parameter p1(0, "x", "�����", "none", "error");
	Parameter p2(1, "y", "�Ҽ���", "none", "error");
	//FunctionExpression fe(std::string("�ӷ�����"),"22", true,"none", std::initializer_list<Parameter>{ p1,p2 });
	FunctionInfo finfo(std::string("+"), std::string("�ӷ������"), std::string("x+y"), { p1,p2 }, true, 6, FcnCombine::Left, 2);
	functionlist.emplace("+", finfo);

	Parameter p3(0, "x", "������", "none", "error");
	Parameter p4(1, "y", "����", "none", "error");
	FunctionInfo finfo1(std::string("-"), std::string("���������"), std::string("x-y"), { p3,p4 }, true, 6, FcnCombine::Left, 2);
	functionlist.emplace("-", finfo1);

	Parameter p5(0, "x", "������", "none", "error");
	Parameter p6(1, "y", "����", "none", "error");
	FunctionInfo finfo2(std::string("*"), std::string("�˷������"), std::string("x*y"), { p5,p6 }, true, 5, FcnCombine::Left, 2);
	functionlist.emplace("*", finfo2);

	Parameter p7(0, "x", "������", "none", "error");
	Parameter p8(1, "y", "����", "none", "error");
	FunctionInfo finfo3(std::string("/"), std::string("���������"), std::string("x/y"), { p7,p8 }, true, 5, FcnCombine::Left, 2);
	functionlist.emplace("/", finfo3);


	Parameter p9(0, "x", "�Ƕȣ������ƣ�", "none", "error");
	FunctionInfo finfo4(std::string("sin"), std::string("���Һ���"), std::string("sin(x)"), { p9 }, true, 5, FcnCombine::Left, 1);
	functionlist.emplace("sin", finfo4);

	Parameter p10(0, "x", "����1", "none", "error");
	Parameter p11(1, "y", "����2", "none", "error");
	FunctionInfo finfo5(std::string("my1"), std::string("my1�����"), std::string("x*2+y"), { p10,p11 }, false, 0, FcnCombine::Left, 2);
	functionlist.emplace("my1", finfo5);

	//Parameter p12(0, "x", "����1", "none", "error");
	//Parameter p13(1, "y", "����2", "none", "error");
	//FunctionInfo finfo6(std::string("my2"), std::string("my2�����"), std::string("x*2+y*sin(x-y)"), { p12,p13 }, false, 0, FcnCombine::Left, 2);
	//functionlist.emplace("my2", finfo6);

}

//��ʼ�����ļ��������ȫ�ֳ����б�
void init_constslist()
{
	//Const c1("3.1415926", "PI");//ע�⣺�����ôд�ͻᵼ���ڴ���ͷ�
	global_constslist.emplace("PI", Const("3.1415926", "PI"));

}

ExpNodeBase * Transfer_ExpToExpTree(std::string e, int lower, int upper, std::map<std::string, Parameter>* unklist, std::vector<Log>* log)
{
	std::stack<Function*> fcns;//����Ǳ��溯��������������ŵ�ջ
	std::stack<ExpNodeBase*> cab;//���㵥Ԫ
	std::string exp(e);
	if (e.size() < 1)
	{
		if (log)
			log->emplace_back(LogType::ERROR, "���ʽ̫��");
		return nullptr;
	}
	//submarkĬ��ȫ��0 >=1 ��ʾ���ŵĲ��� ����sin((9)) submark[3]=1 submark[4]=2 submark[6]=2 submark[7]=1;
	for (int beg = lower; beg <= upper;)
	{
		//����-��Ϊ��Ԫ�����������ǹ涨���֮ǰû�в�������cabջ�Ĵ�СΪ0����ô���ǵ�Ԫ�ġ����ʱ�����ڱ��ʽ�е� ����������Ĳ��������ߺ��� �ǿ϶��������ŵ� ����������ڵݹ��н��
		if ((exp[beg] >= '0' && exp[beg] <= '9'))
		{
			int endn = beg + 1;
			for (; exp[endn] == '.' || (exp[endn] >= '0' && exp[endn] <= '9'); ++endn)
				;
			ExpNodeBase *num = new Number(exp.substr(beg, endn - beg));
			if (!num)
			{
				//������Ҫ�����ڴ�������
				if (log)
					log->emplace_back(LogType::ERROR, "�ڴ����ʧ��");
				return nullptr;
			}
			cab.push(num);
			beg = endn;
		}
		else
		{
			if (exp[beg] == '(')
			{
				if (beg < upper)
				{

					fcns.push(nullptr);//������
					++beg;
				}
			}
			else
			{
				if (exp[beg] == ')')
				{
					for (; !fcns.empty() && fcns.top() != nullptr; fcns.pop())
					{
						fcns.top()->setParameterPtr(cab);
						cab.push(fcns.top());
					}
					if (!fcns.empty())
					{
						fcns.pop();
					}
					else
					{
						//������Ҫ�����ڴ�������
						if (log)
							log->emplace_back(LogType::ERROR, "����ƥ��ʧ��");
						return nullptr;
					}
					++beg;
				}
				else
				{
					if (isalpha(exp[beg]) || exp[beg] == '+' || exp[beg] == '-' || exp[beg] == '*' || exp[beg] == '/' ||
						exp[beg] == '=')
					{
						//������ͷһ����ĸ �Լ��������� ���԰�����ͷ��������ĸ ���������� ��ĸ �»���
						int la = do_trasfer_FcnandPresent(exp, fcns, cab, beg, upper, unklist, log);
						if (la == -1)
						{
							//������Ҫ�����ڴ�������
							if (log)
								log->emplace_back(LogType::ERROR, "�����ӱ��ʽ��������");
							return nullptr;
						}
						beg = la;
					}
					else
					{
						//������Ҫ�����ڴ�������
						if (log)
							log->emplace_back(LogType::ERROR, "���ʽ������δ������ַ�");
						return nullptr;
					}
				}
			}
		}
	}
	for (; !fcns.empty(); fcns.pop())
	{
		//buildTreeNode(fcns.top(), cab);
		fcns.top()->setParameterPtr(cab);
		cab.push(fcns.top());
	}
	if (cab.size() == 1)
	{
		return cab.top();
	}
	else
	{
		//������Ҫ�����ڴ�������
		if (log)
			log->emplace_back(LogType::ERROR, "���ʽ���ڴ���");
		return nullptr;
	}
}

int do_trasfer_FcnandPresent(const std::string & exp, std::stack<Function*>& fcns, std::stack<ExpNodeBase*>& cab, int & l, int & u, std::map<std::string, Parameter>* unklist, std::vector<Log>* log)
{
	//ȷ�����������ƣ�ͬһ���������岻ͬ�������庯������û�����ŵ��ǲ����������������һ���ǲ�һ���� ��Ҳ����ʹ��-(x)�ĵ�����ʽ ������ķ��� �� -
	//���� -3-1 ��һ��-ʱȡ��һԪ������d�ڶ���-�Ǽ�����������Ԫ
	Function *fptr = nullptr;
	if (exp[l] == '-' && cab.size() == 1)
	{
		//�������ŵ�����ʽ ������������� �����Ǻ�����
		//auto *fninfo = getFunctionInfo(std::string("-"));
		fptr = new BuiltInFunction("-");
		fcns.push(fptr);
		return l + 1;
	}
	else
	{

		if (exp[l] == '+' || exp[l] == '-' || exp[l] == '*' || exp[l] == '/')
		{
			fptr = new BuiltInFunction(exp[l]);
			FunctionInfo *curr = fptr->getFcnExpPtr(), *statop;
			if (curr == nullptr)
			{
				return -1;//error 
			}
			for (; !fcns.empty() && fcns.top() != nullptr; fcns.pop())
			{
				statop = fcns.top()->getFcnExpPtr();
				if (statop == nullptr)
					return -1;//unknown opts
				if (curr->_priority < statop->_priority)
					break;
				bool rs = fcns.top()->setParameterPtr(cab, log);
				if (rs)
					cab.push(fcns.top());
				else
					return -1;
			}
			fcns.push(fptr);
			return l + 1;
		}//��ʵ�������һЩ����������� �����߼��ȵ� ˼·����Ϊ��������Ϳ�����
		else
		{
			int split = l + 1;//split�Ǳ����������ĺ�һλ ��exp[split]=='('��ʾ���� �����Ǳ�����
			for (; isalnum(exp[split]) || exp[split] == '_'; ++split)
				;
			if (exp[split] == '(')
			{
				std::string fcnname = exp.substr(l, split - l);
				int right = do_transfer_getLastKuohao(exp, split, u);
				if (right == -1)
					return -1;//error 
				std::vector<std::pair<int, int>> poss;
				do_transfer_spiltFunctionSubExp(exp, split + 1, right - 1, poss);
				if (poss.size() < 1)
					return -1;//error
				std::vector<ExpNodeBase*> real_exp(poss.size());
				for (int i = 0; i < poss.size(); ++i)
				{
					real_exp[i] = Transfer_ExpToExpTree(exp, poss[i].first, poss[i].second, unklist, log);
					if (real_exp[i] == nullptr)
					{
						return -1;
					}
				}
				fptr = do_realUnkown_Function(fcnname, &real_exp, poss.size(), log);
				if (fptr)
				{
					cab.push(fptr);//����ֵ����
					return right + 1;
				}
				else
				{
					return -1;
				}


			}
			else
			{
				//���Ȳ��ҳ�������Ժ��ټ���
				//��������
				std::string name = exp.substr(l, split - l);
				auto cfind = getConst(name);
				if (cfind)
				{
					//����
					Present *pre = new Present(PresentType::CONST, cfind->getValueExp());
					cab.push(pre);
					return split;
				}
				else
				{
					if (unklist)
					{
						auto id = unklist->find(name);
						if (id != unklist->end())
						{
							Present *pre = new Present(PresentType::UNK, nullptr, id->second._paraID);
							cab.push(pre);
							return split;
						}
					}
					else
					{
						return -1;//error
					}
				}
			}
		}
	}
	if (log)
		log->emplace_back(LogType::UNKNOWN, "�����������û��ߴ�����ʱ�����������״��������������Ѿ���ֹ��");
	return -1;
}

int do_transfer_getLastKuohao(const std::string &exp, int beg, int u)
{
	int tra = 0;
	for (; beg <= u; ++beg)
	{
		if (exp[beg] == '(')
			++tra;
		if (exp[beg] == ')')
			--tra;
		if (tra == 0)
			break;
	}
	if (beg > u)
		return -1;
	return beg;
}

void do_transfer_spiltFunctionSubExp(const std::string &exp, int l, int u, std::vector<std::pair<int, int>> &poss)
{
	//ע���ӱ��ʽҲ���ܰ�����������
	int prePos = l;//ǰһ���ӱ��ʽ����ֹλ�õ���һ������һ�� ����һ���ӱ��ʽ����ʼλ��
	for (; l <= u;)
	{
		int temp = l;
		if (exp[temp] == ',')
		{
			poss.emplace_back(prePos, temp - 1);
			l = prePos = temp + 1;
		}
		else
		{
			if (exp[temp] == '(')
			{
				//�����ӱ��ʽ
				int lastkuohao = do_transfer_getLastKuohao(exp, temp, u);
				if (lastkuohao == -1)
				{
					poss.clear();
					return;
				}
				if (lastkuohao == u || (lastkuohao < u && exp[lastkuohao + 1] == ','))
				{
					//�ҵ���һ���ӱ��ʽ
					poss.emplace_back(prePos, lastkuohao);
					l = prePos = lastkuohao + 2;
				}
				else
				{
					//����ʽ�������ı��ʽ ������Ҫ�������㲻Ҫ����prepos
					l = lastkuohao + 1;
					continue;
				}
			}
			else
			{
				++l;
			}
		}

	}
	if (prePos <= u)
		poss.emplace_back(prePos, u);//�������ֻ��һ���������Ҳ�����һ���������õı��ʽ�Ķ���ɵ���Ӷ������������
}

Function* do_realUnkown_Function(const std::string &fcnname, std::vector<ExpNodeBase*> *real_exp, int totalunk, std::vector<Log> *log)
{
	auto finfo = getFunctionInfo(fcnname);//�û��Զ���ĺ��������ڽ���Եĸ�������ȼ��ĸ��� ���Ǵ�����
	Function* fptr = nullptr;
	if (finfo)
	{
		//�Ƿ����û��Զ��庯��
		if (finfo->_isbuiltin == true)
		{
			fptr = new BuiltInFunction(fcnname);
		}
		else
		{
			fptr = new UserFunction(fcnname);
		}
		bool rs = fptr->setParameterPtr(*real_exp, log);
		if (rs)
		{
			return fptr;
		}
		else
		{
			if (log)
				log->emplace_back(LogType::ERROR, "�޷����������ı��ʽ", finfo->_FcnName);
			return nullptr;
		}
	}
	else
	{
		if (log)
			log->emplace_back(LogType::ERROR, "�޷���ȡ��������Ϣ");
		return nullptr;
	}
}


FunctionInfo *getFunctionInfo(const std::string &id)
{
	auto i = functionlist.find(id);
	if (i != functionlist.end())
	{
		return &(*i).second;
	}
	else
	{
		return nullptr;
	}
}
Const *getConst(const std::string &constname)
{
	auto cfind = global_constslist.find(constname);
	if (cfind != global_constslist.end())
	{
		return &(cfind->second);
	}
	else
	{
		return nullptr;
	}
}