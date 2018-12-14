// LL1Parser.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include <sstream>
#include <vector>
#include "lex_parser.h"
#include "lr_parser.h"
#include "inter_grammerTree.h"
int main()
{
	// LL1
	/*TableStack res;
	res.getAns();*/
	//res.getAns();
	//LR0 ���� SLR
	/*std::cout << "����Ĳ���ʽ�ĸ����Լ�����ʽ��" << std::endl;
	LR0SLR1Parser p(std::cin);
	p.demo();*/
	// �ʷ�����
	/*IdentifierTable table;
	LexParser parser;
	std::string ss("int a=5\nint b=3\nint c=a+b");
	std::stringstream src;
	src << ss;
	parser.parse(table, std::cout, src);*/
	// �沨�����﷨��
	init_functionlist();
	init_constslist();
	std::string s = "sin(PI-PI/2)+6*my1(5,3)";
	std::vector<Log> lo;
	//auto p = Transfer_ExpToExpTree(s, 0, s.size() - 1,nullptr,&lo);
	//auto j = BuiltInFuntionCalc(nullptr, 2, nullptr);
	Expression ep(s, false, false);
	auto j = ep.Eval(nullptr, &lo);
	Number *nn = new Number(2.33);

	//�������ڴ��ظ��ͷ�
	//ExpNodeBase* nj = new Number(j->asLongDouble().first);
	//ExpNodeBase *nnn = new Number(*nn);
	Expression ep2("my1", true, true);
	std::vector<ExpNodeBase*> par{ j->clone(),nn->clone() };
	auto i = ep2.Eval(&par, &lo);
	// ���
	j->print(std::cout);
	std::cout << std::endl;
	i->print(std::cout);
	ep2.~Expression();
	ep.~Expression();
	return 0;

	return 0;
}