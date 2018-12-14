#pragma once
// 中间代码生成 - 语法树 + 逆波兰式分析
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <string>
#include <map>
#include <memory>
#include <cmath>
#include <cstdlib>

const long double PI = 3.1415926535898l;//PI常数
const long double DEGTORAD = PI / 180;//角度制转弧度制
//函数结合性的枚举类型：注意对于自定义函数不支持自定义结合性：因为我在处理自定义函数式将其视为一个计算节点的数字
enum class FcnCombine { Left, Right };
//代表类型的presentfor的类型
enum class PresentType { UNK, CONST };
//节点的类型 这是为了避免dynamic_cast的开销；
/*
NUM：数字类型；FCN：函数类型（非叶子）；PRE：代表类型（可以视为未知数、非数字类型和常量）；MARTX：矩阵类型
SET：集合类型；
*/
enum class ExpNodeBaseType { NUM, FCN, PRE, MATRIX, SET };
//日志定义的错误信息
enum class LogType { NONE, ERROR, WARNNING, UNKNOWN };
//定义的默认的数值的类型
typedef long double ValueType;
struct ExpNodeBase;
struct ValueTypeBase;



//函数的参数的描述的信息
struct Parameter {
	int _paraID;//参数的索引值 从0开始
	std::string _description;//他们的解释信息
	std::string _name;//参数的标识符
	ExpNodeBase *_condition;//获取代数的值的时候判断是否满足条件（即是否符合定义域，一般而言这个需要返回0或者1，当然了这需要我的解析器zici）
	std::string _errorMsg;//当满足_condition的值为false时的出错消息
	Parameter() = default;
	Parameter(int id, const std::string &name, const std::string &des, const std::string &cond, const std::string error);
};
//日志信息
struct Log {
	LogType _logtype;//日志信息的类型
	std::string _message;//日志的消息
	std::string _exp;//发生错误的表达式信息
	Log(LogType logtype, const std::string &info, const std::string &exp = std::string());
	Log(LogType logtype, const char *msg, const char *exp = nullptr);
	//这个相当于默认的构造函数 默认为没有错误
	Log(LogType logtype = LogType::NONE);
};
//常量描述的信息
struct ExpNodeBase {

	//纯虚函数：返回表达式的有意义的值的指针
	virtual ValueTypeBase* eval(std::vector<Log>* log = nullptr) = 0;
	//动态识别类型：避免RTTI的开销
	virtual ExpNodeBaseType getType();
	ExpNodeBase* pChild;//第一个孩子指针
	ExpNodeBase* pBrother;//同一级别的第一个对象
	ExpNodeBase();
	virtual ~ExpNodeBase();
};
//表达式树的叶子类型，可以认为是数字或者是某些符号
struct ValueTypeBase :public ExpNodeBase
{
	//如果式叶子结点，那么可以继承这些接口，如果不需要，继承默认的版本就可以了
	virtual std::pair<ValueType, Log> asLongDouble() const;//转换为数值类型
	virtual std::pair<ValueTypeBase*, Log> opt_add(ValueTypeBase* r);//加法
	virtual std::pair<ValueTypeBase*, Log> opt_divide(ValueTypeBase* r);//减法
	virtual std::pair<ValueTypeBase*, Log> opt_subtract(ValueTypeBase* r);//乘法
	virtual std::pair<ValueTypeBase*, Log> opt_multiply(ValueTypeBase* r);//除法
	virtual std::pair<ValueTypeBase*, Log> opt_reverse();//求逆元
	virtual ~ValueTypeBase();
	virtual ValueTypeBase *clone() = 0;//这个时克隆一个指针 会进行深拷贝，一次解决内存释放的问题
	virtual void print(std::ostream &os)=0; // 给控制台输出
	
};
//函数表达式类：用来描述函数的表达式的要求，比如操作数对象要求，参数数量等
class FunctionInfo {
private:

public:
	std::string _expression;//函数的解析式
	std::string _FcnName;//函数的显示名称
	std::string _displayname;//显示的名称
	std::map<std::string, Parameter> _map_nameToInfo;//参数名字和到参数信息的映射
	bool _isbuiltin;//是否是内置的表达式
	int _priority;//函数的优先级
	FcnCombine _combine;//函数的结合性
	int _unkcount;//未知数的的数量
	FunctionInfo(const std::string &fcnname, const std::string &disname, const std::string &exp, \
		std::initializer_list<Parameter> parainit, bool builtin, int prior, FcnCombine comb, int unkcount);
};


