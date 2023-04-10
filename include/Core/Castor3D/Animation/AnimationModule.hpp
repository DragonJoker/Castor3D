/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationModule_H___
#define ___C3D_AnimationModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	/**@name Animation */
	//@{

	/**
	*\~english
	*\brief
	*	Animation State Enumerator.
	*\~french
	*\brief
	*	Enumération des états d'une animation.
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
	C3D_API castor::String getName( AnimationState value );
	/**
	*\~english
	*\brief
	*	Animation types enumeration.
	*\~french
	*\brief
	*	Enumération des types d'animation.
	*/
	enum class AnimationType
		: uint8_t
	{
		//!\~english	Scene node animation.
		//!\~french		Animation de noeud de scène.
		eSceneNode,
		//!\~english	Skeleton animation.
		//!\~french		Animation de squelette.
		eSkeleton,
		//!\~english	Submesh animation (morphing).
		//!\~french		Animation de sous-maillage (morphing).
		eMesh,
		//!\~english	Texture animation.
		//!\~french		Animation de texture.
		eTexture,
		CU_ScopedEnumBounds( eSceneNode )
	};
	C3D_API castor::String getName( AnimationType value );
	/**
	*\~english
	*\brief
	*	Interpolation modes enumeration.
	*\~french
	*\brief
	*	Enumération des modes d'înterpolation.
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
	C3D_API castor::String getName( InterpolatorType value );
	/**
	*\~english
	*\brief
	*	Animable public interface
	*\~french
	*\brief
	*	interface publique d'animable
	*/
	template< typename OwnerT >
	class AnimableT;
	/**
	*\~english
	*\brief
	*	Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	*\~french
	*\brief
	*	Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< typename T >
	class Interpolator;
	/**
	*\~english
	*\brief
	*	Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	*\~french
	*\brief
	*	Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type, InterpolatorType Mode >
	class InterpolatorT;
	/**
	*\~english
	*\brief
	*	Animation base class.
	*\remarks
	*	An animation is played using an AnimationInstance.
	*\~french
	*\brief
	*	Classe de base des animations.
	*\remarks
	*	Une animation est jouée au travers d'une AnimationInstance.
	*/
	template< typename OwnerT >
	class AnimationT;
	/**
	*\~english
	*\brief
	*	Used to play an Animation on a specific object.
	*\~french
	*\brief
	*	Utilisée pour jouer une animation sur un objet particulier.
	*/
	template< typename OwnerT >
	class AnimationInstanceT;
	/**
	*\~english
	*\brief
	*	Key frames are the frames where the animation must be at a precise state.
	*\~french
	*\brief
	*	Les key frames sont les frames auxquelles une animation est dans un état précis.
	*/
	class AnimationKeyFrame;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class AnimationImporter;
	/**
	*\~english
	*\brief
	*	The importer factory.
	*\~french
	*\brief
	*	La fabrique d'importeurs.
	*/
	class AnimationImporterFactory;

	using Point3rInterpolator = Interpolator< castor::Point3f >;
	using QuaternionInterpolator = Interpolator< castor::Quaternion >;

	template< typename DataT >
	using InterpolatorPtr = std::unique_ptr< Interpolator< DataT > >;

	CU_DeclareSmartPtr( castor3d, AnimationKeyFrame, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimationImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimationImporterFactory, C3D_API );

	CU_DeclareTemplateSmartPtr( castor3d, Animation );
	CU_DeclareTemplateSmartPtr( castor3d, AnimationInstance );

	using AnimationKeyFrameArray = std::vector< AnimationKeyFrameUPtr >;
	//@}
}

#endif
