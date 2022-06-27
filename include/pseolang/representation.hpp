#pragma once
#ifndef   PSEOLANG_REPRESENTATION_HPP
#define   PSEOLANG_REPRESENTATION_HPP


// standard library
#include <string>
#include <vector>


namespace pseolang::details
{
	using std::string;
	using std::vector;

	class PseoTree;


	// block representation
	class PseoBlock
	{
	public: // one of the types defined from the instruction
		enum class Style
		{
			ALG = 0,
			PROCESS = 1,
			IO = 2,
			DECISION = 3
		};

	public: // creation and destruction of objects
		explicit PseoBlock(Style style, string const& text = string())
			: style(style), text(text) {}

		virtual ~PseoBlock() noexcept = default;

	public: // members
		Style style;
		string text;
	};

	// relation representation
	class PseoRelation
	{
	public: // creation and destruction of objects
		explicit PseoRelation(PseoTree* next, string const& text = string())
			: next(next), text(text) {}

		virtual ~PseoRelation() noexcept = default;

	public: // members
		PseoTree* next;
		string text;
	};

	// tree view combining blocks and relations
	class PseoTree
	{
		using rels_t = vector<PseoRelation>;

	public: // creation and destruction of objects
		explicit PseoTree(PseoBlock const& block, rels_t const& branches = rels_t())
			: block(block), branches(branches) {}

		virtual ~PseoTree() noexcept = default;

	public: // members
		PseoBlock block;
		rels_t branches;
	};
}

namespace pseolang
{
	using details::PseoTree;
	using details::PseoBlock;
	using details::PseoRelation;
}


#endif // PSEOLANG_REPRESENTATION_HPP
