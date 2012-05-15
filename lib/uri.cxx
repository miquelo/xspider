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
 
#include <sstream>

#include "uri.h"

#define DEFAULT_PORT 0l

using namespace std;
using namespace xspider;

/*
 * uri implementation
 */

static void uri_abnf_ruleset(abnf_ruleset& rset);

abnf_ruleset uri::_rset;

uri::uri(void):
_port(DEFAULT_PORT)
{
}

istream& xspider::operator >> (istream& is, uri& u)
{
	u._scheme.clear();
	u._userinfo.clear();
	u._host.clear();
	u._fragment.clear();
	u._path.clear();
	u._query.clear();
	
	abnf_ruleset& rset = uri::_rset;
	if (not rset.defined("ALPHA"))
		uri_abnf_ruleset(rset);
	
	abnf_rule& r_uriend = rset.get("URI-reference");
	abnf_rule& r_scheme = rset.get("scheme");
	abnf_rule& r_userinfo = rset.get("userinfo");
	abnf_rule& r_host = rset.get("host");
	abnf_rule& r_fragment = rset.get("fragment");
	abnf_rule& r_port = rset.get("port");
	abnf_rule& r_abs_path = rset.get("abs_path");
	abnf_rule& r_rel_path = rset.get("rel_path");
	abnf_rule& r_query = rset.get("query");
	
	r_uriend.read(is);
	if (r_scheme.read_count() > 0)
	{
		ostringstream oss;
		r_scheme.write(0, oss);
		u._scheme = oss.str();
	}
	if (r_userinfo.read_count() > 0)
	{
		ostringstream oss;
		r_userinfo.write(0, oss);
		u._userinfo = oss.str();
	}
	if (r_host.read_count() > 0)
	{
		ostringstream oss;
		r_host.write(0, oss);
		u._host = oss.str();
	}
	if (r_fragment.read_count() > 0)
	{
		ostringstream oss;
		r_fragment.write(0, oss);
		u._fragment = oss.str();
	}
	if (r_port.read_count() == 0)
		u._port = DEFAULT_PORT;
	else
	{
		stringstream ss;
		r_port.write(0, ss);
		ss >> u._port;
	}
	bool has_rel_path = r_rel_path.read_count() > 0;
	if (r_abs_path.read_count() > 0 or has_rel_path)
	{
		stringstream ss;
		if (has_rel_path)
			r_rel_path.write(0, ss);
		else
		{
			r_abs_path.write(0, ss);
			ss.ignore();
			u._path.push_back("/");
		}
		
		const int seg_max = 1024;
		char seg[seg_max];
		while (ss.good())
		{
			ss.getline(seg, seg_max, '/');
			u._path.push_back(seg);
		}
	}
	if (r_query.read_count() > 0)
	{
		stringstream ss;
		r_query.write(0, ss);
		
		const int seg_max = 1024;
		char seg[seg_max];
		while (ss.good())
		{
			ss.getline(seg, seg_max, '&');
			string str = seg;
			int sep = str.find_first_of('=');
			if (sep == string::npos)
				u._query.insert(pair<string, string>(str, ""));
			else
				u._query.insert(pair<string, string>(str.substr(0, sep),
						str.substr(sep + 1)));
		}
	}
	r_uriend.clear();
	
	return is;
}

ostream& xspider::operator << (ostream& os, const uri& u)
{
	bool has_scheme = not u._scheme.empty();
	bool has_userinfo = not u._userinfo.empty();
	bool has_host = not u._host.empty();
	bool has_port = u._port not_eq DEFAULT_PORT;
	bool has_fragment = not u._fragment.empty();
	bool has_query = not u._query.empty();
	
	if (has_scheme)
		os << u._scheme << ":";
	if (has_userinfo or has_host)
		os << "//";
	if (has_userinfo)
		os << u._userinfo << "@";
	if (has_host)
		os << u._host;
	if (has_port)
		os << ":" << u._port;
	
	list<string>::const_iterator it = u._path.begin();
	bool it_end = false;
	while (not it_end)
	{
		string seg = *it++;
		os << seg;
		if (not (it_end = it == u._path.end()) and seg not_eq "/")
			os << "/";
	}
	
	if (has_query)
	{
		os << "?";
		multimap<string, string>::const_iterator it = u._query.begin();
		bool it_end = false;
		while (not it_end)
		{
			os << it->first;
			if (not it->second.empty())
				os << "=" << it->second;
			if (not (it_end = ++it == u._query.end()))
				os << "&";
		}
	}
	
	if (has_fragment)
		os << "#" << u._fragment;
	
	return os;
}

