// LL1Parser.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
	//LR0 或者 SLR
	/*std::cout << "输入的产生式的个数以及产生式：" << std::endl;
	LR0SLR1Parser p(std::cin);
	p.demo();*/
	// 词法分析
	/*IdentifierTable table;
	LexParser parser;
	std::string ss("int a=5\nint b=3\nint c=a+b");
	std::stringstream src;
	src << ss;
	parser.parse(table, std::cout, src);*/
	// 逆波兰的语法树
	init_functionlist();
	init_constslist();
	std::string s = "sin(PI-PI/2)+6*my1(5,3)";
	std::vector<Log> lo;
	//auto p = Transfer_ExpToExpTree(s, 0, s.size() - 1,nullptr,&lo);
	//auto j = BuiltInFuntionCalc(nullptr, 2, nullptr);
	Expression ep(s, false, false);
	auto j = ep.Eval(nullptr, &lo);
	Number *nn = new Number(2.33);

	//深拷贝解决内存重复释放
	//ExpNodeBase* nj = new Number(j->asLongDouble().first);
	//ExpNodeBase *nnn = new Number(*nn);
	Expression ep2("my1", true, true);
	std::vector<ExpNodeBase*> par{ j->clone(),nn->clone() };
	auto i = ep2.Eval(&par, &lo);
	// 输出
	j->print(std::cout);
	std::cout << std::endl;
	i->print(std::cout);
	ep2.~Expression();
	ep.~Expression();
	return 0;

	return 0;
}