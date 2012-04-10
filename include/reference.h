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
 
#ifndef REFERENCE_H
#define REFERENCE_H

/*!
 * \file
 * \brief Management for referenced instances.
 */
 
#include <cstdlib>
#include <exception>

namespace xspider {

/*!
 * \brief Exception thrown when an empty reference is deferenced.
 */
class bad_pointer:
public std::exception
{
	public:
	
	/*!
	 * \brief Rerturns <i>"attempt to access null pointer"</i>.
	 */
	const char* what(void) const
	throw ()
	{
		return "attempt to access null pointer";
	}
};
 
/*!
 * \brief Standard deletion function.
 *
 * Its purpose is to serve as deletion function for instances managed by a
 * reference which where instantiated with the standard \c new operator.
 *
 * \tparam type
 *			Type of the instance to be deleted.
 *
 * \param ptr
 *			Pointer to the managed instance which should be deleted.
 *
 * \sa reference
 */
template <typename type>
inline void std_delete_fn(type* ptr)
{
	delete ptr;
}

/*!
 * \brief Reference instances of the type specified in order to manage their
 * lifecycle.
 *
 * \tparam type
 *			Type of instance managed by this reference.
 */
template <typename type>
class reference
{
	public:
	
	/*!
	 * \brief Empty reference.
	 */
	reference(void):
	_ptr(NULL),
	_ref_count(NULL),
	_del_fn(NULL)
	{
	}
	
	/*!
	 * \brief Initialized reference. It makes an instance to be managed.
	 *
	 * \warning Do not pass a pointer to an already managed instance. It should
	 * make an instance to be deleted more than one time.
	 *
	 * \param ptr
	 *			Pointer to the instance to be managed by this reference.
	 * \param del_fn
	 *			Pointer to the function which will be used to delete the
	 *			instance managed by this reference.
	 *
	 * \throw bad_pointer
	 *			If the given instance pointer is \c null.
	 */
	reference(type* ptr, void (*del_fn)(type*) = std_delete_fn)
	throw (bad_pointer):
	_ptr(ptr),
	_ref_count(new long(1l)),
	_del_fn(del_fn)
	{
		if (_ptr == NULL)
			throw bad_pointer();
	}
	
	/*!
	 * \brief Initializes a reference from another reference of the same type.
	 *
	 * Makes the instance managed by \p ref to be managed by this reference
	 * too and increases by one its reference count.
	 *
	 * \param ref
	 *			Reference which will be used to initialize this reference.
	 */
	reference(const reference<type>& ref):
	_ptr(ref._ptr),
	_ref_count(ref._ref_count),
	_del_fn(ref._del_fn)
	{
		_ref();
	}
	
	/*!
	 * \brief Initializes a reference from another reference of a derived type.
	 *
	 * Makes the instance managed by \p ref to be managed by this reference
	 * too and increases by one its reference count.
	 *
	 * \tparam derived_type
	 *			Derived type of the type of this reference.
	 *
	 * \param ref
	 *			Reference which will be used to initialize this reference.
	 */
	template <typename derived_type>
	reference(const reference<derived_type>& ref):
	_ptr(ref._ptr),
	_ref_count(ref._ref_count),
	_del_fn(reinterpret_cast<void (*)(type*)>(ref._del_fn))
	{
		_ref();
	}
	
	/*!
	 * \brief Destroys this reference and decreases by one the reference count
	 * of its managed instance.
	 *
	 * When the reference count reaches a zero value, the managed instance is
	 * processed by \c del_fn in order to be deleted.
	 */
	~reference(void)
	{
		_unref();
	}
	
	/*!
	 * \brief Does a dynamic type casting on to this reference.
	 *
	 * If the instance managed by this instance is not of the specified type,
	 * the cast will fail and an empty reference will be returned.
	 *
	 * \tparam derived_type
	 *			Derived type of the type of this reference.
	 *
	 * \return
	 *			The reference of that derived type or an empty reference if it
	 *			fails.
	 */
	template <typename derived_type>
	reference<derived_type> cast(void) const
	{
		derived_type* ptr = dynamic_cast<derived_type*>(_ptr);
		if (ptr == NULL)
			return reference<derived_type>();
		reference<derived_type> ref;
		ref._ptr = ptr;
		ref._ref_count = _ref_count;
		ref._del_fn = reinterpret_cast<void (*)(derived_type*)>(_del_fn);
		ref._ref();
		return ref;
	}
	
