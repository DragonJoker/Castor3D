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
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimatedSkeleton( AnimatedSkeleton && rhs ) = default;
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton && rhs ) = default;
		C3D_API AnimatedSkeleton( AnimatedSkeleton const & rhs ) = delete;
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The object name.
		 *\param[in]	skeleton	The skeleton.
		 *\param[in]	mesh		The mesh to which the skeleton is bound.
		 *\param[in]	geometry	The geometry instantiating the mesh.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom de l'objet.
		 *\param[in]	skeleton	Le squelette.
		 *\param[in]	mesh		Le maillage auquel le squelette est lié.
		 *\param[in]	geometry	La géométrie instanciant le maillage.
		 */
		C3D_API AnimatedSkeleton( castor::String const & name
			, Skeleton & skeleton
			, Mesh & mesh
			, Geometry & geometry );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedSkeleton();
		/**
		 *\~english
		 *\brief		Fills a shader variable with this object's skeleton transforms.
		 *\param[out]	variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations du squelette de cet objet.
		 *\param[out]	variable	Reçoit les transformations.
		 */
		C3D_API void fillShader( Uniform4x4r & variable )const;
		/**
		 *\~english
		 *\brief		Fills a buffer with this object's skeleton transforms.
		 *\param[out]	buffer	Receives the transforms.
		 *\~french
		 *\brief		Remplit un tampon avec les transformations du squelette de cet objet.
		 *\param[out]	buffer	Reçoit les transformations.
		 */
		C3D_API void fillBuffer( uint8_t * buffer )const;
		/**
		 *\copydoc		castor3d::AnimatedObject::update
		 */
		C3D_API void update( castor::Milliseconds const & elapsed )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::isPlayingAnimation
		 */
		C3D_API bool isPlayingAnimation()const override
		{
			return !m_playingAnimations.empty();
		}
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		inline Skeleton const & getSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The mesh.
		 *\~french
		 *\return		Le maillage.
		 */
		inline Mesh const & getMesh()const
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The mesh.
		 *\~french
		 *\return		Le maillage.
		 */
		inline Mesh & getMesh()
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The geometry instantiating the mesh.
		 *\~french
		 *\return		La géométrie instanciant le maillage.
		 */
		inline Geometry const & getGeometry()const
		{
			return m_geometry;
		}
		/**
		 *\~english
		 *\return		The geometry instantiating the mesh.
		 *\~french
		 *\return		La géométrie instanciant le maillage.
		 */
		inline Geometry & getGeometry()
		{
			return m_geometry;
		}

	private:
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doAddAnimation( castor::String const & name )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doStartAnimation( AnimationInstance & animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doStopAnimation( AnimationInstance & animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doClearAnimations()override;

	protected:
		//!\~english	The skeleton affected by the animations.
		//!\~french		Le squelette affecté par les animations.
		Skeleton & m_skeleton;
		//!\~english	The mesh using the skeleton.
		//!\~french		Le maillage utilisant le squelette.
		Mesh & m_mesh;
		//!\~english	The geometry instantiating the mesh.
		//!\~french		La géométrie instanciant le maillage.
		Geometry & m_geometry;
		//!\~english	Currently playing animations.
		//!\~french		Les animations en cours de lecture.
		SkeletonAnimationInstanceArray m_playingAnimations;
	};
}

#endif

