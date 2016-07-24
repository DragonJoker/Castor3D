/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___

#include "ComParameterCast.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutter
	{
		typedef void ( Class::*Function )( Index, Value );
		IndexedVariablePutter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				( m_instance->*m_function )( parameter_cast< Index >( index ), parameter_cast< Value >( value ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutter< Class, Value, Index >
	make_indexed_putter( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutterEvt
	{
		typedef void ( Class::*Function )( Index, Value );
		IndexedVariablePutterEvt( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				m_instance->GetEngine()->PostEvent( Castor3D::MakeFunctorEvent( Castor3D::eEVENT_TYPE_PRE_RENDER, [this, index, value]()
				{
					( m_instance->*m_function )( parameter_cast< Index >( index ), parameter_cast< Value >( value ) );
				} ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutterEvt< Class, Value, Index >
	make_indexed_putter_evt( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutterEvt< Class, Value, Index >( ( Class * )instance, function );
	}

#define DECLARE_INDEXED_VAL_PUTTER( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutter< nmspc::type, Value, Index >\
	{\
		typedef void ( nmspc::type::*Function )( Value, Index );\
		IndexedVariablePutter( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			( m_instance->*m_function )( parameter_cast< Value >( value ), index );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutter< nmspc::type, Value, Index >\
	make_indexed_putter( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutter< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}

#define DECLARE_INDEXED_VAL_PUTTER_EVT( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	{\
		typedef void ( nmspc::type::*Function )( Value, Index );\
		IndexedVariablePutterEvt( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			m_instance->GetEngine()->PostEvent( Castor3D::MakeFunctorEvent( Castor3D::eEVENT_TYPE_PRE_RENDER, [this, value, index]()\
			{\
				( m_instance->*m_function )( parameter_cast< Value >( value ), index );\
			} ) );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	make_indexed_putter_evt( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutterEvt< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}
}

#endif
