/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_BLEND_STATE_H__
#define __COMC3D_COM_BLEND_STATE_H__

#include "ComColour.hpp"

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

		COM_PROPERTY( IndependantBlendEnabled, boolean, make_getter( m_state.get(), &castor3d::BlendState::isIndependantBlendEnabled ), make_putter( m_state.get(), &castor3d::BlendState::enableIndependantBlend ) );
		COM_PROPERTY( BlendFactors, IColour *, make_getter( m_state.get(), &castor3d::BlendState::getBlendFactors ), make_putter( m_state.get(), &castor3d::BlendState::setBlendFactors ) );

		COM_PROPERTY_INDEXED( BlendEnabled, unsigned int, boolean, make_indexed_getter( m_state.get(), &castor3d::BlendState::isBlendEnabled ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::enableBlend ) );
		COM_PROPERTY_INDEXED( RgbSrcBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &castor3d::BlendState::getRgbSrcBlend ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setRgbSrcBlend ) );
		COM_PROPERTY_INDEXED( RgbDstBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &castor3d::BlendState::getRgbDstBlend ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setRgbDstBlend ) );
		COM_PROPERTY_INDEXED( RgbBlendOp, unsigned int, eBLEND_OP, make_indexed_getter( m_state.get(), &castor3d::BlendState::getRgbBlendOp ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setRgbBlendOp ) );
		COM_PROPERTY_INDEXED( AlphaSrcBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &castor3d::BlendState::getAlphaSrcBlend ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setAlphaSrcBlend ) );
		COM_PROPERTY_INDEXED( AlphaDstBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &castor3d::BlendState::getAlphaDstBlend ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setAlphaDstBlend ) );
		COM_PROPERTY_INDEXED( AlphaBlendOp, unsigned int, eBLEND_OP, make_indexed_getter( m_state.get(), &castor3d::BlendState::getAlphaBlendOp ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setAlphaBlendOp ) );
		COM_PROPERTY_INDEXED( WriteMask, unsigned int, byte, make_indexed_getter( m_state.get(), &castor3d::BlendState::getWriteMask ), make_indexed_putter_rev( m_state.get(), &castor3d::BlendState::setWriteMask ) );

		STDMETHOD( setColourMask )( /* [in] */ eWRITING_MASK red, /* [in] */ eWRITING_MASK green, /* [in] */ eWRITING_MASK blue, /* [in] */ eWRITING_MASK alpha );

	private:
		castor3d::BlendStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( BlendState ), CBlendState );

	DECLARE_VARIABLE_PTR_GETTER( BlendState, castor3d, BlendState );
	DECLARE_VARIABLE_PTR_PUTTER( BlendState, castor3d, BlendState );
}

#endif
