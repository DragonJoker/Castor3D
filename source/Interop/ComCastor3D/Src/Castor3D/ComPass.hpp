/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComColour.hpp"
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

		COM_PROPERTY( TwoSided, boolean, make_getter( m_internal.get(), &castor3d::Pass::IsTwoSided ), make_putter( m_internal.get(), &castor3d::Pass::setTwoSided ) );
		COM_PROPERTY( Opacity, float, make_getter( m_internal.get(), &castor3d::Pass::getOpacity ), make_putter( m_internal.get(), &castor3d::Pass::setOpacity ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, make_getter( m_internal.get(), &castor3d::Pass::getTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( getTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( destroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( getTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		castor3d::PassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );

	DECLARE_VARIABLE_PTR_GETTER( Pass, castor3d, Pass );
	DECLARE_VARIABLE_PTR_PUTTER( Pass, castor3d, Pass );
}

#endif
