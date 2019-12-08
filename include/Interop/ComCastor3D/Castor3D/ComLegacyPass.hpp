/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LEGACY_PASS_H__
#define __COMC3D_COM_LEGACY_PASS_H__

#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"

#include <Castor3D/Material/PhongPass.hpp>

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
	class ATL_NO_VTABLE CPhongPass
		:	COM_ATL_OBJECT( PhongPass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPhongPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPhongPass();

		inline castor3d::PhongPassSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::PhongPassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY_GET( Type
			, ePASS_TYPE
			, makeGetter( m_internal.get(), &castor3d::Pass::getType ) );
		COM_PROPERTY( Emissive
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getEmissive )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setEmissive ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getRefractionRatio )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getAlphaFunc )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getAlphaValue )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal.get(), &castor3d::PhongPass::IsTwoSided )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getOpacity )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setOpacity ) );
		COM_PROPERTY( Diffuse
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getDiffuse )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setDiffuse ) );
		COM_PROPERTY( Specular
			, IRgbColour *
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getSpecular )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setSpecular ) );
		COM_PROPERTY( Shininess
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getShininess )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setShininess ) );
		COM_PROPERTY( Ambient
			, float
			, makeGetter( m_internal.get(), &castor3d::PhongPass::getAmbient )
			, makePutter( m_internal.get(), &castor3d::PhongPass::setAmbient ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, makeGetter( m_internal.get(), &castor3d::PhongPass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::PhongPassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PhongPass ), CPhongPass );

	DECLARE_VARIABLE_PTR_GETTER( PhongPass, castor3d, PhongPass );
	DECLARE_VARIABLE_PTR_PUTTER( PhongPass, castor3d, PhongPass );
}

#endif
