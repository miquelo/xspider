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
 * Matcher for EOF rule.
 */
class abnf_matcher_eof:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized EOF matcher.
	 */
	abnf_matcher_eof(abnf_rule_ri& r):
	abnf_matcher(r)
	{
	}
	
	protected:
	
	/*
	 * Nothing to be done.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if EOF has been reached.
	 */
	bool match_impl(std::istream& is);
};

/*
 * Characters alternate rule.
 */
class abnf_rule_eof:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized EOF rule.
	 */
	abnf_rule_eof(const abnf_ruleset& r_set):
	abnf_rule_ri(r_set)
	{
	}
	
	protected:
	
	/*
	 * Creates an EOF matcher.
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
	abnf_rule_ri* dupl_impl(const abnf_ruleset& r_set,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::eof(void)
{
	return **_r_set.insert(new abnf_rule_eof(*this)).first;
}

/*
 * abnf_matcher_eof implementation
 */
 
void abnf_matcher_eof::commit_impl(void)
{
}

bool abnf_matcher_eof::match_impl(istream& is)
{
	bool eof = (is.peek(), is.eof());
	is.clear();
	return eof;
}

/*
 * abnf_rule_eof implementation
 */

abnf_matcher* abnf_rule_eof::matcher_new(void)
{
	return new abnf_matcher_eof(*this);
}

void abnf_rule_eof::clear_impl(void)
{
}

void abnf_rule_eof::stream_update_impl(std::istream& is)
{
}

abnf_rule_ri* abnf_rule_eof::dupl_impl(const abnf_ruleset& r_set,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_eof(r_set);
}
