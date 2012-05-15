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

#include "abnfm.h"

namespace xspider {

/*
 * Matcher for function terminal rule.
 */
class abnf_matcher_terfn:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized function terminal matcher.
	 */
	abnf_matcher_terfn(abnf_rule_ri& r, int (* const & fn)(int)):
	abnf_matcher(r),
	_fn(fn)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, and only if next character evals to greater than zero for
	 * the terminal function.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	int (* const & _fn)(int);
};

/*
 * Used when an empty terminal function is given.
 *
 * Always return zero.
 */
static int abnf_terfn_false(int c);

/*
 * Function terminal rule.
 */
class abnf_rule_terfn:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized rule.
	 */
	abnf_rule_terfn(const abnf_ruleset& rset, int (*ter_fn)(int)):
	abnf_rule_ri(rset),
	_fn(ter_fn == NULL ? abnf_terfn_false : ter_fn)
	{
	}
	
	protected:
	
	/*
	 * Creates a function terminal matcher.
	 */
	abnf_matcher* matcher_new(void);
	
	/*
	 * Nothing to be done.
	 */
	void clear_impl(void);
	
	/*
	 * Nothing to be done.
	 */
	void stream_update_impl(std::istream& is);
	
	/*
	 * Duplicate operation implementation.
	 */
	abnf_rule_ri* dupl_impl(const abnf_ruleset& rset,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const;
			
	private:
	
	int (* const _fn)(int);
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::terminal(int (*ter_fn)(int))
{
	return **_r_set.insert(new abnf_rule_terfn(*this, ter_fn)).first;
}

/*
 * abnf_matcher_terfn implementation
 */

void abnf_matcher_terfn::commit_impl(void)
{
}

bool abnf_matcher_terfn::match_impl(istream& is)
{
	char c;
	return is.good() and _fn((is.get(c), c)) > 0;
}

/*
 * abnf_rule_terfn implementation
 */
 
int xspider::abnf_terfn_false(int c)
{
	return 0;
}

abnf_matcher* abnf_rule_terfn::matcher_new(void)
{
	return new abnf_matcher_terfn(*this, _fn);
}

void abnf_rule_terfn::clear_impl(void)
{
}

void abnf_rule_terfn::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_terfn::dupl_impl(const abnf_ruleset& rset,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_terfn(rset, _fn);
}
