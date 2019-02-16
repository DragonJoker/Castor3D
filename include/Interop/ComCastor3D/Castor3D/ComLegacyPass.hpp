/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LEGACY_PASS_H__
#define __COMC3D_COM_LEGACY_PASS_H__

#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"

#include <Castor3D/Material/LegacyPass.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a LegacyPass object accessible from COM.
	\~french
	\brief		Cette classe définit un LegacyPass accessible depuis COM.
	*/
	class ATL_NO_VTABLE CLegacyPass
		:	COM_ATL_OBJECT( LegacyPass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CLegacyPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CLegacyPass();

		inline castor3d::LegacyPassSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::LegacyPassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY_GET( Type
			, ePASS_TYPE
			, makeGetter( m_internal.get(), &castor3d::Pass::getType ) );
		COM_PROPERTY( Emissive
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getEmissive )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setEmissive ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getRefractionRatio )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getAlphaFunc )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getAlphaValue )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::IsTwoSided )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getOpacity )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setOpacity ) );
		COM_PROPERTY( Diffuse
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getDiffuse )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setDiffuse ) );
		COM_PROPERTY( Specular
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getSpecular )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setSpecular ) );
		COM_PROPERTY( Shininess
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getShininess )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setShininess ) );
		COM_PROPERTY( Ambient
			, float
			, makeGetter( m_internal.get(), &castor3d::LegacyPass::getAmbient )
			, makePutter( m_internal.get(), &castor3d::LegacyPass::setAmbient ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, makeGetter( m_internal.get(), &castor3d::LegacyPass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::LegacyPassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LegacyPass ), CLegacyPass );

	DECLARE_VARIABLE_PTR_GETTER( LegacyPass, castor3d, LegacyPass );
	DECLARE_VARIABLE_PTR_PUTTER( LegacyPass, castor3d, LegacyPass );
}

#endif
