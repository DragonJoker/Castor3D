/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_METALLIC_ROUGHNESS_PBR_PASS_H__
#define __COMC3D_COM_METALLIC_ROUGHNESS_PBR_PASS_H__

#include "ComRgbColour.hpp"
#include "ComShaderProgram.hpp"

#include <Material/MetallicRoughnessPbrPass.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a MetallicRoughnessPbrPass object accessible from COM.
	\~french
	\brief		Cette classe définit un MetallicRoughnessPbrPass accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMetallicRoughnessPbrPass
		: COM_ATL_OBJECT( MetallicRoughnessPbrPass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMetallicRoughnessPbrPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMetallicRoughnessPbrPass();

		inline castor3d::MetallicRoughnessPbrPassSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::MetallicRoughnessPbrPassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( Emissive
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getEmissive )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setEmissive ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getRefractionRatio )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getAlphaFunc )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getAlphaValue )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::IsTwoSided )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getOpacity )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setOpacity ) );
		COM_PROPERTY( Albedo
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getAlbedo )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setAlbedo ) );
		COM_PROPERTY( Metallic
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getMetallic )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setMetallic ) );
		COM_PROPERTY( Roughness
			, float
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getRoughness )
			, makePutter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::setRoughness ) );

		COM_PROPERTY_GET( TextureUnitCount
			, unsigned int
			, makeGetter( m_internal.get(), &castor3d::MetallicRoughnessPbrPass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::MetallicRoughnessPbrPassSPtr m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( MetallicRoughnessPbrPass ), CMetallicRoughnessPbrPass );

	DECLARE_VARIABLE_PTR_GETTER( MetallicRoughnessPbrPass, castor3d, MetallicRoughnessPbrPass );
	DECLARE_VARIABLE_PTR_PUTTER( MetallicRoughnessPbrPass, castor3d, MetallicRoughnessPbrPass );
}

#endif
