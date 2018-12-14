#pragma once

#include <vector>
#include <set>
#include <string>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;
struct node
{
	char left;
	string right;
};
// 基本属性类, 定义了求 FIRST FOLLOW 集合的过程, 最大允许的符号数量为 100 个
class Base
{
protected:
	int T;
	node analy_str[100]; //输入文法解析

	set<char> first_set[100];//first集
	set<char> follow_set[100];//follow集
	vector<char> ter_copy; //去$终结符
	vector<char> ter_colt;//终结符
	vector<char> non_colt;//非终结符

	// 按照 拓扑排序求 FIRST 集合的排列顺序, 一般队列的头部是 最好求的. 但是这个算法并不是最优甚至无法避免诸如 E->T T->E 这种情况, 待改进
	void do_toplogically_get_first_order(std::queue<char> &order);
	
public:
	Base() :T(0) {}
	// 初始化 FIRST FOLLOW 集合以及构造输入的产生式序列, 这个是构造函数
	Base(std::istream &stream);
	bool isNotSymbol(char c);
	int get_index(char target);//获得在终结符集合中的下标
	int get_nindex(char target);//获得在非终结符集合中的下标
	void get_first(char target); //得到first集合.
	void get_follow(char target);//得到follow集合
	void inputAndSolve(); //处理得到first和follow
	void display(); //显示
	virtual ~Base(){}
};
