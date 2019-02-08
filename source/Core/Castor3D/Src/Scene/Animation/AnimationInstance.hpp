/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATION_INSTANCE_H___
#define ___C3D_ANIMATION_INSTANCE_H___

#include "Animation/Animation.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/06/2016
	\~english
	\brief		Animation instance class.
	\remark		Used to play an Animation on a specific object.
	\~french
	\brief		Classe d'instance d'animation
	\remark		Utilisée pour jouer une animation sur un objet particulier.
	*/
	class AnimationInstance
		: public castor::OwnedBy< AnimatedObject >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	object		The parent AnimatedObject.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	object		L'AnimatedObject parent.
		 *\param[in]	animation	L'animation.
		 */
		C3D_API AnimationInstance( AnimatedObject & object, Animation & animation );
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
		 *\param[in]	elapsed	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	elapsed	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API void update( castor::Milliseconds const & elapsed );
		/**
		 *\~english
		 *\brief		Plays the animation.
		 *\~french
		 *\brief		Démarre l'animation.
		 */
		C3D_API void play();
		/**
		 *\~english
		 *\brief		Pauses the animation.
		 *\~french
		 *\brief		Met l'animation en pause.
		 */
		C3D_API void pause();
		/**
		 *\~english
		 *\brief		Stops the animation.
		 *\~french
		 *\brief		Stoppe l'animation.
		 */
		C3D_API void stop();
		/**
		 *\~english
		 *\return		The animation state.
		 *\~french
		 *\return		L'état de l'animation.
		 */
		inline AnimationState getState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\return		The animation time scale.
		 *\~french
		 *\return		Le multiplicateur de temps de l'animation.
		 */
		inline real getScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Sets the animation time scale
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setScale( real value )
		{
			m_scale = value;
		}
		/**
		 *\~english
		 *\return		The animation loop status.
		 *\~french
		 *\return		L'état de boucle de l'animation.
		 */
		inline bool isLooped()const
		{
			return m_looped;
		}
		/**
		 *\~english
		 *\brief		Sets the animation loop status
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit l'état de boucle de l'animation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setLooped( bool value )
		{
			m_looped = value;
		}
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		inline Animation const & getAnimation()const
		{
			return m_animation;
		}

	private:
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 */
		virtual void doUpdate() = 0;

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
		castor::Milliseconds m_currentTime{ 0 };
		//!\~english	The current state of the animation.
		//!\~french		L'état actuel de l'animation.
		AnimationState m_state{ AnimationState::eStopped };
	};
}

#endif
