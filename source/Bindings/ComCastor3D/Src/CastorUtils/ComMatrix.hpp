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
#ifndef __COMC3D_COM_SQUARE_MATRIX_H__
#define __COMC3D_COM_SQUARE_MATRIX_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <SquareMatrix.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMatrix object accessible from COM.
	\~french
	\brief		Cette classe définit un CMatrix accessible depuis COM
	*/
	class ATL_NO_VTABLE CMatrix
		:	COM_ATL_OBJECT( Matrix )
		,	public Castor::Matrix4x4r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CMatrix();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CMatrix();

		STDMETHOD( Transpose )();
		STDMETHOD( Invert )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Matrix ), CMatrix )

	template< typename Class >
	struct VariableParamGetter
	{
		typedef void ( Class::*Function )( Castor::Matrix4x4r & )const;
		VariableParamGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IMatrix ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CMatrix::CreateInstance( value );

					if ( hr == S_OK )
					{
						( m_instance->*m_function )( *static_cast< CMatrix * >( *value ) );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IMatrix,							// This is the GUID of component throwing error
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

	template< typename Class, typename _Class >
	VariableParamGetter< Class >
	make_getter( _Class * instance, void ( Class::*function )( Castor::Matrix4x4r & )const )
	{
		return VariableParamGetter< Class >( ( Class * )instance, function );
	}

	template< typename Class >
	struct VariableRefGetter< Class, Castor::Matrix4x4r >
	{
		typedef Castor::Matrix4x4r const & ( Class::*Function )()const;
		VariableRefGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IMatrix ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CMatrix::CreateInstance( value );

					if ( hr == S_OK )
					{
						*static_cast< Castor::Matrix4x4r * >( static_cast< CMatrix * >( *value ) ) = ( m_instance->*m_function )();
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IMatrix,							// This is the GUID of component throwing error
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

	template< typename Class >
	struct VariablePutter< Class, Castor::Matrix4x4r const & >
	{
		typedef void ( Class::*Function )( Castor::Matrix4x4r const & );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IMatrix * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( *static_cast< CMatrix * >( value ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IMatrix,							// This is the GUID of component throwing error
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
}

#endif
