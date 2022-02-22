#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

CU_ImplementCUSmartPtr( castor3d, QueueRenderNodes )

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	namespace
	{
		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT * node
			, NodeCulledT< NodeT > const & culled
			, NodeByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, NodeMapT< NodeT >{} ).first->second;
			pipelineMap.emplace( &culled, node );
		}

		template< typename NodeT >
		void doAddInstantiatedRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeT * node
			, Submesh & object
			, NodeCulledT< NodeT > const & culled
			, ObjectNodesByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, ObjectNodesByPassMapT< NodeT >{} ).first->second;
			auto & passMap = pipelineMap.emplace( &pass, ObjectNodesMapT< NodeT >{} ).first->second;
			auto & objectMap = passMap.emplace( &object, NodeMapT< NodeT >{} ).first->second;
			objectMap.emplace( &culled, node );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene const & scene
			, castor::String const & name )
		{
			AnimatedObjectSPtr result;
			auto & cache = scene.getAnimatedObjectGroupCache();
			using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };

			for ( auto group : cache )
			{
				if ( !result )
				{
					auto it = group.second->getObjects().find( name );

					if ( it != group.second->getObjects().end() )
					{
						result = it->second;
					}
				}
			}

			return result;
		}

		void doAddSkinningNode( RenderNodesPass & renderPass
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton
			, CulledSubmesh const & culled
			, QueueRenderNodes::SkinnedNodesMap & animated
			, QueueRenderNodes::InstantiatedSkinnedNodesMap & instanced
			, bool front )
		{
			if ( checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
			{
				doAddInstantiatedRenderNode( pass
					, pipeline
					, renderPass.createSkinningNode( pass
						, pipeline
						, submesh
						, primitive
						, skeleton )
					, submesh
					, culled
					, ( front
						? instanced.frontCulled
						: instanced.backCulled ) );
			}
			else
			{
				doAddRenderNode( pipeline
					, renderPass.createSkinningNode( pass
						, pipeline
						, submesh
						, primitive
						, skeleton )
					, culled
					, ( front
						? animated.frontCulled
						: animated.backCulled ) );
			}
		}

		void doAddMorphingNode( RenderNodesPass & renderPass
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh
			, CulledSubmesh const & culled
			, QueueRenderNodes::MorphingNodesMap & animated
			, bool front )
		{
			doAddRenderNode( pipeline
				, renderPass.createMorphingNode( pass
					, pipeline
					, submesh
					, primitive
					, mesh )
				, culled
				, ( front
					? animated.frontCulled
					: animated.backCulled ) );
		}

		void doAddStaticNode( RenderNodesPass & renderPass
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, CulledSubmesh const & culled
			, QueueRenderNodes::StaticNodesMap & statics
			, QueueRenderNodes::InstantiatedStaticNodesMap & instanced
			, bool front )
		{
			if ( checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
			{
				doAddInstantiatedRenderNode( pass
					, pipeline
					, renderPass.createStaticNode( pass
						, pipeline
						, submesh
						, primitive )
					, submesh
					, culled
					, ( front
						? instanced.frontCulled
						: instanced.backCulled ) );
			}
			else
			{
				doAddRenderNode( pipeline
					, renderPass.createStaticNode( pass
						, pipeline
						, submesh
						, primitive )
					, culled
					, ( front
						? statics.frontCulled
						: statics.backCulled ) );
			}
		}

		QueueRenderNodes::AnimatedObjects doAdjustFlags( RenderSystem const & renderSystem
			, ProgramFlags & programFlags
			, SceneFlags const & sceneFlags
			, Scene const & scene
			, Pass const & pass
			, RenderNodesPass const & renderPass
			, castor::String const & name )
		{
			auto submeshFlags = programFlags;
			remFlag( programFlags, ProgramFlag::eSkinning );
			remFlag( programFlags, ProgramFlag::eMorphing );
			auto mesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
				? std::static_pointer_cast< AnimatedMesh >( doFindAnimatedObject( scene, name + cuT( "_Mesh" ) ) )
				: nullptr;
			auto skeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
				? std::static_pointer_cast< AnimatedSkeleton >( doFindAnimatedObject( scene, name + cuT( "_Skeleton" ) ) )
				: nullptr;

			if ( skeleton  )
			{
				addFlag( programFlags, ProgramFlag::eSkinning );
			}

			if ( mesh )
			{
				addFlag( programFlags, ProgramFlag::eMorphing );
			}

			if ( !mesh
				&& checkFlag( submeshFlags, ProgramFlag::eInstantiation )
				&& ( !pass.hasOnlyAlphaBlending() || renderPass.isOrderIndependent() )
				&& !pass.hasEnvironmentMapping() )
			{
				if ( !checkFlag( programFlags, ProgramFlag::eSkinning )
					|| renderSystem.getGpuInformations().hasShaderStorageBuffers() )
				{
					addFlag( programFlags, ProgramFlag::eInstantiation );
				}
				else
				{
					remFlag( programFlags, ProgramFlag::eInstantiation );
				}
			}
			else
			{
				remFlag( programFlags, ProgramFlag::eInstantiation );
			}

			return { mesh, skeleton };
		}

		//*****************************************************************************************

		void doListRenderNodes( RenderNodesPass & renderPass
			, RenderMode mode
			, SceneNode const * ignored
			, QueueRenderNodes & nodes )
		{
			for ( auto & culledNode : renderPass.getCuller().getAllSubmeshes( mode ).objects )
			{
				auto pass = culledNode.pass;

				if ( ignored != &culledNode.sceneNode )
				{
					pass->prepareTextures();

					if ( renderPass.isValidPass( *pass ) )
					{
						nodes.allSubmeshNodes.push_back( &culledNode );
					}
				}
			}

			for ( auto & culledNode : renderPass.getCuller().getAllBillboards( mode ).objects )
			{
				auto & billboard = culledNode.data;
				auto & pass = culledNode.pass;
				pass->prepareTextures();

				if ( renderPass.isValidPass( *pass )
					&& !isShadowMapProgram( billboard.getProgramFlags() ) )
				{
					nodes.allBillboardNodes.push_back( &culledNode );
				}
			}
		}

		void doSortRenderNodes( RenderNodesPass & renderPass
			, RenderMode mode
			, SceneNode const * ignored
			, QueueRenderNodes & nodes )
		{
			uint32_t instanceMult = renderPass.getInstanceMult();
			auto & scene = nodes.getOwner()->getCuller().getScene();

			for ( auto it : nodes.allSubmeshNodes )
			{
				auto & culledNode = *it;
				auto & submesh = culledNode.data;
				auto pass = culledNode.pass;
				auto & instance = culledNode.instance;
				auto material = pass->getOwner();
				auto programFlags = submesh.getProgramFlags( material );
				auto sceneFlags = scene.getFlags();
				auto textures = pass->getTexturesMask();
				auto animated = doAdjustFlags( *renderPass.getEngine()->getRenderSystem()
					, programFlags
					, sceneFlags
					, scene
					, *pass
					, renderPass
					, instance.getName() );
				auto pipelineFlags = renderPass.createPipelineFlags( *pass
					, textures
					, programFlags
					, sceneFlags
					, submesh.getTopology() );
				auto vertexLayouts = submesh.getGeometryBuffers( renderPass.getShaderFlags()
					, pipelineFlags.programFlags
					, material
					, instanceMult
					, textures
					, checkFlag( pipelineFlags.programFlags, ProgramFlag::eForceTexCoords ) ).layouts;
				auto & backPipeline = renderPass.prepareBackPipeline( pipelineFlags
					, vertexLayouts
					, nodes.getDescriptorSetLayouts( *pass
						, submesh
						, animated.mesh.get()
						, animated.skeleton.get() ) );
				nodes.addRenderNode( backPipeline
					, animated
					, culledNode
					, instance
					, *pass
					, submesh
					, renderPass
					, false );

				auto needsFront = ( mode == RenderMode::eTransparentOnly )
					|| pass->isTwoSided()
					|| renderPass.forceTwoSided()
					|| checkFlags( textures, TextureFlag::eOpacity ) != textures.end();

				if ( needsFront )
				{
					auto & frontPipeline = renderPass.prepareFrontPipeline( pipelineFlags
						, vertexLayouts
						, nodes.getDescriptorSetLayouts( *pass
							, submesh
							, animated.mesh.get()
							, animated.skeleton.get() ) );
					nodes.addRenderNode( frontPipeline
						, animated
						, culledNode
						, instance
						, *pass
						, submesh
						, renderPass
						, true );
				}
			}

			for ( auto & it : nodes.allBillboardNodes )
			{
				auto & culledNode = *it;
				auto & billboard = culledNode.data;
				auto & pass = culledNode.pass;
				auto programFlags = billboard.getProgramFlags();
				auto sceneFlags = scene.getFlags();
				auto textures = pass->getTexturesMask();
				auto pipelineFlags = renderPass.createPipelineFlags( *pass
					, textures
					, programFlags
					, sceneFlags
					, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
				auto & pipeline = renderPass.prepareBackPipeline( pipelineFlags
					, billboard.getGeometryBuffers().layouts
					, nodes.getDescriptorSetLayouts( *pass
						, billboard ) );
				nodes.addRenderNode( pipeline
					, culledNode
					, *pass
					, billboard
					, renderPass );
			}
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doInitialiseNodes( RenderNodesPass & renderPass
			, NodeByPipelineMapT< NodeT > & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNodes : nodes )
			{
				RenderPipeline & pipeline = *pipelineNodes.first;
				pipeline.createDescriptorPools( uint32_t( pipelineNodes.second.size() ) );

				if ( pipeline.hasDescriptorSetLayout() )
				{
					renderPass.initialiseAdditionalDescriptor( pipeline
						, shadowMaps );
				}
			}
		}

		template< typename NodeT >
		void doInitialiseInstancedNodes( RenderNodesPass & renderPass
			, ObjectNodesByPipelineMapT< NodeT > & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNodes : nodes )
			{
				uint32_t size = 0u;
				RenderPipeline & pipeline = *pipelineNodes.first;

				for ( auto & passNodes : pipelineNodes.second )
				{
					size += uint32_t( passNodes.second.size() );
				}

				pipeline.createDescriptorPools( size );

				if ( pipeline.hasDescriptorSetLayout() )
				{
					renderPass.initialiseAdditionalDescriptor( pipeline
						, shadowMaps );
				}
			}
		}
	}

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
	{
	}

	void QueueRenderNodes::parse()
	{
		allSubmeshNodes.clear();
		allBillboardNodes.clear();

		auto & queue = *getOwner();
		castor3d::doListRenderNodes( *queue.getOwner()
			, queue.getMode()
			, queue.getIgnoredNode()
			, *this );
	}

	void QueueRenderNodes::sort( ShadowMapLightTypeArray & shadowMaps )
	{
		instancedStaticNodes.backCulled.clear();
		instancedStaticNodes.frontCulled.clear();
		staticNodes.backCulled.clear();
		staticNodes.frontCulled.clear();
		skinnedNodes.backCulled.clear();
		skinnedNodes.frontCulled.clear();
		instancedSkinnedNodes.backCulled.clear();
		instancedSkinnedNodes.frontCulled.clear();
		morphingNodes.backCulled.clear();
		morphingNodes.frontCulled.clear();
		billboardNodes.backCulled.clear();
		billboardNodes.frontCulled.clear();

		auto & queue = *getOwner();
		castor3d::doSortRenderNodes( *queue.getOwner()
			, queue.getMode()
			, queue.getIgnoredNode()
			, *this );

		auto & renderPass = *queue.getOwner();
		renderPass.getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [&renderPass, this, shadowMaps]( RenderDevice const & device
				, QueueData const & queueData )
			{
				getOwner()->getCuller().getScene().getRenderNodes().initialiseNodes( device );

				doInitialiseNodes( renderPass, staticNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, staticNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, billboardNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, billboardNodes.backCulled, shadowMaps );

				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.backCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.backCulled, shadowMaps );
			} ) );
	}

	void QueueRenderNodes::addRenderNode( RenderPipeline & pipeline
		, AnimatedObjects const & animated
		, CulledSubmesh const & culledNode
		, Geometry & instance
		, Pass & pass
		, Submesh & submesh
		, RenderNodesPass & renderPass
		, bool front )
	{
		if ( instance.isShadowCaster()
			|| ( !isShadowMapProgram( pipeline.getFlags().programFlags ) ) )
		{
			if ( animated.skeleton )
			{
				doAddSkinningNode( renderPass
					, pipeline
					, pass
					, submesh
					, instance
					, *animated.skeleton
					, culledNode
					, skinnedNodes
					, instancedSkinnedNodes
					, front );
			}
			else if ( animated.mesh )
			{
				doAddMorphingNode( renderPass
					, pipeline
					, pass
					, submesh
					, instance
					, *animated.mesh
					, culledNode
					, morphingNodes
					, front );
			}
			else
			{
				doAddStaticNode( renderPass
					, pipeline
					, pass
					, submesh
					, instance
					, culledNode
					, staticNodes
					, instancedStaticNodes
					, front );
			}
		}
	}

	void QueueRenderNodes::addRenderNode( RenderPipeline & pipeline
		, CulledBillboard const & culledNode
		, Pass & pass
		, BillboardBase & billboard
		, RenderNodesPass & renderPass )
	{
		doAddRenderNode( pipeline
			, renderPass.createBillboardNode( pass
				, pipeline
				, billboard )
			, culledNode
			, billboardNodes.backCulled );
	}

	bool QueueRenderNodes::hasNodes()const
	{
		return !allBillboardNodes.empty()
			|| !allSubmeshNodes.empty();
	}

	SubmeshRenderNode & QueueRenderNodes::createNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( std::move( passNode )
			, buffers
			, sceneNode
			, data
			, instance
			, mesh
			, skeleton );
	}

	BillboardRenderNode & QueueRenderNodes::createNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & instance )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( std::move( passNode )
			, buffers
			, sceneNode
			, instance );
	}

	ashes::DescriptorSetLayoutCRefArray QueueRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, BillboardBase const & billboard )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().getDescriptorSetLayouts( pass
			, billboard );
	}

	ashes::DescriptorSetLayoutCRefArray QueueRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, Submesh const & submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().getDescriptorSetLayouts( pass
			, submesh
			, mesh
			, skeleton );
	}
}
