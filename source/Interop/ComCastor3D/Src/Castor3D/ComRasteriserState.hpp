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
#ifndef __COMC3D_COM_RASTERISER_STATE_H__
#define __COMC3D_COM_RASTERISER_STATE_H__

#include "ComColour.hpp"

#include <State/RasteriserState.hpp>

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
		CRasteriserState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRasteriserState();

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
		COM_PROPERTY( Scissor, boolean, make_getter( m_state.get(), &Castor3D::RasteriserState::GetScissor ), make_putter( m_state.get(), &Castor3D::RasteriserState::SetScissor ) );

	private:
		Castor3D::RasteriserStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RasteriserState ), CRasteriserState );

	DECLARE_VARIABLE_PTR_GETTER( RasteriserState, Castor3D, RasteriserState );
	DECLARE_VARIABLE_PTR_PUTTER( RasteriserState, Castor3D, RasteriserState );
}

#endif
