/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef __COMC3D_COM_DEPTH_STENCIL_STATE_H__
#define __COMC3D_COM_DEPTH_STENCIL_STATE_H__

#include "ComColour.hpp"

#include <DepthStencilState.hpp>

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

		inline Castor3D::DepthStencilStateSPtr GetInternal()const
		{
			return m_state;
		}

		inline void SetInternal( Castor3D::DepthStencilStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( DepthTest, boolean, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetDepthTest ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetDepthTest ) );
		COM_PROPERTY( DepthFunc, eDEPTH_FUNC, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetDepthFunc ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetDepthFunc ) );
		COM_PROPERTY( DepthMask, eWRITING_MASK, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetDepthMask ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetDepthMask ) );
		COM_PROPERTY( StencilTest, boolean, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilTest ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilTest ) );
		COM_PROPERTY( StencilReadMask, unsigned long, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilReadMask ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilReadMask ) );
		COM_PROPERTY( StencilWriteMask, unsigned long, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilWriteMask ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilWriteMask ) );
		COM_PROPERTY( StencilFrontFunc, eSTENCIL_FUNC, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilFrontFunc ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilFrontFunc ) );
		COM_PROPERTY( StencilFrontFailOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilFrontFailOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilFrontFailOp ) );
		COM_PROPERTY( StencilFrontDepthFailOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilFrontDepthFailOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilFrontDepthFailOp ) );
		COM_PROPERTY( StencilFrontPassOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilFrontPassOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilFrontPassOp ) );
		COM_PROPERTY( StencilBackFunc, eSTENCIL_FUNC, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilBackFunc ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilBackFunc ) );
		COM_PROPERTY( StencilBackFailOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilBackFailOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilBackFailOp ) );
		COM_PROPERTY( StencilBackDepthFailOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilBackDepthFailOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilBackDepthFailOp ) );
		COM_PROPERTY( StencilBackPassOp, eSTENCIL_OP, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetStencilBackPassOp ), make_putter( m_state.get(), &Castor3D::DepthStencilState::SetStencilBackPassOp ) );

		COM_PROPERTY_GET( DepthNear, double, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetDepthNear ) );
		COM_PROPERTY_GET( DepthFar, double, make_getter( m_state.get(), &Castor3D::DepthStencilState::GetDepthFar ) );

		STDMETHOD( SetDepthRange )( /* [in] */ double dnear, /* [in] */ double dfar );
		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		Castor3D::DepthStencilStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DepthStencilState ), CDepthStencilState );

	DECLARE_VARIABLE_PTR_GETTER( DepthStencilState, Castor3D, DepthStencilState );
	DECLARE_VARIABLE_PTR_PUTTER( DepthStencilState, Castor3D, DepthStencilState );
}

#endif
