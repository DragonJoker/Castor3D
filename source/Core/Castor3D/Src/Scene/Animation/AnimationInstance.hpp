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
#ifndef ___C3D_ANIMATION_INSTANCE_H___
#define ___C3D_ANIMATION_INSTANCE_H___

#include "Animation/Animation.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/06/2016
	\todo		Write and Read functions.
	\~english
	\brief		Animation instance class.
	\remark		Used to play an Animation on a specific object.
	\~french
	\brief		Classe d'instance d'animation
	\remark		Utilisée pour jouer une animation sur un objet particulier.
	*/
	class AnimationInstance
		: public Castor::OwnedBy< AnimatedObject >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_object	The parent AnimatedObject.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_object	L'AnimatedObject parent.
		 *\param[in]	p_animation	L'animation.
		 */
		C3D_API AnimationInstance( AnimatedObject & p_object, Animation const & p_animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~AnimationInstance();
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\param[in]	p_tslf	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	p_tslf	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API void Update( real p_tslf );
		/**
		 *\~english
		 *\brief		Plays the animation.
		 *\~french
		 *\brief		Démarre l'animation.
		 */
		C3D_API void Play();
		/**
		 *\~english
		 *\brief		Pauses the animation.
		 *\~french
		 *\brief		Met l'animation en pause.
		 */
		C3D_API void Pause();
		/**
		 *\~english
		 *\brief		Stops the animation.
		 *\~french
		 *\brief		Stoppe l'animation.
		 */
		C3D_API void Stop();
		/**
		 *\~english
		 *\return		The key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames.
		 */
		C3D_API virtual void SetInterpolationMode( InterpolatorType p_mode ) = 0;
		/**
		 *\~english
		 *\return		The animation state.
		 *\~french
		 *\return		L'état de l'animation.
		 */
		inline AnimationState GetState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\return		The animation time scale.
		 *\~french
		 *\return		Le multiplicateur de temps de l'animation.
		 */
		inline real GetScale()const
		{
			return m_scale;
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
		 *\return		The animation loop status.
		 *\~french
		 *\return		L'état de boucle de l'animation.
		 */
		inline bool IsLooped()const
		{
			return m_looped;
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
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		inline Animation const & GetAnimation()const
		{
			return m_animation;
		}

	private:
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\param[in]	p_tslf	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	p_tslf	Le temps écoulé depuis la dernière frame.
		 */
		virtual void DoUpdate( real p_tslf ) = 0;

	protected:
		//!\~english	The animation.
		//!\~french		L'animation.
		Animation const & m_animation;
		//!\~english	The animation time scale.
		//!\~french		Le multiplicateur de temps.
		real m_scale{ 1.0_r };
		//!\~english	Tells whether or not the animation is looped.
		//!\~french		Dit si oui ou non l'animation est bouclée.
		bool m_looped{ false };
		//!\~english	The current playing time.
		//!\~french		L'index de temps courant.
		real m_currentTime{ 0.0_r };
		//!\~english	The current state of the animation.
		//!\~french		L'état actuel de l'animation.
		AnimationState m_state{ AnimationState::Stopped };
	};
}

#endif
