/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMABLE_H___
#define ___C3D_ANIMABLE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.7.0
	\~english
	\brief		Animable public interface
	\~french
	\brief		interface publique d'animable
	*/
	class Animable
		: public castor::OwnedBy< Scene >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_scene	La scène.
		 */
		explicit Animable( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Animable();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Animable( Animable && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Animable & operator=( Animable && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Animable( Animable const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Animable & operator=( Animable const & p_rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		 */
		C3D_API void cleanupAnimations();
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
		C3D_API bool hasAnimation( castor::String const & p_name )const;
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
		C3D_API Animation const & getAnimation( castor::String const & p_name )const;
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
		C3D_API Animation & getAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\return		The animations.
		 *\~french
		 *\return		Les animations.
		 */
		inline AnimationPtrStrMap const & getAnimations()const
		{
			return m_animations;
		}

	protected:
		/**
		 *\~english
		 *\brief		adds an animation.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	p_animation	L'animation.
		 */
		void doAddAnimation( AnimationSPtr && p_animation );
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
		template< typename Type >
		Type & doGetAnimation( castor::String const & p_name )
		{
			return static_cast< Type & >( getAnimation( p_name ) );
		}

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationPtrStrMap m_animations;
	};
}

#endif