void uri_abnf_ruleset(abnf_ruleset& rset)
{
	rset.include(abnf_ruleset::core_ruleset());
	abnf_rule& r_alphanum = rset.terminal(isalnum);
	abnf_rule& r_hex = rset.terminal(isxdigit);
	abnf_rule& r_hexhex = rset.concat(r_hex, r_hex);
	abnf_rule& r_percent = rset.terminal('%');
	abnf_rule& r_escaped = rset.concat(r_percent, r_hexhex);
	abnf_rule& r_mark = rset.alternat("-_.!~*'()");
	abnf_rule& r_unreserved = rset.alternat(r_alphanum, r_mark);
	abnf_rule& r_reserved = rset.alternat(";/?:@&=+$,");
	abnf_rule& r_res_unres = rset.alternat(r_reserved, r_unreserved);
	abnf_rule& r_uric = rset.alternat(r_res_unres, r_escaped);
	abnf_rule& r_fragment = rset.repet(0, r_uric);
	abnf_rule& r_query = rset.repet(0, r_uric);
	abnf_rule& r_pcharch = rset.alternat(":@&=+$,");
	abnf_rule& r_unres_esc = rset.alternat(r_unreserved, r_escaped);
	abnf_rule& r_pchar = rset.alternat(r_unres_esc, r_pcharch);
	abnf_rule& r_param = rset.repet(0, r_pchar);
	abnf_rule& r_semic = rset.terminal(';');
	abnf_rule& r_semicparam = rset.concat(r_semic, r_param);
	abnf_rule& r_rsemicparam = rset.repet(0, r_semicparam);
	abnf_rule& r_rpchar = rset.repet(0, r_pchar);
	abnf_rule& r_segment = rset.concat(r_rpchar, r_rsemicparam);
	abnf_rule& r_sl = rset.terminal('/');
	abnf_rule& r_slseg = rset.concat(r_sl, r_segment);
	abnf_rule& r_rslseg = rset.repet(0, r_slseg);
	abnf_rule& r_path_seg = rset.concat(r_segment, r_rslseg);
	abnf_rule& r_abs_path = rset.concat(r_sl, r_path_seg);
	abnf_rule& r_uric_no_slch = rset.alternat(";?:@&=+$,");
	abnf_rule& r_uric_no_sl = rset.alternat(r_unres_esc, r_uric_no_slch);
	abnf_rule& r_ruric = rset.repet(0, r_uric);
	abnf_rule& r_opaq_part = rset.concat(r_uric_no_sl, r_ruric);
	abnf_rule& r_abs_path_opaq_part = rset.alternat(r_abs_path, r_opaq_part);
	abnf_rule& r_path = rset.repet(0, 1, r_abs_path_opaq_part);
	abnf_rule& r_digit = rset.get("digit");
	abnf_rule& r_port = rset.repet(0, r_digit);
	abnf_rule& r_rdigit = rset.repet(1, r_digit);
	abnf_rule& r_dot = rset.terminal('.');
	abnf_rule& r_ipv4address1 = rset.concat(r_rdigit, r_dot);
	abnf_rule& r_ipv4address2 = rset.concat(r_ipv4address1, r_rdigit);
	abnf_rule& r_ipv4address3 = rset.concat(r_ipv4address2, r_dot);
	abnf_rule& r_ipv4address4 = rset.concat(r_ipv4address3, r_rdigit);
	abnf_rule& r_ipv4address5 = rset.concat(r_ipv4address4, r_dot);
	abnf_rule& r_ipv4address = rset.concat(r_ipv4address5, r_rdigit);
	abnf_rule& r_alpha = rset.get("alpha");
	abnf_rule& r_min = rset.terminal('-');
	abnf_rule& r_alphanum_min = rset.alternat(r_alphanum, r_min);
	abnf_rule& r_ralphanum_min = rset.repet(0, r_alphanum_min);
	abnf_rule& r_alraln_min = rset.concat(r_alpha, r_ralphanum_min);
	abnf_rule& r_alraln_minaln = rset.concat(r_alraln_min, r_alphanum);
	abnf_rule& r_toplabel = rset.alternat(r_alpha, r_alraln_minaln);
	abnf_rule& r_alnraln_min = rset.concat(r_alphanum, r_ralphanum_min);
	abnf_rule& r_alnraln_minaln = rset.concat(r_alnraln_min, r_alphanum);
	abnf_rule& r_domainlabel = rset.alternat(r_alphanum, r_alnraln_minaln);
	abnf_rule& r_domlabdot = rset.concat(r_domainlabel, r_dot);
	abnf_rule& r_rdomlabdot = rset.repet(0, r_domlabdot);
	abnf_rule& r_rdot = rset.repet(0, 1, r_dot);
	abnf_rule& r_rdomlabdot_toplab = rset.concat(r_rdomlabdot, r_toplabel);
	abnf_rule& r_hostname = rset.concat(r_rdomlabdot_toplab, r_rdot);
	abnf_rule& r_host = rset.alternat(r_hostname, r_ipv4address);
	abnf_rule& r_colon = rset.terminal(':');
	abnf_rule& r_colonport = rset.concat(r_colon, r_port);
	abnf_rule& r_rcolonport = rset.repet(0, 1, r_colonport);
	abnf_rule& r_hostport = rset.concat(r_host, r_rcolonport);
	abnf_rule& r_userinfoch = rset.alternat(";:&=+$,");
	abnf_rule& r_unres_esc_userich = rset.alternat(r_unres_esc, r_userinfoch);
	abnf_rule& r_userinfo = rset.repet(0, r_unres_esc_userich);
	abnf_rule& r_arroba = rset.terminal('@');
	abnf_rule& r_useriarr = rset.concat(r_userinfo, r_arroba);
	abnf_rule& r_ruseriarr = rset.repet(0, 1, r_useriarr);
	abnf_rule& r_ruseriarrhport = rset.concat(r_ruseriarr, r_hostport);
	abnf_rule& r_server = rset.repet(0, 1, r_ruseriarrhport);
	abnf_rule& r_regnch = rset.alternat("$,;:@&=+");
	abnf_rule& r_unres_esc_regnch = rset.alternat(r_unres_esc, r_regnch);
	abnf_rule& r_reg_name = rset.repet(1, r_unres_esc_regnch);
	abnf_rule& r_authority = rset.alternat(r_server, r_reg_name);
	abnf_rule& r_schemech = rset.alternat("+-.");
	abnf_rule& r_alnum_schemech = rset.alternat(r_alphanum, r_schemech);
	abnf_rule& r_ralnum_schemech = rset.repet(0, r_alnum_schemech);
	abnf_rule& r_scheme = rset.concat(r_alpha, r_ralnum_schemech);
	abnf_rule& r_relsegch = rset.alternat(";@&=+$,");
	abnf_rule& r_unres_esc_relsegch = rset.alternat(r_unres_esc, r_relsegch);
	abnf_rule& r_rel_seg = rset.repet(1, r_unres_esc_relsegch);
	abnf_rule& r_rabs_path = rset.repet(0, 1, r_abs_path);
	abnf_rule& r_rel_path = rset.concat(r_rel_seg, r_rabs_path);
	abnf_rule& r_dslash = rset.terminal("//");
	abnf_rule& r_dslashauth = rset.concat(r_dslash, r_authority);
	abnf_rule& r_net_path = rset.concat(r_dslashauth, r_abs_path);
	abnf_rule& r_qm = rset.terminal('?');
	abnf_rule& r_qmquery = rset.concat(r_qm, r_query);
	abnf_rule& r_rqmquery = rset.repet(0, 1, r_qmquery);
	abnf_rule& r_npath_apath = rset.alternat(r_net_path, r_abs_path);
	abnf_rule& r_hier_part = rset.concat(r_npath_apath, r_rqmquery);
	abnf_rule& r_npth_apth_rpth = rset.alternat(r_npath_apath, r_rel_path);
	abnf_rule& r_reluri = rset.concat(r_npth_apth_rpth, r_rqmquery);
	abnf_rule& r_schemecol = rset.concat(r_scheme, r_colon);
	abnf_rule& r_hier_opaq = rset.alternat(r_hier_part, r_opaq_part);
	abnf_rule& r_absuri = rset.concat(r_schemecol, r_hier_opaq);
	abnf_rule& r_abs_rel = rset.alternat(r_absuri, r_reluri);
	abnf_rule& r_rabs_rel = rset.repet(0, 1, r_abs_rel);
	abnf_rule& r_nsign = rset.terminal('#');
	abnf_rule& r_nsignfrag = rset.concat(r_nsign, r_fragment);
	abnf_rule& r_rnsignfrag = rset.repet(0, 1, r_nsignfrag);
	abnf_rule& r_uri = rset.concat(r_rabs_rel, r_rnsignfrag);
	abnf_rule& r_end = rset.alternat(rset.terminal(isspace), rset.eof());
	abnf_rule& r_uriend = rset.concat(r_uri, r_end);
	
	rset.define("URI-reference", r_uriend);
	rset.define("scheme", r_scheme);
	rset.define("userinfo", r_userinfo);
	rset.define("host", r_host);
	rset.define("fragment", r_fragment);
	rset.define("port", r_port);
	rset.define("abs_path", r_abs_path);
	rset.define("rel_path", r_rel_path);
	rset.define("query", r_query);
}
