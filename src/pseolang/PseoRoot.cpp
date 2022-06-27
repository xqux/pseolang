// related header
#include <pseolang/PseoRoot.hpp>

// standard library
#include <regex>
#include <algorithm>

// project's headers
#include "details/stack-info.inl"


using namespace pseolang::details;

using std::regex;
using std::cmatch;
using std::csub_match;
using std::cregex_token_iterator;
using std::size_t;
using std::bad_alloc;

using std::find;
using std::for_each;
using std::regex_match;


// creating a basic regular expression
// that parses data line by line,
// determining how to process tokens
auto build_regex() -> regex
{
	return regex(
			"(.*[^ \\S].*)"    // invalid_whitespace [1]
		"|"
			"(.* )"            // end_line_space     [2]
		"|"
			"( *)"             // indent             [3]
			"(?:"
					"(\\.)?"     // breakage           [4]
					"(\\w+)"     // command            [5]
					" *"         // spaces
					"([-|:><])*" // delimiter          [6]
				"|"
					"([^ \\w]+)" // operation          [7]
			")"
			" *"               // spaces
			"(.*)"             // activity           [8]
	);
}


void parse_command(cmatch const& match, StackSnapshot& stack,
                   vector<unique_ptr<PseoTree>>& storage)
{
	auto indent = static_cast<int>(match[3].length()); //! static_cast is a potential overflow (unlikely)
	auto breakage = match[4].matched;
	auto command = match[5];
	auto delimiter = match[6].matched;
	auto activity = match[8];

	auto curr_branch = stack.branches.top();

	// stack work
	if (indent > stack.scopes.back().indent)
	{
		// | decision: INFO
		// | # =>
		// |   process: INFO
		//   ^^^^ no arrow to show a command relation (uncomment for correctness)
		if (!stack.relation_name.has_value())
		{ throw PseoRoot::Error::UNACCEPTABLE_SCOPE; }

		// save the new indent to iterate over it later,
		// leaving the old indent (and all its loops) behind
		stack.scopes.push_back({indent, {}});

		// don't delete the current branch (PseoTree*),
		// because we will need to add children!
	}
	else
	{
		// unwind the stack
		while (indent != stack.scopes.back().indent)
		{
			if (indent > stack.scopes.back().indent)
			{ throw PseoRoot::Error::UNACCEPTABLE_SCOPE; }

			stack.branches.pop();
			stack.scopes.pop_back();
		}

		// we can only process 1 command,
		// we saved the value of the current one (see curr_branch)
		// => pop this element from the stack
		stack.branches.pop();
	}

	// a delimiter can't be empty
	if (!delimiter)
	{ throw PseoRoot::Error::NO_DELIMITER; }

	// loop
	if (command == "loop")
	{
		for (auto const& scope: stack.scopes)
		{
			auto res = scope.loops.find(activity);
			if (res != scope.loops.end())
			{
				curr_branch->branches.push_back(
					PseoRelation(res->second, stack.relation_name.value_or(string()))
				);

				stack.relation_name.reset();

				return; // fix
			}
		}

		stack.scopes.back().loops.emplace(activity, curr_branch);
		return;
	}

	// alg || process || io || decision
	PseoTree* val;

	if (command == "alg")
	{ val = new PseoTree(PseoBlock(PseoBlock::Style::ALG, activity)); }
	else if (command == "process")
	{ val = new PseoTree(PseoBlock(PseoBlock::Style::PROCESS, activity)); }
	else if (command == "io")
	{ val = new PseoTree(PseoBlock(PseoBlock::Style::IO, activity)); }
	else if (command == "decision")
	{ val = new PseoTree(PseoBlock(PseoBlock::Style::DECISION, activity)); }
	else
	{ throw PseoRoot::Error::INVALID_INSTRUCTION; }

	//
	storage.push_back(unique_ptr<PseoTree>(val));

	//
	//
	curr_branch->branches.push_back(
		PseoRelation(val, stack.relation_name.value_or(""))
	);

	stack.relation_name.reset();

	//
	stack.branches.push(val);
}

void parse_operation(cmatch const& match, StackSnapshot& stack)
{
	auto indent = static_cast<int>(match[3].length());
	auto operation = match[7];
	auto activity = match[8];

	if (operation == "#")
	{ return; }

	//!stack.branches.pop();

	while (indent != stack.scopes.back().indent)
	{
		if (indent > stack.scopes.back().indent)
		{ throw PseoRoot::Error::UNACCEPTABLE_SCOPE; }

		stack.branches.pop();
		stack.scopes.pop_back();
	}

	if (operation == "=>")
	{
		// | => RELATION_1
		// | => RELATION_2
		if (stack.relation_name.has_value())
		{ throw PseoRoot::Error::RELATION_WITHOUT_BLOCK; }

		stack.relation_name = activity.str();
	}
	else
	{ throw PseoRoot::Error::INVALID_INSTRUCTION;}
}


void parse(csub_match const& line, cmatch& match, regex const& re,
           StackSnapshot& stack, vector<unique_ptr<PseoTree>>& storage)
{
	if (regex_match(line.first, line.second, match, re))
	{
		if (match[1].matched) // invalid_whitespace
		{ throw PseoRoot::Error::INVALID_WHITESPACE_CHAR; }
		if (match[2].matched) // end_line_space
		{ throw PseoRoot::Error::SPACE_AS_LAST_CHAR_OF_LINE; }

		if (match[5].matched) // command
		{ parse_command(match, stack, storage); }
		else // match[7].matched :: operation
		{ parse_operation(match, stack); }
	}
}


PseoRoot::PseoRoot(string_view text) noexcept
	: m_error(Error::NO_ERROR)
	, m_error_line(size_t(0))
	, m_tree(nullptr)
	, m_storage()
{
	if (text.empty())
	{ return; }

	auto curr_line = m_error_line;

	try
	{
		// building a regex object for parsing
		auto re = build_regex();
		auto nl_re = regex("\\r|\\n|\\r\\n"); // pattern for parsing each line

		auto match = cmatch();

		// gag object,
		// needed because the loop requires a vertex block
		auto tree = PseoTree(PseoBlock(PseoBlock::Style::ALG));

		// needed for parsing
		StackSnapshot stack;
		stack.branches.push(&tree);
		stack.scopes.push_back({0, {}});
		stack.relation_name = string(); // initialization to disallow arrow at start

		//!
		auto is_breakage = false;

		// line-by-line processing, skipping empty lines
		for_each(cregex_token_iterator(text.begin(), text.end(), nl_re, -1),
		         cregex_token_iterator(), [&](auto const& line)
		{
			curr_line += 1;
			parse(line, match, re, stack, m_storage);
		});

		if (!tree.branches.empty())
		{ m_tree = tree.branches[0].next; }

		return;
	}
	catch (PseoRoot::Error const& error)
	{ m_error = error; }
	catch (bad_alloc const&)
	{ m_error = Error::NOT_ENOUGH_MEMORY; }
	catch (...)
	{ m_error = Error::UNKNOWN_ERROR; }

	m_error_line = curr_line;
	m_storage.clear();
}

// clearing an object is the same as
// assigning the element with the default state
void PseoRoot::clear() noexcept
{ *this = PseoRoot(); }

// if the tree is non-empty,
// there is at least one element allocated in dynamic memory
auto PseoRoot::empty() const noexcept -> bool
{ return m_storage.empty(); }
