#include "pch.h"
#include "tableStack.h"

// 生成 LL(1) 分析表, 隐含求 SELECT 的过程
void TableStack::get_table()
{
	for (int i = 0; i < T; i++)
	{
		char tmp = analy_str[i].right[0]; // 每一个产生式右部
		if (!isNotSymbol(tmp))
		{
			// 终结符
			if (tmp != '$')
				tableMap[get_index(analy_str[i].left)][get_nindex(tmp)] = i; // 非空终结符（行号），已经对应的终结符号的位置（列号）
			if (tmp == '$')
			{
				// 如果右部是的第一个符号是空字符(可以推出空船而且本身也是空串), 遍历所有的以这个产生式的左部的所有 FOLLOW 集合, 加入到 LL1 分析表中
				set<char>::iterator  it;
				for (it = follow_set[get_index(analy_str[i].left)].begin(); it != follow_set[get_index(analy_str[i].left)].end(); it++)
				{
					tableMap[get_index(analy_str[i].left)][get_nindex(*it)] = i;
				}
			}
		}
		else
		{
			set<char>::iterator ti;
			// 本身不是空串也推不出空串, select 使用的 FIRST 集合
			for (ti = first_set[get_index(tmp)].begin(); ti != first_set[get_index(tmp)].end(); ti++)
			{
				tableMap[get_index(analy_str[i].left)][get_nindex(*ti)] = i;
			}
			if (first_set[get_index(tmp)].count('$') != 0)
			{
				// 本身不是空串, 但是能够推出空串. 上面的循环已经可以设置了 FIRST . 这里只要弄 FOLLOW
				set<char>::iterator  it;
				for (it = follow_set[get_index(analy_str[i].left)].begin(); it != follow_set[get_index(analy_str[i].left)].end(); it++)
				{
					tableMap[get_index(analy_str[i].left)][get_nindex(*it)] = i;
				}
			}
		}
	}
}
// LL(1) 分析

void TableStack::analyExp(string s)
{
	// 符号串压入
	for (int i = s.length() - 1; i >= 0; i--)
		left_any.push_back(s[i]);
	// 压入最后的结尾字符作为符号串的开头
	to_any.push_back('#');
	// 压入文法的起始字符
	to_any.push_back(non_colt[0]);


	while (left_any.size() > 0)
	{
		//cout<<"分析栈：";
		string outs = "";
		for (int i = 0; i < to_any.size(); i++)
			outs += to_any[i]; // 送入当前分析的过程需要的符号送入分析栈
		cout << setw(15) << outs;

		//cout<<"剩余输入串：";
		outs = "";
		for (int i = left_any.size() - 1; i >= 0; i--)
			outs += left_any[i]; // 余留字符串送入栈
		cout << setw(15) <<outs;

		char char1 = to_any[to_any.size() - 1]; // 获取分析栈的字符
		char char2 = left_any[left_any.size() - 1];  // 获取余留字符的指针指向的字符(一般就是最后一个)
		if (char1 == char2 && char1 == '#') // 接受
		{
			cout << setw(15) << "Accepted!" << endl;
			return;
		}
		if (char1 == char2)
		{
			to_any.pop_back();
			left_any.pop_back();
			cout << setw(15) << char1 << "匹配" << endl;
		}
		else if (tableMap[get_index(char1)][get_nindex(char2)] != -1) // tableMap 默认用 -1 初始化
		{
			// 获取指定的产生式编号
			int tg = tableMap[get_index(char1)][get_nindex(char2)];
			to_any.pop_back(); // 分析栈栈顶元素弹出

			if (analy_str[tg].right != "$") // 如果这个产生式推出的不是空串
			{
				for (int i = analy_str[tg].right.length() - 1; i >= 0; i--)
					to_any.push_back(analy_str[tg].right[i]); // 将当前分析栈替换为这个所有符号, 注意需要是逆序进入分析栈
			}
			// 如果是空就无所谓了
			//cout<<"推导："<<analy_str[tg].right<<endl; 
			cout << setw(15) << analy_str[tg].right << endl;
		}
		else
		{
			// 不可能的句子
			cout << setw(15) << "Error!" << endl;
			return;
		}
	}
}

void TableStack::print_out()
{
	//table
	for (int i = 0; i < ter_copy.size(); i++)
	{
		cout << setw(10) << ter_copy[i];
	}
	cout << endl;
	for (int i = 0; i < non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		for (int j = 0; j < ter_copy.size(); j++)
		{
			if (tableMap[i][j] == -1)
				cout << setw(10) << "error";
			else
				cout << setw(10) << analy_str[tableMap[i][j]].right;

		}
		cout << endl;
	}
}

void TableStack::getAns()
{
	inputAndSolve();
	display();
	get_table();
	print_out();
	//栈匹配
	string ss;
	cout << "请输入符号串：(以 # 结束)" << endl;
	cin >> ss;
	cout << setw(15) << "分析栈" << setw(15) << "剩余输入串" << setw(15) << "产生式右部" << endl;
	analyExp(ss);

}