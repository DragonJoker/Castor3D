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
#ifndef __COMC3D_COM_MESH_H__
#define __COMC3D_COM_MESH_H__

#include "ComAtlObject.hpp"

#include <Mesh.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CMesh accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMesh
		:	COM_ATL_OBJECT( Mesh )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CMesh();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CMesh();

		inline Castor3D::MeshSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::MeshSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( MeshType, eMESH_TYPE, make_getter( m_internal.get(), &Castor3D::Mesh::GetMeshType ) );
		COM_PROPERTY_GET( SubmeshCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Mesh::GetSubmeshCount ) );
		COM_PROPERTY_GET( FaceCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Mesh::GetFaceCount ) );
		COM_PROPERTY_GET( VertexCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Mesh::GetVertexCount ) );

		STDMETHOD( GetSubmesh )( /* [in] */ unsigned int val, /* [out, retval] */ ISubmesh ** pVal );
		STDMETHOD( CreateSubmesh )( /* [out, retval] */ ISubmesh ** pVal );
		STDMETHOD( DeleteSubmesh )( /* [in] */ ISubmesh * val );
	private:
		Castor3D::MeshSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Mesh ), CMesh )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::MeshSPtr >
	{
		typedef Castor3D::MeshSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IMesh ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CMesh::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CMesh * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IMesh,								// This is the GUID of component throwing error
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
	struct VariablePutter< Class, Castor3D::MeshSPtr >
	{
		typedef void ( Class::*Function )( Castor3D::MeshSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IMesh * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CMesh * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IMesh,								// This is the GUID of component throwing error
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
