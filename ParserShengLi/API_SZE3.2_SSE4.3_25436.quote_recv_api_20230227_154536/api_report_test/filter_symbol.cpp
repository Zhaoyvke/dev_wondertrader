#include <set>
#include "filter_symbol.h"
#include <string.h>



struct st_symbol_id
{
	long long	m_i_symbol_id:48;
};

struct st_opt_symbol_id
{
	long long	m_i_symbol_id;
};


static set<long long> g_symbols;
static set<long long> g_opt_symbols;

void push_filter_symbol(const char* str_symbol)
{
	g_symbols.insert(((st_symbol_id*)(str_symbol))->m_i_symbol_id);
}

bool is_vaild_symbol(const char* symbol)
{
	return ( g_symbols.empty() ) || ( 0 != g_symbols.count( ( (st_symbol_id*) ( symbol ) )->m_i_symbol_id ) );
}

void push_filter_opt_symbol(const char* str_symbol)
{
	g_opt_symbols.insert(((st_opt_symbol_id*)(str_symbol))->m_i_symbol_id);
}

bool is_vaild_opt_symbol(const char* symbol)
{
	return 0 != g_opt_symbols.count(((st_opt_symbol_id*)(symbol))->m_i_symbol_id);
}

bool string_split(const char* str_src, vector<string>& str_dst, const string& str_separator)
{
	if (NULL == str_src)
	{
		return false;
	}

	//注意，这里选择了清空输入的字符数组
	str_dst.clear();

	size_t sep_len = str_separator.length();
	if (0 == sep_len)
		return false;

	size_t src_len = strlen(str_src);
	if (0 == src_len)
		return true;

	if (src_len < sep_len)//这种情况，认为只有一个元素而返回
	{
		str_dst.push_back(str_src);
		return true;
	}

	//2010-08-05 Find Problem ls
	//测试发现分割字符串，没有考虑最后一块，修改部分如下：
	//-----------------------------------------------------------
	/// add by zhou.you  review 2012/7/30
	/// 原修改不合理，应该先判断是否以分隔符结尾的。
	string srcstring = str_src;
	if (srcstring.substr(src_len - sep_len, sep_len) != str_separator)
		srcstring += str_separator;

	size_t lastIdx = 0;
	size_t idx = srcstring.find_first_of(str_separator);
	if (idx == string::npos)//这种情况，认为只有一个元素而返回
	{
		str_dst.push_back(str_src);
		return true;
	}

	while (idx != string::npos)
	{
		string strTemp = srcstring.substr(lastIdx, idx - lastIdx);//有可能空字符串，也作为结果加入。
		str_dst.push_back(strTemp);
		lastIdx = idx + sep_len;
		idx = srcstring.find_first_of(str_separator, lastIdx);
	}
	return true;
}

#ifdef WINDOWS
#include <Windows.h>
#include <time.h>
int clock_gettime(int, struct timespec* spec)
{
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= 116444736000000000i64;
	spec->tv_sec = wintime / 10000000i64;
	spec->tv_nsec = wintime % 10000000i64 * 100;
	return 0;
}
#endif
