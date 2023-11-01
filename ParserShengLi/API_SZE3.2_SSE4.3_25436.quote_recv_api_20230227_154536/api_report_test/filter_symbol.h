#include <vector>
#include <string>

using namespace std;

void push_filter_symbol( const char* str_symbol );

bool is_vaild_symbol( const char* symbol );

void push_filter_opt_symbol( const char* str_symbol );

bool is_vaild_opt_symbol( const char* symbol );

bool string_split( const char* str_src , vector<string>& str_dst , const string& str_separator );

#ifdef WINDOWS
#define CLOCK_REALTIME (0)
int clock_gettime(int, struct timespec* spec);
#endif
