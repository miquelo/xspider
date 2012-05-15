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

#ifndef ABNFR_H
#define ABNFR_H

#include <stdexcept>
#include <vector>

#include "abnf.h"

namespace xspider {

class abnf_matcher;

/*
 * Stream segment, with begin and end positions, begin included.
 */
class abnf_segment
{
	public:
	
	/*
	 * Uninitialized empty segment.
	 */
	abnf_segment(void):
	_beg(0l),
	_end(0l)
	{
	}
	
	/*
	 * Initialized segment.
	 *
	 * Precondition:
	 *		0 ≤ beg < end
	 */
	abnf_segment(std::streampos beg, std::streampos end):
	_beg(beg),
	_end(end)
	{
	}
	
	/*
	 * Write segment delimited content of given input stream to the given
	 * output stream.
	 *
	 * Postcondition:
	 *		is.tellg() = is'.tellg()
	 */
	void write(std::istream& is, std::ostream& os) const
	{
		std::streampos pos = is.tellg();
		char c;
		is.seekg(_beg);
		while (is.good() and os.good() and is.tellg() < _end)
			os.put((is.get(c), c));
		is.seekg(pos);
	}
	
	private:
	
	std::streampos _beg, _end;
};

/*
 * Rule reference implementation.
 */
class abnf_rule_ri:
public abnf_rule
{
	public:
	
	/*
	 * Reference implementation casting from a reference rule.
	 */
	static abnf_rule_ri& cast(abnf_rule& r)
	{
		return dynamic_cast<abnf_rule_ri&>(r);
	}
	
	/*
	 * Initialized rule, with its owner, empty stream and empty segment vector.
	 */
	abnf_rule_ri(const abnf_ruleset& rset):
	abnf_rule(rset),
	_is(NULL)
	{
	}
	
	/*
	 * Returns this rule to its initial state. It uses clear_impl to do the
	 * same with its children.
	 *
	 * Postcondition:
	 *		empty stream
	 *		empty segment vector
	 */
	void clear();
	
	/*
	 * Perform a matching operation of this rule on to the given stream.
	 *
	 * Postcondition:
	 *		stream initialized for whole rule tree according the given stream
	 *		segment vector filled according the matching operation
	 */
	void read(std::istream& is);
	
	/*
	 * Number of segments stored at last read operation on to this rule of any
	 * of its parents.
	 */
	size_t read_count(void) const;
	
	/*
	 * Write the nth segment content of the current stream on to the given
	 * output stream.
	 *
	 * If n < 0 or n ≥ size of segment vector or current stream is empty,
	 * nothing is done.
	 */
	void write(size_t n, std::ostream& os) const;
	
	/*
	 * Add a beg,end segment to this rule.
	 *
	 * Precondition:
	 *		beg < end
	 * Postcondition:
	 *		_seg_vect contains a beg,end segment
	 */
	void segment_add(std::streampos beg, std::streampos end)
	{
		_seg_vect.push_back(abnf_segment(beg, end));
	}
	
	/*
	 * Updates the current stream of this rule and its children through
	 * stream_update_impl.
	 *
	 * Postcondition:
	 *		current stream of this rule tree points to the given stream address
	 */
	void stream_update(std::istream& is)
	{
		stream_update_impl(is);
		_is = &is;
	}
	
	/*
	 * Duplicates this rule with the given rule set as an owner.
	 *
	 * If its duplicated is in the given map it is retrieved and returned.
	 * Otherwise, it is duplicated throug dupl_impl and stored to the map.
	 *
	 * Postcondition:
	 *		d_map contains an entry with this as key and duplicated as value
	 */
	abnf_rule_ri* dupl(const abnf_ruleset& rset,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const
	{
		std::map<const abnf_rule*, abnf_rule_ri*>::const_iterator it =
				d_map.find(this);
		if (it not_eq d_map.end())
			return it->second;
		return d_map[this] = dupl_impl(rset, d_map);
	}
	
	/*
	 * Creates a matcher adequate to this rule.
	 */
	virtual abnf_matcher* matcher_new(void) = 0;
	
	protected:

	/*
	 * Call clear recursively on to children rules.
	 */
	virtual void clear_impl(void) = 0;
	
	/*
	 * Call stream_update recursively on to children rules.
	 */
	virtual void stream_update_impl(std::istream& is) = 0;
	
	/*
	 * Call dupl recursively on to children rules.
	 */
	virtual abnf_rule_ri* dupl_impl(const abnf_ruleset& rset,
			std::map<const abnf_rule*, abnf_rule_ri*>& d_map) const = 0;
			
	private:
	
	std::istream* _is;
	std::vector<abnf_segment> _seg_vect;
};

/*
 * Throws a detailed std::invalid_argument exception if the owner rule set of
 * the given rule is not the same as the given rule set.
 */
inline void owner_test(const abnf_ruleset& rset, const abnf_rule& r)
{
	if (&rset not_eq &r.ruleset())
		throw std::invalid_argument("rule from different rule set");
}

} // namespace xspider

#endif // ABNFR_H
