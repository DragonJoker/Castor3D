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
#ifndef __COMC3D_COM_RASTERISER_STATE_H__
#define __COMC3D_COM_RASTERISER_STATE_H__

#include "ComColour.hpp"

#include <RasteriserState.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CRasteriserState object accessible from COM.
	\~french
	\brief		Cette classe définit un CRasteriserState accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRasteriserState
		:	COM_ATL_OBJECT( RasteriserState )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CRasteriserState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CRasteriserState();

		inline Castor3D::RasteriserStateSPtr GetInternal()const
		{
			return m_state;
		}

		inline void SetInternal( Castor3D::RasteriserStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( FillMode, eFILL_MODE, make_getter( m_state.get(), &Castor3D::RasteriserState::GetFillMode ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetFillMode ) );
		COM_PROPERTY( CulledFaces, eFACE, make_getter( m_state.get(), &Castor3D::RasteriserState::GetCulledFaces ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetCulledFaces ) );
		COM_PROPERTY( FrontCCW, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetFrontCCW ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetFrontCCW ) );
		COM_PROPERTY( AntialiasedLines, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetAntialiasedLines ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetAntialiasedLines ) );
		COM_PROPERTY( DepthBias, float, make_getter( m_state.get(), &Castor3D::RasteriserState::GetDepthBias ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetDepthBias ) );
		COM_PROPERTY( DepthClipping, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetDepthClipping ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetDepthClipping ) );
		COM_PROPERTY( Multisample, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetMultisample ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetMultisample ) );
		COM_PROPERTY( Scissor, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetScissor ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetScissor ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		Castor3D::RasteriserStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RasteriserState ), CRasteriserState );

	DECLARE_VARIABLE_PTR_GETTER( RasteriserState, Castor3D, RasteriserState );
	DECLARE_VARIABLE_PTR_PUTTER( RasteriserState, Castor3D, RasteriserState );
}

#endif
