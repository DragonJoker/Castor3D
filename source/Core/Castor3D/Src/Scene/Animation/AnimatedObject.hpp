/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_OBJECT_H___
#define ___C3D_ANIMATED_OBJECT_H___

#include "AnimationInstance.hpp"

#include <Design/Named.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		10/12/2013
	\~english
	\brief		Represents the animated objects
	\~french
	\brief		Représente les objets animés
	*/
	class AnimatedObject
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet
		 */
		C3D_API explicit AnimatedObject( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObject();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimatedObject( AnimatedObject && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimatedObject & operator=( AnimatedObject && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimatedObject( AnimatedObject const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimatedObject & operator=( AnimatedObject const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		adds the animation to the list
		 *\param[in]	p_name	The animation to add
		 *\~english
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	p_name	L'animation à ajouter
		 */
		C3D_API void addAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void startAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void stopAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void pauseAnimation( castor::String const & p_name );
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
		 *\brief		Checks if an animation with given name exists.
		 *\param[in]	p_name	The animation name.
		 *\~french
		 *\brief		Vérifie si l'animation avec le nom donné existe.
		 *\param[in]	p_name	Le nom de l'animation.
		 */
		C3D_API bool hasAnimation( castor::String const & p_name )
		{
			return m_animations.find( p_name ) != m_animations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	p_name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	p_name	Le nom de l'animation
		 *\return		L'animation
		 */
		C3D_API AnimationInstance & getAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Updates the animations of the object, given the time since the last frame
		 *\param[in]	p_tslf		Time elapsed since the last frame
		 *\~french
		 *\brief		Met à jour les animations de l'objet, selon le temps écoulé depuis la dernière frame
		 *\param[in]	p_tslf		Le temps écoulé depuis la dernière frame
		 */
		C3D_API virtual void update( castor::Milliseconds const & p_tslf ) = 0;
		/**
		 *\~english
		 *\return		\p true if the object is playing an animation.
		 *\~french
		 *\return		\p true si l'objet joue une animation.
		 */
		C3D_API virtual bool isPlayingAnimation()const = 0;
		/**
		 *\~english
		 *\return		The animations for this object.
		 *\~french
		 *\return		Les animations de cet objet.
		 */
		inline AnimationInstancePtrStrMap const & getAnimations()const
		{
			return m_animations;
		}

	private:
		/**
		 *\~english
		 *\brief		adds the animation to the list
		 *\param[in]	p_name	The animation to add
		 *\~english
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	p_name	L'animation à ajouter
		 */
		virtual void doAddAnimation( castor::String const & p_name ) = 0;
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		virtual void doStartAnimation( AnimationInstance & p_animation ) = 0;
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		virtual void doStopAnimation( AnimationInstance & p_animation ) = 0;
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
	};
}

#endif

