/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_RASTER_STATE_H___
#define ___C3D_RASTER_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

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
		C3D_API RasteriserState( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RasteriserState();
		/**
		 *\~english
		 *\brief		Initialises the states
		 *\~french
		 *\brief		Initialise les états
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the states
		 *\~french
		 *\brief		Nettoie les états
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the states
		 *\~french
		 *\brief		Applique les états
		 */
		C3D_API virtual bool Apply() = 0;
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
			m_changed |= m_eFillMode != p_mode;
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
			m_changed |= m_eCulledFaces != p_eFace;
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
			m_changed |= m_bFrontCCW != p_bCCW;
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
			m_changed |= m_bAntialiasedLines != p_bAA;
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
			m_changed |= m_fDepthBias != p_fBias;
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
			m_changed |= m_bDepthClipping != p_enable;
			m_bDepthClipping = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the multisample activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'acitvation du multisample
		 *\return		La valeur
		 */
		inline bool GetMultisample()const
		{
			return m_bMultisampled;
		}
		/**
		 *\~english
		 *\brief		Sets the multisample activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'acitvation du multisample
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void SetMultisample( bool p_enable )
		{
			m_changed |= m_bMultisampled != p_enable;
			m_bMultisampled = p_enable;
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
			m_changed |= m_bScissor != p_enable;
			m_bScissor = p_enable;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates the state used to save the current state
		 *\~french
		 *\brief		Crée l'état utilisé pour stocker l'état courant
		 */
		C3D_API void CreateCurrent();
		/**
		 *\~english
		 *\brief		Destroys the state used to save the current state
		 *\~french
		 *\brief		Détruit l'état utilisé pour stocker l'état courant
		 */
		C3D_API void DestroyCurrent();
		/**
		 *\~english
		 *\brief		Creates the state used to save the current state
		 *\~french
		 *\brief		Crée l'état utilisé pour stocker l'état courant
		 */
		C3D_API virtual RasteriserStateSPtr DoCreateCurrent() = 0;

	protected:
		//!\~english Tells it has changed.	\~french Dit que l'état a changé.
		bool m_changed;
		//!\~english The fill mode.	\~french Définit le mode de remplissage.
		eFILL_MODE m_eFillMode;
		//!\~english The culled faces.	\~french Les faces cachées.
		eFACE m_eCulledFaces;
		//!\~english The faces considered front facing.	\~french Les faces considérées comme tournées vers l'avant.
		bool m_bFrontCCW;
		//!\~english The antialiased lines status.	\~french Le statut d'antialising des lignes.
		bool m_bAntialiasedLines;
		//!\~english The depth bias.	\~french Le depth bias.
		float m_fDepthBias;
		//!\~english The scissor test activation status.	\~french Le statut d'acitvation du scissor test.
		bool m_bScissor;
		//!\~english The depth clipping status.	\~french Le statut de depth clipping.
		bool m_bDepthClipping;
		//!\~english The multisample activation status.	\~french Le statut d'acitvation du multisample.
		bool m_bMultisampled;
		//!\~english	The internal global state used to commit only the changed states.	\~french	Etat interne global, utilisé pour n'appliquer que les changements d'état.
		static RasteriserStateWPtr m_wCurrentState;
		//!\~english	Shared_pointer to the internal global state, to use reference counting for this static member.	\~french	Pointeur partag2 sur l'état interne global, utilisé pour avoir le comptage de références pour ce membre statique.
		RasteriserStateSPtr m_currentState;
	};
}

#endif
