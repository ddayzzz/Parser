#pragma once
// �м�������� - �﷨�� + �沨��ʽ����
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <string>
#include <map>
#include <memory>
#include <cmath>
#include <cstdlib>

const long double PI = 3.1415926535898l;//PI����
const long double DEGTORAD = PI / 180;//�Ƕ���ת������
//��������Ե�ö�����ͣ�ע������Զ��庯����֧���Զ������ԣ���Ϊ���ڴ����Զ��庯��ʽ������Ϊһ������ڵ������
enum class FcnCombine { Left, Right };
//�������͵�presentfor������
enum class PresentType { UNK, CONST };
//�ڵ������ ����Ϊ�˱���dynamic_cast�Ŀ�����
/*
NUM���������ͣ�FCN���������ͣ���Ҷ�ӣ���PRE���������ͣ�������Ϊδ֪�������������ͺͳ�������MARTX����������
SET���������ͣ�
*/
enum class ExpNodeBaseType { NUM, FCN, PRE, MATRIX, SET };
//��־����Ĵ�����Ϣ
enum class LogType { NONE, ERROR, WARNNING, UNKNOWN };
//�����Ĭ�ϵ���ֵ������
typedef long double ValueType;
struct ExpNodeBase;
struct ValueTypeBase;



//�����Ĳ�������������Ϣ
struct Parameter {
	int _paraID;//����������ֵ ��0��ʼ
	std::string _description;//���ǵĽ�����Ϣ
	std::string _name;//�����ı�ʶ��
	ExpNodeBase *_condition;//��ȡ������ֵ��ʱ���ж��Ƿ��������������Ƿ���϶�����һ����������Ҫ����0����1����Ȼ������Ҫ�ҵĽ�����zici��
	std::string _errorMsg;//������_condition��ֵΪfalseʱ�ĳ�����Ϣ
	Parameter() = default;
	Parameter(int id, const std::string &name, const std::string &des, const std::string &cond, const std::string error);
};
//��־��Ϣ
struct Log {
	LogType _logtype;//��־��Ϣ������
	std::string _message;//��־����Ϣ
	std::string _exp;//��������ı��ʽ��Ϣ
	Log(LogType logtype, const std::string &info, const std::string &exp = std::string());
	Log(LogType logtype, const char *msg, const char *exp = nullptr);
	//����൱��Ĭ�ϵĹ��캯�� Ĭ��Ϊû�д���
	Log(LogType logtype = LogType::NONE);
};
//������������Ϣ
struct ExpNodeBase {

	//���麯�������ر��ʽ���������ֵ��ָ��
	virtual ValueTypeBase* eval(std::vector<Log>* log = nullptr) = 0;
	//��̬ʶ�����ͣ�����RTTI�Ŀ���
	virtual ExpNodeBaseType getType();
	ExpNodeBase* pChild;//��һ������ָ��
	ExpNodeBase* pBrother;//ͬһ����ĵ�һ������
	ExpNodeBase();
	virtual ~ExpNodeBase();
};
//���ʽ����Ҷ�����ͣ�������Ϊ�����ֻ�����ĳЩ����
struct ValueTypeBase :public ExpNodeBase
{
	//���ʽҶ�ӽ�㣬��ô���Լ̳���Щ�ӿڣ��������Ҫ���̳�Ĭ�ϵİ汾�Ϳ�����
	virtual std::pair<ValueType, Log> asLongDouble() const;//ת��Ϊ��ֵ����
	virtual std::pair<ValueTypeBase*, Log> opt_add(ValueTypeBase* r);//�ӷ�
	virtual std::pair<ValueTypeBase*, Log> opt_divide(ValueTypeBase* r);//����
	virtual std::pair<ValueTypeBase*, Log> opt_subtract(ValueTypeBase* r);//�˷�
	virtual std::pair<ValueTypeBase*, Log> opt_multiply(ValueTypeBase* r);//����
	virtual std::pair<ValueTypeBase*, Log> opt_reverse();//����Ԫ
	virtual ~ValueTypeBase();
	virtual ValueTypeBase *clone() = 0;//���ʱ��¡һ��ָ�� ����������һ�ν���ڴ��ͷŵ�����
	virtual void print(std::ostream &os)=0; // ������̨���
	
};
//�������ʽ�ࣺ�������������ı��ʽ��Ҫ�󣬱������������Ҫ�󣬲���������
class FunctionInfo {
private:

public:
	std::string _expression;//�����Ľ���ʽ
	std::string _FcnName;//��������ʾ����
	std::string _displayname;//��ʾ������
	std::map<std::string, Parameter> _map_nameToInfo;//�������ֺ͵�������Ϣ��ӳ��
	bool _isbuiltin;//�Ƿ������õı��ʽ
	int _priority;//���������ȼ�
	FcnCombine _combine;//�����Ľ����
	int _unkcount;//δ֪���ĵ�����
	FunctionInfo(const std::string &fcnname, const std::string &disname, const std::string &exp, \
		std::initializer_list<Parameter> parainit, bool builtin, int prior, FcnCombine comb, int unkcount);
};


