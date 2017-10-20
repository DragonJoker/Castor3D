/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_SKELETON_H___
#define ___C3D_ANIMATED_SKELETON_H___

#include "AnimatedObject.hpp"

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
	class AnimatedSkeleton
		: public AnimatedObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_skeleton	The skeleton.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de l'objet.
		 *\param[in]	p_skeleton	Le squelette.
		 */
		C3D_API AnimatedSkeleton( castor::String const & p_name, Skeleton & p_skeleton );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedSkeleton();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimatedSkeleton( AnimatedSkeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimatedSkeleton( AnimatedSkeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Fills a shader variable with this object's skeleton transforms.
		 *\param[out]	p_variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations du squelette de cet objet.
		 *\param[out]	p_variable	Reçoit les transformations.
		 */
		C3D_API void fillShader( Uniform4x4r & p_variable )const;
		/**
		 *\~english
		 *\brief		Fills a buffer with this object's skeleton transforms.
		 *\param[out]	p_buffer	Receives the transforms.
		 *\~french
		 *\brief		Remplit un tampon avec les transformations du squelette de cet objet.
		 *\param[out]	p_buffer	Reçoit les transformations.
		 */
		C3D_API void fillBuffer( uint8_t * p_buffer )const;
		/**
		 *\copydoc		castor3d::AnimatedObject::Update
		 */
		C3D_API void update( castor::Milliseconds const & p_tslf )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::isPlayingAnimation
		 */
		C3D_API bool isPlayingAnimation()const override
		{
			return !m_playingAnimations.empty();
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Skeleton const & getSkeleton()const
		{
			return m_skeleton;
		}

	private:
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doAddAnimation( castor::String const & p_name )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doStartAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doStopAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doClearAnimations()override;

	protected:
		//!\~english	The skeleton affected by the animations.
		//!\~french		Le squelette affecté par les animations.
		Skeleton & m_skeleton;
		//!\~english	Currently playing animations.
		//!\~french		Les animations en cours de lecture.
		SkeletonAnimationInstanceArray m_playingAnimations;
	};
}

#endif

