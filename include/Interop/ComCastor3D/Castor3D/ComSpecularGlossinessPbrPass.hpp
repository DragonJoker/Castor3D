/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SPECULAR_GLOSSINESS_PBR_PASS_H__
#define __COMC3D_COM_SPECULAR_GLOSSINESS_PBR_PASS_H__

#include "ComRgbColour.hpp"

#include <Material/SpecularGlossinessPbrPass.hpp>

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
	class ATL_NO_VTABLE CSpecularGlossinessPbrPass
		:	COM_ATL_OBJECT( SpecularGlossinessPbrPass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSpecularGlossinessPbrPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSpecularGlossinessPbrPass();

		inline castor3d::SpecularGlossinessPbrPassSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::SpecularGlossinessPbrPassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY_GET( Type
			, ePASS_TYPE
			, makeGetter( m_internal.get(), &castor3d::Pass::getType ) );
		COM_PROPERTY( Emissive
			, float
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getEmissive )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setEmissive ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getRefractionRatio )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getAlphaFunc )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getAlphaValue )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::IsTwoSided )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getOpacity )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setOpacity ) );
		COM_PROPERTY( Diffuse
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getDiffuse )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setDiffuse ) );
		COM_PROPERTY( Specular
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getSpecular )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setSpecular ) );
		COM_PROPERTY( Glossiness
			, float
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getGlossiness )
			, makePutter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::setGlossiness ) );

		COM_PROPERTY_GET( TextureUnitCount
			, unsigned int
			, makeGetter( m_internal.get(), &castor3d::SpecularGlossinessPbrPass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::SpecularGlossinessPbrPassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpecularGlossinessPbrPass ), CSpecularGlossinessPbrPass );

	DECLARE_VARIABLE_PTR_GETTER( SpecularGlossinessPbrPass, castor3d, SpecularGlossinessPbrPass );
	DECLARE_VARIABLE_PTR_PUTTER( SpecularGlossinessPbrPass, castor3d, SpecularGlossinessPbrPass );
}

#endif
