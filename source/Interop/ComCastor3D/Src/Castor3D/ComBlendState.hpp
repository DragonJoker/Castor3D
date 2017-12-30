/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_BLEND_STATE_H__
#define __COMC3D_COM_BLEND_STATE_H__

#include "ComRgbaColour.hpp"

#include <State/BlendState.hpp>

namespace CastorCom
{
	DECLARE_INDEXED_VAL_PUTTER_EVT( castor3d, BlendState );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CBlendState object accessible from COM.
	\~french
	\brief		Cette classe définit un CBlendState accessible depuis COM.
	*/
	class ATL_NO_VTABLE CBlendState
		:	COM_ATL_OBJECT( BlendState )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CBlendState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CBlendState();

		inline castor3d::BlendStateSPtr getInternal()const
		{
			return m_state;
		}

		inline void setInternal( castor3d::BlendStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( IndependantBlendEnabled, boolean, makeGetter( m_state.get(), &castor3d::BlendState::isIndependantBlendEnabled ), makePutter( m_state.get(), &castor3d::BlendState::enableIndependantBlend ) );
		COM_PROPERTY( BlendFactors, IRgbaColour *, makeGetter( m_state.get(), &castor3d::BlendState::getBlendFactors ), makePutter( m_state.get(), &castor3d::BlendState::setBlendFactors ) );

		COM_PROPERTY_INDEXED( BlendEnabled, unsigned int, boolean, makeIndexedGetter( m_state.get(), &castor3d::BlendState::isBlendEnabled ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::enableBlend ) );
		COM_PROPERTY_INDEXED( RgbSrcBlend, unsigned int, eBLEND, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getRgbSrcBlend ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setRgbSrcBlend ) );
		COM_PROPERTY_INDEXED( RgbDstBlend, unsigned int, eBLEND, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getRgbDstBlend ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setRgbDstBlend ) );
		COM_PROPERTY_INDEXED( RgbBlendOp, unsigned int, eBLEND_OP, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getRgbBlendOp ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setRgbBlendOp ) );
		COM_PROPERTY_INDEXED( AlphaSrcBlend, unsigned int, eBLEND, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getAlphaSrcBlend ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setAlphaSrcBlend ) );
		COM_PROPERTY_INDEXED( AlphaDstBlend, unsigned int, eBLEND, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getAlphaDstBlend ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setAlphaDstBlend ) );
		COM_PROPERTY_INDEXED( AlphaBlendOp, unsigned int, eBLEND_OP, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getAlphaBlendOp ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setAlphaBlendOp ) );
		COM_PROPERTY_INDEXED( WriteMask, unsigned int, byte, makeIndexedGetter( m_state.get(), &castor3d::BlendState::getWriteMask ), makeIndexedPutterRev( m_state.get(), &castor3d::BlendState::setWriteMask ) );

		STDMETHOD( SetColourMask )( /* [in] */ eWRITING_MASK red, /* [in] */ eWRITING_MASK green, /* [in] */ eWRITING_MASK blue, /* [in] */ eWRITING_MASK alpha );

	private:
		castor3d::BlendStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( BlendState ), CBlendState );

	DECLARE_VARIABLE_PTR_GETTER( BlendState, castor3d, BlendState );
	DECLARE_VARIABLE_PTR_PUTTER( BlendState, castor3d, BlendState );
}

#endif
