#include "lr_parser.h"
Jizu<LRJizuItem> LR0SLR1Parser::do_generateJizu(
	int jizuId,
	const std::list<LRJizuItem> &initialItems)
{
	_jizu_item_marker.clear();
	// 插入默认的产生式子
	Jizu<LRJizuItem> current_jizu = Jizu<LRJizuItem>(jizuId, initialItems);
	// 遍历当前集族中所有的产生式, BFS
	std::queue<char> symbols;
	// 记录有哪些可用的符号供转移使用
	for (auto &&beg = initialItems.cbegin(); beg != initialItems.cend(); ++beg)
	{
		char to_add = beg->charForClosure();
		if (to_add != '\0')
		{
			symbols.push(to_add);  // 插入默认 . 后面的内容
			_jizu_item_marker.insert(*beg); // 插入默认的生成式到标记
		}
	}
	while (!symbols.empty())
	{
		// 搜索
		char current_symbol = symbols.front();
		symbols.pop();
		if (current_symbol == '\0')
			continue; // 已经是终态了
		if (this->isNotSymbol(current_symbol))
		{
			// 还有符号可以探测
			for (int i = 0; i < T; ++i)
			{
				// 获取产生式的右部相同的产生式左部, 加入到新的集族对象中 同时更新下次需压迫探索的符号队列
				if (analy_str[i].left == current_symbol)
				{
					auto &&temp_marker_obj = LRJizuItem(analy_str[i], 0, i);
					// 初始化所有的标记的产生式
					if (_jizu_item_marker.find(temp_marker_obj) != _jizu_item_marker.end())
						continue;
					else
						_jizu_item_marker.insert(temp_marker_obj);
					//需要探测的符号
					symbols.push(analy_str[i].right[0]);
					// 添加和添加 . 到起始的位置
					current_jizu.insertNewP(temp_marker_obj);
				}
			}
		}
	}

	auto &&allPs = current_jizu.getAllP();
	std::cout << std::endl << "=>I" << next_avaiable_id << ":" << std::endl;
	for (auto &&p_in_jizu : allPs)
	{
		//_jizu_marked[p_in_jizu.getPId()] = false;// 去掉已经访问的标记
		std::cout << p_in_jizu << std::endl;
	}
	// 

	return current_jizu;
}

void LR0SLR1Parser::do_push_status(int number, std::string & stack)
{
	bool find_first = false;
	// number 的编号最多100
	int s[3] = { number / 100, (number / 10) % 10, number % 10 };
	for (int i = 0; i < 3; ++i)
	{
		if (find_first)
			stack.push_back(s[i] + '0');
		else if (s[i] != 0)
		{
			find_first = true;
			stack.push_back(s[i] + '0');
		}
	}
}

std::pair<int, int> LR0SLR1Parser::do_check_status(const std::string& stack)
{
	int num = 0;
	int base = 1;
	int i;
	for (i = stack.size() - 1; i >= 0 && (stack[i] >= '0' && stack[i] <= '9'); --i)
	{
		num += (stack[i] - '0') * base;
		base *= 10;
	}

	return std::make_pair(num, i);
}

bool LR0SLR1Parser::do_check_complete(const std::string& stack, char input_top)
{
	auto &&ret = this->do_check_status(stack);
	// 计算转移到的状态
	return this->_table[ret.first][this->get_nindex(input_top)].getItemType() == TableItemType::Accept;
}

CoflictType LR0SLR1Parser::do_check_placable(const Jizu<LRJizuItem> &jizu, const node &accept) const
{
	bool fnd_reduce = false;
	bool fnd_shift = false;
	// 遍历集族中所有的产生式
	for (auto &&p : jizu.getAllP())
	{
		auto &&p_node = p.getNodeInfo();
		if (accept.left == p_node.left && accept.right == p_node.right)
		{
			return CoflictType::Normal; // 忽略掉 Aceept 的项目
		}

		if (p.charForClosure() == '\0')
		{
			if (fnd_reduce)
				return CoflictType::RR;// 重复的规约
			else
				fnd_reduce = true; // 规约的项目
		}
		else
			fnd_shift = true; // 待约项或者移进项
		// 检查
		if (fnd_reduce && fnd_shift)
			return CoflictType::SR;
	}
	return CoflictType::Normal;
}

void LR0SLR1Parser::do_toplogically_get_first_order(std::queue<char>& order)
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

LR0SLR1Parser::LR0SLR1Parser()
{
}

