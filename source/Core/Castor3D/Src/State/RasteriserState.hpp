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
	{
	public:
		/**
		 *\~english
		 *\return		The fill mode.
		 *\~french
		 *\return		La mode de remplissage.
		 */
		inline Castor3D::FillMode GetFillMode()const
		{
			return m_fillMode;
		}
		/**
		 *\~english
		 *\brief		Sets the fill mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode de remplissage.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetFillMode( Castor3D::FillMode p_value )
		{
			m_fillMode = p_value;
		}
		/**
		 *\~english
		 *\return		The culled faces
		 *\~french
		 *\return		Les faces cachées
		 */
		inline Castor3D::Culling GetCulledFaces()const
		{
			return m_culledFaces;
		}
		/**
		 *\~english
		 *\brief		Sets the culled faces.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit les faces cachées.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetCulledFaces( Castor3D::Culling p_value )
		{
			m_culledFaces = p_value;
		}
		/**
		 *\~english
		 *\return		The orientation of the faces considered front facing.
		 *\~french
		 *\return		L'orientation des faces considérées comme tournées vers l'avant.
		 */
		inline bool GetFrontCCW()const
		{
			return m_frontCCW;
		}
		/**
		 *\~english
		 *\brief		Sets the orientation of the faces considered front facing.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le'orientation des faces considérées comme tournées vers l'avant.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetFrontCCW( bool p_value )
		{
			m_frontCCW = p_value;
		}
		/**
		 *\~english
		 *\return		The antialiased lines status.
		 *\~french
		 *\return		Le statut d'antialising des lignes.
		 */
		inline bool GetAntialiasedLines()const
		{
			return m_antialiasedLines;
		}
		/**
		 *\~english
		 *\brief		Sets the antialiased lines status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'antialising des lignes.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAntialiasedLines( bool p_value )
		{
			m_antialiasedLines = p_value;
		}
		/**
		 *\~english
		 *\return		The depth bias factor.
		 *\~french
		 *\return		Le facteur de modificateur de profondeur.
		 */
		inline float GetDepthBiasFactor()const
		{
			return m_depthBiasFactor;
		}
		/**
		 *\~english
		 *\brief		Sets the depth bias factor.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le facteur de modificateur de profondeur.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDepthBiasFactor( float p_value )
		{
			m_depthBiasFactor = p_value;
		}
		/**
		 *\~english
		 *\return		The depth bias unit.
		 *\~french
		 *\return		L'unité du modificateur de profondeur.
		 */
		inline float GetDepthBiasUnits()const
		{
			return m_depthBiasUnits;
		}
		/**
		 *\~english
		 *\brief		Sets the depth bias unit.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit l'unité du modificateur de profondeur.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDepthBiasUnits( float p_value )
		{
			m_depthBiasUnits = p_value;
		}
		/**
		 *\~english
		 *\return		The depth clipping status.
		 *\~french
		 *\return		Le statut de depth clipping.
		 */
		inline bool GetDepthClipping()const
		{
			return m_depthClipping;
		}
		/**
		 *\~english
		 *\brief		Sets the depth clipping status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de depth clipping.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDepthClipping( bool p_value )
		{
			m_depthClipping = p_value;
		}
		/**
		 *\~english
		 *\return		The scissor test activation status.
		 *\~french
		 *\return		La statut d'activation du scissor test.
		 */
		inline bool GetScissor()const
		{
			return m_scissorTest;
		}
		/**
		 *\~english
		 *\brief		Sets the scissor test activation status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation du scissor test.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetScissor( bool p_value )
		{
			m_scissorTest = p_value;
		}
		/**
		 *\~english
		 *\return		The primitive discard status.
		 *\~french
		 *\return		La statut de défausse des primitives.
		 */
		inline bool GetDiscardPrimitives()const
		{
			return m_discardPrimitives;
		}
		/**
		 *\~english
		 *\brief		Sets the primitive discard status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de défausse des primitives.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDiscardPrimitives( bool p_value )
		{
			m_discardPrimitives = p_value;
		}

	protected:
		//!\~english	The fill mode.
		//!\~french		Définit le mode de remplissage.
		FillMode m_fillMode{ FillMode::eSolid };
		//!\~english	The culled faces.
		//!\~french		Les faces cachées.
		Culling m_culledFaces{ Culling::eBack };
		//!\~english	The faces considered front facing.
		//!\~french		Les faces considérées comme tournées vers l'avant.
		bool m_frontCCW{ true };
		//!\~english	The antialiased lines status.
		//!\~french		Le statut d'antialising des lignes.
		bool m_antialiasedLines{ false };
		//!\~english	The depth bias factor.
		//!\~french		Le facteur du modificateur de profondeur.
		float m_depthBiasFactor{ 0.0f };
		//!\~english	The depth bias unit.
		//!\~french		L'unité du modificateur de profondeur.
		float m_depthBiasUnits{ 0.0f };
		//!\~english	The scissor test activation status.
		//!\~french		Le statut d'activation du scissor test.
		bool m_scissorTest{ false };
		//!\~english	The depth clipping status.
		//!\~french		Le statut de depth clipping.
		bool m_depthClipping{ true };
		//!\~english	The primitive discard status.
		//!\~french		Le statut de défausse des primitives.
		bool m_discardPrimitives{ false };
	};
}

#endif