struct Const {
	std::string _description;//������������Ϣ
	std::string _exp;//�����ı��ʽ
	ValueTypeBase *_value = nullptr;//����õ��ı��ʽ��ֵ

	//����ǿ��Դӱ��ʽ������ĳ�������ʽ �����Ļ�����Ҫ��������
	Const(const std::string &exp, const std::string &des = std::string());
	Const(ValueTypeBase *restree, const std::string &des = std::string());
	~Const();
	ValueTypeBase *getValueExp();
	//һ��Ҫ���
	Const(const Const &rhs);
	Const &operator=(const Const &rhs);
};
//ȫ�ֺ�����
static std::map<std::string, FunctionInfo> functionlist;
static std::map<std::string, Const> global_constslist;//�������ȫ�ֵĳ����б�

//����ǳ�ʼ����ȫ�ֺ����б�
void init_functionlist();
//��ʼ�����ļ��������ȫ�ֳ����б�
void init_constslist();

//���ʽ���Ļ������ͣ�ӵ��һ�����Ӷ��ӽڵ�͵�һ��ͬ���Ľڵ�����ָ���򣺼�����˫�ױ�ʾ��



//��ValueTypeBase������Ҷ������
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
	// ���
	void print(std::ostream &os) override;
	//�������� ����û��ʲô�����ͷŵ�
	virtual ~Number();
	virtual Number *clone();
};

//����ǡ��������ͣ����������Ϳ��Դ������֡����ʽ���ĸ��ڵ���������Դ��������ϵ�����
class Present :public ExpNodeBase {

public:
	int _id;//���ʽ��type=UNK��ʱ��ʹ�ã���ʾ����δ֪���ں����ı�ţ���0��ʼ��
	PresentType _type;
	ExpNodeBase* _presentfor;//ָ����ָ��
	Present(PresentType type, ExpNodeBase *presentfor = nullptr, int id = 0);
	ExpNodeBaseType getType() override;
	ValueTypeBase* eval(std::vector<Log>* log) override;
	virtual ~Present() override;
};

//����������������
class Function :public ExpNodeBase {
private:
	//���ú�������Ϣָ��
	void do_setFcnPtr();
protected:
	FunctionInfo* fcnPtr;//��������Ϣָ��
	int realUnknown;//�������������е�ʵ�ʵĲ��� ������ʵ�ʵĲ������������;����ں����ļ�����Ϊ
	std::string fcnID;//������Ψһ�ı�ʶ���Ƶ�ID
	ValueTypeBase* result = nullptr;//�����������Ľ��������һ�����ϵ����ݣ�ע��Ҫ�ͷš�֮������ô������Ϊ��Ҫ���ֶ�̬������
public:
	ExpNodeBaseType getType() override;
	FunctionInfo *getFcnExpPtr();
	Function(const std::string &fn);
	Function(const char c);
	virtual bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* = nullptr) = 0;//��������Ƿ����Զ��庯������Ҫ���
	virtual bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>* = nullptr) = 0;//��������Ҫ��� ʹ��ջģʽ��
	virtual ~Function();//��ΪUserFuntion��resultָ������ú������ǹ��õ� ���Ծ���Ҫ�·��ͷŲ�����result��ָ��ֻ��builtinfunction�����ͷš���Ϊuserfunction��builtfunctionͨ��fcnexpr����

};
//�û�����ĺ���
/*
һ����ʽΪfcn(x,y,x+z)�ⶼ�ǿ��Եģ��㷨��ִ�еĹ����У����Ὣ�����Ľ���ʽ���м���Ȼ�󱣴���fcnExpr�У�������ʵ��
*/
class UserFunction :public Function
{
private:
	ExpNodeBase *fcnExpr;//ʵ�ʵĺ����Ľ���ʽ ���������ú�����ʱ����Ҫ�����������򲻹�
	//�������ĸ������������뺯���Ľ���ʽ�ı��ʽ�� �޸�present��δ֪�����͵�present
	bool do_trasfer_presentRealParameter(std::vector<ExpNodeBase*> &real, ExpNodeBase *exptree, std::vector<Log> *log = nullptr);
	bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>*);//�������� ����Ҫ
