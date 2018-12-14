
#include "lg_base.h"
#include <iostream>
using namespace std;

void Base::do_toplogically_get_first_order(std::queue<char>& order)
{
	int not_syms_size = this->non_colt.size();
	// 拓扑排序, 建立所有符号的有向图模型
	int **graph = new int*[not_syms_size];
	for (int i = 0; i < not_syms_size; ++i)
	{
		graph[i] = new int[not_syms_size];
		memset(graph[i], 0, not_syms_size * sizeof(int));
	}
	// 保存入度信息
	std::vector<int> in_degree(not_syms_size);
	// 遍历所有的产生式
	for (int i = 0; i < T; ++i)
	{
		auto &&node_p = analy_str[i];
		if (this->isNotSymbol(node_p.right[0]))
		{
			int w = this->get_index(node_p.right[0]);
			int v = this->get_index(node_p.left);
			// 建立从 w->v 的有向图路径
			graph[w][v] += 1; // 例如 产生式 A->B 我认为从 B->A 存在路径依赖
		}
	}
	// graph 保存从产生式右部到左部非终结符的连接信息, 保存的是非终结符的在 non_colt 的位置
	std::queue<int> p_lefts;
	// 查找零入度的点, 也就是产生式右部第一个字符为终结符
	for (int i = 0; i < not_syms_size; ++i)
	{
		int j;
		int counter = 0; // 计算j的入读是多少
		for (j = 0; j < not_syms_size; ++j)
		{
			if (i == j)
				continue; // 忽略左递归的形式
			// 右部第一个 -> 左部符号 i->j
			if (graph[j][i] > 0)
				counter += 1;
		}
		// 登记入度
		in_degree[i] = counter;
		if (counter <= 0)
		{
			// 右部第一个是终结符
			p_lefts.push(i);
		}
	}
	// 开始拓扑排序 Khan 算法
	while (!p_lefts.empty())//只要保存入度为0的队列为空那么久循环
	{

		int v = p_lefts.front(); p_lefts.pop();//选择第一个开始的入度为0的顶点  这里我们以对头作为开始
		order.push(this->non_colt[v]);//先把结果压入, 是相关的非终结符
		std::vector<int> paths;

		for (int i = 0; i < not_syms_size; ++i)
		{
			if (graph[v][i] >= 1)
			{
				// 有路径连接
				if (0 == --in_degree[i])//注意要减去已经找到的顶点 如果目标的终点的入度为0了那么这个顶点就是可以入队的
					p_lefts.push(i);
			}
		}
	}
	for (int i = 0; i < not_syms_size; ++i)
		delete[] graph[i];
}



