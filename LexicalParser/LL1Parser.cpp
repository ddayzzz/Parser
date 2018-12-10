// LL1Parser.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
	//LR0 或者 SLR
	LR0SLR1Parser p;
	p.demo();

	return 0;
}