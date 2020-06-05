/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedObjectGroupCache_H___
#define ___C3D_AnimatedObjectGroupCache_H___

#include "Castor3D/Cache/CacheBase.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		AnimatedObjectGroup cache.
	\~french
	\brief		Cache de AnimatedObjectGroup.
	*/
	template<>
	class Cache< AnimatedObjectGroup, castor::String >
		: public CacheBase< AnimatedObjectGroup, castor::String >
	{
	public:
		struct SkeletonPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedSkeleton const & skeleton;
			UniformBufferOffset< SkinningUbo::Configuration > skinningUbo;
		};
		struct MeshPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedMesh const & mesh;
			UniformBufferOffset< MorphingUbo::Configuration > morphingUbo;
		};
		using MyCache = CacheBase< AnimatedObjectGroup, castor::String >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using LockType = typename MyCacheType::LockType;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	produce			The element producer.
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	produce			Le créateur d'objet.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 */
		C3D_API Cache( Engine & engine
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Cache();
		/**
		 *\~english
		 *\brief		Updates the UBO pools data.
		 *\~french
		 *\brief		Met à jour le contenu des pools d'UBO.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Updates the UBO pools in VRAM.
		 *\~french
		 *\brief		Met à jour les pools d'UBO en VRAM.
		 */
		C3D_API void uploadUbos( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Cleans up the UBO pools.
		 *\~french
		 *\brief		Nettoie les pools d'UBO.
		 */
		C3D_API void cleanupUbos();
		/**
		 *\~english
		 *\return		The entry related to given AnimatedMesh.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedMesh donné.
		 */
		C3D_API MeshPoolsEntry getUbos( AnimatedMesh const & mesh )const;
		/**
		 *\~english
		 *\return		The entry related to given AnimatedSkeleton.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedSkeleton donné.
		 */
		C3D_API SkeletonPoolsEntry getUbos( AnimatedSkeleton const & skeleton )const;
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Adds an object.
		 *\param[in]	element	The object.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	element	L'objet.
		 */
		C3D_API void add( ElementPtr element );
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name	The object name.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\return		L'objet créé.
		 */
		C3D_API ElementPtr add( Key const & name );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name	Le nom d'objet.
		 */
		C3D_API void remove( Key const & name );

	private:
		using MyCache::add;

	private:
		SkeletonPoolsEntry doCreateEntry( AnimatedObjectGroup const & group
			, AnimatedSkeleton const & skeleton );
		MeshPoolsEntry doCreateEntry( AnimatedObjectGroup const & group
			, AnimatedMesh const & mesh );
		void doRemoveEntry( AnimatedSkeleton const & skeleton );
		void doRemoveEntry( AnimatedMesh const & mesh );
		void doRegister( AnimatedObjectGroup & group );
		void doUnregister( AnimatedObjectGroup & group );

	private:
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
		std::map< AnimatedSkeleton const *, SkeletonPoolsEntry > m_skeletonEntries;
		std::map< AnimatedMesh const *, MeshPoolsEntry > m_meshEntries;
		std::map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonAddedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonRemovedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshAddedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshRemovedConnections;
		UniformBufferPool< SkinningUbo::Configuration > m_skinningUboPool;
		UniformBufferPool< MorphingUbo::Configuration > m_morphingUboPool;
		RenderPassTimerSPtr m_updateTimer;
	};
}

#endif
