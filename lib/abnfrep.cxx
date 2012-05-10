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
 * Matcher for repetition rule.
 */
class abnf_matcher_rep:
public abnf_matcher
{
	public:
	
	/*
	 * Initialized repetition matcher.
	 *
	 * Store r_min matchers.
	 */
	abnf_matcher_rep(abnf_rule_ri& r, int r_min, int r_max, abnf_rule_ri& ru):
	abnf_matcher(r),
	_min(r_min),
	_max(r_max),
	_ru(ru),
	_count(_min),
	_last_pos(-1l)
	{
	}
	
	/*
	 * Delete stored matchers.
	 */
	~abnf_matcher_rep(void);
	
	protected:
	
	/*
	 * Available if, and only if any matcher is available or maximum is not
	 * reached.
	 */
	bool availability_test(void) const;
	
	/*
	 * Commit stored matchers.
	 */
	void commit_impl(void);
	
	/*
	 * Matches if, and only if all matchers matches.
	 */
	bool match_impl(std::istream& is);
			
	private:
	
	const int _min, _max;
	abnf_rule_ri& _ru;
	int _count;
	std::streampos _last_pos;
	std::vector<abnf_matcher*> _m_vect;
};

/*
 * Repetition rule.
 */
class abnf_rule_rep:
public abnf_rule_ri
{
	public:
	
	/*
	 * Initialized repetition rule.
	 */
	abnf_rule_rep(const abnf_ruleset& r_set, int r_min, int r_max,
			abnf_rule_ri& r):
	abnf_rule_ri(r_set),
	_min(std::max(0, r_min)),
	_max(std::max(_min, r_max)),
	_r(r)
	{
	}
	
	protected:
	
	/*
	 * Creates a repetition matcher.
	 */
	abnf_matcher* matcher_new(void);
	
	/*
	 * Clear repeated rule.
	 */
	void clear_impl(void);
	
	/*
	 * Update stream of repeated rule.
	 */
	void stream_update_impl(std::istream& is);
	
	/*
	 * Duplicate operation implementation.
	 */
	abnf_rule_ri* dupl_impl(const abnf_ruleset& r_set,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const;
			
	private:
	
	const int _min, _max;
	abnf_rule_ri& _r;
};

} // namespace xspider

using namespace std;
using namespace xspider;

/*
 * abnf_ruleset implementation
 */
 
abnf_rule& abnf_ruleset::repet(int r_min, abnf_rule& r)
{
	return repet(r_min, INT_MAX, r);
}

abnf_rule& abnf_ruleset::repet(int r_min, int r_max, abnf_rule& r)
{
	owner_test(*this, r);
	
	abnf_rule_ri& r_ri = abnf_rule_ri::cast(r);
	return **_r_set.insert(new abnf_rule_rep(*this, r_min, r_max, r_ri)).first;
}

/*
 * abnf_matcher_rep implementation
 */
 
abnf_matcher_rep::~abnf_matcher_rep(void)
{
	vector<abnf_matcher*>::const_iterator it = _m_vect.begin();
	while (it not_eq _m_vect.end())
		delete *it++;
}

bool abnf_matcher_rep::availability_test(void) const
{
	return _count not_eq -1;
}

void abnf_matcher_rep::commit_impl(void)
{
	vector<abnf_matcher*>::iterator it = _m_vect.begin();
	while (it not_eq _m_vect.end())
		(*it++)->commit();
}
#include <iostream>
bool abnf_matcher_rep::match_impl(istream& is)
{
	bool mismatched = false;
	bool matched = _count == 0;
	
	if (_last_pos == -1l)
		_last_pos = stream_beg();
	
	while (not mismatched and not matched)
	{
		if (_m_vect.empty())
			_m_vect.push_back(_ru.matcher_new());
			
		is.seekg(_last_pos);
		
		if (_m_vect.back()->match(is))
		{
			_last_pos = _m_vect.back()->stream_end();
			
			if (_m_vect.size() == _count)
				matched = true;
			else
				_m_vect.push_back(_ru.matcher_new());
		}
		else
		{
			delete _m_vect.back();
			_m_vect.pop_back();
			_count = max(_count - 1, _min);
			
			if (_m_vect.empty())
				mismatched = true;
			else
			{
				_m_vect.back()->mismatch();
				_last_pos = _m_vect.back()->stream_beg();
			}
		}
	}
	
	if (_count == _max)
		_count == -1;
	else
	{
		++_count;
		_m_vect.push_back(_ru.matcher_new());
	}
	return matched;
}

/*
 * abnf_rule_rep implementation
 */

abnf_matcher* abnf_rule_rep::matcher_new(void)
{
	return new abnf_matcher_rep(*this, _min, _max, _r);
}

void abnf_rule_rep::clear_impl(void)
{
	_r.clear();
}

void abnf_rule_rep::stream_update_impl(std::istream& is)
{
	_r.stream_update(is);
}

abnf_rule_ri* abnf_rule_rep::dupl_impl(const abnf_ruleset& r_set,
		map<const abnf_rule*, abnf_rule_ri*>& d_map) const
{
	return new abnf_rule_rep(r_set, _min, _max, _r);
}