Base::Base(std::istream & stream)
{
	string s;
	// cout << "输入的产生式的个数：" << endl;
	stream >> T; // 输入的P个数
	for (int index = 0; index < T; index++)
	{
		stream >> s;
		string temp = "";
		// 构造新的字符串
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}
		analy_str[index].left = temp[0]; // 产生式左部的字符串
		for (int i = 3; i < temp.length(); i++) // 越过 X-> 三个字符
			analy_str[index].right += temp[i];

		// 取每一个产生式
		for (int i = 0; i < temp.length(); i++)
		{
			// 注意忽略掉产生除 V 的字符
			if (temp[i] != '-' && temp[i] != '>')
			{
				// 判断是否是非终结符
				if (isNotSymbol(temp[i]))
				{
					// 这个标记用来确定产生式是否是新的非终结符, 是则新建一个非终结符;否则添加新的非终结符
					int flag = 0;
					for (int j = 0; j < non_colt.size(); j++)
					{
						if (non_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					// 如果没有或者是 V_N 为空
					if (!flag)
						non_colt.push_back(temp[i]); // 将符号添加到非终结符列表

				}
				else
				{
					// 终结符
					int flag = 0;
					for (int j = 0; j < ter_colt.size(); j++)
					{
						// 同理, 防止重复添加终结符
						if (ter_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					if (!flag)
						ter_colt.push_back(temp[i]); // 添加到终结符列表
				}
			}
		}

	}
	// 后面是用于求 FIRST FOLLOW 表
	ter_colt.push_back('#');
	// FIRST
	std::queue<char> first_order;
	do_toplogically_get_first_order(first_order);
	while (!first_order.empty())
	{
		char sym = first_order.front();
		first_order.pop();
		get_first(sym);
	}
	// FOLLOW
	for (int i = 0; i < non_colt.size(); i++)
	{
		if (i == 0)
			follow_set[0].insert('#'); // 第一个用 #
		get_follow(non_colt[i]);
	}

	for (int i = 0; i < ter_colt.size(); i++)
	{
		if (ter_colt[i] != '$')
			ter_copy.push_back(ter_colt[i]);
	}
}

// 是否是非终结符
bool Base::isNotSymbol(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}
// 获取非终结符的位置(在 FIRST, 位置是依据于产生式的编号)
int Base::get_index(char target)
{
	for (int i = 0; i < non_colt.size(); i++)
	{
		if (target == non_colt[i])
			return i;
	}
	return -1;
}
// 获取终结符的位置(不包含$)
int Base::get_nindex(char target)
{
	for (int i = 0; i < ter_copy.size(); i++)
	{
		if (target == ter_copy[i])
			return i;
	}
	return -1;
}
// 求 FIRST
void Base::get_first(char target)
{
	int tag = 0;
	int flag = 0;
	// 遍历所有的产生式
	for (int i = 0; i < T; i++)
	{
		if (analy_str[i].left == target) //匹配产生式左部
		{
			if (!isNotSymbol(analy_str[i].right[0]))//对于终结符，直接加入 FIRST
			{
				first_set[get_index(target)].insert(analy_str[i].right[0]); // 插入产生式右部终结符进入对应的 FIRST 集合
			}
			else
			{
				// 搜索这个产生式右部第一个非终结字符
				for (int j = 0; j < analy_str[i].right.length(); j++)
				{
					if (!isNotSymbol(analy_str[i].right[j])) // 终结符结束, 这个是右部最后一个的终结字符
					{
						first_set[get_index(target)].insert(analy_str[i].right[j]);
						break;
					}
					if (analy_str[i].right[j] == analy_str[i].left)
					{
						bool flag = false;
						// 类似于左递归的情形, 如果存在 T->$才继续, 否则直接break, 会通过相同的非左递归的式求出
						for (int k = 0; k < T; ++k)
						{
							if (analy_str[k].left == analy_str[i].left && analy_str[k].right[0] == '$')
							{
								flag = true;
								break;
							}
						}
						if (flag)
							continue;
						else
							break;
					}
					// 非终结符情况
					// 从 right[0...j] 的位置还没有终结符. 需要划分子问题: 假设[0...j-1]已经求出 first, 则从right[0...j]
					// 的非终结符是 right[0...j-1] 并 getFrist(right[j])
					get_first(analy_str[i].right[j]);//递归
					// 目前求出了 right[0...j] 的终结符
					// 集合迭代器, 用于扫描以当前非终结符 right[j] 的所有的的产生式的左部对应的集合
					// 形如 X->right[j] 的 产生式的编号, 利用这个编号求 FIRST[X], 然后并入我要求的字符的 FIRST
					set<char>::iterator it;
					int u = get_index(analy_str[i].right[j]);
					for (it = first_set[u].begin(); it != first_set[u].end(); it++)
					{
						// 如果产生式子包含空串
						if (*it == '$')
							flag = 1;
						else
							first_set[get_index(target)].insert(*it);//将FIRST(X)中的非$就加入FIRST(X)
					}
					if (flag == 0)
						break; // 将全部的非空符号加入到了 FIRST
					else
					{
						tag += flag; // 包含空 epsilon
						flag = 0; // 清空, 忽略这个 空字符继续求后面的符号的 FIRST(如果有的话)
					}
				}
				if (tag == analy_str[i].right.length()) // 所有右部FIRST(Y) 都有$, 将$加入 FIRST(X) 中. 
					first_set[get_index(target)].insert('$');
			}
		}
	}
}
// 求 FOLLOW
void Base::get_follow(char target)
{
	for (int i = 0; i < T; i++)
	{
		int index = -1;
		// 产生式右部长度
		int len = analy_str[i].right.length();
		// 寻找第一个匹配要求的左部的产生式编号
		for (int j = 0; j < len; j++)
		{
			if (analy_str[i].right[j] == target)
			{
				index = j;
				break;
			}
		}
		// 存在一产生式
		if (index != -1 && index < len - 1)
		{
			char nxt = analy_str[i].right[index + 1];
			if (!isNotSymbol(nxt))
			{
				// 情况: 与他连接的是终结符, 直接加入到 FOLLOW
				follow_set[get_index(target)].insert(nxt);
			}
			else
			{
				// 情况: 下一个是非终结符
				int isExt = 0;
				set<char>::iterator it;
				// 子情况: 非1+非2 FIRST[非2] 属于 FOLLOW[非1]
				for (it = first_set[get_index(nxt)].begin(); it != first_set[get_index(nxt)].end(); it++)
				{
					if (*it == '$')
						isExt = 1;
					else
						follow_set[get_index(target)].insert(*it);
				}

				if (isExt && analy_str[i].left != target)
				{
					// 情况: 非1->非2. 如果存在一个空字符以及产生式左部 != 右部非终结 FOLLOW(非1) 属于 FOLLOW(非2)
					get_follow(analy_str[i].left);
					set<char>::iterator it;
					// tmp 非1
					char tmp = analy_str[i].left;
					for (it = follow_set[get_index(tmp)].begin(); it != follow_set[get_index(tmp)].end(); it++)
						follow_set[get_index(target)].insert(*it);
				}
			}
		}
		// 需要进行更新，防止漏过。
		else if (index != -1 && index == len - 1 && target != analy_str[i].left)
		{
			get_follow(analy_str[i].left);
			set<char>::iterator it;
			char tmp = analy_str[i].left;
			for (it = follow_set[get_index(tmp)].begin(); it != follow_set[get_index(tmp)].end(); it++)
				follow_set[get_index(target)].insert(*it);
		}

	}
}
// 根据产生式新建对象
void Base::inputAndSolve()
{
	string s;
	cout << "输入的产生式的个数：" << endl;
	cin >> T; // 输入的P个数
	for (int index = 0; index < T; index++)
	{
		cin >> s;
		string temp = "";
		// 构造新的字符串
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}
		analy_str[index].left = temp[0]; // 产生式左部的字符串
		for (int i = 3; i < temp.length(); i++) // 越过 X-> 三个字符
			analy_str[index].right += temp[i];

		// 取每一个产生式
		for (int i = 0; i < temp.length(); i++)
		{
			// 注意忽略掉产生除 V 的字符
			if (temp[i] != '-' && temp[i] != '>')
			{
				// 判断是否是非终结符
				if (isNotSymbol(temp[i]))
				{
					// 这个标记用来确定产生式是否是新的非终结符, 是则新建一个非终结符;否则添加新的非终结符
					int flag = 0;
					for (int j = 0; j < non_colt.size(); j++)
					{
						if (non_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					// 如果没有或者是 V_N 为空
					if (!flag)
						non_colt.push_back(temp[i]); // 将符号添加到非终结符列表

				}
				else
				{
					// 终结符
					int flag = 0;
					for (int j = 0; j < ter_colt.size(); j++)
					{
						// 同理, 防止重复添加终结符
						if (ter_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					if (!flag)
						ter_colt.push_back(temp[i]); // 添加到终结符列表
				}
			}
		}

	}
	// 后面是用于求 FIRST FOLLOW 表
	ter_colt.push_back('#');
	// FIRST
	for (int i = 0; i < non_colt.size(); i++)
	{
		get_first(non_colt[i]);
	}
	// FOLLOW
	for (int i = 0; i < non_colt.size(); i++)
	{
		if (i == 0)
			follow_set[0].insert('#'); // 第一个用 #
		get_follow(non_colt[i]);
	}

	for (int i = 0; i < ter_colt.size(); i++)
	{
		if (ter_colt[i] != '$')
			ter_copy.push_back(ter_colt[i]);
	}
}

void Base::display()
{
	cout << "FIRST集合" << endl;
	for (int i = 0; i < non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = first_set[i].begin(); it != first_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}

	cout << "FOLLOW集合" << endl;
	for (int i = 0; i < non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = follow_set[i].begin(); it != follow_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}
}