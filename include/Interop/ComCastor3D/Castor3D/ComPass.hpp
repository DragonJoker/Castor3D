/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComCastor3D/CastorUtils/ComHdrRgbColour.hpp"
#include "ComCastor3D/CastorUtils/ComHdrRgbaColour.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"
#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"

#include <Castor3D/Material/Pass/Pass.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Pass );
	/*!
	\~english
	\brief		This class defines a CPass object accessible from COM.
	\~french
	\brief		Cette classe définit un CPass accessible depuis COM.
	*/
	class CPass
		: public CComAtlObject< Pass, castor3d::Pass >
	{
	public:
		static castor::RgbColour getEmissiveColour( castor3d::Pass const * pass );
		static void setEmissiveColour( castor3d::Pass * pass, castor::RgbColour v );

		static float getEmissiveFactor( castor3d::Pass const * pass );
		static void setEmissiveFactor( castor3d::Pass * pass, float v );

		static float getRefractionRatio( castor3d::Pass const * pass );
		static void setRefractionRatio( castor3d::Pass * pass, float v );

		static eCOMPARISON_FUNC getAlphaFunc( castor3d::Pass const * pass );
		static void setAlphaFunc( castor3d::Pass * pass, eCOMPARISON_FUNC v );

		static eCOMPARISON_FUNC getBlendAlphaFunc( castor3d::Pass const * pass );
		static void setBlendAlphaFunc( castor3d::Pass * pass, eCOMPARISON_FUNC v );

		static float getAlphaValue( castor3d::Pass const * pass );
		static void setAlphaValue( castor3d::Pass * pass, float v );

		static bool isTwoSided( castor3d::Pass const * pass );
		static void setTwoSided( castor3d::Pass * pass, bool v );

		static float getOpacity( castor3d::Pass const * pass );
		static void setOpacity( castor3d::Pass * pass, float v );

		static castor::HdrRgbColour getAlbedo( castor3d::Pass const * pass );
		static void setAlbedo( castor3d::Pass * pass, castor::HdrRgbColour v );

		static castor::RgbColour getDiffuse( castor3d::Pass const * pass );
		static void setDiffuse( castor3d::Pass * pass, castor::RgbColour v );

		static castor::RgbColour getSpecular( castor3d::Pass const * pass );
		static void setSpecular( castor3d::Pass * pass, castor::RgbColour v );

		static float getMetalness( castor3d::Pass const * pass );
		static void setMetalness( castor3d::Pass * pass, float v );

		static float getRoughness( castor3d::Pass const * pass );
		static void setRoughness( castor3d::Pass * pass, float v );

		static float getGlossiness( castor3d::Pass const * pass );
		static void setGlossiness( castor3d::Pass * pass, float v );

		static float getShininess( castor3d::Pass const * pass );
		static void setShininess( castor3d::Pass * pass, float v );

		COM_PROPERTY_GET( Type
			, UINT
			, makeGetter( m_internal, &castor3d::Pass::getTypeID ) );
		COM_PROPERTY_GET( TextureUnitCount
			, uint32_t
			, makeGetter( m_internal, &castor3d::Pass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );

		COM_PROPERTY( EmissiveColour
			, IRgbColour *
			, makeGetter( m_internal, getEmissiveColour )
			, makePutter( m_internal, setEmissiveColour ) );
		COM_PROPERTY( EmissiveFactor
			, float
			, makeGetter( m_internal, getEmissiveFactor )
			, makePutter( m_internal, setEmissiveFactor ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal, getRefractionRatio )
			, makePutter( m_internal, setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal, getAlphaFunc )
			, makePutter( m_internal, setAlphaFunc ) );
		COM_PROPERTY( BlendAlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal, getBlendAlphaFunc )
			, makePutter( m_internal, setBlendAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal, getAlphaValue )
			, makePutter( m_internal, setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal, isTwoSided )
			, makePutter( m_internal, setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal, getOpacity )
			, makePutter( m_internal, setOpacity ) );
		COM_PROPERTY( Albedo
			, IHdrRgbColour *
			, makeGetter( m_internal, getAlbedo )
			, makePutter( m_internal, setAlbedo ) );
		COM_PROPERTY( Diffuse
			, IRgbColour *
			, makeGetter( m_internal, getDiffuse )
			, makePutter( m_internal, setDiffuse ) );
		COM_PROPERTY( Specular
			, IRgbColour *
			, makeGetter( m_internal, getSpecular )
			, makePutter( m_internal, setSpecular ) );
		COM_PROPERTY( Metalness
			, float
			, makeGetter( m_internal, getMetalness )
			, makePutter( m_internal, setMetalness ) );
		COM_PROPERTY( Roughness
			, float
			, makeGetter( m_internal, getRoughness )
			, makePutter( m_internal, setRoughness ) );
		COM_PROPERTY( Glossiness
			, float
			, makeGetter( m_internal, getGlossiness )
			, makePutter( m_internal, setGlossiness ) );
		COM_PROPERTY( Shininess
			, float
			, makeGetter( m_internal, getShininess )
			, makePutter( m_internal, setShininess ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );
}

#endif
