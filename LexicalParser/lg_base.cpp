
#include "lg_base.h"
#include <iostream>
using namespace std;

void Base::do_toplogically_get_first_order(std::queue<char>& order)
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



Base::Base(std::istream & stream)
{
	string s;
	// cout << "����Ĳ���ʽ�ĸ�����" << endl;
	stream >> T; // �����P����
	for (int index = 0; index < T; index++)
	{
		stream >> s;
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
}

// �Ƿ��Ƿ��ս��
bool Base::isNotSymbol(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}
// ��ȡ���ս����λ��(�� FIRST, λ���������ڲ���ʽ�ı��)
int Base::get_index(char target)
{
	for (int i = 0; i < non_colt.size(); i++)
	{
		if (target == non_colt[i])
			return i;
	}
	return -1;
}
// ��ȡ�ս����λ��(������$)
int Base::get_nindex(char target)
{
	for (int i = 0; i < ter_copy.size(); i++)
	{
		if (target == ter_copy[i])
			return i;
	}
	return -1;
}
// �� FIRST
void Base::get_first(char target)
{
	int tag = 0;
	int flag = 0;
	// �������еĲ���ʽ
	for (int i = 0; i < T; i++)
	{
		if (analy_str[i].left == target) //ƥ�����ʽ��
		{
			if (!isNotSymbol(analy_str[i].right[0]))//�����ս����ֱ�Ӽ��� FIRST
			{
				first_set[get_index(target)].insert(analy_str[i].right[0]); // �������ʽ�Ҳ��ս�������Ӧ�� FIRST ����
			}
			else
			{
				// �����������ʽ�Ҳ���һ�����ս��ַ�
				for (int j = 0; j < analy_str[i].right.length(); j++)
				{
					if (!isNotSymbol(analy_str[i].right[j])) // �ս������, ������Ҳ����һ�����ս��ַ�
					{
						first_set[get_index(target)].insert(analy_str[i].right[j]);
						break;
					}
					if (analy_str[i].right[j] == analy_str[i].left)
					{
						bool flag = false;
						// ��������ݹ������, ������� T->$�ż���, ����ֱ��break, ��ͨ����ͬ�ķ���ݹ��ʽ���
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
					// ���ս�����
					// �� right[0...j] ��λ�û�û���ս��. ��Ҫ����������: ����[0...j-1]�Ѿ���� first, ���right[0...j]
					// �ķ��ս���� right[0...j-1] �� getFrist(right[j])
					get_first(analy_str[i].right[j]);//�ݹ�
					// Ŀǰ����� right[0...j] ���ս��
					// ���ϵ�����, ����ɨ���Ե�ǰ���ս�� right[j] �����еĵĲ���ʽ���󲿶�Ӧ�ļ���
					// ���� X->right[j] �� ����ʽ�ı��, ������������ FIRST[X], Ȼ������Ҫ����ַ��� FIRST
					set<char>::iterator it;
					int u = get_index(analy_str[i].right[j]);
					for (it = first_set[u].begin(); it != first_set[u].end(); it++)
					{
						// �������ʽ�Ӱ����մ�
						if (*it == '$')
							flag = 1;
						else
							first_set[get_index(target)].insert(*it);//��FIRST(X)�еķ�$�ͼ���FIRST(X)
					}
					if (flag == 0)
						break; // ��ȫ���ķǿշ��ż��뵽�� FIRST
					else
					{
						tag += flag; // ������ epsilon
						flag = 0; // ���, ������� ���ַ����������ķ��ŵ� FIRST(����еĻ�)
					}
				}
				if (tag == analy_str[i].right.length()) // �����Ҳ�FIRST(Y) ����$, ��$���� FIRST(X) ��. 
					first_set[get_index(target)].insert('$');
			}
		}
	}
}
// �� FOLLOW
void Base::get_follow(char target)
{
	for (int i = 0; i < T; i++)
	{
		int index = -1;
		// ����ʽ�Ҳ�����
		int len = analy_str[i].right.length();
		// Ѱ�ҵ�һ��ƥ��Ҫ����󲿵Ĳ���ʽ���
		for (int j = 0; j < len; j++)
		{
			if (analy_str[i].right[j] == target)
			{
				index = j;
				break;
			}
		}
		// ����һ����ʽ
		if (index != -1 && index < len - 1)
		{
			char nxt = analy_str[i].right[index + 1];
			if (!isNotSymbol(nxt))
			{
				// ���: �������ӵ����ս��, ֱ�Ӽ��뵽 FOLLOW
				follow_set[get_index(target)].insert(nxt);
			}
			else
			{
				// ���: ��һ���Ƿ��ս��
				int isExt = 0;
				set<char>::iterator it;
				// �����: ��1+��2 FIRST[��2] ���� FOLLOW[��1]
				for (it = first_set[get_index(nxt)].begin(); it != first_set[get_index(nxt)].end(); it++)
				{
					if (*it == '$')
						isExt = 1;
					else
						follow_set[get_index(target)].insert(*it);
				}

				if (isExt && analy_str[i].left != target)
				{
					// ���: ��1->��2. �������һ�����ַ��Լ�����ʽ�� != �Ҳ����ս� FOLLOW(��1) ���� FOLLOW(��2)
					get_follow(analy_str[i].left);
					set<char>::iterator it;
					// tmp ��1
					char tmp = analy_str[i].left;
					for (it = follow_set[get_index(tmp)].begin(); it != follow_set[get_index(tmp)].end(); it++)
						follow_set[get_index(target)].insert(*it);
				}
			}
		}
		// ��Ҫ���и��£���ֹ©����
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
// ���ݲ���ʽ�½�����
void Base::inputAndSolve()
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
	for (int i = 0; i < non_colt.size(); i++)
	{
		get_first(non_colt[i]);
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
}

void Base::display()
{
	cout << "FIRST����" << endl;
	for (int i = 0; i < non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = first_set[i].begin(); it != first_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}

	cout << "FOLLOW����" << endl;
	for (int i = 0; i < non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = follow_set[i].begin(); it != follow_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}
}