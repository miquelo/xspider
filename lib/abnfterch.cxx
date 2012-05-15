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
 * Matcher for character terminal rule.
 */
class abnf_matcher_terch:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized character terminal matcher.
	 */
	abnf_matcher_terch(abnf_rule_ri& r, int& ch):
	abnf_matcher(r),
	_ch(ch)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, and only if next character is equals than this matcher
	 * character.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	const int& _ch;
};

/*
 * Character terminal rule.
 */
class abnf_rule_terch:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized rule.
	 */
	abnf_rule_terch(const abnf_ruleset& rset, int ch):
	abnf_rule_ri(rset),
	_ch(ch)
	{
	}
	
	protected:
	
	/*
	 * Creates a character terminal matcher.
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
	
	int _ch;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::terminal(int ter_ch)
{
	return **_r_set.insert(new abnf_rule_terch(*this, ter_ch)).first;
}

/*
 * abnf_matcher_terch implementation
 */
 
void abnf_matcher_terch::commit_impl(void)
{
}

bool abnf_matcher_terch::match_impl(istream& is)
{
	char c;
	return is.good() and (is.get(c), c) == _ch;
}

/*
 * abnf_rule_terch implementation
 */

abnf_matcher* abnf_rule_terch::matcher_new(void)
{
	return new abnf_matcher_terch(*this, _ch);
}

void abnf_rule_terch::clear_impl(void)
{
}

void abnf_rule_terch::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_terch::dupl_impl(const abnf_ruleset& rset,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_terch(rset, _ch);
}
