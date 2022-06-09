/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationKeyFrame_H___
#define ___C3D_SkeletonAnimationKeyFrame_H___

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"

namespace castor3d
{
	class SkeletonAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< SkeletonAnimation >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	skeletonAnimation	The parent animation.
		 *\param[in]	timeIndex			When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	skeletonAnimation	L'animation parente.
		 *\param[in]	timeIndex			Quand la key frame commence.
		 */
		C3D_API SkeletonAnimationKeyFrame( SkeletonAnimation & skeletonAnimation
			, castor::Milliseconds const & timeIndex );
		/**
		 *\~english
		 *\brief		Adds an animation object.
		 *\param[in]	object		The animation object.
		 *\param[in]	translate	The translation at start time.
		 *\param[in]	rotate		The rotation at start time.
		 *\param[in]	scale		The scaling at start time.
		 *\~french
		 *\brief		Ajoute un objet d'animation.
		 *\param[in]	object		L'objet d'animation.
		 *\param[in]	translate	La translation au temps de début.
		 *\param[in]	rotate		La rotation au temps de début.
		 *\param[in]	scale		L'échelle au temps de début.
		*/
		C3D_API void addAnimationObject( SkeletonAnimationObject & object
			, castor::Point3f const & translate
			, castor::Quaternion const & rotate
			, castor::Point3f const & scale );
		/**
		 *\~english
		 *\brief		Adds an animation object.
		 *\param[in]	object		The animation object.
		 *\param[in]	transform	The transformation at start time.
		 *\~french
		 *\brief		Ajoute un objet d'animation.
		 *\param[in]	object		L'objet d'animation.
		 *\param[in]	transform	La transformation au temps de début.
		*/
		C3D_API void addAnimationObject( SkeletonAnimationObject & object
			, castor::Matrix4x4f const & transform );
		/**
		*\~english
		*\return		\p true if the given object is into the transforms map (not the cumulative one).
		*\~french
		*\return		\p true si l'objet donné est dans la map des transformations (pas celle des transformations cumulatives).
		*/
		C3D_API bool hasObject( SkeletonAnimationObject const & object )const;
		/**
		 *\~english
		 *\return		The iterator matching given animation object, into cumulative transforms map.
		 *\~french
		 *\return		L'itérateur correspondant à l'objet d'animation donné, dans la map des transformations cumulatives.
		 */
		C3D_API TransformArray::const_iterator find( SkeletonAnimationObject const & object )const;
		/**
		 *\~english
		 *\return		The iterator matching given bone, into cumulative transforms map.
		 *\~french
		 *\return		L'itérateur correspondant à l'os donné, dans la map des transformations cumulatives.
		 */
		C3D_API TransformArray::const_iterator find( BoneNode const & bone )const;
		/**
		 *\~english
		 *\brief		Initialises the keyframe.
		 *\~french
		 *\brief		Initialise la keyframe.
		 */
		C3D_API void initialise()override;
		/**
		 *\~english
		 *\return		The beginning of the cumulative transforms map.
		 *\~french
		 *\return		Le début de la map des transformations cumulatives.
		 */
		TransformArray::const_iterator begin()const
		{
			return m_cumulative.begin();
		}
		/**
		 *\~english
		 *\return		The beginning of the cumulative transforms map.
		 *\~french
		 *\return		Le début de la map des transformations cumulatives.
		 */
		TransformArray::iterator begin()
		{
			return m_cumulative.begin();
		}
		/**
		 *\~english
		 *\return		The end of the cumulative transforms map.
		 *\~french
		 *\return		La fin de la map des transformations cumulatives.
		 */
		TransformArray::const_iterator end()const
		{
			return m_cumulative.end();
		}
		/**
		 *\~english
		 *\return		The end of the cumulative transforms map.
		 *\~french
		 *\return		La fin de la map des transformations cumulatives.
		 */
		TransformArray::iterator end()
		{
			return m_cumulative.end();
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		//!\~english	The transformations, per animation object.
		//!\~french		Les transformations, par objet d'animation.
		TransformArray m_transforms;
		//!\~english	The cumulative transformations, per animation object.
		//!\~french		Les transformations cumulatives, par objet d'animation.
		TransformArray m_cumulative;

		friend class BinaryParser< SkeletonAnimationKeyFrame >;
		friend class BinaryWriter< SkeletonAnimationKeyFrame >;
	};
}

#endif

