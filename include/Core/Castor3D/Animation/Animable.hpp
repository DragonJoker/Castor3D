/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animable_H___
#define ___C3D_Animable_H___

#include "Castor3D/Animation/Animation.hpp"

namespace castor3d
{
	template< typename AnimableHanlerT >
	class AnimableT
		: public castor::OwnedBy< AnimableHanlerT >
	{
	protected:
		using Animation = AnimationT< AnimableHanlerT >;
		using AnimationPtr = std::unique_ptr< Animation >;
		using AnimationPtrStrMap = std::map< castor::String, AnimationPtr >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	owner	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	owner	Le parent.
		 */
		inline explicit AnimableT( AnimableHanlerT & owner );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AnimableT() = default;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimableT( AnimableT && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimableT & operator=( AnimableT && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimableT( AnimableT const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimableT & operator=( AnimableT const & rhs ) = delete;

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
		inline Animation const & getAnimation( castor::String const & name )const;
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
		inline Animation & getAnimation( castor::String const & name );
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
		inline void doAddAnimation( AnimationPtr animation );
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
		inline AnimationType & doGetAnimation( castor::String const & name );

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationPtrStrMap m_animations;
	};
}

#include "Animable.inl"

#endif
