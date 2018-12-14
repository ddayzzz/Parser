
#include "lex_parser.h"

LexItem::LexItem(TokenType tokentype, IDEntry entry): _token_type(tokentype), _id_entry(entry)
{
}

LexItem::LexItem(TokenType tokentype):_token_type(tokentype), _id_entry(-1)
{
}

// 存放 token 同时设置下一个要识别的文件

void LexParser::do_next(IdentifierTable &table, std::istream &_input_stream)
{
	// 调用这个函数总是能够得到下一个 token
	char token;
	_input_stream >> token;
	char temp;
	int line = 1;
	while (token > 0)
	{
		if (token == '#') {
			// 宏处理
			while ((temp = _input_stream.get()) != '\n')
				continue;
		}
		else if (token == '\n') {
			// 换行符
			++line;
			token = _input_stream.get();
		}
		else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_'))
		{
			// 关键字或者是标识符

			// 初始化
			std::string token_str;
			// 标志符或者函数名
			while ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_'))
			{
				token_str.push_back(token);
				token = _input_stream.get();
			}
			TokenType token_type = TokenType::Id;
			if (token_str == "int" || token_str == "float")
			{
				// 目前支持的关键字
				token_type = TokenType::Key;
			}
			// 寻找可能存在标志符，线性探测
			IDEntry i = table.existed(token_str);
			if (i == -1)
			{
				i = table.insert(token_str);
				_tokens.emplace_back(token_type, i);
			}
			else
			{
				// 存在了
				_tokens.emplace_back(token_type, i);
			}
		}
		else if (token >= '0' && token <= '9')
		{
			// 解析数字：支持 十进制 十六进制0x 八进制0
			int token_val = token - '0';
			if (token_val > 0) {
				// 十进制
				while (token >= '0' && token <= '9') {
					token_val = token_val * 10 + token - '0';
					token = _input_stream.get();
				}
			}
			else
			{
				token = _input_stream.get(); // X 或者是 八进制的起始数字
											 // 0 开始
				if (token == 'x' || token == 'X') {
					token = _input_stream.get();
					// 十六进制
					while ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')) {
						token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
						token = _input_stream.get();
					}
				}
				else {
					// 八进制
					while (token >= '0' && token <= '7') {
						token_val = token_val * 8 + token - '0';
						token = _input_stream.get();
					}
				}
			}
			auto i = table.insert(Identifier("Const", IDType::Int, token_val));
			_tokens.emplace_back(TokenType::Num, i);
		}
		else if (token == '*') {
			_tokens.emplace_back(TokenType::Mul);
			token = _input_stream.get();
		}
		else if (token == '/') {
			_tokens.emplace_back(TokenType::Div);
			token = _input_stream.get();
		}
		else if (token == '=') {
			_tokens.emplace_back(TokenType::Assign);
			token = _input_stream.get();
		}
		else if (token == '+') {
			_tokens.emplace_back(TokenType::Add);
			token = _input_stream.get();
		}
		else if (token == '-') {
			_tokens.emplace_back(TokenType::Sub);
			token = _input_stream.get();
		}
		else if (token == '<') {
			_tokens.emplace_back(TokenType::Lt);
			token = _input_stream.get();
		}
		else if (token == '>') {
			_tokens.emplace_back(TokenType::Gt);
			token = _input_stream.get();
		}
		else if (token == '^') {
			_tokens.emplace_back(TokenType::Xor);
			token = _input_stream.get();
		}
		else if (token == '%') {
			_tokens.emplace_back(TokenType::Mod);
			token = _input_stream.get();
		}
		else if (token == ' ' || token == '\t') {
			token = _input_stream.get();
		}
	}
}

void LexParser::parse(IdentifierTable & table, std::ostream & os, std::istream & _input_stream)
{
	do_next(table, _input_stream);
	// 绘制 tokens
	for (auto &&item : _tokens)
	{
		std::cout << item << std::endl;
	}
}

const std::vector<LexItem>& LexParser::getTokens() const
{
	return this->_tokens;
}


std::ostream & operator<<(std::ostream &os, const LexItem &lhs)
{
	os << '(';
	// enum 的显示
	os << token_strings[lhs._token_type] << ',';
	if (lhs._id_entry == -1)
	{
		os << "_)";
	}
	else
	{
		os << lhs._id_entry << ')';
	}
	return os;
}