void LR0SLR1Parser::getJizu(char target)
{
	// 获取规范集族
	// 初始化 I0
	int i0_p_index = this->get_index(target); // 获取产生式编号, 一般是0
	node item = this->analy_str[i0_p_index];  // 第一个产生式(拓广文法)
	// 插入拓广文法开始的符号 也就是 S'->S => S'->.S
	//this->_jizu.insert(std::make_pair(0, Jizu<LR0JizuItem>(0)));
	// 重新记录
	// 重新写入活前缀表

	for (int i = 0; i < 100; ++i)
	{
		for (int j = 0; j < 100; ++j)
			this->_jizu_dfa[i][j] = '\0';

	}
	// 从第一个集族开始, 进行 BFS 遍历, 所以不可能存在前面的集族与后面的某些集族重叠, 查重方便
	std::queue<Jizu<LRJizuItem>> jizus;
	// 初始化
	std::multimap<char, LRJizuItem> p_to_transfer;
	std::set<char> symbols_reached;
	// 送入第一个
	auto &&jizu_init = this->do_generateJizu(0, std::list<LRJizuItem>{LRJizuItem(analy_str[0], 0, 0)});
	this->_jizu.insert(std::make_pair(0, jizu_init));
	jizus.push(jizu_init);
	next_avaiable_id = 1;
	// 开始遍历
	while (!jizus.empty())
	{
		// 拿出当前已经有的, 双端队列从头部是队首
		auto &&current_jizu = jizus.front();
		int current_jizu_id = current_jizu.getId();
		// 获取能够转移到的状态
		p_to_transfer.clear(); symbols_reached.clear();
		current_jizu.getTransferSymbolsAndPs(p_to_transfer, symbols_reached);
		// 分析当前能够转移进入的状态也就是搜索 symbols_reached
		for (auto &&symbol : symbols_reached)
		{
			auto initial_states = std::list<LRJizuItem>();
			// 取出转移到的符号
			auto ps_range = p_to_transfer.equal_range(symbol);
			for (auto i = ps_range.first; i != ps_range.second; ++i)
				initial_states.push_back(i->second);
			// 移动 . 
			for (auto &&advanced_initial_status : initial_states)
			{
				++advanced_initial_status;
			}
			// 构建一个新的 jizu
			// 临时的变量
			auto &&new_jizu = this->do_generateJizu(next_avaiable_id, initial_states);
			// 查重
			bool existed = false;
			for (auto &&beg = _jizu.cbegin(); beg != _jizu.cend(); ++beg)
			{
				if (beg->second == new_jizu)
				{
					// 存在重复的集族, 记录下转移的符号
					this->_jizu_dfa[current_jizu_id][beg->first] = symbol;
					existed = true;
					break;
				}
			}
			if (!existed)
			{
				// 这是一个新的集族
				this->_jizu[new_jizu.getId()] = new_jizu;
				this->_jizu_dfa[current_jizu_id][new_jizu.getId()] = symbol;
				jizus.push(new_jizu);
				++next_avaiable_id;
			}
		}
		jizus.pop(); // 最后弹出
	}

}

void LR0SLR1Parser::demo()
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
	// SLR文法需要 FOLLOW 集 所以就需要进行 求 FIRST FOLLOW 的工作
	this->display();
	this->getJizu(this->non_colt[0]);
	this->getDFAForActivePrefix();
	this->getTable();
	this->printTable();
	this->analyse();
}

LR0SLR1Parser::~LR0SLR1Parser()
{
}

void LR0SLR1Parser::getDFAForActivePrefix()
{
	cout << "识别文法 G[" << this->analy_str[0].left << "] 的全部活前缀的DFA:" << std::endl;
	// 我已经记录下来了相关的活前缀的转移路径
	for (int i = 0; i < next_avaiable_id; ++i)
	{
		// 获得所有的集族的编号

		for (int j = 0; j < next_avaiable_id; ++j)
		{
			if (j == i)
				continue;
			if (_jizu_dfa[i][j])
				std::cout << "I" << i << "--(识别 '" << _jizu_dfa[i][j] << "')-->" << 'I' << j << std::endl;
		}
	}
}

