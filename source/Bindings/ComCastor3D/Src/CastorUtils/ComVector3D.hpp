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
#ifndef __COMC3D_COM_VECTOR_3D_H__
#define __COMC3D_COM_VECTOR_3D_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"

#include <Point.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CVector3D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector3D accessible depuis COM.
	*/
	class ATL_NO_VTABLE CVector3D
		:	COM_ATL_OBJECT( Vector3D )
		,	public Castor::Point3r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CVector3D();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CVector3D();

		COM_PROPERTY( X, FLOAT, make_getter( this, &Castor::Point3r::operator[], 0 ), make_putter( this, &Castor::Point3r::operator[], 0 ) );
		COM_PROPERTY( Y, FLOAT, make_getter( this, &Castor::Point3r::operator[], 1 ), make_putter( this, &Castor::Point3r::operator[], 1 ) );
		COM_PROPERTY( Z, FLOAT, make_getter( this, &Castor::Point3r::operator[], 2 ), make_putter( this, &Castor::Point3r::operator[], 2 ) );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector3D * pVal, FLOAT * pRet );
		STDMETHOD( Cross )( /* [in] */ IVector3D * val, /* [out,retval] */ IVector3D ** pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector3D ), CVector3D )

	template< typename Class >
	struct VariableGetter< Class, Castor::Point3r >
	{
		typedef Castor::Point3r( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IVector3D ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CVector3D::CreateInstance( value );

					if ( hr == S_OK )
					{
						*static_cast< Castor::Point3r * >( static_cast< CVector3D * >( *value ) ) = ( m_instance->*m_function )();
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IVector3D,							// This is the GUID of component throwing error
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
	struct VariableRefGetter< Class, Castor::Point3r >
	{
		typedef Castor::Point3r const & ( Class::*Function )()const;
		VariableRefGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IVector3D ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CVector3D::CreateInstance( value );

					if ( hr == S_OK )
					{
						*static_cast< Castor::Point3r * >( static_cast< CVector3D * >( *value ) ) = ( m_instance->*m_function )();
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IVector3D,							// This is the GUID of component throwing error
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
	struct VariablePutter< Class, Castor::Point3r const & >
	{
		typedef void ( Class::*Function )( Castor::Point3r const & );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IVector3D * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( *static_cast< CVector3D * >( value ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IVector3D,							// This is the GUID of component throwing error
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
