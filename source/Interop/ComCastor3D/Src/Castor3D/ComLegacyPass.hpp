/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LEGACY_PASS_H__
#define __COMC3D_COM_LEGACY_PASS_H__

#include "ComColour.hpp"
#include "ComHdrColour.hpp"
#include "ComShaderProgram.hpp"

#include <Material/LegacyPass.hpp>

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

		COM_PROPERTY( Diffuse, IColour *, make_getter( m_internal.get(), &castor3d::LegacyPass::getDiffuse ), make_putter( m_internal.get(), &castor3d::LegacyPass::setDiffuse ) );
		COM_PROPERTY( Specular, IColour *, make_getter( m_internal.get(), &castor3d::LegacyPass::getSpecular ), make_putter( m_internal.get(), &castor3d::LegacyPass::setSpecular ) );
		COM_PROPERTY( Emissive, IHdrColour *, make_getter( m_internal.get(), &castor3d::LegacyPass::getEmissive ), make_putter( m_internal.get(), &castor3d::LegacyPass::setEmissive ) );
		COM_PROPERTY( Shininess, float, make_getter( m_internal.get(), &castor3d::LegacyPass::getShininess ), make_putter( m_internal.get(), &castor3d::LegacyPass::setShininess ) );
		COM_PROPERTY( TwoSided, boolean, make_getter( m_internal.get(), &castor3d::LegacyPass::IsTwoSided ), make_putter( m_internal.get(), &castor3d::LegacyPass::setTwoSided ) );
		COM_PROPERTY( Opacity, float, make_getter( m_internal.get(), &castor3d::LegacyPass::getOpacity ), make_putter( m_internal.get(), &castor3d::LegacyPass::setOpacity ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, make_getter( m_internal.get(), &castor3d::LegacyPass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( getTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( destroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( getTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::LegacyPassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LegacyPass ), CLegacyPass );

	DECLARE_VARIABLE_PTR_GETTER( LegacyPass, castor3d, LegacyPass );
	DECLARE_VARIABLE_PTR_PUTTER( LegacyPass, castor3d, LegacyPass );
}

#endif
