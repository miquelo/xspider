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

#include <climits>

#include "abnfm.h"

namespace xspider {

/*
 * Matcher for concatenation rule.
 */
class abnf_matcher_con:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized concatenation matcher.
	 */
	abnf_matcher_con(abnf_rule_ri& r, abnf_rule_ri& rl, abnf_rule_ri& rr):
	abnf_matcher(r),
	_l_test(true),
	_rr(rr),
	_ml(rl.matcher_new()),
	_mr(rr.matcher_new())
	{
	}
	
	/*
	 * Delete left and right matchers.
	 */
	~abnf_matcher_con(void);
	
	/*
	 * Available if, and only if any of two matchers is available.
	 */
	bool available(void) const;
	
	protected:
	
	/*
	 * Commit left and right matchers.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, and only if two matchers matches.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	bool _l_test;
	abnf_rule_ri& _rr;
	abnf_matcher* _ml;
	abnf_matcher* _mr;
};

/*
 * Concatenation rule.
 */
class abnf_rule_con:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized concatenation rule.
	 */
	abnf_rule_con(const abnf_ruleset& r_set, abnf_rule_ri& rl,
			abnf_rule_ri& rr):
	abnf_rule_ri(r_set),
	_rl(rl),
	_rr(rr)
	{
	}
	
	protected:
	
	/*
	 * Creates a concatenation matcher.
	 */
	abnf_matcher* matcher_new(void);
	
	/*
	 * Clear left and right rules.
	 */
	void clear_impl(void);
	
	/*
	 * Update stream of left and right rules.
	 */
	void stream_update_impl(std::istream& is);
	
	/*
	 * Duplicate operation implementation.
	 */
	abnf_rule_ri* dupl_impl(const abnf_ruleset& r_set,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const;
			
	private:
	
	abnf_rule_ri& _rl;
	abnf_rule_ri& _rr;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::concat(abnf_rule& rl, abnf_rule& rr)
{
	owner_test(*this, rl);
	owner_test(*this, rr);
	
	abnf_rule_ri& rl_ri = abnf_rule_ri::cast(rl);
	abnf_rule_ri& rr_ri = abnf_rule_ri::cast(rr);
	return **_r_set.insert(new abnf_rule_con(*this, rl_ri, rr_ri)).first;
}

/*
 * abnf_matcher_con implementation
 */

abnf_matcher_con::~abnf_matcher_con(void)
{
	delete _ml;
	delete _mr;
}

bool abnf_matcher_con::available(void) const
{
	return _ml->available() or _mr->available();
}

void abnf_matcher_con::commit_impl(void)
{
	_ml->commit();
	_mr->commit();
}

bool abnf_matcher_con::match_impl(istream& is)
{
	bool l_matched = false;
	bool r_matched = false;
	
	do
	{
		is.clear();
		is.seekg(stream_beg());
			
		if (l_matched = _l_test ? _ml->match(is) : true)
		{
			is.seekg(_ml->stream_end());
			
			if (_l_test = not (r_matched = _mr->match(is)))
				_mr = (delete _mr, _rr.matcher_new());
		}
	}
	while (l_matched and not r_matched);
	
	return l_matched and r_matched;
}

/*
 * abnf_rule_con implementation
 */

abnf_matcher* abnf_rule_con::matcher_new(void)
{
	return new abnf_matcher_con(*this, _rl, _rr);
}

void abnf_rule_con::clear_impl(void)
{
	_rl.clear();
	_rr.clear();
}

void abnf_rule_con::stream_update_impl(std::istream& is)
{
	_rl.stream_update(is);
	_rr.stream_update(is);
}

abnf_rule_ri* abnf_rule_con::dupl_impl(const abnf_ruleset& r_set,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_con(r_set, _rl, _rr);
}
