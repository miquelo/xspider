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

#include <cctype>

#include "abnf.h"

using namespace std;
using namespace xspider;

const abnf_ruleset& abnf_ruleset::core_ruleset(void)
{
	if (_core_rset.defined("ALPHA"))
		return _core_rset;
		
	abnf_rule& r_alpha = _core_rset.terminal(isalpha);
	abnf_rule& r_bit = _core_rset.terminal("01");
	abnf_rule& r_char = _core_rset.alternat(0x01, 0x7f);
	abnf_rule& r_cr = _core_rset.terminal(0x0d);
	abnf_rule& r_lf = _core_rset.terminal(0x0a);
	abnf_rule& r_crlf = _core_rset.concat(r_cr, r_lf);
	abnf_rule& r_ctl = _core_rset.terminal(iscntrl);
	abnf_rule& r_digit = _core_rset.terminal(isdigit);
	abnf_rule& r_dquote = _core_rset.terminal(0x22);
	abnf_rule& r_upperhex = _core_rset.alternat("ABCDEF");
	abnf_rule& r_hexdig = _core_rset.alternat(r_digit, r_upperhex);
	abnf_rule& r_htab = _core_rset.terminal(0x09);
	abnf_rule& r_sp = _core_rset.terminal(0x20);
	abnf_rule& r_wsp = _core_rset.alternat(r_sp, r_htab);
	abnf_rule& r_crlfwsp = _core_rset.concat(r_crlf, r_wsp);
	abnf_rule& r_wsp_crlfwsp = _core_rset.alternat(r_wsp, r_crlfwsp);
	abnf_rule& r_lwsp = _core_rset.repet(0, r_wsp_crlfwsp);
	abnf_rule& r_octet = _core_rset.alternat(0x00, 0xff);
	abnf_rule& r_vchar = _core_rset.alternat(0x21, 0x7e);
	
	_core_rset.define("ALPHA", r_alpha);
	_core_rset.define("BIT", r_bit);
	_core_rset.define("CHAR", r_char);
	_core_rset.define("CR", r_cr);
	_core_rset.define("CRLF", r_crlf);
	_core_rset.define("CTL", r_ctl);
	_core_rset.define("DIGIT", r_digit);
	_core_rset.define("DQUOTE", r_dquote);
	_core_rset.define("HEXDIG", r_hexdig);
	_core_rset.define("HTAB", r_htab);
	_core_rset.define("LF", r_lf);
	_core_rset.define("LWSP", r_lwsp);
	_core_rset.define("OCTET", r_octet);
	_core_rset.define("SP", r_sp);
	_core_rset.define("VCHAR", r_vchar);
	_core_rset.define("WSP", r_wsp);
	
	return _core_rset;
}
