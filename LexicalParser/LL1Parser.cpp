// LL1Parser.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include <vector>
#include "TableStack.h"
#include "lr_parser.h"
int main()
{
	// LL1
	/*TableStack res;
	res.getAns();*/
	//res.getAns();
	//LR0 ���� SLR
	LR0SLR1Parser p;
	p.demo();

	return 0;
}