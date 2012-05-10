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

using namespace std;
using namespace xspider;

/*
 * abnf_rule implementation
 */

abnf_rule::abnf_rule(const abnf_ruleset& r_set):
_r_set(r_set)
{
}

const abnf_ruleset& abnf_rule::ruleset(void) const
{
	return _r_set;
}

/*
 * abnf_rule_ri implementation
 */

void abnf_rule_ri::clear()
{
	clear_impl();
	
	_is = NULL;
	_seg_vect.clear();
}

void abnf_rule_ri::read(istream& is)
{
	stream_update(is);
	
	abnf_matcher* m = matcher_new();
	if (m->match(is))
		m->commit();
	else
		is.seekg(m->stream_beg());
	delete m;
}

size_t abnf_rule_ri::read_count(void) const
{
	return _seg_vect.size();
}

void abnf_rule_ri::write(size_t n, ostream& os) const
{
	if (_is not_eq NULL and n >= 0 and n < _seg_vect.size())
		_seg_vect[n].write(*_is, os);
}
