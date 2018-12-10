#include "lr_parser.h"
Jizu<LRJizuItem> LR0SLR1Parser::do_generateJizu(
	int jizuId,
	const std::list<LRJizuItem> &initialItems)
{
	_jizu_item_marker.clear();
	// ����Ĭ�ϵĲ���ʽ��
	Jizu<LRJizuItem> current_jizu = Jizu<LRJizuItem>(jizuId, initialItems);
	// ������ǰ���������еĲ���ʽ, BFS
	std::queue<char> symbols;
	// ��¼����Щ���õķ��Ź�ת��ʹ��
	for (auto &&beg = initialItems.cbegin(); beg != initialItems.cend(); ++beg)
	{
		char to_add = beg->charForClosure();
		if (to_add != '\0')
		{
			symbols.push(to_add);  // ����Ĭ�� . ���������
			_jizu_item_marker.insert(*beg); // ����Ĭ�ϵ�����ʽ�����
		}
	}
	while (!symbols.empty())
	{
		// ����
		char current_symbol = symbols.front();
		symbols.pop();
		if (current_symbol == '\0')
			continue; // �Ѿ�����̬��
		if (this->isNotSymbol(current_symbol))
		{
			// ���з��ſ���̽��
			for (int i = 0; i < T; ++i)
			{
				// ��ȡ����ʽ���Ҳ���ͬ�Ĳ���ʽ��, ���뵽�µļ�������� ͬʱ�����´���ѹ��̽���ķ��Ŷ���
				if (analy_str[i].left == current_symbol)
				{
					auto &&temp_marker_obj = LRJizuItem(analy_str[i], 0, i);
					// ��ʼ�����еı�ǵĲ���ʽ
					if (_jizu_item_marker.find(temp_marker_obj) != _jizu_item_marker.end())
						continue;
					else
						_jizu_item_marker.insert(temp_marker_obj);
					//��Ҫ̽��ķ���
					symbols.push(analy_str[i].right[0]);
					// ��Ӻ���� . ����ʼ��λ��
					current_jizu.insertNewP(temp_marker_obj);
				}
			}
		}
	}

	auto &&allPs = current_jizu.getAllP();
	std::cout << std::endl << "=>I" << next_avaiable_id << ":" << std::endl;
	for (auto &&p_in_jizu : allPs)
	{
		//_jizu_marked[p_in_jizu.getPId()] = false;// ȥ���Ѿ����ʵı��
		std::cout << p_in_jizu << std::endl;
	}
	// 

	return current_jizu;
}

void LR0SLR1Parser::do_push_status(int number, std::string & stack)
{
	bool find_first = false;
	// number �ı�����100
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
	// ����ת�Ƶ���״̬
	return this->_table[ret.first][this->get_nindex(input_top)].getItemType() == TableItemType::Accept;
}

CoflictType LR0SLR1Parser::do_check_placable(const Jizu<LRJizuItem> &jizu, const node &accept) const
{
	bool fnd_reduce = false;
	bool fnd_shift = false;
	// �������������еĲ���ʽ
	for (auto &&p : jizu.getAllP())
	{
		auto &&p_node = p.getNodeInfo();
		if (accept.left == p_node.left && accept.right == p_node.right)
		{
			return CoflictType::Normal; // ���Ե� Aceept ����Ŀ
		}

		if (p.charForClosure() == '\0')
		{
			if (fnd_reduce)
				return CoflictType::RR;// �ظ��Ĺ�Լ
			else
				fnd_reduce = true; // ��Լ����Ŀ
		}
		else
			fnd_shift = true; // ��Լ������ƽ���
		// ���
		if (fnd_reduce && fnd_shift)
			return CoflictType::SR;
	}
	return CoflictType::Normal;
}

