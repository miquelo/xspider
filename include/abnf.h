/*
 * This file is part of the XSpider project.
 *
 * Copyright (C) 2012-2013 Miquel Ferran <miquel.ferran.gonzalez@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
 
#ifndef ABNF_H
#define ABNF_H

#include <istream>
#include <map>
#include <ostream>
#include <set>

/*!
 * \file
 * \brief <i>Augmented Backus - Naur Form</i> (ABNF) utility.
 */

namespace xspider {

class abnf_rule;

/*!
 * \brief ABFN rule set.
 */
class abnf_ruleset
{
	public:
	
	/*!
	 * \brief Ruleset with core ABNF rules as defined in RFC 5234.
	 *
	 * \return
	 * 			A reference to the core ruleset.
	 */
	static const abnf_ruleset& core_ruleset(void);
	
	/*!
	 * \brief Creates an empty rule set.
	 */
	abnf_ruleset(void);
	
	/*!
	 * \brief Copies the given rule set by including its rules and definitions
	 * to this empty rule set.
	 *
	 * \param rset
	 *			A ruleset to be copied.
	 */
	abnf_ruleset(const abnf_ruleset& rset);
	
	/*!
	 * \brief Releases this rule set.
	 */
	~abnf_ruleset(void);
	
	/*!
	 * \brief Includes to this rule set those rules and definitions of the
	 * given rule set.
	 *
	 * \param rset
	 *			Included rule set.
	 */
	void include(const abnf_ruleset& rset);
	
	/*!
	 * \brief Indicates if a rule was defined with the given name.
	 *
	 * \param r_name
	 *			Case insensitive name of a rule.
	 *
	 * \retval true
	 *			if a rule was defined with \p r_name;
	 * \retval false
	 *			otherwise.
	 */
	bool defined(const char* r_name) const;
	
	/*!
	 * \brief Get the named rule from this rule set.
	 *
	 * If there is not any rule with the given case insensitive name, an empty
	 * rule will be returned. An empty rule is a rule which does not match with
	 * anything.
	 *
	 * \param r_name
	 *			Case insensitive mame of the rule.
	 *
	 * \return
	 *			The named rule or an empty rule if it does not exist.
	 */
	abnf_rule& get(const char* r_name) const;
	
	/*!
	 * \brief Gives a name to an specific rule in order to be accessed.
	 *
	 * Rules created by a rule set are initially annonymous. This method make
	 * them named and accessible with \link get \endlink.
	 *
	 * \param r_name
	 *			A case insentitive name for the given rule.
	 * \param r
	 *			A rule to be named.
	 *
	 * \return
	 *			The named rule.
	 *
	 * \throw std::invalid_argument
	 *			If \p r is not created by this rule set.
	 */
	abnf_rule& define(const char* r_name, abnf_rule& r);
	
	/*!
	 * \brief Creates an special rule for an EOF reaching.
	 *
	 * Matches when the end of stream has been reached.
	 *
	 * \return
	 *			The created EOF rule.
	 */
	abnf_rule& eof(void);
	
	/*!
	 * \brief Creates a terminal rule with a single character for this rule
	 * set.
	 *
	 * \param ter_ch
	 *			A single character defining the terminal rule.
	 *
	 * \return
	 *			The created terminal rule.
	 */
	abnf_rule& terminal(int ter_ch);
	
	/*!
	 * \brief Creates a terminal rule with a case insensitive character string
	 * for this rule set.
	 *
	 * \param ter_str
	 *			A case insensitive character string defining the terminal rule.
	 *			If it is empty, anything matches.
	 *
	 * \return
	 *			The created terminal rule.
	 */
	abnf_rule& terminal(const char* ter_str);
	
