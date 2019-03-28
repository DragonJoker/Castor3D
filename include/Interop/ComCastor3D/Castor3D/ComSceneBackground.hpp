/* See LICENSE file in root folder */
#ifndef __COMC3D_ComSceneBackground_H__
#define __COMC3D_ComSceneBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Background/Background.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		29/06/2018
	\~english
	\brief		This class defines a CSceneBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CSceneBackground accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSceneBackground
		:	COM_ATL_OBJECT( SceneBackground )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSceneBackground();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSceneBackground();

		inline castor3d::SceneBackgroundSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::SceneBackgroundSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Type
			, eBACKGROUND_TYPE
			, makeGetter( m_internal.get(), &castor3d::SceneBackground::getType ) );

	private:
		castor3d::SceneBackgroundSPtr m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( SceneBackground ), CSceneBackground );

	DECLARE_VARIABLE_PTR_GETTER( SceneBackground, castor3d, SceneBackground );
	DECLARE_VARIABLE_PTR_PUTTER( SceneBackground, castor3d, SceneBackground );
}

#endif
