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
#ifndef ___C3D_RasterState___
#define ___C3D_RasterState___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API RasteriserState
	{
	protected:
		bool m_bChanged;
		eFILL_MODE m_eFillMode;
		eFACE m_eCulledFaces;
		bool m_bFrontCCW;
		bool m_bAntialiasedLines;
		float m_fDepthBias;
		bool m_bScissor;
		bool m_bDepthClipping;
		bool m_bMultisampled;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		RasteriserState();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RasteriserState();
		/**
		 *\~english
		 *\brief		Initialises the states
		 *\~french
		 *\brief		Initialise les états
		 */
		 virtual bool Initialise()=0;
		/**
		 *\~english
		 *\brief		Cleans the states
		 *\~french
		 *\brief		Nettoie les états
		 */
		 virtual void Cleanup()=0;
		/**
		 *\~english
		 *\brief		Applies the states
		 *\~french
		 *\brief		Applique les états
		 */
		 virtual bool Apply()=0;
		/**
		 *\~english
		 *\brief		Retrieves the fill mode
		 *\return		The value
		 *\~french
		 *\brief		Récupère le mode de remplissage
		 *\return		La valeur
		 */
		inline Castor3D::eFILL_MODE GetFillMode()const { return m_eFillMode; }
		/**
		 *\~english
		 *\brief		Sets the fill mode
		 *\param[in]	p_eMode	The new value
		 *\~french
		 *\brief		Définit le mode de remplissage
		 *\param[in]	p_eMode	La nouvelle valeur
		 */
		inline void SetFillMode( Castor3D::eFILL_MODE p_eMode ) { m_bChanged |= m_eFillMode != p_eMode;m_eFillMode = p_eMode; }
		/**
		 *\~english
		 *\brief		Retrieves the culled faces
		 *\return		The value
		 *\~french
		 *\brief		Récupère les faces cachées
		 *\return		La valeur
		 */
		inline Castor3D::eFACE GetCulledFaces()const { return m_eCulledFaces; }
		/**
		 *\~english
		 *\brief		Sets the culled faces
		 *\param[in]	p_eFace	The new value
		 *\~french
		 *\brief		Définit les faces cachées
		 *\param[in]	p_eFace	La nouvelle valeur
		 */
		inline void SetCulledFaces( Castor3D::eFACE p_eFace ) { m_bChanged |= m_eCulledFaces != p_eFace;m_eCulledFaces = p_eFace; }
		/**
		 *\~english
		 *\brief		Retrieves the faces considered front facing
		 *\return		The value
		 *\~french
		 *\brief		Récupère les faces considérées comme tournées vers l'avant
		 *\return		La valeur
		 */
		inline bool GetFrontCCW()const { return m_bFrontCCW; }
		/**
		 *\~english
		 *\brief		Sets the faces considered front facing
		 *\param[in]	p_bCCW	The new value
		 *\~french
		 *\brief		Définit les faces considérées comme tournées vers l'avant
		 *\param[in]	p_bCCW	La nouvelle valeur
		 */
		inline void SetFrontCCW( bool p_bCCW ) { m_bChanged |= m_bFrontCCW != p_bCCW;m_bFrontCCW = p_bCCW; }
		/**
		 *\~english
		 *\brief		Retrieves the antialiased lines status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'antialising des lignes
		 *\return		La valeur
		 */
		inline bool GetAntialiasedLines()const { return m_bAntialiasedLines; }
		/**
		 *\~english
		 *\brief		Sets the antialiased lines status
		 *\param[in]	p_bAA	The new value
		 *\~french
		 *\brief		Définit le statut d'antialising des lignes
		 *\param[in]	p_bAA	La nouvelle valeur
		 */
		inline void SetAntialiasedLines( bool p_bAA ) { m_bChanged |= m_bAntialiasedLines != p_bAA;m_bAntialiasedLines = p_bAA; }
		/**
		 *\~english
		 *\brief		Retrieves the depth bias
		 *\return		The value
		 *\~french
		 *\brief		Récupère le depth bias
		 *\return		La valeur
		 */
		inline float GetDepthBias()const { return m_fDepthBias; }
		/**
		 *\~english
		 *\brief		Sets the depth bias
		 *\param[in]	p_fBias	The new value
		 *\~french
		 *\brief		Définit le depth bias
		 *\param[in]	p_fBias	La nouvelle valeur
		 */
		inline void SetDepthBias( float p_fBias ) { m_bChanged |= m_fDepthBias != p_fBias;m_fDepthBias = p_fBias; }
		/**
		 *\~english
		 *\brief		Retrieves the depth clipping status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de depth clipping
		 *\return		La valeur
		 */
		inline bool GetDepthClipping()const { return m_bDepthClipping; }
		/**
		 *\~english
		 *\brief		Sets the depth clipping status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut de depth clipping
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void SetDepthClipping( bool p_bEnable ) { m_bChanged |= m_bDepthClipping != p_bEnable;m_bDepthClipping = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the multisample activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'acitvation du multisample
		 *\return		La valeur
		 */
		inline bool GetMultisample()const { return m_bMultisampled; }
		/**
		 *\~english
		 *\brief		Sets the multisample activation status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut d'acitvation du multisample
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void SetMultisample( bool p_bEnable ) { m_bChanged |= m_bMultisampled != p_bEnable;m_bMultisampled = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the scissor test activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'acitvation du scissor test
		 *\return		La valeur
		 */
		inline bool GetScissor()const { return m_bScissor; }
		/**
		 *\~english
		 *\brief		Sets the scissor test activation status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut d'acitvation du scissor test
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void SetScissor( bool p_bEnable ) { m_bChanged |= m_bScissor != p_bEnable;m_bScissor = p_bEnable; }
	};
}

#pragma warning( pop )

#endif
