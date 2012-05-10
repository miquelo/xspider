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
	_max(r_max),
	_ru(ru),
	_first_try(true)
	{
		for (int i = 0; i < r_min; ++i)
			_m_vect.push_back(_ru.matcher_new());
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
	
	const int _max;
	abnf_rule_ri& _ru;
	bool _first_try;
	std::vector<abnf_matcher*> _m_vect;
	
	void _reset_remaining(const std::vector<abnf_matcher*>::iterator from)
	{
		std::vector<abnf_matcher*>::iterator it = from;
		while (it not_eq _m_vect.end())
		{
			delete *it;
			*it++ = _ru.matcher_new();
		}
	}
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
	vector<abnf_matcher*>::const_iterator it = _m_vect.begin();
	while (it not_eq _m_vect.end())
		if ((*it)->available())
			return true;
	return _m_vect.size() < _max;
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
	if (_first_try)
	{
		cout << "matched = true, size = 0" << endl;
		
		_first_try = false;
		if (_m_vect.empty())
			return true;
	}
	
	bool avail = false;
	vector<abnf_matcher*>::reverse_iterator rit = _m_vect.rbegin();
	while (rit not_eq _m_vect.rend() and not avail)
		if (not (avail = (*rit)->available()))
			++rit;
	if (avail)
	{
		if (rit not_eq _m_vect.rbegin())
			_reset_remaining(rit.base() + 1);
		(*rit)->mismatch();
	}
	else if (_m_vect.size() < _max)
		_m_vect.push_back(_ru.matcher_new());
	
	bool mismatched = false;
	bool matched = false;
	bool rollback = false;
	vector<abnf_matcher*>::iterator it = _m_vect.begin();
	
	while (not mismatched and not matched)
	{
		abnf_matcher* m = *it;
	
		if (rollback)
		{
			is.seekg(m->stream_beg());
			m->mismatch();
			rollback = false;
		}
	
		if (m->match(is))
			matched = ++it == _m_vect.end();
		else if (it == _m_vect.begin())
			mismatched = true;
		else
		{
			rollback = true;
			_reset_remaining(it--);
		}
	}
	cout << "matched = " << (matched ? "true" : "false");
	cout << ", size = " << _m_vect.size() << endl;
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
