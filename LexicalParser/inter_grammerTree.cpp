#include "inter_grammerTree.h"

inline Parameter::Parameter(int id, const std::string & name, const std::string & des, const std::string & cond, const std::string error) :\
_paraID(id), _name(name), _description(des), _errorMsg(error), _condition(nullptr) {}

//发生错误的表达式信息

inline Log::Log(LogType logtype, const std::string & info, const std::string & exp) :_logtype(logtype), _message(info), _exp(exp) {}

inline Log::Log(LogType logtype, const char * msg, const char * exp) : _logtype(logtype), _message(msg) { if (exp) _exp = exp; }

//这个相当于默认的构造函数 默认为没有错误

inline Log::Log(LogType logtype) : _logtype(logtype) {}

//动态识别类型：避免RTTI的开销

inline ExpNodeBaseType ExpNodeBase::getType()
{
	return ExpNodeBaseType::FCN;
}


//同一级别的第一个对象

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

//如果式叶子结点，那么可以继承这些接口，如果不需要，继承默认的版本就可以了

inline std::pair<ValueType, Log> ValueTypeBase::asLongDouble() const { return { ValueType(),Log(LogType::ERROR,"不支持将操作数转换为Long Double类型") }; }

//转换为数值类型

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_add(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"操作数类型不支持操作符\'+\'") }; }

//加法

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_divide(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"操作数类型不支持操作符\'-\'") }; }

//减法

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_subtract(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"操作数类型不支持操作符\'*\'") }; }

//乘法

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_multiply(ValueTypeBase * r) { return { nullptr,Log(LogType::ERROR,"操作数类型不支持操作符\'/\'") }; }

//除法

inline std::pair<ValueTypeBase*, Log> ValueTypeBase::opt_reverse() { return { nullptr,Log(LogType::ERROR,"操作数类型不支持\'逆元操作\'") }; }

//求逆元

inline ValueTypeBase::~ValueTypeBase() {}

//未知数的的数量

inline FunctionInfo::FunctionInfo(const std::string & fcnname, const std::string & disname, const std::string & exp, std::initializer_list<Parameter> parainit, bool builtin, int prior, FcnCombine comb, int unkcount) :\
_priority(prior), _unkcount(unkcount), _expression(exp), _FcnName(fcnname), _displayname(disname), \
_isbuiltin(builtin), _combine(comb)
{
	for (auto i = parainit.begin(); i != parainit.end(); ++i)
	{
		_map_nameToInfo.emplace(i->_name, *i);//根据参数的名字建立映射表
	}
}

//这个是可以从表达式中载入的常量计算式 这样的话就需要析构函数

inline Const::Const(const std::string & exp, const std::string & des) :_description(des), _exp(exp) {
	if (_exp.size() > 0)
	{
		auto res = Transfer_ExpToExpTree(_exp, 0, _exp.size() - 1);//常量的表达式不能包含变量
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

//一定要深拷贝

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
		return { nullptr,Log(LogType::ERROR,"右操作数不支持转换为Long Double类型") };
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
		return { nullptr,Log(LogType::ERROR,"右操作数不支持转换为Long Double类型") };
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
		return { nullptr,Log(LogType::ERROR,"右操作数不支持转换为Long Double类型") };
	}
}

