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

		inline castor3d::RasteriserStateSPtr getInternal()const
		{
			return m_state;
		}

		inline void setInternal( castor3d::RasteriserStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( FillMode, eFILL_MODE, make_getter( m_state.get(), &castor3d::RasteriserState::getFillMode ), make_putter( m_state.get(), &castor3d::RasteriserState::setFillMode ) );
		COM_PROPERTY( CulledFaces, eFACE, make_getter( m_state.get(), &castor3d::RasteriserState::getCulledFaces ), make_putter( m_state.get(), &castor3d::RasteriserState::setCulledFaces ) );
		COM_PROPERTY( FrontCCW, boolean, make_getter( m_state.get(), &castor3d::RasteriserState::getFrontCCW ), make_putter( m_state.get(), &castor3d::RasteriserState::setFrontCCW ) );
		COM_PROPERTY( AntialiasedLines, boolean, make_getter( m_state.get(), &castor3d::RasteriserState::getAntialiasedLines ), make_putter( m_state.get(), &castor3d::RasteriserState::setAntialiasedLines ) );
		COM_PROPERTY( DepthBiasUnits, float, make_getter( m_state.get(), &castor3d::RasteriserState::getDepthBiasUnits ), make_putter( m_state.get(), &castor3d::RasteriserState::setDepthBiasUnits ) );
		COM_PROPERTY( DepthBiasFactor, float, make_getter( m_state.get(), &castor3d::RasteriserState::getDepthBiasFactor ), make_putter( m_state.get(), &castor3d::RasteriserState::setDepthBiasFactor ) );
		COM_PROPERTY( DepthClipping, boolean, make_getter( m_state.get(), &castor3d::RasteriserState::getDepthClipping ), make_putter( m_state.get(), &castor3d::RasteriserState::setDepthClipping ) );
		COM_PROPERTY( Scissor, boolean, make_getter( m_state.get(), &castor3d::RasteriserState::getScissor ), make_putter( m_state.get(), &castor3d::RasteriserState::setScissor ) );

	private:
		castor3d::RasteriserStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RasteriserState ), CRasteriserState );

	DECLARE_VARIABLE_PTR_GETTER( RasteriserState, castor3d, RasteriserState );
	DECLARE_VARIABLE_PTR_PUTTER( RasteriserState, castor3d, RasteriserState );
}

#endif