struct Const {
	std::string _description;//常量的描述信息
	std::string _exp;//常量的表达式
	ValueTypeBase *_value = nullptr;//计算得到的表达式的值

	//这个是可以从表达式中载入的常量计算式 这样的话就需要析构函数
	Const(const std::string &exp, const std::string &des = std::string());
	Const(ValueTypeBase *restree, const std::string &des = std::string());
	~Const();
	ValueTypeBase *getValueExp();
	//一定要深拷贝
	Const(const Const &rhs);
	Const &operator=(const Const &rhs);
};
//全局函数表
static std::map<std::string, FunctionInfo> functionlist;
static std::map<std::string, Const> global_constslist;//这个保存全局的常量列表

//这个是初始化的全局函数列表
void init_functionlist();
//初始化从文件中载入的全局常量列表
void init_constslist();

//表达式树的基本类型：拥有一个长子儿子节点和第一个同级的节点两个指针域：即树的双亲表示法



//从ValueTypeBase派生的叶子类型
class Number :public ValueTypeBase
{
private:
	ValueType _val = 0.0;
	void strTonNum(const std::string &s);
public:
	Number(const std::string &s);
	Number(const ValueType &v);
	ValueTypeBase* eval(std::vector<Log>* log) override;
	ExpNodeBaseType getType() override;
	std::pair<ValueType, Log> asLongDouble() const override;
	std::pair<ValueTypeBase*, Log> opt_add(ValueTypeBase* r) override;
	std::pair<ValueTypeBase*, Log> opt_subtract(ValueTypeBase* r) override;
	std::pair<ValueTypeBase*, Log> opt_multiply(ValueTypeBase* r) override;
	std::pair<ValueTypeBase*, Log> opt_divide(ValueTypeBase* r) override;
	std::pair<ValueTypeBase*, Log> opt_reverse() override;
	// 输出
	void print(std::ostream &os) override;
	//析构函数 但是没有什么可以释放的
	virtual ~Number();
	virtual Number *clone();
};

//这个是“代表”类型，“代表”类型可以代表数字、表达式树的根节点和其他可以存在于树上的类型
class Present :public ExpNodeBase {

public:
	int _id;//这个式当type=UNK的时候使用，表示的是未知数在函数的编号，从0开始的
	PresentType _type;
	ExpNodeBase* _presentfor;//指代的指针
	Present(PresentType type, ExpNodeBase *presentfor = nullptr, int id = 0);
	ExpNodeBaseType getType() override;
	ValueTypeBase* eval(std::vector<Log>* log) override;
	virtual ~Present() override;
};

//函数、操作符类型
class Function :public ExpNodeBase {
private:
	//设置函数的信息指针
	void do_setFcnPtr();
protected:
	FunctionInfo* fcnPtr;//函数的信息指针
	int realUnknown;//函数解析过程中的实际的参数 函数的实际的参数、参数类型决定于函数的计算行为
	std::string fcnID;//函数的唯一的标识名称的ID
	ValueTypeBase* result = nullptr;//用来保存计算的结果，这是一个堆上的内容，注意要释放。之所以这么做是因为需要保持多态的性质
public:
	ExpNodeBaseType getType() override;
	FunctionInfo *getFcnExpPtr();
	Function(const std::string &fn);
	Function(const char c);
	virtual bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* = nullptr) = 0;//这个无论是否是自定义函数都需要这个
	virtual bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>* = nullptr) = 0;//操作数需要这个 使用栈模式的
	virtual ~Function();//因为UserFuntion的result指针和内置函数的是公用的 所以就需要下放释放操作，result的指针只由builtinfunction析构释放。因为userfunction和builtfunction通过fcnexpr关联

};
//用户定义的函数
/*
一般形式为fcn(x,y,x+z)这都是可以的，算法在执行的过程中，将会将函数的解析式进行计算然后保存在fcnExpr中，最后带入实参
*/
class UserFunction :public Function
{
private:
	ExpNodeBase *fcnExpr;//实际的函数的解析式 当不是内置函数的时候需要解析；内置则不管
	//构造树的辅助函数：带入函数的解析式的表达式树 修改present的未知数类型的present
	bool do_trasfer_presentRealParameter(std::vector<ExpNodeBase*> &real, ExpNodeBase *exptree, std::vector<Log> *log = nullptr);
	bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>*);//仅仅定义 不需要
