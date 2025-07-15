/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedObjectGroupCache_H___
#define ___C3D_AnimatedObjectGroupCache_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/ResourceCacheBase.hpp>

namespace c3d
{
	/**
	\~english
	\brief		AnimatedObjectGroup cache.
	\~french
	\brief		Cache de AnimatedObjectGroup.
	*/
	template<>
	class ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >
		: public OwnedBy< Scene >
		, public ResourceCacheBaseT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >
	{
	public:
		struct SkeletonPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedSkeleton const & skeleton;
		};
		struct MeshPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedMesh const & mesh;
			Submesh const & submesh;
		};

		using ElementT = AnimatedObjectGroup;
		using ElementKeyT = String;
		using ElementCacheTraitsT = AnimatedObjectGroupCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene	The parent scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène parent.
		 */
		C3D_API explicit ResourceCacheT( Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ResourceCacheT()noexcept override;
		/**
		 *\~english
		 *\brief		Initialises the cache buffers.
		 *\~french
		 *\brief		Initialise les buffers du cache.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Vide la collection.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void clear( RenderDevice const & device );

		C3D_API Vector< AnimatedObject * > findObject( String const & name )const;

		GpuBufferOffsetT< MorphingWeightsConfiguration > const & getMorphingWeights()const
		{
			return m_morphingWeights;
		}

		GpuBufferOffsetT< SkinningTransformsConfiguration > const & getSkinningTransformsBuffer()const
		{
			return m_skinningTransformsData;
		}

	private:
		using ElementCacheT::clear;

	private:
		SkeletonPoolsEntry doCreateEntry( RenderDevice const & device
			, AnimatedObjectGroup const & group
			, AnimatedSkeleton const & skeleton )const;
		MeshPoolsEntry doCreateEntry( RenderDevice const & device
			, AnimatedObjectGroup const & group
			, AnimatedMesh const & mesh
			, Submesh const & submesh )const;
		void doRemoveEntry( RenderDevice const & device
			, AnimatedSkeleton const & skeleton );
		void doRemoveEntry( RenderDevice const & device
			, AnimatedMesh const & mesh
			, Submesh const & submesh );
		void doRemoveEntry( RenderDevice const & device
			, AnimatedTexture const & texture )const;
		void doRegister( AnimatedObjectGroup & group );
		void doUnregister( AnimatedObjectGroup & group );

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		Map< AnimatedSkeleton const *, SkeletonPoolsEntry > m_skeletonEntries;
		Map< size_t, MeshPoolsEntry > m_meshEntries;
		Map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonAddedConnections;
		Map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonRemovedConnections;
		Map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshAddedConnections;
		Map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshRemovedConnections;
		Map< AnimatedObjectGroup *, OnAnimatedTextureChangeConnection > m_textureRemovedConnections;
		GpuBufferOffsetT< MorphingWeightsConfiguration > m_morphingWeights;
		GpuBufferOffsetT< SkinningTransformsConfiguration > m_skinningTransformsData;
		FramePassTimerUPtr m_timerAnimations;
	};
}

#endif
