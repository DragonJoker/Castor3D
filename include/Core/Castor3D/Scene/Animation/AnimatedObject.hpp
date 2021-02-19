/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_OBJECT_H___
#define ___C3D_ANIMATED_OBJECT_H___

#include "AnimationInstance.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	class AnimatedObject
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimatedObject( AnimatedObject && rhs ) = default;
		C3D_API AnimatedObject & operator=( AnimatedObject && rhs ) = default;
		C3D_API AnimatedObject( AnimatedObject const & rhs ) = delete;
		C3D_API AnimatedObject & operator=( AnimatedObject const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	kind	The animation type.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	kind	Le type d'animation.
		 *\param[in]	name	Le nom de l'objet.
		 */
		C3D_API explicit AnimatedObject( AnimationType kind
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObject();
		/**
		 *\~english
		 *\brief		adds the animation to the list
		 *\param[in]	name	The animation to add
		 *\~french
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	name	L'animation à ajouter
		 */
		C3D_API void addAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void startAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void stopAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void pauseAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Starts all animations
		 *\~french
		 *\brief		Démarre toutes les animations
		 */
		C3D_API void startAllAnimations();
		/**
		 *\~english
		 *\brief		Stops all animations
		 *\~french
		 *\brief		Stoppe toutes les animations
		 */
		C3D_API void stopAllAnimations();
		/**
		 *\~english
		 *\brief		Pauses all animations
		 *\~french
		 *\brief		Met en pause toutes les animations
		 */
		C3D_API void pauseAllAnimations();
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		L'animation
		 */
		C3D_API AnimationInstance & getAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Updates the animations of the object, given the time since the last frame
		 *\param[in]	elapsed		Time elapsed since the last frame
		 *\~french
		 *\brief		Met à jour les animations de l'objet, selon le temps écoulé depuis la dernière frame
		 *\param[in]	elapsed		Le temps écoulé depuis la dernière frame
		 */
		C3D_API virtual void update( castor::Milliseconds const & elapsed ) = 0;
		/**
		 *\~english
		 *\return		\p true if the object is playing an animation.
		 *\~french
		 *\return		\p true si l'objet joue une animation.
		 */
		C3D_API virtual bool isPlayingAnimation()const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		bool hasAnimation( castor::String const & name )
		{
			return m_animations.find( name ) != m_animations.end();
		}

		AnimationInstancePtrStrMap const & getAnimations()const
		{
			return m_animations;
		}

		AnimationType getKind()const
		{
			return m_kind;
		}
		/**@}*/

	private:
		/**
		 *\~english
		 *\brief		adds the animation to the list
		 *\param[in]	name	The animation to add
		 *\~french
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	name	L'animation à ajouter
		 */
		virtual void doAddAnimation( castor::String const & name ) = 0;
		/**
		 *\~english
		 *\brief		Starts the given animation.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Démarre l'animation donnée.
		 *\param[in]	animation	L'animation.
		 */
		virtual void doStartAnimation( AnimationInstance & animation ) = 0;
		/**
		 *\~english
		 *\brief		Stops the given animation.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Stoppe l'animation donnée.
		 *\param[in]	animation	L'animation.
		 */
		virtual void doStopAnimation( AnimationInstance & animation ) = 0;
		/**
		 *\~english
		 *\brief		Stops all animations
		 *\~french
		 *\brief		Stoppe toutes les animations
		 */
		virtual void doClearAnimations() = 0;

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationInstancePtrStrMap m_animations;

	private:
		AnimationType m_kind;
	};
}

#endif

