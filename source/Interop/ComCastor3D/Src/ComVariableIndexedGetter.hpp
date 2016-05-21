/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___

#include "ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariableGetter
	{
		typedef Value( Class::*Function )( Index )const;
		IndexedVariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value >( ( m_instance->*m_function )( parameter_cast< Index >( index ) ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_Castor3D,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariableGetter< Class, Value, Index >
	make_indexed_getter( _Class * instance, Value( Class::*function )( Index )const )
	{
		return IndexedVariableGetter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedRefVariableGetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		IndexedRefVariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value & >( ( m_instance->*m_function )( parameter_cast< Index >( index ) ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_Castor3D,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedRefVariableGetter< Class, Value, Index >
	make_indexed_getter( _Class * instance, Value const & ( Class::*function )( Index )const )
	{
		return IndexedRefVariableGetter< Class, Value, Index >( ( Class * )instance, function );
	}
}

#endif
