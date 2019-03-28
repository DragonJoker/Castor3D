/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_NODE_H__
#define __COMC3D_COM_SCENE_NODE_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"

#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace CastorCom
{
	template< typename Value >
	struct VariablePutterEvt< castor3d::SceneNode, Value >
	{
		typedef void ( castor3d::SceneNode::*Function )( Value );
		VariablePutterEvt( castor3d::SceneNode * instance, Function function )
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
					m_instance->getScene()->getEngine()->postEvent( castor3d::makeFunctorEvent( castor3d::EventType::ePreRender
						, [this, value]
						{
							( m_instance->*m_function )( parameterCast< Value >( value ) );
						} ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL, IID_ISceneNode, cuT( "nullptr instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, nullptr );
			}

			return hr;
		}

	private:
		castor3d::SceneNode * m_instance;
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

		inline castor3d::SceneNodeSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::SceneNodeSPtr internal )
		{
			m_internal = internal;
		}

		COM_EVT_PROPERTY( Position, IVector3D *, makeGetter( m_internal.get(), &castor3d::SceneNode::getPosition ), makePutterEvt( m_internal.get(), &castor3d::SceneNode::setPosition ) );
		COM_EVT_PROPERTY( Orientation, IQuaternion *, makeGetter( m_internal.get(), &castor3d::SceneNode::getOrientation ), makePutterEvt( m_internal.get(), &castor3d::SceneNode::setOrientation ) );
		COM_EVT_PROPERTY( Scaling, IVector3D *, makeGetter( m_internal.get(), &castor3d::SceneNode::getScale ), makePutterEvt( m_internal.get(), &castor3d::SceneNode::setScale ) );

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
		castor3d::SceneNodeSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SceneNode ), CSceneNode );

	DECLARE_VARIABLE_PTR_GETTER( SceneNode, castor3d, SceneNode );
	DECLARE_VARIABLE_PTR_PUTTER( SceneNode, castor3d, SceneNode );
}

#endif