	/*!
	 * \brief Creates a terminal rule with a character testing function which
	 * determines if a character is a matching character for the terminal.
	 *
	 * For example,
	 *
	 * \code
	 * abnf_ruleset rset;
	 * abnf_rule& r_digit = rset.terminal(isdigit);
	 * \endcode
	 *
	 * creates a rule of a terminal whose matching consist in digit characters,
	 * through \<cctype\>'s \c isdigit function.
	 *
	 * Of course, it can be used with a custom character testing function.
	 *
	 * For example,
	 *
	 * \code
	 * int isvowel(int c)
	 * {
	 *     const char* vowels = "AEIOUaeiou";
	 *     for (int i = 0; i < 10; ++i)
	 *         if (vowels[i] == c)
	 *             return 1;
	 *     return 0;
	 * }
	 *
	 * ...
	 *
	 * abnf_ruleset rset;
	 * abnf_rule& r_vowel = rset.terminal(isvowel);
	 * \endcode
	 *
	 * \param ter_fn
	 *			A character testing function. If \c null, no one character
	 *			matches.
	 *
	 * \return
	 *			The created terminal rule.
	 */
	abnf_rule& terminal(int (*ter_fn)(int));
	
	/*!
	 * \brief Creates a concatenation of two rules.
	 *
	 * It can be used to create a concatenation of more than two rules using
	 * it repeatedly.
	 *
	 * For example,
	 *
	 * \code
	 * abnf_ruleset rset;
	 * abnf_rule& r_foo = rset.terminal("foo");
	 * abnf_rule& r_bar = rset.terminal("bar");
	 * abnf_rule& r_barfoo = rset.concat(r_bar, r_foo);
	 * abnf_rule& r_concat = rset.concat(r_foo, r_barfoo);
	 * \endcode
	 *
	 * creates the <tt>"foo" "bar" "foo"</tt> rule.
	 *
	 * \param rl
	 *			Left rule.
	 * \param rr
	 *			Right rule.
	 *
	 * \return
	 *			The created concatenation rule of \p rl and \p rr.
	 *
	 * \throw std::invalid_argument
	 *			If \p rl|rr is not created by this rule set.
	 */
	abnf_rule& concat(abnf_rule& rl, abnf_rule& rr);
	
	/*!
	 * \brief Creates an alternative rule consisting in a range of characters
	 * from \p ci and \p ce, both included.
	 *
	 * An alternative with <tt>ci ≥ ce</tt> is the same of a terminal rule with
	 * \p ci character.
	 *
	 * \param ci
	 *			Initial character value of the alternative range, included.
	 * \param ce
	 *			Ending character value of the alternative range, included.
	 *
	 * \return
	 *			The created <tt>[ci,ce]</tt> range alternative rule.
	 */
	abnf_rule& alternat(int ci, int ce);
	
	/*!
	 * \brief Creates a convenient rule consisting in a multiple alternative
	 * characters.
	 *
	 * It is the same as a multiple consecutive alternatives of character
	 * terminals.
	 *
	 * For example,
	 *
	 * \code
	 * abnf_ruleset rset;
	 * abnf_rule& r_foo = rset.alternat("%&$");
	 * \endcode
	 *
	 * creates the <tt>"%" / "&" / "$"</tt> rule.
	 *
	 * \param altch
	 *			Alternative characters.
	 *
	 * \return
	 *			The created character alternatives rule.
	 */
	abnf_rule& alternat(const char* altch);
	
	/*!
	 * \brief Creates an alternative rule of two rules.
	 *
	 * It can be used to create an alternative list of more than two rules
	 * using it repeatedly.
	 *
	 * For example,
	 *
	 * \code
	 * abnf_ruleset rset;
	 * abnf_rule& r_foo = rset.terminal("foo");
	 * abnf_rule& r_bar = rset.terminal("bar");
	 * abnf_rule& r_baz = rset.terminal("baz");
	 * abnf_rule& r_foobar = rset.alternat(r_foo, r_bar);
	 * abnf_rule& r_altern = rset.alternat(r_foobar, r_baz);
	 * \endcode
	 *
	 * creates the <tt>"foo" / "bar" / "baz"</tt> rule.
	 *
	 * \param rl
	 *			Left rule.
	 * \param rr
	 *			Right rule.
	 *
	 * \return
	 *			The created alternative rule of \p rl and \p rr.
	 *
	 *
	 * \throw std::invalid_argument
	 *			If \p rr|rl is not created by this rule set.
	 */
	abnf_rule& alternat(abnf_rule& rl, abnf_rule& rr);
	
