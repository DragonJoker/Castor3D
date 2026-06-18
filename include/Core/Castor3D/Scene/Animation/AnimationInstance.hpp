/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationInstance_H___
#define ___C3D_AnimationInstance_H___

#include "AnimationModule.hpp"

namespace c3d
{
	class AnimationInstance
		: public OwnedBy< AnimatedObject >
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimationInstance & operator=( AnimationInstance && rhs )noexcept = delete;
		C3D_API AnimationInstance( AnimationInstance const & rhs ) = delete;
		C3D_API AnimationInstance & operator=( AnimationInstance const & rhs ) = delete;
		C3D_API AnimationInstance( AnimationInstance && rhs )noexcept = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	object		The parent AnimatedObject.
		 *\param[in]	animation	The animation.
		 *\param[in]	looped		Is the animation looped or not.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	object		L'AnimatedObject parent.
		 *\param[in]	animation	L'animation.
		 *\param[in]	looped		L'animation tourne-t-elle en boucle ou pas.
		 */
		C3D_API AnimationInstance( AnimatedObject & object
			, Animation & animation
			, InterpolatorType defaultInterpolation
			, bool looped = false );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AnimationInstance()noexcept = default;
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\param[in]	elapsed	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	elapsed	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API void update( Milliseconds const & elapsed );
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
		 *\brief		Sets the animation effective length.
		 *\~french
		 *\brief		Définit le temps effectif de l'animation.
		 */
		C3D_API void setTotalLength( Milliseconds const & time );
		/**
		 *name Getters.
		 **/
		/**@{*/
		AnimationState getState()const noexcept
		{
			return m_state;
		}

		float getScale()const noexcept
		{
			return m_scale;
		}

		Milliseconds const & getStartingPoint()const noexcept
		{
			return m_startingPoint;
		}

		Milliseconds const & getStoppingPoint()const noexcept
		{
			return m_stoppingPoint;
		}

		bool isLooped()const noexcept
		{
			return m_looped;
		}

		InterpolatorType getInterpolation()const noexcept
		{
			return m_interpolation;
		}

		Animation & getAnimation()const noexcept
		{
			return *m_animation;
		}
		/**@}*/
		/**
		 *name Setters.
		 **/
		/**@{*/
		void setScale( float value )noexcept
		{
			m_scale = value;
		}

		void setStartingPoint( Milliseconds value )noexcept
		{
			m_startingPoint = c3d::move( value );

			if ( m_currentTime < m_startingPoint )
			{
				m_currentTime = m_startingPoint;
			}
		}

		void setStoppingPoint( Milliseconds value )noexcept
		{
			m_stoppingPoint = c3d::move( value );

			if ( m_currentTime > m_stoppingPoint )
			{
				m_currentTime = m_stoppingPoint;
			}
		}

		void setLooped( bool value )noexcept
		{
			m_looped = value;
		}

		void setInterpolation( InterpolatorType value )noexcept
		{
			m_interpolation = value;
		}

	private:
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 */
		virtual void doUpdate() = 0;
		/**
		 *\~english
		 *\brief		Stops the animation.
		 *\~french
		 *\brief		Stoppe l'animation.
		 */
		virtual void doStop()
		{
		}

	protected:
		//!\~english	The animation.
		//!\~french		L'animation.
		Animation * m_animation;
		//!\~english	The animation time scale.
		//!\~french		Le multiplicateur de temps.
		float m_scale{ 1.0f };
		//!\~english	Tells whether or not the animation is looped.
		//!\~french		Dit si oui ou non l'animation est bouclée.
		std::atomic_bool m_looped;
		//!\~english	The starting point.
		//!\~french		Le point de départ.
		Milliseconds m_startingPoint{ 0 };
		//!\~english	The stopping point.
		//!\~french		Le point d'arrêt.
		Milliseconds m_stoppingPoint{ 0 };
		//!\~english	The current playing time.
		//!\~french		L'index de temps courant.
		Milliseconds m_currentTime{ 0 };
		//!\~english	The animation total time.
		//!\~french		La durée totale de l'animation.
		Milliseconds m_totalTime{ 0 };
		//!\~english	The current state of the animation.
		//!\~french		L'état actuel de l'animation.
		AnimationState m_state{ AnimationState::eStopped };
		//!\~english	The interpolation for the animation.
		//!\~french		L'interplation de l'animation.
		InterpolatorType m_interpolation{ InterpolatorType::eLinear };
	};
}

#endif