public:
	UserFunction(const std::string &fcnname);
	ValueTypeBase* eval(std::vector<Log>* log) override;

	bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* log);
	virtual ~UserFunction();
};
//内置的函数的定义；与用户的区别是不需要解析式，直接使用指针保存参数信息
class BuiltInFunction :public Function {
private:
	ValueTypeBase* result = nullptr;//用来保存计算的结果，这是一个堆上的内容，注意要释放。之所以这么做是因为需要保持多态的性质
protected:
public:
	BuiltInFunction(const std::string &fcnid);
	BuiltInFunction(char c);
	ValueTypeBase* eval(std::vector<Log>* log) override;
	//这个主要用股userfunction的解析式的带入形参的过程
	bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* log);
	//这个主要用于解析表达式的时候，用的是栈
	bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>* log);
	virtual ~BuiltInFunction();
	virtual void setResultPtr(ValueTypeBase *res);
};


//根据表达式串e[w(lower),w[upper]，转换为表达式树。返回一个指针和日志信息。如果表达式是一个带有参数的表达式，那么请指定实参列表
ExpNodeBase * Transfer_ExpToExpTree(std::string e, int lower, int upper, std::map<std::string, Parameter>* unklist=nullptr, std::vector<Log>* log=nullptr);

//解析函数和未知数的辅助函数 表达式，保存操作符的栈，保存数字、函数的栈，上界，下界，未知数列表
int do_trasfer_FcnandPresent(const std::string & exp, std::stack<Function*>& fcns, std::stack<ExpNodeBase*>& cab, int & l, int & u, std::map<std::string, Parameter>* unklist, std::vector<Log>* log);
//获取子表达式的结尾位置索引   beg表示的是左括号位置 -1表示失败
int do_transfer_getLastKuohao(const std::string &exp, int beg, int u);
//拆分子表达式项（用于函数解析）如果失败：清空poss 例如 fcn(x+3,u+2) 函数返回 x+3   u+2的起点终点坐标 l是左括号+1 u是右括号的位置-1
void do_transfer_spiltFunctionSubExp(const std::string &exp, int l, int u, std::vector<std::pair<int, int>> &poss);
Function* do_realUnkown_Function(const std::string &fcnname, std::vector<ExpNodeBase*> *real_exp, int totalunk, std::vector<Log> *log = nullptr);
//计算内置的函数 需要指定函数的信息，未知数的实际数量，参数（以长子节点开始，一次访问他的兄弟，默认从左往右计算）
ValueTypeBase* BuiltInFuntionCalc(const FunctionInfo *fptr, int realunk, ExpNodeBase *paras, std::vector<Log> *log = nullptr);
//获取函数的信息返回FunctionInfo的指针，如果id不存在于全局函数表中，那么返回nullptr
FunctionInfo *getFunctionInfo(const std::string &id);

Const *getConst(const std::string &constname);


class Expression
{
private:
	std::string _fcnname_or_expr;
	ExpNodeBase *_expTree = nullptr;
	FunctionInfo *_fptr = nullptr;
	ValueTypeBase *_result = nullptr;
	bool _isfcn;
	bool _isunk;
	//支持带有参数以及指定函数名的表达式
	void do_specify_unknown(std::vector<ExpNodeBase*> *real_para, std::vector<Log>* log);
public:
	//指定函数名或者是表达式
	Expression(const std::string &fcnname_or_expr, bool isunk, bool isfcn);
	//开始计算
	ValueTypeBase *Eval(std::vector<ExpNodeBase*> *real_para = nullptr, std::vector<Log>* log = nullptr);
	~Expression();
};


