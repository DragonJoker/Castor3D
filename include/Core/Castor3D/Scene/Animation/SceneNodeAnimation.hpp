/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeAnimation_H___
#define ___C3D_SceneNodeAnimation_H___

#include "AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Animation/Animation.hpp"

namespace castor3d
{
	class SceneNodeAnimation
		: public Animation
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API SceneNodeAnimation( SceneNodeAnimation && rhs ) = default;
		C3D_API SceneNodeAnimation & operator=( SceneNodeAnimation && rhs ) = delete;
		C3D_API SceneNodeAnimation( SceneNodeAnimation const & rhs ) = delete;
		C3D_API SceneNodeAnimation & operator=( SceneNodeAnimation const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animable	The parent animable object.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animable	L'objet animable parent.
		 *\param[in]	name		Le nom de l'animation.
		 */
		C3D_API explicit SceneNodeAnimation( SceneNode & node
			, castor::String const & name = castor::cuEmptyString );

	protected:
		friend class BinaryWriter< SceneNodeAnimation >;
		friend class BinaryParser< SceneNodeAnimation >;
		friend class SceneNodeAnimationInstance;
	};
}

#endif
