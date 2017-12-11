/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComRgbColour.hpp"
#include "ComShaderProgram.hpp"

#include <Material/Pass.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CPass object accessible from COM.
	\~french
	\brief		Cette classe définit un CPass accessible depuis COM.
	*/
	class ATL_NO_VTABLE CPass
		:	COM_ATL_OBJECT( Pass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPass();

		inline castor3d::PassSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::PassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( Emissive
			, float
			, makeGetter( m_internal.get(), &castor3d::Pass::getEmissive )
			, makePutter( m_internal.get(), &castor3d::Pass::setEmissive ) );
		COM_PROPERTY( RefractionRatio
			, float
			, makeGetter( m_internal.get(), &castor3d::Pass::getRefractionRatio )
			, makePutter( m_internal.get(), &castor3d::Pass::setRefractionRatio ) );
		COM_PROPERTY( AlphaFunc
			, eCOMPARISON_FUNC
			, makeGetter( m_internal.get(), &castor3d::Pass::getAlphaFunc )
			, makePutter( m_internal.get(), &castor3d::Pass::setAlphaFunc ) );
		COM_PROPERTY( AlphaRefValue
			, float
			, makeGetter( m_internal.get(), &castor3d::Pass::getAlphaValue )
			, makePutter( m_internal.get(), &castor3d::Pass::setAlphaValue ) );
		COM_PROPERTY( TwoSided
			, boolean
			, makeGetter( m_internal.get(), &castor3d::Pass::IsTwoSided )
			, makePutter( m_internal.get(), &castor3d::Pass::setTwoSided ) );
		COM_PROPERTY( Opacity
			, float
			, makeGetter( m_internal.get(), &castor3d::Pass::getOpacity )
			, makePutter( m_internal.get(), &castor3d::Pass::setOpacity ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, makeGetter( m_internal.get(), &castor3d::Pass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::PassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );

	DECLARE_VARIABLE_PTR_GETTER( Pass, castor3d, Pass );
	DECLARE_VARIABLE_PTR_PUTTER( Pass, castor3d, Pass );
}

#endif
