/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animable_H___
#define ___C3D_Animable_H___

#include "Castor3D/Animation/AnimationModule.hpp"

namespace castor3d
{
	class Animable
		: public castor::OwnedBy< Engine >
	{
	protected:
		using AnimationPtr = castor::UniquePtr< Animation >;
		using AnimationsMap = castor::StringMap< AnimationPtr >;
		/**
		 *\~english
		 *\name Construction / Destruction.
		 *\~french
		 *\name Construction / Destruction.
		 **/
		/**@{*/
		C3D_API explicit Animable( Engine & owner );
		C3D_API Animable( Animable && rhs )noexcept = default;
		C3D_API Animable & operator=( Animable && rhs )noexcept = delete;
		C3D_API Animable( Animable const & rhs ) = delete;
		C3D_API Animable & operator=( Animable const & rhs ) = delete;
		/**@}*/

	public:
		C3D_API virtual ~Animable()noexcept = default;
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		 */
		C3D_API void cleanupAnimations();
		/**
		 *\~english
		 *\return		\p true if the object has an animation.
		 *\~french
		 *\return		\p true si l'objet a une animation.
		 */
		C3D_API virtual bool hasAnimation()const noexcept;
		/**
		 *\~english
		 *\param[in]	name	The animation name
		 *\return		\p true it the object has an animation with given name.
		 *\~french
		 *\param[in]	name	Le nom de l'animation
		 *\return		\p true si l'objet a une animation ayant le nom donné.
		 */
		C3D_API bool hasAnimation( castor::String const & name )const;
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
		C3D_API Animation const & getAnimation( castor::String const & name )const;
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
		C3D_API Animation & getAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Adds an animation.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	animation	L'animation.
		 */
		C3D_API void addAnimation( AnimationPtr animation );
		/**
		 *\~english
		 *\return		The animations.
		 *\~french
		 *\return		Les animations.
		 */
		AnimationsMap const & getAnimations()const
		{
			return m_animations;
		}

	protected:
		/**
		 *\~english
		 *\brief		Removes an animation.
		 *\param[in]	name	The animation name
		 *\~french
		 *\brief		Enlève une animation.
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void doRemoveAnimation( castor::String const & name );
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
		template< typename AnimationType >
		AnimationType & doGetAnimation( castor::String const & name )
		{
			return static_cast< AnimationType & >( getAnimation( name ) );
		}
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
		template< typename AnimationType >
		AnimationType const & doGetAnimation( castor::String const & name )const
		{
			return static_cast< AnimationType const & >( getAnimation( name ) );
		}

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationsMap m_animations;
	};
}

#endif