void LR0SLR1Parser::getTable()
{
	// 获取 LR 分析表
	// 初始化
	int jizu_size = this->next_avaiable_id;
	int not_sym_size = this->non_colt.size(); // 非终结符的个数, 不用去掉文法开始的符号
	int term_sym_size = this->ter_copy.size(); // 包含了 # 
	_table = vector<std::vector<TableItem>>(jizu_size);
	for (int i = 0; i < jizu_size; ++i)
	{
		_table[i] = std::vector<TableItem>(term_sym_size + not_sym_size);
	}
	// 文法开始的形式
	for (int i = 0; i < jizu_size; ++i)
	{
		// 用于表示集族是否包含冲突
		bool conflict = false;
		// 逐一获取这几个集族的属性, 
		auto &current_jizu = this->_jizu.at(i);
		// 判断是否有冲突
		auto conflict_type = do_check_placable(current_jizu, analy_str[0]);
		if (conflict_type != CoflictType::Normal)
		{
			// 有冲突
			switch (conflict_type)
			{
			case CoflictType::RR:
				std::cout << "集族: I" << i << " 存在规约-规约冲突" << std::endl; break;
			case CoflictType::SR:
				std::cout << "集族: I" << i << " 存在移进-规约冲突" << std::endl; break;
			}
			conflict = true; // 存在冲突
		}

		// 判断集族接受的类型
		for (auto &&p : current_jizu.getAllP())
		{
			char nxt = p.charForClosure();
			auto &&node_info = p.getNodeInfo();
			if (nxt == '\0' && node_info.left == analy_str[0].left && analy_str[0].right[0] == node_info.right[0])
			{
				int index = get_nindex('#');
				_table[i][index] = TableItem(TableItemType::Accept, index, '#', 0); // Accept
				continue;
			}
			// Goto
			if (isNotSymbol(nxt))
			{
				// 这字符在 非终结符的位置
				int column_index = term_sym_size + get_index(nxt);
				// 非终结符, 搜索输入 nxt 转移到的状态
				int zt;
				for (zt = 0; zt < jizu_size; ++zt)
					if (_jizu_dfa[i][zt] == nxt)
						break;
				// 按道理是在范围的
				_table[i][column_index] = TableItem(TableItemType::Goto, column_index, nxt, zt);
				continue;
			}
			// S 下一个字符是 终结符且非空
			else if (nxt != '\0')
			{
				// S
				int index = get_nindex(nxt);
				// 转移到的状态
				int zt;
				for (zt = 0; zt < next_avaiable_id; ++zt)
					if (_jizu_dfa[i][zt] == nxt)
						break;
				// zt 应该是有效的, 从 i转移到zt , 使用字符 nxt
				auto &&tp = TableItem(TableItemType::S, index, nxt, zt);
				//auto &&tr = _table[i][index];
				_table[i][index] = tp;
				continue;
			}
			// R
			if (nxt == '\0')
			{
				// pu 必须 <T
				// LR0 对于所有的都要添非终结符要加
				if (!conflict)
				{
					//LR0
					for (int j = 0; j < term_sym_size; ++j)
					{
						// r
						int index = get_nindex(this->ter_copy[j]);
						auto &&tp = TableItem(TableItemType::R, index, this->ter_copy[j], p.getPId());
						_table[i][index] = tp;
					}
				}
				else
				{

					// SLR1
					// 将要规约的项目用 FOLLOW(对应的产生式左部) 进行替换为 R 
					auto &&follow_set_of_tp = this->follow_set[this->get_index(node_info.left)]; // R保存的是对应的产生式
					for (auto &&follow : follow_set_of_tp)
					{
						int col = this->get_nindex(follow);
						_table[i][col] = TableItem(TableItemType::R, col, follow, p.getPId());
					}
					// S 保持不变 
				}
				continue;
			}
		}
	}
	// 
}

void LR0SLR1Parser::printTable()
{
	int jizu_size = this->next_avaiable_id;
	int not_sym_size = this->non_colt.size(); // 非终结符的个数
	int term_sym_size = this->ter_copy.size(); // 包含了 # 
	std::cout << std::setw(8) << "Input|" << std::string(8 * term_sym_size - 7, ' ') << "ACTION|" << std::string(8 * not_sym_size - 5, ' ') << "GOTO|" << std::endl;
	std::cout << std::setw(8) << "Status|";
	// Action
	for (int i = 0; i < term_sym_size; ++i)
	{
		// 输出状态
		//std::cout << std::setw(8) << i << '|';
		std::cout << std::setw(8) << this->ter_copy[i];
	}
	// GOTO
	for (int i = 0; i < not_sym_size; ++i)
	{
		std::cout << std::setw(8) << this->non_colt[i];
	}
	std::cout << std::endl;
	// 状态 ->各个信息
	for (int i = 0; i < jizu_size; ++i)
	{
		// 输出状态
		std::cout << std::setw(7) << i << '|';
		// action
		for (int column = 0; column < term_sym_size; ++column)
		{

			// 判断输出的类型
			auto &&item = this->_table[i][column];
			switch (item.getItemType())
			{
			case TableItemType::Accept:
				std::cout << std::setw(8) << "Acc"; break;
			case TableItemType::S:
				std::cout << std::setw(7) << "S" << item.getItemData(); break; // 这个长度控制有些问题, setw 只对后面的一个有效
			case TableItemType::R:
				std::cout << std::setw(7) << "r" << item.getItemData(); break;
			default:
				std::cout << std::setw(8) << " ";
				break;
			}
		}
		// goto
		for (int column = 0; column < not_sym_size; ++column)
		{
			std::cout << std::setw(8);
			if (this->_table[i][column + term_sym_size].getItemData())
				std::cout << this->_table[i][column + term_sym_size].getItemData();
			else
				std::cout << ' ';

		}
		std::cout << std::endl;
	}
}

