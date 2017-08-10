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
#ifndef __COMC3D_COM_DEPTH_STENCIL_STATE_H__
#define __COMC3D_COM_DEPTH_STENCIL_STATE_H__

#include "ComColour.hpp"

#include <State/DepthStencilState.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CDepthStencilState object accessible from COM.
	\~french
	\brief		Cette classe définit un CDepthStencilState accessible depuis COM.
	*/
	class ATL_NO_VTABLE CDepthStencilState
		:	COM_ATL_OBJECT( DepthStencilState )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CDepthStencilState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CDepthStencilState();

		inline castor3d::DepthStencilStateSPtr getInternal()const
		{
			return m_state;
		}

		inline void setInternal( castor3d::DepthStencilStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( DepthTest, boolean, make_getter( m_state.get(), &castor3d::DepthStencilState::getDepthTest ), make_putter( m_state.get(), &castor3d::DepthStencilState::setDepthTest ) );
		COM_PROPERTY( DepthFunc, eDEPTH_FUNC, make_getter( m_state.get(), &castor3d::DepthStencilState::getDepthFunc ), make_putter( m_state.get(), &castor3d::DepthStencilState::setDepthFunc ) );
		COM_PROPERTY( DepthMask, eWRITING_MASK, make_getter( m_state.get(), &castor3d::DepthStencilState::getDepthMask ), make_putter( m_state.get(), &castor3d::DepthStencilState::setDepthMask ) );
		COM_PROPERTY( StencilTest, boolean, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilTest ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilTest ) );
		COM_PROPERTY( StencilReadMask, unsigned long, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilReadMask ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilReadMask ) );
		COM_PROPERTY( StencilWriteMask, unsigned long, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilWriteMask ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilWriteMask ) );
		COM_PROPERTY( StencilFrontFunc, eSTENCIL_FUNC, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilFrontFunc ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilFrontFunc ) );
		COM_PROPERTY( StencilFrontFailOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilFrontFailOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilFrontFailOp ) );
		COM_PROPERTY( StencilFrontDepthFailOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilFrontDepthFailOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilFrontDepthFailOp ) );
		COM_PROPERTY( StencilFrontPassOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilFrontPassOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilFrontPassOp ) );
		COM_PROPERTY( StencilBackFunc, eSTENCIL_FUNC, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilBackFunc ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilBackFunc ) );
		COM_PROPERTY( StencilBackFailOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilBackFailOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilBackFailOp ) );
		COM_PROPERTY( StencilBackDepthFailOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilBackDepthFailOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilBackDepthFailOp ) );
		COM_PROPERTY( StencilBackPassOp, eSTENCIL_OP, make_getter( m_state.get(), &castor3d::DepthStencilState::getStencilBackPassOp ), make_putter( m_state.get(), &castor3d::DepthStencilState::setStencilBackPassOp ) );

		COM_PROPERTY_GET( DepthNear, double, make_getter( m_state.get(), &castor3d::DepthStencilState::getDepthNear ) );
		COM_PROPERTY_GET( DepthFar, double, make_getter( m_state.get(), &castor3d::DepthStencilState::getDepthFar ) );

		STDMETHOD( setDepthRange )( /* [in] */ double dnear, /* [in] */ double dfar );

	private:
		castor3d::DepthStencilStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DepthStencilState ), CDepthStencilState );

	DECLARE_VARIABLE_PTR_GETTER( DepthStencilState, castor3d, DepthStencilState );
	DECLARE_VARIABLE_PTR_PUTTER( DepthStencilState, castor3d, DepthStencilState );
}

#endif