public:
	UserFunction(const std::string &fcnname);
	ValueTypeBase* eval(std::vector<Log>* log) override;

	bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* log);
	virtual ~UserFunction();
};
//���õĺ����Ķ��壻���û��������ǲ���Ҫ����ʽ��ֱ��ʹ��ָ�뱣�������Ϣ
class BuiltInFunction :public Function {
private:
	ValueTypeBase* result = nullptr;//�����������Ľ��������һ�����ϵ����ݣ�ע��Ҫ�ͷš�֮������ô������Ϊ��Ҫ���ֶ�̬������
protected:
public:
	BuiltInFunction(const std::string &fcnid);
	BuiltInFunction(char c);
	ValueTypeBase* eval(std::vector<Log>* log) override;
	//�����Ҫ�ù�userfunction�Ľ���ʽ�Ĵ����βεĹ���
	bool setParameterPtr(std::vector<ExpNodeBase*> &real, std::vector<Log>* log);
	//�����Ҫ���ڽ������ʽ��ʱ���õ���ջ
	bool setParameterPtr(std::stack<ExpNodeBase*> &sta, std::vector<Log>* log);
	virtual ~BuiltInFunction();
	virtual void setResultPtr(ValueTypeBase *res);
};


//���ݱ��ʽ��e[w(lower),w[upper]��ת��Ϊ���ʽ��������һ��ָ�����־��Ϣ��������ʽ��һ�����в����ı��ʽ����ô��ָ��ʵ���б�
ExpNodeBase * Transfer_ExpToExpTree(std::string e, int lower, int upper, std::map<std::string, Parameter>* unklist=nullptr, std::vector<Log>* log=nullptr);

//����������δ֪���ĸ������� ���ʽ�������������ջ���������֡�������ջ���Ͻ磬�½磬δ֪���б�
int do_trasfer_FcnandPresent(const std::string & exp, std::stack<Function*>& fcns, std::stack<ExpNodeBase*>& cab, int & l, int & u, std::map<std::string, Parameter>* unklist, std::vector<Log>* log);
//��ȡ�ӱ��ʽ�Ľ�βλ������   beg��ʾ����������λ�� -1��ʾʧ��
int do_transfer_getLastKuohao(const std::string &exp, int beg, int u);
//����ӱ��ʽ����ں������������ʧ�ܣ����poss ���� fcn(x+3,u+2) �������� x+3   u+2������յ����� l��������+1 u�������ŵ�λ��-1
void do_transfer_spiltFunctionSubExp(const std::string &exp, int l, int u, std::vector<std::pair<int, int>> &poss);
Function* do_realUnkown_Function(const std::string &fcnname, std::vector<ExpNodeBase*> *real_exp, int totalunk, std::vector<Log> *log = nullptr);
//�������õĺ��� ��Ҫָ����������Ϣ��δ֪����ʵ���������������Գ��ӽڵ㿪ʼ��һ�η��������ֵܣ�Ĭ�ϴ������Ҽ��㣩
ValueTypeBase* BuiltInFuntionCalc(const FunctionInfo *fptr, int realunk, ExpNodeBase *paras, std::vector<Log> *log = nullptr);
//��ȡ��������Ϣ����FunctionInfo��ָ�룬���id��������ȫ�ֺ������У���ô����nullptr
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
	//֧�ִ��в����Լ�ָ���������ı��ʽ
	void do_specify_unknown(std::vector<ExpNodeBase*> *real_para, std::vector<Log>* log);
public:
	//ָ�������������Ǳ��ʽ
	Expression(const std::string &fcnname_or_expr, bool isunk, bool isfcn);
	//��ʼ����
	ValueTypeBase *Eval(std::vector<ExpNodeBase*> *real_para = nullptr, std::vector<Log>* log = nullptr);
	~Expression();
};