inline std::pair<ValueTypeBase*, Log> Number::opt_divide(ValueTypeBase * r)
{
	auto p = r->eval()->asLongDouble();
	if (p.second._logtype == LogType::NONE)
	{
		if (p.first == 0.0l)
		{
			return { nullptr,Log(LogType::ERROR,"除数不能为0") };
		}
		return { new Number(_val / p.first),Log() };
	}
	else
	{
		return { nullptr,Log(LogType::ERROR,"右操作数不支持转换为Long Double类型") };
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

//析构函数 但是没有什么可以释放的

inline Number::~Number() {}

inline Number * Number::clone()
{
	return new Number(*this);
}

//指代的指针

inline Present::Present(PresentType type, ExpNodeBase * presentfor, int id) :_type(type), _id(id), _presentfor(presentfor) {}

inline ExpNodeBaseType Present::getType() { return ExpNodeBaseType::PRE; }

inline ValueTypeBase * Present::eval(std::vector<Log>* log) {
	if (_presentfor)
	{
		return _presentfor->eval();//返回被代表的指针
	}
	//error
	return nullptr;
}

inline Present::~Present()
{
	if (_type == PresentType::CONST)
		return;//常量不会释放
	if (_presentfor)
	{
		delete _presentfor;
		_presentfor = nullptr;
	}
}

//设置函数的信息指针

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

//操作数需要这个 使用栈模式的

inline Function::~Function() {}

//构造树的辅助函数：带入函数的解析式的表达式树 修改present的未知数类型的present

inline bool UserFunction::do_trasfer_presentRealParameter(std::vector<ExpNodeBase*>& real, ExpNodeBase * exptree, std::vector<Log>* log)
{
	ExpNodeBase* pchild = exptree->pChild;
	bool res = true;
	while (pchild)
	{
		if (pchild->getType() == ExpNodeBaseType::FCN)
		{
			//仍然式解析式
			res = res & do_trasfer_presentRealParameter(real, pchild, log);//那么进行下一层的递归
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
								if (log) log->emplace_back(LogType::ERROR, "指定的参数不足");
								return false;
							}
						}
						else
						{
							if (pre->_type == PresentType::CONST)
								;//常量已经得到了值 这个我就不管了
						}
					}
				}
				else
				{
					if (log) log->emplace_back(LogType::ERROR, "未定义的类型");
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
		//没有指定参数
		if (log)
			log->emplace_back(LogType::ERROR, "函数没有指定的表达式");
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
				delete result;//清除已经计算过的信息
			realUnknown = real.size();
			return do_trasfer_presentRealParameter(real, fcnExpr, log);
		}
		else
		{
			if (log)
				log->emplace_back(LogType::ERROR, "无法构建表达式树", fcnPtr->_expression);
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

//这个主要用股userfunction的解析式的带入形参的过程

inline bool BuiltInFunction::setParameterPtr(std::vector<ExpNodeBase*>& real, std::vector<Log>* log) {
	int j;
	ExpNodeBase **pnode = &pChild;
	for (j = 0; j < real.size(); ++j)
	{
		*pnode = real[j];
		pnode = &((*pnode)->pBrother);
	}
	realUnknown = j;
	//动态解析参数的类型
	return true;
}

//这个主要用于解析表达式的时候，用的是栈

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
		realUnknown = j;//保存实际的参数
		return true;
	}
	else
	{
		if (log)
			log->emplace_back(LogType::ERROR, "无法解析函数信息：没有有效的函数信息指针");
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

//支持带有参数以及指定函数名的表达式

inline void Expression::do_specify_unknown(std::vector<ExpNodeBase*>* real_para, std::vector<Log>* log)
{
	if (_isfcn == false)
	{
		if (_isunk == false)
		{
			//非函数 且不带参数 直接计算 不可能需要重新计算
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
			//非函数 但是带参数 不存在，每一个带有未知数的表达式必须存在与函数列表中
		}
	}
	else
	{
		if (_fptr == nullptr)
		{
			_fptr = getFunctionInfo(_fcnname_or_expr);
		}
		//函数 可能带有或者是不带参数 如果不带参数，就不要多次计算
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

//指定函数名或者是表达式

Expression::Expression(const std::string & fcnname_or_expr, bool isunk, bool isfcn) :_fcnname_or_expr(fcnname_or_expr), _isfcn(isfcn), _isunk(isunk)
{
	//需要获取函数的表达式
}

//开始计算

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
			log->emplace_back(LogType::ERROR, "没有指定函数指针类型");
		return nullptr;
	}
	if (realunk == 2)
	{
		if (!(paras && paras->pBrother))
		{
			if (log)
				log->emplace_back(LogType::ERROR, "函数的参数与实际的参数不相符", fptr->_FcnName);
			return nullptr;
		}
		//二元函数
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
				log->emplace_back(LogType::ERROR, std::string("运算符或函数\'").append(fptr->_FcnName).append("\'与实际参数数量不符或者是类型不兼容"));
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
					log->emplace_back(LogType::ERROR, "函数的参数与实际的参数不相符", fptr->_FcnName);
				return nullptr;
			}
			//一元函数注意取反的问题 使用number的逆元操作
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
				//不需要进行下次的判断
			}
			else
			{
				//unknown opts
				if (log)
					log->emplace_back(LogType::ERROR, std::string("运算符或函数\'").append(fptr->_FcnName).append("\'与实际参数数量不符或者是类型不兼容"));
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



//这个是初始化的全局函数列表
void init_functionlist()
{
	Parameter p1(0, "x", "左加数", "none", "error");
	Parameter p2(1, "y", "右加数", "none", "error");
	//FunctionExpression fe(std::string("加法运算"),"22", true,"none", std::initializer_list<Parameter>{ p1,p2 });
	FunctionInfo finfo(std::string("+"), std::string("加法运算符"), std::string("x+y"), { p1,p2 }, true, 6, FcnCombine::Left, 2);
	functionlist.emplace("+", finfo);

	Parameter p3(0, "x", "被减数", "none", "error");
	Parameter p4(1, "y", "减数", "none", "error");
	FunctionInfo finfo1(std::string("-"), std::string("减法运算符"), std::string("x-y"), { p3,p4 }, true, 6, FcnCombine::Left, 2);
	functionlist.emplace("-", finfo1);

	Parameter p5(0, "x", "被乘数", "none", "error");
	Parameter p6(1, "y", "乘数", "none", "error");
	FunctionInfo finfo2(std::string("*"), std::string("乘法运算符"), std::string("x*y"), { p5,p6 }, true, 5, FcnCombine::Left, 2);
	functionlist.emplace("*", finfo2);

	Parameter p7(0, "x", "被除数", "none", "error");
	Parameter p8(1, "y", "除数", "none", "error");
	FunctionInfo finfo3(std::string("/"), std::string("除法运算符"), std::string("x/y"), { p7,p8 }, true, 5, FcnCombine::Left, 2);
	functionlist.emplace("/", finfo3);


	Parameter p9(0, "x", "角度（弧度制）", "none", "error");
	FunctionInfo finfo4(std::string("sin"), std::string("正弦函数"), std::string("sin(x)"), { p9 }, true, 5, FcnCombine::Left, 1);
	functionlist.emplace("sin", finfo4);

	Parameter p10(0, "x", "参数1", "none", "error");
	Parameter p11(1, "y", "参数2", "none", "error");
	FunctionInfo finfo5(std::string("my1"), std::string("my1运算符"), std::string("x*2+y"), { p10,p11 }, false, 0, FcnCombine::Left, 2);
	functionlist.emplace("my1", finfo5);

	//Parameter p12(0, "x", "参数1", "none", "error");
	//Parameter p13(1, "y", "参数2", "none", "error");
	//FunctionInfo finfo6(std::string("my2"), std::string("my2运算符"), std::string("x*2+y*sin(x-y)"), { p12,p13 }, false, 0, FcnCombine::Left, 2);
	//functionlist.emplace("my2", finfo6);

}

//初始化从文件中载入的全局常量列表
void init_constslist()
{
	//Const c1("3.1415926", "PI");//注意：如果这么写就会导致内存的释放
	global_constslist.emplace("PI", Const("3.1415926", "PI"));

}

ExpNodeBase * Transfer_ExpToExpTree(std::string e, int lower, int upper, std::map<std::string, Parameter>* unklist, std::vector<Log>* log)
{
	std::stack<Function*> fcns;//这个是保存函数、运算符和括号的栈
	std::stack<ExpNodeBase*> cab;//计算单元
	std::string exp(e);
	if (e.size() < 1)
	{
		if (log)
			log->emplace_back(LogType::ERROR, "表达式太短");
		return nullptr;
	}
	//submark默认全是0 >=1 表示括号的层数 例如sin((9)) submark[3]=1 submark[4]=2 submark[6]=2 submark[7]=1;
	for (int beg = lower; beg <= upper;)
	{
		//对于-作为单元操作符，我们规定如果之前没有操作数即cab栈的大小为0，那么就是单元的。如果时出现在表达式中的 且是有歧义的操作符或者函数 是肯定会有括号的 而括号则会在递归中解决
		if ((exp[beg] >= '0' && exp[beg] <= '9'))
		{
			int endn = beg + 1;
			for (; exp[endn] == '.' || (exp[endn] >= '0' && exp[endn] <= '9'); ++endn)
				;
			ExpNodeBase *num = new Number(exp.substr(beg, endn - beg));
			if (!num)
			{
				//可能需要调用内存清理函数
				if (log)
					log->emplace_back(LogType::ERROR, "内存分配失败");
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

					fcns.push(nullptr);//左括号
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
						//可能需要调用内存清理函数
						if (log)
							log->emplace_back(LogType::ERROR, "括号匹配失败");
						return nullptr;
					}
					++beg;
				}
				else
				{
					if (isalpha(exp[beg]) || exp[beg] == '+' || exp[beg] == '-' || exp[beg] == '*' || exp[beg] == '/' ||
						exp[beg] == '=')
					{
						//函数的头一个字母 以及变量名字 可以包含开头必须是字母 其余是数字 字母 下划线
						int la = do_trasfer_FcnandPresent(exp, fcns, cab, beg, upper, unklist, log);
						if (la == -1)
						{
							//可能需要调用内存清理函数
							if (log)
								log->emplace_back(LogType::ERROR, "解析子表达式出现问题");
							return nullptr;
						}
						beg = la;
					}
					else
					{
						//可能需要调用内存清理函数
						if (log)
							log->emplace_back(LogType::ERROR, "表达式出现了未定义的字符");
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
		//可能需要调用内存清理函数
		if (log)
			log->emplace_back(LogType::ERROR, "表达式存在错误");
		return nullptr;
	}
}

int do_trasfer_FcnandPresent(const std::string & exp, std::stack<Function*>& fcns, std::stack<ExpNodeBase*>& cab, int & l, int & u, std::map<std::string, Parameter>* unklist, std::vector<Log>* log)
{
	//确定函数的名称：同一个符号意义不同的有歧义函数可以没有括号但是参数数量或参数类型一定是不一样的 但也可以使用-(x)的调用形式 有歧义的符号 ： -
	//例如 -3-1 第一个-时取反一元函数；d第二个-是减法操作，二元
	Function *fptr = nullptr;
	if (exp[l] == '-' && cab.size() == 1)
	{
		//不是括号调用形式 后面可以是数字 或者是函数名
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
		}//其实可以添加一些更长的运算符 诸如逻辑等等 思路是作为函数处理就可以了
		else
		{
			int split = l + 1;//split是变量函数名的后一位 若exp[split]=='('表示函数 否则是变量名
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
					cab.push(fptr);//放入值类型
					return right + 1;
				}
				else
				{
					return -1;
				}


			}
			else
			{
				//优先查找常量这个以后再加入
				//变量名字
				std::string name = exp.substr(l, split - l);
				auto cfind = getConst(name);
				if (cfind)
				{
					//常量
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
		log->emplace_back(LogType::UNKNOWN, "解析函数调用或者代数的时候发生了意外的状况，但计算过程已经终止。");
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
	//注意子表达式也可能包含函数调用
	int prePos = l;//前一个子表达式的终止位置的下一个的下一个 即下一个子表达式的起始位置
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
				//存在子表达式
				int lastkuohao = do_transfer_getLastKuohao(exp, temp, u);
				if (lastkuohao == -1)
				{
					poss.clear();
					return;
				}
				if (lastkuohao == u || (lastkuohao < u && exp[lastkuohao + 1] == ','))
				{
					//找到了一个子表达式
					poss.emplace_back(prePos, lastkuohao);
					l = prePos = lastkuohao + 2;
				}
				else
				{
					//并不式是完整的表达式 所以需要继续计算不要更新prepos
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
		poss.emplace_back(prePos, u);//避免出现只有一个参数，且参数是一个函数调用的表达式的而造成的添加多余参数的问题
}

Function* do_realUnkown_Function(const std::string &fcnname, std::vector<ExpNodeBase*> *real_exp, int totalunk, std::vector<Log> *log)
{
	auto finfo = getFunctionInfo(fcnname);//用户自定义的函数不存在结合性的概念和优先级的概念 都是从左到右
	Function* fptr = nullptr;
	if (finfo)
	{
		//是否是用户自定义函数
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
				log->emplace_back(LogType::ERROR, "无法解析函数的表达式", finfo->_FcnName);
			return nullptr;
		}
	}
	else
	{
		if (log)
			log->emplace_back(LogType::ERROR, "无法读取函数的信息");
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