void LR0SLR1Parser::do_toplogically_get_first_order(std::queue<char>& order)
{
	int not_syms_size = this->non_colt.size();
	// ��������, �������з��ŵ�����ͼģ��
	int **graph = new int*[not_syms_size];
	for (int i = 0; i < not_syms_size; ++i)
	{
		graph[i] = new int[not_syms_size];
		memset(graph[i], 0, not_syms_size * sizeof(int));
	}
	// ���������Ϣ
	std::vector<int> in_degree(not_syms_size);
	// �������еĲ���ʽ
	for (int i = 0; i < T; ++i)
	{
		auto &&node_p = analy_str[i];
		if (this->isNotSymbol(node_p.right[0]))
		{
			int w = this->get_index(node_p.right[0]);
			int v = this->get_index(node_p.left);
			// ������ w->v ������ͼ·��
			graph[w][v] += 1; // ���� ����ʽ A->B ����Ϊ�� B->A ����·������
		}
	}
	// graph ����Ӳ���ʽ�Ҳ����󲿷��ս����������Ϣ, ������Ƿ��ս������ non_colt ��λ��
	std::queue<int> p_lefts;
	// ��������ȵĵ�, Ҳ���ǲ���ʽ�Ҳ���һ���ַ�Ϊ�ս��
	for (int i = 0; i < not_syms_size; ++i)
	{
		int j;
		int counter = 0; // ����j������Ƕ���
		for (j = 0; j < not_syms_size; ++j)
		{
			if (i == j)
				continue; // ������ݹ����ʽ
			// �Ҳ���һ�� -> �󲿷��� i->j
			if (graph[j][i] > 0)
				counter += 1;
		}
		// �Ǽ����
		in_degree[i] = counter;
		if (counter <= 0)
		{
			// �Ҳ���һ�����ս��
			p_lefts.push(i);
		}
	}
	// ��ʼ�������� Khan �㷨
	while (!p_lefts.empty())//ֻҪ�������Ϊ0�Ķ���Ϊ����ô��ѭ��
	{

		int v = p_lefts.front(); p_lefts.pop();//ѡ���һ����ʼ�����Ϊ0�Ķ���  ���������Զ�ͷ��Ϊ��ʼ
		order.push(this->non_colt[v]);//�Ȱѽ��ѹ��, ����صķ��ս��
		std::vector<int> paths;

		for (int i = 0; i < not_syms_size; ++i)
		{
			if (graph[v][i] >= 1)
			{
				// ��·������
				if (0 == --in_degree[i])//ע��Ҫ��ȥ�Ѿ��ҵ��Ķ��� ���Ŀ����յ�����Ϊ0����ô���������ǿ�����ӵ�
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
	// ��ȡ�淶����
	// ��ʼ�� I0
	int i0_p_index = this->get_index(target); // ��ȡ����ʽ���, һ����0
	node item = this->analy_str[i0_p_index];  // ��һ������ʽ(�ع��ķ�)
	// �����ع��ķ���ʼ�ķ��� Ҳ���� S'->S => S'->.S
	//this->_jizu.insert(std::make_pair(0, Jizu<LR0JizuItem>(0)));
	// ���¼�¼
	// ����д���ǰ׺��

	for (int i = 0; i < 100; ++i)
	{
		for (int j = 0; j < 100; ++j)
			this->_jizu_dfa[i][j] = '\0';

	}
	// �ӵ�һ�����忪ʼ, ���� BFS ����, ���Բ����ܴ���ǰ��ļ���������ĳЩ�����ص�, ���ط���
	std::queue<Jizu<LRJizuItem>> jizus;
	// ��ʼ��
	std::multimap<char, LRJizuItem> p_to_transfer;
	std::set<char> symbols_reached;
	// �����һ��
	auto &&jizu_init = this->do_generateJizu(0, std::list<LRJizuItem>{LRJizuItem(analy_str[0], 0, 0)});
	this->_jizu.insert(std::make_pair(0, jizu_init));
	jizus.push(jizu_init);
	next_avaiable_id = 1;
	// ��ʼ����
	while (!jizus.empty())
	{
		// �ó���ǰ�Ѿ��е�, ˫�˶��д�ͷ���Ƕ���
		auto &&current_jizu = jizus.front();
		int current_jizu_id = current_jizu.getId();
		// ��ȡ�ܹ�ת�Ƶ���״̬
		p_to_transfer.clear(); symbols_reached.clear();
		current_jizu.getTransferSymbolsAndPs(p_to_transfer, symbols_reached);
		// ������ǰ�ܹ�ת�ƽ����״̬Ҳ�������� symbols_reached
		for (auto &&symbol : symbols_reached)
		{
			auto initial_states = std::list<LRJizuItem>();
			// ȡ��ת�Ƶ��ķ���
			auto ps_range = p_to_transfer.equal_range(symbol);
			for (auto i = ps_range.first; i != ps_range.second; ++i)
				initial_states.push_back(i->second);
			// �ƶ� . 
			for (auto &&advanced_initial_status : initial_states)
			{
				++advanced_initial_status;
			}
			// ����һ���µ� jizu
			// ��ʱ�ı���
			auto &&new_jizu = this->do_generateJizu(next_avaiable_id, initial_states);
			// ����
			bool existed = false;
			for (auto &&beg = _jizu.cbegin(); beg != _jizu.cend(); ++beg)
			{
				if (beg->second == new_jizu)
				{
					// �����ظ��ļ���, ��¼��ת�Ƶķ���
					this->_jizu_dfa[current_jizu_id][beg->first] = symbol;
					existed = true;
					break;
				}
			}
			if (!existed)
			{
				// ����һ���µļ���
				this->_jizu[new_jizu.getId()] = new_jizu;
				this->_jizu_dfa[current_jizu_id][new_jizu.getId()] = symbol;
				jizus.push(new_jizu);
				++next_avaiable_id;
			}
		}
		jizus.pop(); // ��󵯳�
	}

}

void LR0SLR1Parser::demo()
{
	string s;
	cout << "����Ĳ���ʽ�ĸ�����" << endl;
	cin >> T; // �����P����
	for (int index = 0; index < T; index++)
	{
		cin >> s;
		string temp = "";
		// �����µ��ַ���
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}
		analy_str[index].left = temp[0]; // ����ʽ�󲿵��ַ���
		for (int i = 3; i < temp.length(); i++) // Խ�� X-> �����ַ�
			analy_str[index].right += temp[i];

		// ȡÿһ������ʽ
		for (int i = 0; i < temp.length(); i++)
		{
			// ע����Ե������� V ���ַ�
			if (temp[i] != '-' && temp[i] != '>')
			{
				// �ж��Ƿ��Ƿ��ս��
				if (isNotSymbol(temp[i]))
				{
					// ����������ȷ������ʽ�Ƿ����µķ��ս��, �����½�һ�����ս��;��������µķ��ս��
					int flag = 0;
					for (int j = 0; j < non_colt.size(); j++)
					{
						if (non_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					// ���û�л����� V_N Ϊ��
					if (!flag)
						non_colt.push_back(temp[i]); // ��������ӵ����ս���б�

				}
				else
				{
					// �ս��
					int flag = 0;
					for (int j = 0; j < ter_colt.size(); j++)
					{
						// ͬ��, ��ֹ�ظ�����ս��
						if (ter_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					if (!flag)
						ter_colt.push_back(temp[i]); // ��ӵ��ս���б�
				}
			}
		}

	}
	// ������������ FIRST FOLLOW ��
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
			follow_set[0].insert('#'); // ��һ���� #
		get_follow(non_colt[i]);
	}

	for (int i = 0; i < ter_colt.size(); i++)
	{
		if (ter_colt[i] != '$')
			ter_copy.push_back(ter_colt[i]);
	}
	// SLR�ķ���Ҫ FOLLOW �� ���Ծ���Ҫ���� �� FIRST FOLLOW �Ĺ���
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
	cout << "ʶ���ķ� G[" << this->analy_str[0].left << "] ��ȫ����ǰ׺��DFA:" << std::endl;
	// ���Ѿ���¼��������صĻ�ǰ׺��ת��·��
	for (int i = 0; i < next_avaiable_id; ++i)
	{
		// ������еļ���ı��

		for (int j = 0; j < next_avaiable_id; ++j)
		{
			if (j == i)
				continue;
			if (_jizu_dfa[i][j])
				std::cout << "I" << i << "--(ʶ�� '" << _jizu_dfa[i][j] << "')-->" << 'I' << j << std::endl;
		}
	}
}

void LR0SLR1Parser::getTable()
{
	// ��ȡ LR ������
	// ��ʼ��
	int jizu_size = this->next_avaiable_id;
	int not_sym_size = this->non_colt.size(); // ���ս���ĸ���, ����ȥ���ķ���ʼ�ķ���
	int term_sym_size = this->ter_copy.size(); // ������ # 
	_table = vector<std::vector<TableItem>>(jizu_size);
	for (int i = 0; i < jizu_size; ++i)
	{
		_table[i] = std::vector<TableItem>(term_sym_size + not_sym_size);
	}
	// �ķ���ʼ����ʽ
	for (int i = 0; i < jizu_size; ++i)
	{
		// ���ڱ�ʾ�����Ƿ������ͻ
		bool conflict = false;
		// ��һ��ȡ�⼸�����������, 
		auto &current_jizu = this->_jizu.at(i);
		// �ж��Ƿ��г�ͻ
		auto conflict_type = do_check_placable(current_jizu, analy_str[0]);
		if (conflict_type != CoflictType::Normal)
		{
			// �г�ͻ
			switch (conflict_type)
			{
			case CoflictType::RR:
				std::cout << "����: I" << i << " ���ڹ�Լ-��Լ��ͻ" << std::endl; break;
			case CoflictType::SR:
				std::cout << "����: I" << i << " �����ƽ�-��Լ��ͻ" << std::endl; break;
			}
			conflict = true; // ���ڳ�ͻ
		}

		// �жϼ�����ܵ�����
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
				// ���ַ��� ���ս����λ��
				int column_index = term_sym_size + get_index(nxt);
				// ���ս��, �������� nxt ת�Ƶ���״̬
				int zt;
				for (zt = 0; zt < jizu_size; ++zt)
					if (_jizu_dfa[i][zt] == nxt)
						break;
				// ���������ڷ�Χ��
				_table[i][column_index] = TableItem(TableItemType::Goto, column_index, nxt, zt);
				continue;
			}
			// S ��һ���ַ��� �ս���ҷǿ�
			else if (nxt != '\0')
			{
				// S
				int index = get_nindex(nxt);
				// ת�Ƶ���״̬
				int zt;
				for (zt = 0; zt < next_avaiable_id; ++zt)
					if (_jizu_dfa[i][zt] == nxt)
						break;
				// zt Ӧ������Ч��, �� iת�Ƶ�zt , ʹ���ַ� nxt
				auto &&tp = TableItem(TableItemType::S, index, nxt, zt);
				//auto &&tr = _table[i][index];
				_table[i][index] = tp;
				continue;
			}
			// R
			if (nxt == '\0')
			{
				// pu ���� <T
				// LR0 �������еĶ�Ҫ����ս��Ҫ��
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
					// ��Ҫ��Լ����Ŀ�� FOLLOW(��Ӧ�Ĳ���ʽ��) �����滻Ϊ R 
					auto &&follow_set_of_tp = this->follow_set[this->get_index(node_info.left)]; // R������Ƕ�Ӧ�Ĳ���ʽ
					for (auto &&follow : follow_set_of_tp)
					{
						int col = this->get_nindex(follow);
						_table[i][col] = TableItem(TableItemType::R, col, follow, p.getPId());
					}
					// S ���ֲ��� 
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
	int not_sym_size = this->non_colt.size(); // ���ս���ĸ���
	int term_sym_size = this->ter_copy.size(); // ������ # 
	std::cout << std::setw(8) << "Input|" << std::string(8 * term_sym_size - 7, ' ') << "ACTION|" << std::string(8 * not_sym_size - 5, ' ') << "GOTO|" << std::endl;
	std::cout << std::setw(8) << "Status|";
	// Action
	for (int i = 0; i < term_sym_size; ++i)
	{
		// ���״̬
		//std::cout << std::setw(8) << i << '|';
		std::cout << std::setw(8) << this->ter_copy[i];
	}
	// GOTO
	for (int i = 0; i < not_sym_size; ++i)
	{
		std::cout << std::setw(8) << this->non_colt[i];
	}
	std::cout << std::endl;
	// ״̬ ->������Ϣ
	for (int i = 0; i < jizu_size; ++i)
	{
		// ���״̬
		std::cout << std::setw(7) << i << '|';
		// action
		for (int column = 0; column < term_sym_size; ++column)
		{

			// �ж����������
			auto &&item = this->_table[i][column];
			switch (item.getItemType())
			{
			case TableItemType::Accept:
				std::cout << std::setw(8) << "Acc"; break;
			case TableItemType::S:
				std::cout << std::setw(7) << "S" << item.getItemData(); break; // ������ȿ�����Щ����, setw ֻ�Ժ����һ����Ч
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
	std::cout << "������һ�����ӣ��� # ��β��" << std::endl;
	std::cin >> str;
	// ��ʼ��
	std::string left; // ���ջ���ص㣬 �������ַ�����ʽ�洢�� ��Ҫʹ�ð����������� pop �� push
	std::string input_string;
	left.push_back('0');
	std::cout << std::setw(15) << "Stack" << std::setw(15) << "Input" << std::setw(15) << "Action" << std::setw(15) << "Next Status" << std::endl;
	//
	for (int i = str.size() - 1; i >= 0; --i)
		input_string.push_back(str[i]);
	while (!this->do_check_complete(left, input_string.back()))
	{
		// ������Ҫ���Ƶ�ǰ�е���Ϣ
		std::cout << std::setw(15) << left;
		// ��������, ��ǰ״̬ s
		auto &&s = do_check_status(left);
		auto &&item = _table[s.first][get_nindex(input_string.back())];

		// ������봮
		std::string reversed_str(input_string.crbegin(), input_string.crend());
		std::cout << std::setw(15) << reversed_str;

		if (item.getItemType() == TableItemType::Error)
		{
			std::cout << std::setw(15) << "Error!";
			return;
		}

		// �ж�ת�Ƶ�״̬
		if (item.getItemType() == TableItemType::S)
		{
			int status_n_1 = item.getItemData();
			// ջѹ�����봮Ҳ��Ҫ�������Ǹ�����
			left.push_back(input_string.back());
			input_string.pop_back();
			this->do_push_status(status_n_1, left);
			// ��ȡ�Ķ���
			std::cout << std::setw(15) << ('S' + std::to_string(status_n_1));
		}
		else if (item.getItemType() == TableItemType::R)
		{
			// ʹ�õĵڼ�������ʽ
			int status_r_n_1 = item.getItemData();
			std::cout << std::setw(15) << ('r' + std::to_string(status_r_n_1)) << std::setw(15); // Goto
			// �������ķ��ź�״̬�ķ�Χ�ҵ�
			auto &&node_info = analy_str[status_r_n_1];

			// ƥ�����е���ʽ �������ʽ�Ҳ� A+B ��Ҫƥ��ջ�е����Ƶ���ͬ����ʽ
			int i = left.size() - 1;
			int j = node_info.right.size() - 1; // ��ƥ����Ҳ�ָ����ַ�ָ��
			for (; i >= 0 && j >= 0; --i)
			{
				if (left[i] == node_info.right[j])
					--j;
			}
			// i Ŀǰ��ʾ�ľ��ǵ�һ�������ʽ�ķ��ŵ�ǰ��һ�����ţ� ��������� �� GOTO ����Ϣ
			if (i >= 0)
			{
				// left �� i ֮��ȫ������
				for (int h = left.size() - 1; h > i; --h)
					left.pop_back();
				// һ������������
				int number = 0;
				int base = 1;
				// ����Լ�дһ����ͨ��ba
				for (int n = left.size() - 1; n >= i; --n)
				{
					number += (left[n] - '0') * base;
					base *= 10;
				}
				// number ����ʵ�ʵ�����
				auto &goto_info = _table[number][this->get_index(node_info.left) + term_sym_size];
				if (goto_info.getItemType() != TableItemType::Goto)
				{
					std::cout << "Error!";
					return;
				}

				// ������ѹ��left�� input ջ����
				left.push_back(node_info.left);

				this->do_push_status(goto_info.getItemData(), left);
				std::cout << ("Goto(" + std::to_string(number) + ',' + node_info.left + ')');
			}

		}
		std::cout << std::endl;
	}
	// �Ѿ��������
	std::cout << std::setw(15) << left;
	auto &&s = do_check_status(left);
	auto &&item = _table[s.first][get_nindex(input_string.back())];
	// item �ض��� Accept ����
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