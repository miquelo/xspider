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
 
#ifndef URI_H
#define URI_H

/*!
 * \file
 * \brief Uniform Resource Identifier (URI).
 */
 
#include <exception>
#include <list>
#include <map>
#include <string>

#include "abnf.h"

namespace xspider {

/*!
 * \brief Represents an Uniform Resource Identifier (URI).
 */
class uri
{
	public:
	
	/*!
	 * \brief Constructs an empty URI.
	 */
	uri(void);
	
	/*!
	 * \brief Determines whether this URI is relative or not.
	 *
	 * \retval true
	 *			If this URI is relative;
	 * \retval false
	 *			otherwise.
	 */
	bool relative(void) const
	{
		return _scheme.empty();
	}
	
	/*!
	 * \brief Scheme component of this URI.
	 *
	 * \return
	 *			The scheme of this URI. Empty if it doesn't have any.
	 */
	const std::string& scheme(void) const
	{
		return _scheme;
	}
	
	/*!
	 * \brief User info component of this URI.
	 *
	 * \return
	 *			The user info of this URI. Empty if it doesn't have any.
	 */
	const std::string& userinfo(void) const
	{
		return _userinfo;
	}
	
	/*!
	 * \brief Host component of this URI.
	 *
	 * \return
	 *			The host of this URI. Empty if it doesn't have any.
	 */
	const std::string& host(void) const
	{
		return _host;
	}
	
	/*!
	 * \brief Fragment component of this URI.
	 *
	 * \return
	 *			The fragment of this URI. Empty if it doesn't have any.
	 */
	const std::string& fragment(void) const
	{
		return _fragment;
	}
	
	/*!
	 * \brief Port component of this URI.
	 *
	 * \return
	 *			The port of this URI. Zero if it doesn't have any.
	 */
	unsigned long port(void) const
	{
		return _port;
	}
	
	/*!
	 * \brief Path component of this URI.
	 *
	 * \return
	 *			The path of this URI. Empty if it doesn't have any.
	 */
	const std::list<std::string>& path(void) const
	{
		return _path;
	}
	
	/*!
	 * \brief Query component of this URI.
	 *
	 * \return
	 *			The query of this URI. Empty if it doesn't have any.
	 */
	const std::multimap<std::string, std::string>& query(void) const
	{
		return _query;
	}
	
	private:
	
	static abnf_ruleset _rset;
	std::string _scheme;
	std::string _userinfo;
	std::string _host;
	std::string _fragment;
	unsigned long _port;
	std::list<std::string> _path;
	std::multimap<std::string, std::string> _query;
	
	friend std::istream& operator >> (std::istream& is, uri& u);
	friend std::ostream& operator << (std::ostream& os, const uri& u);
};

/*!
 * \brief Parse an URI from a character stream.
 *
 * \param is
 *			Character stream which contains an URI to be parsed.
 * \param u
 *			Target URI to receive the parsing results.
 *
 * \return
 *			The source character stream.
 */
std::istream& operator >> (std::istream& is, uri& u);

/*!
 * \brief Put an URI representation to a character stream.
 *
 * \param os
 *			Character stream to put the URI representation.
 * \param u
 *			Source URI to be represented.
 *
 * \return
 *			The target character stream.
 */
std::ostream& operator << (std::ostream& os, const uri& u);

} // namespace xspider

#endif // URI_H
