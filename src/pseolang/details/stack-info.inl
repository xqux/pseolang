#pragma once
#ifndef   PSEOLANG_STACK_INFO_INL
#define   PSEOLANG_STACK_INFO_INL


// standard library
#include <map>
#include <optional>
#include <stack>
#include <string>
#include <vector>


namespace pseolang::details
{
	using std::map;
	using std::optional;
	using std::stack;
	using std::string;
	using std::vector;

	class PseoTree;


	// data stored for parsing
	struct Scope
	{
		int indent;
		map<string, PseoTree*> loops;
	};

	struct StackSnapshot
	{
		stack<PseoTree*> branches;
		vector<Scope> scopes;
		optional<string> relation_name;
	};
}


#endif // PSEOLANG_STACK_INFO_INL
