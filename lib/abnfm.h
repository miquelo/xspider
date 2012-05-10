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

#ifndef ABNFM_H
#define ABNFM_H

#include <cassert>

#include "abnfr.h"

namespace xspider {

/*
 * Generic rule matcher.
 */
class abnf_matcher
{
	public:
	
	/*
	 * Initialized generic matcher.
	 *
	 * Not matched and available.
	 */
	abnf_matcher(abnf_rule_ri& r):
	_r(r),
	_matched(false),
	_avail(true),
	_beg(0l),
	_end(0l)
	{
	}
	
	/*
	 * Release matcher resources.
	 */
	virtual ~abnf_matcher(void);
	
	/*
	 * Public access to availability flag.
	 */
	const bool& available(void) const
	{
		return _avail;
	}
	
	/*
	 * Begin of mathing stream.
	 */
	const std::streampos& stream_beg(void) const
	{
		return _beg;
	}
	
	/*
	 * End of matching stream.
	 */
	const std::streampos& stream_end(void) const
	{
		return _end;
	}
	
	/*
	 * Clear match flag.
	 */
	void mismatch(void)
	{
		_matched = false;
	}
	
	/*
	 * Add begin-end segment to this matcher rule if it was matched and it does
	 * not determine an empty segment.
	 */
	void commit(void)
	{
		if (_matched and _end > _beg)
		{
			_r.segment_add(_beg, _end);
			commit_impl();
		}
	}
	
	/*
	 * Test matching from current position of the given stream.
	 *
	 * If this matcher has already matched, matching test matches directly
	 * and state is not changed.
	 *
	 * If required size < 0, it mismatches directly.
	 *
	 * It uses match_impl to determine if it matches and store begin and end
	 * position.
	 *
	 * Returns true if matches; false otherwise.
	 *
	 * Precondition:
	 *		match state flag = true -> is.tellg() = begin position
	 *
	 * Postcondition:
	 *		required size is updated, 1l if mismatched
	 *		begin position is is.tellg() before matching test
	 *		end position is is.tellg() after matching test
	 *		match state flag updated accordingly to matching test
	 */
	bool match(std::istream& is)
	{
		assert(not _matched or is.tellg() == _beg);
			
		if (_matched)
			is.seekg(_end);
		else if (_avail)
		{
			_beg = is.tellg();
			_matched = match_impl(is);
			_end = is.tellg();
		}
		_avail = _matched and availability_test();
		
		return _matched;
	}
	
	protected:
	
	/*
	 * Callback called after this matcher has been created in order to
	 * determine its availability.
	 */
	virtual bool availability_test(void) const;
	
	/*
	 * Commit children segments, if it has any.
	 */
	virtual void commit_impl(void) = 0;
	
	/*
	 * Implementation logic matching from current position of the given stream.
	 *
	 * Returnrs true if it matches; false otherwise.
	 */
	virtual bool match_impl(std::istream& is) = 0;
	
	private:
	
	abnf_rule_ri& _r;
	bool _matched, _avail;
	std::streampos _beg, _end;
};

} // namespace xspider

#endif // ABNFM_H