void LR0SLR1Parser::analyse()
{
	int term_sym_size = this->ter_colt.size();
	std::string str;
	std::cout << "请输入一个句子，以 # 结尾：" << std::endl;
	std::cin >> str;
	// 初始化
	std::string left; // 这个栈的特点， 数字以字符的形式存储， 需要使用帮助函数进行 pop 和 push
	std::string input_string;
	left.push_back('0');
	std::cout << std::setw(15) << "Stack" << std::setw(15) << "Input" << std::setw(15) << "Action" << std::setw(15) << "Next Status" << std::endl;
	//
	for (int i = str.size() - 1; i >= 0; --i)
		input_string.push_back(str[i]);
	while (!this->do_check_complete(left, input_string.back()))
	{
		// 首先需要绘制当前行的信息
		std::cout << std::setw(15) << left;
		// 继续分析, 当前状态 s
		auto &&s = do_check_status(left);
		auto &&item = _table[s.first][get_nindex(input_string.back())];

		// 输出输入串
		std::string reversed_str(input_string.crbegin(), input_string.crend());
		std::cout << std::setw(15) << reversed_str;

		if (item.getItemType() == TableItemType::Error)
		{
			std::cout << std::setw(15) << "Error!";
			return;
		}

		// 判断转移的状态
		if (item.getItemType() == TableItemType::S)
		{
			int status_n_1 = item.getItemData();
			// 栈压入输入串也需要弹出的那个符号
			left.push_back(input_string.back());
			input_string.pop_back();
			this->do_push_status(status_n_1, left);
			// 采取的动作
			std::cout << std::setw(15) << ('S' + std::to_string(status_n_1));
		}
		else if (item.getItemType() == TableItemType::R)
		{
			// 使用的第几个产生式
			int status_r_n_1 = item.getItemData();
			std::cout << std::setw(15) << ('r' + std::to_string(status_r_n_1)) << std::setw(15); // Goto
			// 将关联的符号和状态的范围找到
			auto &&node_info = analy_str[status_r_n_1];

			// 匹配所有的形式 例如产生式右部 A+B 需要匹配栈中的类似的相同的形式
			int i = left.size() - 1;
			int j = node_info.right.size() - 1; // 带匹配的右部指向的字符指针
			for (; i >= 0 && j >= 0; --i)
			{
				if (left[i] == node_info.right[j])
					--j;
			}
			// i 目前表示的就是第一满足产生式的符号的前面一个符号， 用这个符号 求 GOTO 表信息
			if (i >= 0)
			{
				// left 中 i 之后全部弹出
				for (int h = left.size() - 1; h > i; --h)
					left.pop_back();
				// 一般这个会满足的
				int number = 0;
				int base = 1;
				// 这个自己写一个普通的ba
				for (int n = left.size() - 1; n >= i; --n)
				{
					number += (left[n] - '0') * base;
					base *= 10;
				}
				// number 就是实际的数字
				auto &goto_info = _table[number][this->get_index(node_info.left) + term_sym_size];
				if (goto_info.getItemType() != TableItemType::Goto)
				{
					std::cout << "Error!";
					return;
				}

				// 将数据压入left， input 栈不变
				left.push_back(node_info.left);

				this->do_push_status(goto_info.getItemData(), left);
				std::cout << ("Goto(" + std::to_string(number) + ',' + node_info.left + ')');
			}

		}
		std::cout << std::endl;
	}
	// 已经分析完毕
	std::cout << std::setw(15) << left;
	auto &&s = do_check_status(left);
	auto &&item = _table[s.first][get_nindex(input_string.back())];
	// item 必定是 Accept 类型
	if (item.getItemType() == TableItemType::Accept)
	{
		std::cout << std::setw(15) << '#' << std::setw(15) << "Accept" << std::endl;
	}
}


TableItem::TableItem(TableItemType item_type, int column_index, char column_symbol, int column_data) :_item(item_type), _column_data(column_data), _column_index(column_index), _column_symbol(column_symbol)
{
}

TableItem::TableItem() : TableItem(TableItemType::Error, -1, '\0', 0)
{
}

TableItemType TableItem::getItemType() const
{
	return this->_item;
}

int TableItem::getItemData() const
{
	return _column_data;
}