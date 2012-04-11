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

namespace xspider {

/*!
 * \brief Portable port definition.
 */
class port_t
{
	public:
	
	/*!
	 * \brief Port <tt>0</tt>.
	 */
	port_t(void)
	{
		_assign_ulong(0l);
	}
	
	/*!
	 * \brief Port from initial value of largest integral unsigned type.
	 *
	 * \param val
	 *			Initial value.
	 */
	port_t(unsigned long val)
	{
		_assign_ulong(val);
	}
	
	/*!
	 * \brief Port from initial port value.
	 *
	 * \param p
	 *			Initial port value.
	 */
	port_t(const port_t& p)
	{
		_assign_port(p);
	}
	
	/*!
	 * \brief Assignment from another port value.
	 *
	 * \param p
	 *			Port value to be assigned.
	 *
	 * \return
	 *			The port itself.
	 */
	port_t& operator = (const port_t& p)
	{
		return _assign_port(p);
	}
	
	/*!
	 * \brief Compare whether this port is equal than the given port.
	 *
	 * \param p
	 *			The port to be compared with this port.
	 *
	 * \retval true
	 *			If two ports are equal;
	 * \retval false
	 *			otherwise.
	 */
	bool operator == (const port_t& p)
	{
		for (size_t i = 0; i < _size; ++i)
			if (_b[i] not_eq p._b[i])
				return false;
		return true;
	}
	
	/*!
	 * \brief Compare whether this port is not equal than the given port.
	 *
	 * \param p
	 *			The port to be compared with this port.
	 *
	 * \retval true
	 *			If two ports are not equal;
	 * \retval false
	 *			otherwise.
	 */
	bool operator not_eq (const port_t& p)
	{
		for (size_t i = 0; i < _size; ++i)
			if (_b[i] not_eq p._b[i])
				return true;
		return false;
	}
	
	/*!
	 * \brief Compare whether this port is less than the given port.
	 *
	 * \param p
	 *			The port to be compared with this port.
	 *
	 * \retval true
	 *			If this port is less than the given port;
	 * \retval false
	 *			otherwise.
	 */
	bool operator < (const port_t& p)
	{
		for (size_t i = _size - 1; i >= 0; --i)
		{
			if (_b[i] < p._b[i])
				return true;
			else if (_b[i] > p._b[i])
				return false;
		}
		return false;
	}
	
	private:
	
	static const size_t _size = 4;
	unsigned char _b[_size];
	
	port_t& _assign_ulong(unsigned long val)
	{
		for (size_t i = 0; i < _size; ++i)
		{
			_b[i] = val % 256;
			val = val >> 8;
		}
		return *this;
	}
	
	port_t& _assign_port(const port_t& p)
	{
		for (size_t i = 0; i < _size; ++i)
			_b[i] = p._b[i];
	}
};

/*!
 * \brief Represents an Uniform Resource Identifier (URI).
 */
class uri
{
	public:
	
	/*!
	 * \brief Constructs an URI with a given specification.
	 *
	 * \param spec
	 *			URI specification.
	 */
	uri(const std::string& spec);
	
	/*!
	 * \brief Determines whether this URI is relative or not.
	 *
	 * \retval true
	 *			If this URI is relative;
	 * \retval false
	 *			otherwise.
	 */
	bool relative(void) const;
	
	/*!
	 * \brief Scheme component of this URI.
	 *
	 * \return
	 *			The scheme of this URI. Empty if it doesn't have any.
	 */
	const std::string& scheme(void) const;
	
	/*!
	 * \brief User info component of this URI.
	 *
	 * \return
	 *			The user info of this URI. Empty if it doesn't have any.
	 */
	const std::string& userinfo(void) const;
	
	/*!
	 * \brief Host component of this URI.
	 *
	 * \return
	 *			The host of this URI. Empty if it doesn't have any.
	 */
	const std::string& host(void) const;
	
	/*!
	 * \brief Fragment component of this URI.
	 *
	 * \return
	 *			The fragment of this URI. Empty if it doesn't have any.
	 */
	const std::string& fragment(void) const;
	
	/*!
	 * \brief Port component of this URI.
	 *
	 * \return
	 *			The port of this URI. Zero if it doesn't have any.
	 */
	const port_t& port(void) const;
	
	/*!
	 * \brief Path component of this URI.
	 *
	 * \return
	 *			The path of this URI. Empty if it doesn't have any.
	 */
	const std::list<std::string>& path(void) const;
	
	/*!
	 * \brief Query component of this URI.
	 *
	 * \return
	 *			The query of this URI. Empty if it doesn't have any.
	 */
	const std::multimap<std::string, std::string>& query(void) const;
	
	private:
	
	std::string _scheme;
	std::string _userinfo;
	std::string _host;
	std::string _fragment;
	port_t _port;
	std::list<std::string> _path;
	std::multimap<std::string, std::string> _query;
};

} // namespace xspider

#endif // URI_H
