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
	 */
	abnf_matcher(abnf_rule_ri& r):
	_r(r),
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
	 * Add begin-end segment to this matcher rule if it was matched and it does
	 * not determine an empty segment.
	 */
	void commit(void)
	{
		if (_end > _beg)
		{
			_r.segment_add(_beg, _end);
			commit_impl();
		}
	}
	
	/*
	 * Test matching from current position of the given stream.
	 *
	 * It uses match_impl to determine if it matches and store begin and end
	 * position.
	 *
	 * Returns true if matches; false otherwise.
	 *
	 * Postcondition:
	 *		begin position is is.tellg() before matching test
	 *		end position is is.tellg() after matching test
	 */
	bool match(std::istream& is)
	{
		if (_avail)
		{
			_beg = is.tellg();
			bool matched = match_impl(is);
			_end = is.tellg();
			_avail = matched and available();
			
			return matched;
		}
		return false;
	}
	
	/*
	 * Availability test.
	 */
	virtual bool available(void) const;
	
	protected:
	
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
	bool _avail;
	std::streampos _beg, _end;
};

} // namespace xspider

#endif // ABNFM_H
