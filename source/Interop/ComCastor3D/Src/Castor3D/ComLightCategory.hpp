/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHT_CATEGORY_H__
#define __COMC3D_COM_LIGHT_CATEGORY_H__

#include "ComVector3D.hpp"

#include <Scene/Light/LightCategory.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CLightCategory object accessible from COM.
	\~french
	\brief		Cette classe définit un CLightCategory accessible depuis COM.
	*/
	class ATL_NO_VTABLE CLightCategory
		:	COM_ATL_OBJECT( LightCategory )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CLightCategory();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CLightCategory();

		inline castor3d::LightCategorySPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::LightCategorySPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Colour, IVector3D *, make_getter( m_internal.get(), &castor3d::LightCategory::getColour ), make_putter( m_internal.get(), &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( AmbientIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getAmbientIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setAmbientIntensity ) );
		COM_PROPERTY( DiffuseIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getDiffuseIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getSpecularIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setSpecularIntensity ) );

	private:
		castor3d::LightCategorySPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LightCategory ), CLightCategory );

	//DECLARE_VARIABLE_PTR_GETTER( LightCategory, castor3d, LightCategory );
	//DECLARE_VARIABLE_PTR_PUTTER( LightCategory, castor3d, LightCategory );
}

#endif
