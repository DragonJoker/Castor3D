/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animable_H___
#define ___C3D_Animable_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	template< typename OwnerT >
	class Animable
		: public castor::OwnedBy< OwnerT >
	{
	protected:
		using AnimationPtr = std::unique_ptr< Animation< OwnerT > >;
		using AnimationPtrStrMap = std::map< castor::String, AnimationPtr >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	owner	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	owner	Le parent.
		 */
		inline explicit Animable( OwnerT & owner );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Animable() = default;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Animable( Animable && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Animable & operator=( Animable && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Animable( Animable const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Animable & operator=( Animable const & rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		 */
		inline void cleanupAnimations();
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
		inline bool hasAnimation( castor::String const & name )const;
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
		inline Animation< OwnerT > const & getAnimation( castor::String const & name )const;
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
		inline Animation< OwnerT > & getAnimation( castor::String const & name );
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
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	animation	L'animation.
		 */
		inline void doAddAnimation( AnimationSPtr && animation );
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
		template< typename Type >
		inline Type & doGetAnimation( castor::String const & name );

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationPtrStrMap m_animations;
	};
}

#include "Animable.inl"

#endif
