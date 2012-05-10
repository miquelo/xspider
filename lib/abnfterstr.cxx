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
 * Matcher for string terminal rule.
 */
class abnf_matcher_terstr:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized string terminal matcher.
	 */
	abnf_matcher_terstr(abnf_rule_ri& r, const std::string& str):
	abnf_matcher(r),
	_str(str)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, and only if next characters matches with this matcher
	 * string.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	const std::string& _str;
};

/*
 * String terminal rule.
 */
class abnf_rule_terstr:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized rule.
	 */
	abnf_rule_terstr(const abnf_ruleset& r_set, const char* ter_str):
	abnf_rule_ri(r_set),
	_str(ter_str == NULL ? "" : ter_str)
	{
	}
	
	protected:
	
	/*
	 * Creates a string terminal matcher.
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
	abnf_rule_ri* dupl_impl(const abnf_ruleset& r_set,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const;
			
	private:
	
	const std::string _str;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::terminal(const char* ter_str)
{
	return **_r_set.insert(new abnf_rule_terstr(*this, ter_str)).first;
}

/*
 * abnf_matcher_terstr implementation
 */

void abnf_matcher_terstr::commit_impl(void)
{
}

bool abnf_matcher_terstr::match_impl(istream& is)
{
	if (_str.empty())
		return false;
		
	char c;
	string::const_iterator it = _str.begin();
	while (it not_eq _str.end())
		if (is.bad() or tolower(*it++) not_eq tolower((is.get(c), c)))
			return false;
	return true;
}

/*
 * abnf_rule_terstr implementation
 */

abnf_matcher* abnf_rule_terstr::matcher_new(void)
{
	return new abnf_matcher_terstr(*this, _str);
}

void abnf_rule_terstr::clear_impl(void)
{
}

void abnf_rule_terstr::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_terstr::dupl_impl(const abnf_ruleset& r_set,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_terstr(r_set, _str.c_str());
}
