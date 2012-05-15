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

#include <algorithm>

#include "abnfm.h"

namespace xspider {

/*
 * Matcher for an empty rule.
 */
class abnf_matcher_empty:
public abnf_matcher
{
	public:
	
	/*
	 * Matcher for the given empty rule.
	 */
	abnf_matcher_empty(abnf_rule_ri& r):
	abnf_matcher(r)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Nothing to be done. Returns false.
	 */
	bool match_impl(std::istream& is);
};

class abnf_rule_empty:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized empty rule.
	 */
	abnf_rule_empty(const abnf_ruleset& rset):
	abnf_rule_ri(rset)
	{
	}
	
	protected:
	
	/*
	 * Creates a new empty matcher.
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
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_ruleset abnf_ruleset::_core_rset;

abnf_ruleset::abnf_ruleset(void):
_empty_r(new abnf_rule_empty(*this))
{
}

abnf_ruleset::abnf_ruleset(const abnf_ruleset& rset):
_empty_r(new abnf_rule_empty(*this))
{
	include(rset);
}

abnf_ruleset::~abnf_ruleset(void)
{
	set<abnf_rule*>::const_iterator it = _r_set.begin();
	while (it not_eq _r_set.end())
		delete *it++;
}

void abnf_ruleset::include(const abnf_ruleset& rset)
{
	std::map<const abnf_rule*, abnf_rule_ri*> d_map;
	
	// Duplicates all rules of copied rule set and store them to d_map
	set<abnf_rule*>::const_iterator a_it = rset._r_set.begin();
	while (a_it not_eq rset._r_set.end())
		abnf_rule_ri::cast(**a_it++).dupl(*this, d_map);
		
	// Store duplicated rules to this rule set
	map<const abnf_rule*, abnf_rule_ri*>::const_iterator it = d_map.begin();
	while (it not_eq d_map.end())
		_r_set.insert(it++->second);
		
	// Define rules as are defined in copied rule set
	map<string, abnf_rule*>::const_iterator m_it = rset._r_map.begin();
	while (m_it not_eq rset._r_map.end())
		_r_map[m_it->first] = d_map[m_it++->second];
}

bool abnf_ruleset::defined(const char* r_name) const
{
	string str = r_name;
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	map<string, abnf_rule*>::const_iterator it = _r_map.find(str);
	
	// If found, defined. If not, undefined
	return it not_eq _r_map.end();
}

abnf_rule& abnf_ruleset::get(const char* r_name) const
{
	string str = r_name;
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	map<string, abnf_rule*>::const_iterator it = _r_map.find(str);
	
	// Not defined, give the empty rule
	if (it == _r_map.end())
		return *_empty_r;
	
	// Defined rule
	return *it->second;
}

abnf_rule& abnf_ruleset::define(const char* r_name, abnf_rule& r)
{
	owner_test(*this, r);
	
	string str = r_name;
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return *(_r_map[str] = &r);
}

/*
 * abnf_matcher_empty implementation
 */

void abnf_matcher_empty::commit_impl(void)
{
}

bool abnf_matcher_empty::match_impl(istream& is)
{
	return false;
}

/*
 * abnf_rule_empty implementation
 */

abnf_matcher* abnf_rule_empty::matcher_new(void)
{
	return new abnf_matcher_empty(*this);
}

void abnf_rule_empty::clear_impl(void)
{
}

void abnf_rule_empty::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_empty::dupl_impl(const abnf_ruleset& rset,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_empty(rset);
}
