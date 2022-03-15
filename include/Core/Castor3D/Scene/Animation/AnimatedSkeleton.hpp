/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_SKELETON_H___
#define ___C3D_ANIMATED_SKELETON_H___

#include "Castor3D/Scene/Animation/AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
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
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton && rhs ) = delete;
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
		 *\brief		Fills a shader variable with this object's skeleton transforms.
		 *\param[out]	variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations du squelette de cet objet.
		 *\param[out]	variable	Reçoit les transformations.
		 */
		C3D_API uint32_t fillShader( SkinningTransformsConfiguration * variable )const;
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

		bool isPlayingAnimation()const override
		{
			return !m_playingAnimations.empty();
		}

		Skeleton & getSkeleton()const
		{
			return m_skeleton;
		}

		Mesh & getMesh()const
		{
			return m_mesh;
		}

		Geometry & getGeometry()const
		{
			return m_geometry;
		}

		uint32_t getId()const
		{
			return m_id;
		}

		void setId( uint32_t id )
		{
			m_id = id;
		}

	private:
		void doAddAnimation( castor::String const & name )override;
		void doStartAnimation( AnimationInstance & animation )override;
		void doStopAnimation( AnimationInstance & animation )override;
		void doClearAnimations()override;

	protected:
		Skeleton & m_skeleton;
		Mesh & m_mesh;
		Geometry & m_geometry;
		SkeletonAnimationInstanceArray m_playingAnimations;
		uint32_t m_id{};
	};
}

#endif

