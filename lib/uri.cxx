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
 
#include "uri.h"

using namespace std;
using namespace xspider;

uri::uri(const std::string& spec)
{
	// TODO ...
}

bool uri::relative(void) const
{
	return _scheme.empty();
}

const string& uri::scheme(void) const
{
	return _scheme;
}

const string& uri::userinfo(void) const
{
	return _userinfo;
}

const string& uri::host(void) const
{
	return _host;
}

const port_t& uri::port(void) const
{
	return _port;
}

const list<string>& uri::path(void) const
{
	return _path;
}

const multimap<string, string>& uri::query(void) const
{
	return _query;
}
