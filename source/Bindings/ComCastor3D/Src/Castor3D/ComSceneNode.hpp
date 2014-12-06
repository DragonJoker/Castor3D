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
#ifndef __COMC3D_COM_SCENE_NODE_H__
#define __COMC3D_COM_SCENE_NODE_H__

#include "ComTextureBase.hpp"
#include "ComColour.hpp"
#include "ComVector3D.hpp"
#include "ComQuaternion.hpp"

#include <SceneNode.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSceneNode object accessible from COM.
	\~french
	\brief		Cette classe définit un CSceneNode accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSceneNode
		:	COM_ATL_OBJECT( SceneNode )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CSceneNode();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CSceneNode();

		inline Castor3D::SceneNodeSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::SceneNodeSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Position, IVector3D *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetPosition ), make_putter( m_internal.get(), &Castor3D::SceneNode::SetPosition ) );
		COM_PROPERTY( Orientation, IQuaternion *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetOrientation ), make_putter( m_internal.get(), &Castor3D::SceneNode::SetOrientation ) );
		COM_PROPERTY( Scaling, IVector3D *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetScale ), make_putter( m_internal.get(), &Castor3D::SceneNode::SetScale ) );

		STDMETHOD( AttachObject )( /* [in] */ IMovableObject * val );
		STDMETHOD( DetachObject )( /* [in] */ IMovableObject * val );
		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
		STDMETHOD( Yaw )( /* [in] */ IAngle * val );
		STDMETHOD( Pitch )( /* [in] */ IAngle * val );
		STDMETHOD( Roll )( /* [in] */ IAngle * val );
		STDMETHOD( Rotate )( /* [in] */ IQuaternion * val );
		STDMETHOD( Translate )( /* [in] */ IVector3D * val );
		STDMETHOD( Scale )( /* [in] */ IVector3D * val );
	private:
		Castor3D::SceneNodeSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SceneNode ), CSceneNode )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::SceneNodeSPtr >
	{
		typedef Castor3D::SceneNodeSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ISceneNode ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CSceneNode::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CSceneNode * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_ISceneNode,						// This is the GUID of component throwing error
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
	struct VariablePutter< Class, Castor3D::SceneNodeSPtr >
	{
		typedef void ( Class::*Function )( Castor3D::SceneNodeSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ISceneNode * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CSceneNode * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_ISceneNode,						// This is the GUID of component throwing error
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
