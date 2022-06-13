#pragma once
#ifndef   PSEOLANG_PSEOROOT_HPP
#define   PSEOLANG_PSEOROOT_HPP


// standard library
#include <memory>
#include <string_view>

// pseolang
#include <pseolang/representation.hpp>


namespace pseolang::details
{
	using std::size_t;
	using std::string_view;
	using std::unique_ptr;


	// this class performs parsing of the text,
	// bringing it to the language representation,
	// and also manages the lifetime of the allocated memory
	class PseoRoot
	{
	public:
		enum class Error
		{
			NO_ERROR = 0,

			UNKNOWN_ERROR = 1,
			NOT_ENOUGH_MEMORY = 2,

			// instruction syntax
			INVALID_INSTRUCTION = 3,
			NO_DELIMITER = 4,

			// unrecognized instruction
			RELATION_WITHOUT_BLOCK = 5,
			INVALID_WHITESPACE_CHAR = 6,
			SPACE_AS_LAST_CHAR_OF_LINE = 7,
			UNACCEPTABLE_SCOPE = 8
		};

	public: // creation and destruction of objects
		explicit PseoRoot(string_view text = string_view()) noexcept;

		// this object cannot have an explicit destructor!
		// ~PseoRoot() noexcept = default; // no virtual

	public: // the default initial state of the object
		void clear() noexcept;
		auto empty() const noexcept -> bool;

	public:
		auto error() const noexcept -> Error
		{ return m_error; }

		auto is_valid() const noexcept -> bool
		{ return m_error == Error::NO_ERROR; }

		auto error_line() const noexcept -> size_t
		{ return m_error_line; }

	public: // tree object
		auto view() const noexcept -> PseoTree*
		{ return m_tree; }

	public: // array to be cleared when the object is destroyed
		auto ownership() noexcept -> vector<unique_ptr<PseoTree>>&
		{ return m_storage; }

	private: // members
		Error m_error;
		size_t m_error_line;

		PseoTree* m_tree;
		vector<unique_ptr<PseoTree>> m_storage;
	};
}

namespace pseolang
{ using details::PseoRoot; }


#endif // PSEOLANG_PSEOROOT_HPP
