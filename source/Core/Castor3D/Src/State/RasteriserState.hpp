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
#ifndef ___C3D_RASTER_STATE_H___
#define ___C3D_RASTER_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Rasteriser configuration class
	\~french
	\brief		Classe de configuration du rasteriser
	*/
	class RasteriserState
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit RasteriserState( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RasteriserState();
		/**
		 *\~english
		 *\brief		Retrieves the fill mode
		 *\return		The value
		 *\~french
		 *\brief		Récupère le mode de remplissage
		 *\return		La valeur
		 */
		inline Castor3D::eFILL_MODE GetFillMode()const
		{
			return m_eFillMode;
		}
		/**
		 *\~english
		 *\brief		Sets the fill mode
		 *\param[in]	p_mode	The new value
		 *\~french
		 *\brief		Définit le mode de remplissage
		 *\param[in]	p_mode	La nouvelle valeur
		 */
		inline void SetFillMode( Castor3D::eFILL_MODE p_mode )
		{
			m_eFillMode = p_mode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the culled faces
		 *\return		The value
		 *\~french
		 *\brief		Récupère les faces cachées
		 *\return		La valeur
		 */
		inline Castor3D::eFACE GetCulledFaces()const
		{
			return m_eCulledFaces;
		}
		/**
		 *\~english
		 *\brief		Sets the culled faces
		 *\param[in]	p_eFace	The new value
		 *\~french
		 *\brief		Définit les faces cachées
		 *\param[in]	p_eFace	La nouvelle valeur
		 */
		inline void SetCulledFaces( Castor3D::eFACE p_eFace )
		{
			m_eCulledFaces = p_eFace;
		}
		/**
		 *\~english
		 *\brief		Retrieves the faces considered front facing
		 *\return		The value
		 *\~french
		 *\brief		Récupère les faces considérées comme tournées vers l'avant
		 *\return		La valeur
		 */
		inline bool GetFrontCCW()const
		{
			return m_bFrontCCW;
		}
		/**
		 *\~english
		 *\brief		Sets the faces considered front facing
		 *\param[in]	p_bCCW	The new value
		 *\~french
		 *\brief		Définit les faces considérées comme tournées vers l'avant
		 *\param[in]	p_bCCW	La nouvelle valeur
		 */
		inline void SetFrontCCW( bool p_bCCW )
		{
			m_bFrontCCW = p_bCCW;
		}
		/**
		 *\~english
		 *\brief		Retrieves the antialiased lines status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'antialising des lignes
		 *\return		La valeur
		 */
		inline bool GetAntialiasedLines()const
		{
			return m_bAntialiasedLines;
		}
		/**
		 *\~english
		 *\brief		Sets the antialiased lines status
		 *\param[in]	p_bAA	The new value
		 *\~french
		 *\brief		Définit le statut d'antialising des lignes
		 *\param[in]	p_bAA	La nouvelle valeur
		 */
		inline void SetAntialiasedLines( bool p_bAA )
		{
			m_bAntialiasedLines = p_bAA;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth bias
		 *\return		The value
		 *\~french
		 *\brief		Récupère le depth bias
		 *\return		La valeur
		 */
		inline float GetDepthBias()const
		{
			return m_fDepthBias;
		}
		/**
		 *\~english
		 *\brief		Sets the depth bias
		 *\param[in]	p_fBias	The new value
		 *\~french
		 *\brief		Définit le depth bias
		 *\param[in]	p_fBias	La nouvelle valeur
		 */
		inline void SetDepthBias( float p_fBias )
		{
			m_fDepthBias = p_fBias;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth clipping status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de depth clipping
		 *\return		La valeur
		 */
		inline bool GetDepthClipping()const
		{
			return m_bDepthClipping;
		}
		/**
		 *\~english
		 *\brief		Sets the depth clipping status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut de depth clipping
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void SetDepthClipping( bool p_enable )
		{
			m_bDepthClipping = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scissor test activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'acitvation du scissor test
		 *\return		La valeur
		 */
		inline bool GetScissor()const
		{
			return m_bScissor;
		}
		/**
		 *\~english
		 *\brief		Sets the scissor test activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'acitvation du scissor test
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void SetScissor( bool p_enable )
		{
			m_bScissor = p_enable;
		}

	protected:
		//!\~english	The fill mode.
		//!\~french		Définit le mode de remplissage.
		eFILL_MODE m_eFillMode{ eFILL_MODE_SOLID };
		//!\~english	The culled faces.
		//!\~french		Les faces cachées.
		eFACE m_eCulledFaces{ eFACE_BACK };
		//!\~english	The faces considered front facing.
		//!\~french		Les faces considérées comme tournées vers l'avant.
		bool m_bFrontCCW{ true };
		//!\~english	The antialiased lines status.
		//!\~french		Le statut d'antialising des lignes.
		bool m_bAntialiasedLines{ false };
		//!\~english	The depth bias.
		//!\~french		Le depth bias.
		float m_fDepthBias{ 0.0f };
		//!\~english	The scissor test activation status.
		//!\~french		Le statut d'acitvation du scissor test.
		bool m_bScissor{ false };
		//!\~english	The depth clipping status.
		//!\~french		Le statut de depth clipping.
		bool m_bDepthClipping{ true };
	};
}

#endif
