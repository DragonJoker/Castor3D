/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationKeyFrame_H___
#define ___C3D_SkeletonAnimationKeyFrame_H___

#include "Animation/AnimationKeyFrame.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	using KeyFrameMap = std::map< SkeletonAnimationObject const *, castor::Matrix4x4r >;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class SkeletonAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< Skeleton >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	timeIndex	When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	timeIndex	Quand la key frame commence.
		 */
		SkeletonAnimationKeyFrame( Skeleton & skeleton
			, castor::Milliseconds const & timeIndex = 0_ms );
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
		C3D_API void addAnimationObject( SkeletonAnimationObject const & object
			, castor::Point3r const & translate
			, castor::Quaternion const & rotate
			, castor::Point3r const & scale );
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
		C3D_API void addAnimationObject( SkeletonAnimationObject const & object
			, castor::Matrix4x4r const & transform );
		/**
		 *\~english
		 *\return		The iterator matching given animation object.
		 *\~french
		 *\return		L'itérateur correspondant à l'objet d'animation donné.
		 */
		inline KeyFrameMap::const_iterator find( SkeletonAnimationObject const & object )const
		{
			return m_keyFrames.find( &object );
		}
		/**
		 *\~english
		 *\return		The beginning of the animation objects.
		 *\~french
		 *\return		Le début des objets d'animation.
		 */
		inline KeyFrameMap::const_iterator begin()const
		{
			return m_keyFrames.begin();
		}
		/**
		 *\~english
		 *\return		The end of the animation objects.
		 *\~french
		 *\return		La fin des objets d'animation.
		 */
		inline KeyFrameMap::const_iterator end()const
		{
			return m_keyFrames.end();
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		//!\~english	The keyframes, per animation object.
		//!\~french		Les keyframes, par objet d'animation.
		KeyFrameMap m_keyFrames;
	};
}

#endif

