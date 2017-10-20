/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MOVABLE_OBJECT_H__
#define __COMC3D_COM_MOVABLE_OBJECT_H__

#include "ComAtlObject.hpp"

#include <Scene/MovableObject.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMovableObject object accessible from COM.
	\~french
	\brief		Cette classe définit un CMovableObject accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMovableObject
		:	COM_ATL_OBJECT( MovableObject )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMovableObject();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMovableObject();

		inline castor3d::MovableObjectSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::MovableObjectSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &castor3d::MovableObject::getName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &castor3d::MovableObject::getType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &castor3d::MovableObject::getScene ) );

		STDMETHOD( attachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();

	private:
		castor3d::MovableObjectSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( MovableObject ), CMovableObject );

	//DECLARE_VARIABLE_PTR_GETTER( MovableObject, castor3d, MovableObject );
	//DECLARE_VARIABLE_PTR_PUTTER( MovableObject, castor3d, MovableObject );
}

#endif
