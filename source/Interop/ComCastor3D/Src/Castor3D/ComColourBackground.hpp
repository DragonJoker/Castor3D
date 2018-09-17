/* See LICENSE file in root folder */
#ifndef __COMC3D_ComColourBackground_H__
#define __COMC3D_ComColourBackground_H__

#include "ComAtlObject.hpp"

#include <Scene/Background/Colour.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		29/06/2018
	\~english
	\brief		This class defines a CColourBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CColourBackground accessible depuis COM.
	*/
	class ATL_NO_VTABLE CColourBackground
		:	COM_ATL_OBJECT( ColourBackground )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CColourBackground();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CColourBackground();

		inline std::shared_ptr< castor3d::ColourBackground > getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( std::shared_ptr< castor3d::ColourBackground > internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Type
			, eBACKGROUND_TYPE
			, makeGetter( m_internal.get(), &castor3d::SceneBackground::getType ) );

	private:
		std::shared_ptr< castor3d::ColourBackground > m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( ColourBackground ), CColourBackground );

	DECLARE_VARIABLE_PTR_GETTER( ColourBackground, castor3d, ColourBackground );
	DECLARE_VARIABLE_PTR_PUTTER( ColourBackground, castor3d, ColourBackground );
}

#endif
