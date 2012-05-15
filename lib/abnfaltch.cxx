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

#include <cstring>

#include "abnfm.h"

namespace xspider {

/*
 * Matcher for characters alternate rule.
 */
class abnf_matcher_altch:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized characters alternate matcher.
	 */
	abnf_matcher_altch(abnf_rule_ri& r, const char*& altch):
	abnf_matcher(r),
	_altch(altch)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if next character is in alternative characters string.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	const char*& _altch;
};

/*
 * Characters alternate rule.
 */
class abnf_rule_altch:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized range alternate rule.
	 */
	abnf_rule_altch(const abnf_ruleset& rset, const char* altch):
	abnf_rule_ri(rset),
	_altch(altch)
	{
	}
	
	protected:
	
	/*
	 * Creates a characters alternate matcher.
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
	
	const char* _altch;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::alternat(const char* altch)
{
	return **_r_set.insert(new abnf_rule_altch(*this, altch)).first;
}

/*
 * abnf_matcher_altch implementation
 */
 
void abnf_matcher_altch::commit_impl(void)
{
}

bool abnf_matcher_altch::match_impl(istream& is)
{
	if (is.bad())
		return false;
	
	char c;
	is.get(c);
	size_t len = strlen(_altch);
	
	for (int i = 0; i < len; ++i)
		if (_altch[i] == c)
			return true;
	return false;
}

/*
 * abnf_rule_altch implementation
 */

abnf_matcher* abnf_rule_altch::matcher_new(void)
{
	return new abnf_matcher_altch(*this, _altch);
}

void abnf_rule_altch::clear_impl(void)
{
}

void abnf_rule_altch::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_altch::dupl_impl(const abnf_ruleset& rset,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_altch(rset, _altch);
}
