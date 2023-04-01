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

		COMEX_PROPERTY_GET( TextureUnitCount, uint32_t, m_internal, &castor3d::Pass::getTextureUnitsCount );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );

		COMEX_PROPERTY( EmissiveColour, IRgbColour *, m_internal, getEmissiveColour, setEmissiveColour );
		COMEX_PROPERTY( EmissiveFactor, float, m_internal, getEmissiveFactor, setEmissiveFactor );
		COMEX_PROPERTY( RefractionRatio, float, m_internal, getRefractionRatio, setRefractionRatio );
		COMEX_PROPERTY( AlphaFunc, eCOMPARISON_FUNC, m_internal, getAlphaFunc, setAlphaFunc );
		COMEX_PROPERTY( BlendAlphaFunc, eCOMPARISON_FUNC, m_internal, getBlendAlphaFunc, setBlendAlphaFunc );
		COMEX_PROPERTY( AlphaRefValue, float, m_internal, getAlphaValue, setAlphaValue );
		COMEX_PROPERTY( TwoSided, boolean, m_internal, isTwoSided, setTwoSided );
		COMEX_PROPERTY( Opacity, float, m_internal, getOpacity, setOpacity );
		COMEX_PROPERTY( Albedo, IHdrRgbColour *, m_internal, getAlbedo, setAlbedo );
		COMEX_PROPERTY( Diffuse, IRgbColour *, m_internal, getDiffuse, setDiffuse );
		COMEX_PROPERTY( Specular, IRgbColour *, m_internal, getSpecular, setSpecular );
		COMEX_PROPERTY( Metalness, float, m_internal, getMetalness, setMetalness );
		COMEX_PROPERTY( Roughness, float, m_internal, getRoughness, setRoughness );
		COMEX_PROPERTY( Glossiness, float, m_internal, getGlossiness, setGlossiness );
		COMEX_PROPERTY( Shininess, float, m_internal, getShininess, setShininess );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );
}

#endif