	/*!
	 * \brief Checks if this reference is an empty reference.
	 *
	 * \retval
	 *			true if this is not an empty reference;
	 * \retval
	 *			false otherwise.
	 */
	operator bool (void)
	{
		return _ptr not_eq NULL;
	}
	
	/*!
	 * \brief Checks if the instance managed by this reference and the instance
	 * managed by the given reference are the same.
	 *
	 * \retval
	 *			true if they are the same;
	 * \retval
	 *			false otherwise.
	 */
	bool operator == (const reference<type>& ref)
	{
		return _ptr == ref._ptr;
	}
	
	/*!
	 * \brief Checks if the instance managed by this reference and the instance
	 * managed by the given reference are the same.
	 *
	 * \tparam some_type
	 *			All types are valid since whole instance addresses are
	 *			comparable independently of its type.
	 *
	 * \retval
	 *			true if they are the same;
	 * \retval
	 *			false otherwise.
	 */
	template <typename some_type>
	bool operator == (const reference<some_type>& ref)
	{
		return _ptr == ref._ptr;
	}
	
	/*!
	 * \brief Makes the instance managed by another reference of the same type
	 * to be managed by this reference.
	 *
	 * If the instance managed by \p ref is not the same than the instance
	 * managed by this reference, the reference count of the old managed
	 * instance is decreased by one and deleted if it proceed as
	 * \link ~reference(void) \endlink does.
	 *
	 * The reference count of the new managed instance is increased by one.
	 *
	 * \param ref
	 *			Reference whose managed instance will be managed by this
	 *			reference too.
	 *
	 * \return
	 *			The reference itself.
	 */
	reference<type>& operator = (const reference<type>& ref)
	{
		if (_ptr not_eq ref._ptr)
		{
			_unref();
			_ptr = ref._ptr;
			_ref_count = ref._ref_count;
			_del_fn = ref._del_fn;
			_ref();
		}
		return *this;
	}
	
	/*!
	 * \brief Makes the instance managed by another reference of a derived type
	 * to be managed by this reference.
	 *
	 * If the instance managed by \p ref is not the same than the instance
	 * managed by this reference, the reference count of the old managed
	 * instance is decreased by one and deleted if it proceed as
	 * \link ~reference(void) \endlink does.
	 *
	 * The reference count of the new managed instance is increased by one.
	 *
	 * \tparam derived_type
	 *			Derived type of the type of this reference.
	 *
	 * \param ref
	 *			Reference whose managed instance will be managed by this
	 *			reference too.
	 *
	 * \return
	 *			The reference itself.
	 */
	template <typename derived_type>
	reference<type>& operator = (const reference<derived_type>& ref)
	{
		if (_ptr not_eq ref._ptr)
		{
			_unref();
			_ptr = ref._ptr;
			_ref_count = ref._ref_count;
			_del_fn = reinterpret_cast<void (*)(type*)>(ref._del_fn);
			_ref();
		}
		return *this;
	}
	
	/*!
	 * \brief Returns the constant address of the instance managed by this
	 * reference in order to be accessed.
	 *
	 * \return
	 *			The constant address of the instance managed by this reference.
	 *
	 * \throw bad_pointer
	 *			If this reference is empty.
	 */
	const type* operator -> (void) const
	throw (bad_pointer)
	{
		if (_ptr == NULL)
			throw bad_pointer();
		return _ptr;
	}
	
	/*!
	 * \brief Returns the address of the instance managed by this reference in
	 * order to be accessed.
	 *
	 * \return
	 *			The address of the instance managed by this reference.
	 *
	 * \throw bad_pointer
	 *			If this reference is empty.
	 */
	type* operator -> (void)
	throw (bad_pointer)
	{
		if (_ptr == NULL)
			throw bad_pointer();
		return _ptr;
	}
	
	private:
	
	type* _ptr;
	long* _ref_count;
	void (*_del_fn)(type*);
	
	void _ref(void)
	{
		if (_ref_count not_eq NULL)
			++*_ref_count;
	}
	
	void _unref(void)
	{
		if (_ref_count not_eq NULL and --*_ref_count == 0)
		{
			delete _ref_count;
			if (_del_fn not_eq NULL)
				_del_fn(_ptr);
		}
	}
	
	/*!
	 * \brief References of any type must have access to private fields of any
	 * reference.
	 */
	template <typename _type>
	friend class reference;
};

} // namespace xspider

#endif // REFERENCE_H
