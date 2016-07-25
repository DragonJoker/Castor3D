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
#ifndef __COMC3D_COM_SCENE_NODE_H__
#define __COMC3D_COM_SCENE_NODE_H__

#include "ComTextureLayout.hpp"
#include "ComColour.hpp"
#include "ComVector3D.hpp"
#include "ComQuaternion.hpp"

#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

namespace CastorCom
{
	template< typename Value >
	struct VariablePutterEvt< Castor3D::SceneNode, Value >
	{
		typedef void ( Castor3D::SceneNode::*Function )( Value );
		VariablePutterEvt( Castor3D::SceneNode * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					m_instance->GetScene()->GetEngine()->PostEvent( Castor3D::MakeFunctorEvent( Castor3D::eEVENT_TYPE_PRE_RENDER, [this, value]
					{
						( m_instance->*m_function )( parameter_cast< Value >( value ) );
					} ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_ISceneNode, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Castor3D::SceneNode * m_instance;
		Function m_function;
	};

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
		CSceneNode();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSceneNode();

		inline Castor3D::SceneNodeSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::SceneNodeSPtr internal )
		{
			m_internal = internal;
		}

		COM_EVT_PROPERTY( Position, IVector3D *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetPosition ), make_putter_evt( m_internal.get(), &Castor3D::SceneNode::SetPosition ) );
		COM_EVT_PROPERTY( Orientation, IQuaternion *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetOrientation ), make_putter_evt( m_internal.get(), &Castor3D::SceneNode::SetOrientation ) );
		COM_EVT_PROPERTY( Scaling, IVector3D *, make_getter( m_internal.get(), &Castor3D::SceneNode::GetScale ), make_putter_evt( m_internal.get(), &Castor3D::SceneNode::SetScale ) );

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
	OBJECT_ENTRY_AUTO( __uuidof( SceneNode ), CSceneNode );

	DECLARE_VARIABLE_PTR_GETTER( SceneNode, Castor3D, SceneNode );
	DECLARE_VARIABLE_PTR_PUTTER( SceneNode, Castor3D, SceneNode );
}

#endif
