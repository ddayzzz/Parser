#pragma once

#include <vector>
#include <set>
#include <string>
using namespace std;
struct node
{
	char left;
	string right;
};

class Base
{
protected:
	int T;
	node analy_str[100]; //�����ķ�����

	set<char> first_set[100];//first��
	set<char> follow_set[100];//follow��
	vector<char> ter_copy; //ȥ$�ս��
	vector<char> ter_colt;//�ս��
	vector<char> non_colt;//���ս��

public:
	Base() :T(0) {}
	bool isNotSymbol(char c);
	int get_index(char target);//������ս�������е��±�
	int get_nindex(char target);//����ڷ��ս�������е��±�
	void get_first(char target); //�õ�first����.
	void get_follow(char target);//�õ�follow����
	void inputAndSolve(); //����õ�first��follow
	void display(); //��ʾ
	virtual ~Base(){}
};
