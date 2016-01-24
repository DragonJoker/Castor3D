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
#ifndef ___C3D_ANIMATION_H___
#define ___C3D_ANIMATION_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\todo		Review all the animation system because it's not clear, not optimised, and not good enough to be validated
	\todo		Write and Read functions.
	\~english
	\brief		Animation class
	\remark		The class which handles an Animation, its length, key frames ...
	\~french
	\brief		Classe d'animation
	\remark		Classe gérant une Animation, sa durée, les key frames ...
	*/
	class Animation
		: public Castor::Named
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		09/02/2010
		\~english
		\brief		Animation State Enum
		\remark		The enumeration which defines all the states of an animation : playing, stopped, paused
		\~french
		\brief		Enumération des états d'une animation
		*/
		typedef enum eSTATE CASTOR_TYPE( uint8_t )
		{
			eSTATE_PLAYING,	//!< Playing animation state
			eSTATE_STOPPED,	//!< Stopped animation state
			eSTATE_PAUSED,	//!< Paused animation state
			eSTATE_COUNT
		}	eSTATE;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API Animation( Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Animation();
		/**
		 *\~english
		 *\brief		Creates and adds a moving node
		 *\~french
		 *\brief		Crée et ajoute un noeud mouvant
		 */
		C3D_API MovingObjectBaseSPtr AddMovingObject();
		/**
		 *\~english
		 *\brief		Creates and adds a moving bone
		 *\param[in]	p_bone		The bone to add
		 *\~french
		 *\brief		Crée et ajoute un os mouvant
		 *\param[in]	p_bone		L'os
		 */
		C3D_API MovingObjectBaseSPtr AddMovingObject( BoneSPtr p_bone );
		/**
		 *\~english
		 *\brief		Creates and adds a moving object
		 *\param[in]	p_object		The moving object to add
		 *\~french
		 *\brief		Crée et ajoute un objet mouvant
		 *\param[in]	p_object		L'objet déplaçable
		 */
		C3D_API MovingObjectBaseSPtr AddMovingObject( MovableObjectSPtr p_object );
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index
		 *\param[in]	p_tslf	The time since the last frame
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps
		 *\param[in]	p_tslf	Le temps écoulé depuis la dernière frame
		 */
		C3D_API void Update( real p_tslf );
		/**
		 *\~english
		 *\brief		Plays the animation
		 *\~french
		 *\brief		Démarre l'animation
		 */
		C3D_API void Play();
		/**
		 *\~english
		 *\brief		Pauses the animation
		 *\~french
		 *\brief		Met l'animation en pause
		 */
		C3D_API void Pause();
		/**
		 *\~english
		 *\brief		Stops the animation
		 *\~french
		 *\brief		Stoppe l'animation
		 */
		C3D_API void Stop();
		/**
		 *\~english
		 *\brief		Retrieves the animation state
		 *\return		The animation state
		 *\~french
		 *\brief		Récupère l'état de l'animation
		 *\return		L'état de l'animation
		 */
		inline eSTATE GetState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation time scale
		 *\return		The animation time scale
		 *\~french
		 *\brief		Récupère le multiplicateur de temps de l'animation
		 *\return		Le multiplicateur de temps de l'animation
		 */
		inline real GetScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation loop status
		 *\return		The animation loop status
		 *\~french
		 *\brief		Récupère l'état de boucle de l'animation
		 *\return		L'état de boucle de l'animation
		 */
		inline bool IsLooped()const
		{
			return m_looped;
		}
		/**
		 *\~english
		 *\brief		Sets the animation time scale
		 *\param[in]	p_scale	The new value
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation
		 *\param[in]	p_scale	La nouvelle valeur
		 */
		inline void	SetScale( real p_scale )
		{
			m_scale = p_scale;
		}
		/**
		 *\~english
		 *\brief		Sets the animation loop status
		 *\param[in]	p_looped	The new value
		 *\~french
		 *\brief		Définit l'état de boucle de l'animation
		 *\param[in]	p_looped	La nouvelle valeur
		 */
		inline void SetLooped( bool p_looped )
		{
			m_looped = p_looped;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving objects count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Le nombre
		 */
		inline uint32_t GetMovingObjectsCount()const
		{
			return uint32_t( m_toMove.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an interator on the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapIt begin()
		{
			return m_toMove.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant interator on the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapConstIt begin()const
		{
			return m_toMove.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an interator on the end of the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapIt end()
		{
			return m_toMove.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant interator on the end of the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin de la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapConstIt end()const
		{
			return m_toMove.end();
		}

	protected:
		//!\~english The current playing time	\~french L'index de temps courant
		real m_currentTime;
		//!\~english The current state of the animation	\~french L'état actuel de l'animation
		eSTATE m_state;
		//!\~english The animation time scale	\~french Le multiplicateur de temps
		real m_scale;
		//!\~english The animation length	\~french La durée de l'animation
		real m_length;
		//!\~english Tells whether or not the animation is looped	\~french Dit si oui ou non l'animation est bouclée
		bool m_looped;
		//! The moving objects
		MovingObjectPtrStrMap m_toMove;
	};
}

#endif
