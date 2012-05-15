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
 * Matcher for range alternate rule.
 */
class abnf_matcher_ralt:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized range alternate matcher.
	 */
	abnf_matcher_ralt(abnf_rule_ri& r, const int& ci, const int& ce):
	abnf_matcher(r),
	_ci(ci),
	_ce(ce)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, next character is in characters range.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	const int& _ci;
	const int& _ce;
};

/*
 * Range alternate rule.
 */
class abnf_rule_ralt:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized range alternate rule.
	 */
	abnf_rule_ralt(const abnf_ruleset& rset, int ci, int ce):
	abnf_rule_ri(rset),
	_ci(ci),
	_ce(std::max(ci, ce))
	{
	}
	
	protected:
	
	/*
	 * Creates a range alternate matcher.
	 */
	abnf_matcher* matcher_new(void);
	
	/*
	 * Nothing to be done
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
	
	const int _ci, _ce;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::alternat(int ci, int ce)
{
	return **_r_set.insert(new abnf_rule_ralt(*this, ci, ce)).first;
}

/*
 * abnf_matcher_ralt implementation
 */
 
void abnf_matcher_ralt::commit_impl(void)
{
}

bool abnf_matcher_ralt::match_impl(istream& is)
{
	char c;
	return is.good() and (is.get(c), c) >= _ci and c <= _ce;
}

/*
 * abnf_rule_ralt implementation
 */

abnf_matcher* abnf_rule_ralt::matcher_new(void)
{
	return new abnf_matcher_ralt(*this, _ci, _ce);
}

void abnf_rule_ralt::clear_impl(void)
{
}

void abnf_rule_ralt::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_ralt::dupl_impl(const abnf_ruleset& rset,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_ralt(rset, _ci, _ce);
}