	/*!
	 * \brief Creates a repetition rule for the given rule with a minimum
	 * occurrences value.
	 *
	 * It can be used to create an optional rule.
	 *
	 * For example,
	 *
	 * \code
	 * abnf_ruleset rset;
	 * abnf_rule& r_foo = rset.terminal("foo");
	 * abnf_rule& r_optional = rset.repet(0, r_foo);
	 * \endcode
	 *
	 * creates the \c *"foo" rule, which is equivalent of <tt>["foo"]</tt>.
	 *
	 * A repetition with <tt>r_min < 0</tt> is the same of a repetition rule
	 * with <tt>r_min = 0</tt>.
	 *
	 * \param r_min
	 *			Minimum occurrences of the given rule.
	 * \param r
	 *			Rule which may appear repeatedly.
	 *
	 * \return
	 *			The created repetition rule.
	 *
	 * \throw std::invalid_argument
	 *			If \p r is not created by this rule set.
	 */
	abnf_rule& repet(int r_min, abnf_rule& r);
	
	/*!
	 * \brief Creates a repetition rule for the given rule with a minimum
	 * and maximum occurrences value.
	 *
	 * A repetition with <tt>r_min < 0</tt> is the same of a repetition rule
	 * with <tt>r_min = 0</tt>.
	 *
	 * A repetition with <tt>r_max < 0</tt> is the same of a repetition rule
	 * with <tt>r_max = 0</tt>.
	 *
	 * A repetition with <tt>r_min ≥ r_max</tt> is the same of a repetition
	 * rule with <tt>r_min = r_max</tt>.
	 *
	 * \param r_min
	 *			Minimum occurrences of the given rule.
	 * \param r_max
	 *			Maximum occurrences of the given rule.
	 * \param r
	 *			Rule which may appear repeatedly.
	 *
	 * \return
	 *			The created repetition rule.
	 *
	 * \throw std::invalid_argument
	 *			If \p r is not created by this rule set.
	 */
	abnf_rule& repet(int r_min, int r_max, abnf_rule& r);
	
	private:
	
	static abnf_ruleset _core_rset;
	abnf_rule* _empty_r;
	std::set<abnf_rule*> _r_set;
	std::map<std::string, abnf_rule*> _r_map;
};

/*!
 * \brief ABNF rule.
 */
class abnf_rule
{
	public:
	
	/*!
	 * \brief Owner ruleset.
	 *
	 * \return
	 *			The ruleset which owns this rule.
	 */
	const abnf_ruleset& ruleset(void) const;
	
	/*!
	 * \brief Clear the matching results from the last \link read \endlink
	 * operation for this rule tree.
	 */
	virtual void clear(void) = 0;
	
	/*!
	 * \brief Read from the given stream and store the mathing results to this
	 * rule tree.
	 *
	 * The results will be available for this rule until next read or \link
	 * clear \endlink operation.
	 *
	 * \param is
	 *			Content stream.
	 */
	virtual void read(std::istream& is) = 0;
	
	/*!
	 * \brief Number of stream segments matching this rule from the last \link
	 * read \endlink operation.
	 *
	 * \return
	 *			The matching segment count.
	 */
	virtual size_t read_count(void) const = 0;
	
	/*!
	 * \brief Write the <tt>n</tt>th matching segment of this rule, from the
	 * last \link read \endlink operation, to the given stream.
	 *
	 * \param n
	 *			Index of matching segment.
	 * \param os
	 *			Stream where the matching segment must be written.
	 */
	virtual void write(size_t n, std::ostream& os) const = 0;
	
	protected:
	
	/*!
	 * \brief Creates an owned rule.
	 *
	 * \param rset
	 *			Owner ruleset.
	 */
	abnf_rule(const abnf_ruleset& rset);
	
	private:
	
	const abnf_ruleset& _rset;
};

} // namespace xspider

#endif // ABNF_H
