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

namespace castor3d
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
		inline castor3d::FillMode getFillMode()const
		{
			return m_fillMode;
		}
		/**
		 *\~english
		 *\brief		sets the fill mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode de remplissage.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setFillMode( castor3d::FillMode p_value )
		{
			m_fillMode = p_value;
		}
		/**
		 *\~english
		 *\return		The culled faces
		 *\~french
		 *\return		Les faces cachées
		 */
		inline castor3d::Culling getCulledFaces()const
		{
			return m_culledFaces;
		}
		/**
		 *\~english
		 *\brief		sets the culled faces.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit les faces cachées.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setCulledFaces( castor3d::Culling p_value )
		{
			m_culledFaces = p_value;
		}
		/**
		 *\~english
		 *\return		The orientation of the faces considered front facing.
		 *\~french
		 *\return		L'orientation des faces considérées comme tournées vers l'avant.
		 */
		inline bool getFrontCCW()const
		{
			return m_frontCCW;
		}
		/**
		 *\~english
		 *\brief		sets the orientation of the faces considered front facing.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le'orientation des faces considérées comme tournées vers l'avant.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setFrontCCW( bool p_value )
		{
			m_frontCCW = p_value;
		}
		/**
		 *\~english
		 *\return		The antialiased lines status.
		 *\~french
		 *\return		Le statut d'antialising des lignes.
		 */
		inline bool getAntialiasedLines()const
		{
			return m_antialiasedLines;
		}
		/**
		 *\~english
		 *\brief		sets the antialiased lines status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'antialising des lignes.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setAntialiasedLines( bool p_value )
		{
			m_antialiasedLines = p_value;
		}
		/**
		 *\~english
		 *\return		The depth bias factor.
		 *\~french
		 *\return		Le facteur de modificateur de profondeur.
		 */
		inline float getDepthBiasFactor()const
		{
			return m_depthBiasFactor;
		}
		/**
		 *\~english
		 *\brief		sets the depth bias factor.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le facteur de modificateur de profondeur.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setDepthBiasFactor( float p_value )
		{
			m_depthBiasFactor = p_value;
		}
		/**
		 *\~english
		 *\return		The depth bias unit.
		 *\~french
		 *\return		L'unité du modificateur de profondeur.
		 */
		inline float getDepthBiasUnits()const
		{
			return m_depthBiasUnits;
		}
		/**
		 *\~english
		 *\brief		sets the depth bias unit.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit l'unité du modificateur de profondeur.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setDepthBiasUnits( float p_value )
		{
			m_depthBiasUnits = p_value;
		}
		/**
		 *\~english
		 *\return		The depth clipping status.
		 *\~french
		 *\return		Le statut de depth clipping.
		 */
		inline bool getDepthClipping()const
		{
			return m_depthClipping;
		}
		/**
		 *\~english
		 *\brief		sets the depth clipping status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de depth clipping.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setDepthClipping( bool p_value )
		{
			m_depthClipping = p_value;
		}
		/**
		 *\~english
		 *\return		The scissor test activation status.
		 *\~french
		 *\return		La statut d'activation du scissor test.
		 */
		inline bool getScissor()const
		{
			return m_scissorTest;
		}
		/**
		 *\~english
		 *\brief		sets the scissor test activation status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation du scissor test.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setScissor( bool p_value )
		{
			m_scissorTest = p_value;
		}
		/**
		 *\~english
		 *\return		The primitive discard status.
		 *\~french
		 *\return		La statut de défausse des primitives.
		 */
		inline bool getDiscardPrimitives()const
		{
			return m_discardPrimitives;
		}
		/**
		 *\~english
		 *\brief		sets the primitive discard status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de défausse des primitives.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setDiscardPrimitives( bool p_value )
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
