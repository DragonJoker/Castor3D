/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationInstance_H___
#define ___C3D_AnimationInstance_H___

#include "AnimationModule.hpp"

namespace castor3d
{
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
		C3D_API virtual ~AnimationInstance();
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
		AnimationState getState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\return		The animation time scale.
		 *\~french
		 *\return		Le multiplicateur de temps de l'animation.
		 */
		float getScale()const
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
		void setScale( float value )
		{
			m_scale = value;
		}
		/**
		 *\~english
		 *\return		The animation stopping point.
		 *\~french
		 *\return		Le point de départ de l'animation.
		 */
		castor::Milliseconds getStartingPoint()const
		{
			return m_startingPoint;
		}
		/**
		 *\~english
		 *\brief		Sets the animation stopping point.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le point de départ de l'animation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setStartingPoint( castor::Milliseconds value )
		{
			m_startingPoint = value;

			if ( m_currentTime < m_startingPoint )
			{
				m_currentTime = m_startingPoint;
			}
		}
		/**
		 *\~english
		 *\return		The animation stopping point.
		 *\~french
		 *\return		Le point d'arrêt de l'animation.
		 */
		castor::Milliseconds getStoppingPoint()const
		{
			return m_stoppingPoint;
		}
		/**
		 *\~english
		 *\brief		Sets the animation stopping point.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le point d'arrêt de l'animation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setStoppingPoint( castor::Milliseconds value )
		{
			m_stoppingPoint = value;

			if ( m_currentTime > m_stoppingPoint )
			{
				m_currentTime = m_stoppingPoint;
			}
		}
		/**
		 *\~english
		 *\return		The animation loop status.
		 *\~french
		 *\return		L'état de boucle de l'animation.
		 */
		bool isLooped()const
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
		void setLooped( bool value )
		{
			m_looped = value;
		}
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		Animation const & getAnimation()const
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
		float m_scale{ 1.0f };
		//!\~english	Tells whether or not the animation is looped.
		//!\~french		Dit si oui ou non l'animation est bouclée.
		bool m_looped{ false };
		//!\~english	The starting point.
		//!\~french		Le point de départ.
		castor::Milliseconds m_startingPoint{ 0 };
		//!\~english	The stopping point.
		//!\~french		Le point d'arrêt.
		castor::Milliseconds m_stoppingPoint{ 0 };
		//!\~english	The current playing time.
		//!\~french		L'index de temps courant.
		castor::Milliseconds m_currentTime{ 0 };
		//!\~english	The current state of the animation.
		//!\~french		L'état actuel de l'animation.
		AnimationState m_state{ AnimationState::eStopped };
	};
}

#endif
