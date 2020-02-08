/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationModule_H___
#define ___C3D_AnimationModule_H___

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	/**@name Animation */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Animation types enumeration.
	\~french
	\brief		Enumération des types d'animation.
	*/
	enum class AnimationType
		: uint8_t
	{
		//!\~english	Scene node animation.
		//!\~french		Animation de noeud de scène.
		eMovable,
		//!\~english	Skeleton animation.
		//!\~french		Animation de squelette.
		eSkeleton,
		//!\~english	Submesh animation (morphing).
		//!\~french		Animation de sous-maillage (morphing).
		eMesh,
		CU_ScopedEnumBounds( eMovable )
	};
	castor::String getName( AnimationType value );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Interpolation modes enumeration.
	\~french
	\brief		Enumération des modes d'înterpolation.
	*/
	enum class InterpolatorType
		: uint8_t
	{
		//!\~english	No interpolation.
		//!\~french		Pas d'interpolation.
		eNearest,
		//!\~english	Linear mode.
		//!\~french		Mode linéaire.
		eLinear,
		CU_ScopedEnumBounds( eNearest )
	};
	castor::String getName( InterpolatorType value );
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		Animation State Enumerator.
	\~french
	\brief		Enumération des états d'une animation.
	*/
	enum class AnimationState
		: uint8_t
	{
		//!\~english	Playing animation state.
		//!\~french		Animation en cours de lecture.
		ePlaying,
		//!\~english	Stopped animation state.
		//!\~french		Animation stoppée.
		eStopped,
		//!\~english	Paused animation state.
		//!\~french		Animation en pause.
		ePaused,
		CU_ScopedEnumBounds( ePlaying )
	};
	castor::String getName( AnimationState value );
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.7.0
	\~english
	\brief		Animable public interface
	\~french
	\brief		interface publique d'animable
	*/
	template< typename OwnerT >
	class Animable;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< typename T >
	class Interpolator;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type, InterpolatorType Mode >
	class InterpolatorT;
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Animation base class.
	\remarks	An animation is played using an AnimationInstance.
	\~french
	\brief		Classe de base des animations.
	\remarks	Une animation est jouée au travers d'une AnimationInstance.
	*/
	template< typename OwnerT >
	class Animation;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Key frames are the frames where the animation must be at a precise state.
	\~french
	\brief		Les key frames sont les frames auxquelles une animation est dans un état précis.
	*/
	class AnimationKeyFrame;

	CU_DeclareSmartPtr( AnimationKeyFrame );
	using AnimationKeyFrameArray = std::vector< AnimationKeyFrameUPtr >;

	//@}
}

#endif
