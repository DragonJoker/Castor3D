#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

using namespace castor;

using ashes::operator==;
using ashes::operator!=;

//*************************************************************************************************

namespace castor
{
	void Deleter< castor3d::SceneRenderNodes >::operator()( castor3d::SceneRenderNodes * ptr )noexcept
	{
		delete ptr;
	}
}

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		template< typename NodeType, typename MapType, typename CulledType >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeType && node
			, CulledType const & culled
			, MapType & nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			ObjectRenderNodesArray tmp;
			auto itPipeline = nodes.emplace( &pipeline, std::move( tmp ) ).first;
			itPipeline->second.emplace( &culled, std::move( node ) );
		}

		template< typename NodeType, typename MapType, typename CulledType >
		void doAddRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeType && node
			, Submesh & object
			, CulledType const & culled
			, MapType & nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap{} ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap{} ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray{} ).first;
			itObject->second.emplace( &culled, std::move( node ) );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene const & scene
			, String const & name )
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

		template< typename CreatorFunc
			, typename NodesType
			, typename CulledType
			, typename ... Params >
		void doAddNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, NodesType & nodes
			, VkPrimitiveTopology topology
			, CulledType const & culled
			, CreatorFunc creator )
		{
			if ( pass.isTwoSided()
				|| renderPass.forceTwoSided()
				|| pass.hasAlphaBlending() )
			{
				auto pipeline = renderPass.getPipelineFront( flags );

				if ( pipeline )
				{
					doAddRenderNode( *pipeline
						, creator( *pipeline )
						, culled
						, nodes.frontCulled );
				}
			}

			auto pipeline = renderPass.getPipelineBack( flags );

			if ( pipeline )
			{
				doAddRenderNode( *pipeline
					, creator( *pipeline )
					, culled
					, nodes.backCulled );
			}
		}

		void doAddSkinningNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton
			, CulledSubmesh const & culled
			, SceneRenderNodes::SkinnedNodesMap & animated
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.isTwoSided()
					|| renderPass.forceTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						doAddRenderNode( pass
							, *pipeline
							, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
							, submesh
							, culled
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( flags );

				if ( pipeline )
				{
					doAddRenderNode( pass
						, *pipeline
						, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
						, submesh
						, culled
						, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, flags
					, pass
					, animated
					, submesh.getTopology()
					, culled
					, [&renderPass, &pass, &submesh, &primitive, &skeleton]( RenderPipeline & pipeline )
					{
						return renderPass.createSkinningNode( pass
							, pipeline
							, submesh
							, primitive
							, skeleton );
					} );
			}
		}

		void doAddMorphingNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh
			, CulledSubmesh const & culled
			, SceneRenderNodes::MorphingNodesMap & animated )
		{
			doAddNode( renderPass
				, flags
				, pass
				, animated
				, submesh.getTopology()
				, culled
				, [&renderPass, &pass, &submesh, &primitive, &mesh]( RenderPipeline & pipeline )
				{
					return renderPass.createMorphingNode( pass
						, pipeline
						, submesh
						, primitive
						, mesh );
				} );
		}

		void doAddStaticNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, CulledSubmesh const & culled
			, SceneRenderNodes::StaticNodesMap & statics
			, SceneRenderNodes::InstantiatedStaticNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.isTwoSided()
					|| renderPass.forceTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
						doAddRenderNode( pass
							, *pipeline
							, node
							, submesh
							, culled
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( flags );

				if ( pipeline )
				{
					auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
					doAddRenderNode( pass
						, *pipeline
						, node
						, submesh
						, culled
						, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, flags
					, pass
					, statics
					, submesh.getTopology()
					, culled
					, [&renderPass, &pass, &submesh, &primitive]( RenderPipeline & pipeline )
					{
						return renderPass.createStaticNode( pass
							, pipeline
							, submesh
							, primitive );
					} );
			}
		}

		SceneRenderNodes::AnimatedObjects doAdjustFlags( RenderSystem const & renderSystem
			, ProgramFlags & programFlags
			, PassFlags const & passFlags
			, SceneFlags const & sceneFlags
			, Scene const & scene
			, Pass const & pass
			, RenderPass const & renderPass
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

			if ( checkFlag( submeshFlags, ProgramFlag::eInstantiation )
				&& ( !pass.hasAlphaBlending() || renderPass.isOrderIndependent() )
				&& !pass.hasEnvironmentMapping() )
			{
				if ( checkFlag( programFlags, ProgramFlag::eSkinning )
					&& renderSystem.getGpuInformations().hasShaderStorageBuffers() )
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

		void doSortRenderNodes( RenderPass & renderPass
			, RenderMode mode
			, SceneNode const * ignored
			, SceneRenderNodes & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			uint32_t instanceMult = renderPass.getInstanceMult();
			auto & scene = nodes.scene;

			for ( auto & culledNode : renderPass.getCuller().getAllSubmeshes( mode ).objects )
			{
				auto & submesh = culledNode.data;
				auto pass = culledNode.pass;
				auto & instance = culledNode.instance;
				auto material = pass->getOwner()->shared_from_this();

				if ( ignored != &culledNode.sceneNode )
				{
					pass->prepareTextures();
					auto passFlags = pass->getPassFlags();

					if ( isValidNodeForPass( passFlags, mode ) )
					{
						auto programFlags = submesh.getProgramFlags( material );
						auto sceneFlags = scene.getFlags();
						auto textures = pass->getTextures( renderPass.getTexturesMask() );
						auto animated = doAdjustFlags( *renderPass.getEngine()->getRenderSystem()
							, programFlags
							, passFlags
							, sceneFlags
							, scene
							, *pass
							, renderPass
							, instance.getName() );
						auto flags = renderPass.prepareBackPipeline( pass->getColourBlendMode()
							, pass->getAlphaBlendMode()
							, pass->getAlphaFunc()
							, passFlags
							, textures
							, pass->getHeightTextureIndex()
							, programFlags
							, sceneFlags
							, submesh.getTopology()
							, submesh.getGeometryBuffers( material, instanceMult, textures ).layouts );
						nodes.addRenderNode( flags
							, animated
							, culledNode
							, instance
							, *pass
							, submesh
							, renderPass );

						auto needsFront = ( mode == RenderMode::eTransparentOnly )
							|| pass->isTwoSided()
							|| renderPass.forceTwoSided()
							|| checkFlags( textures, TextureFlag::eOpacity ) != textures.end();

						if ( needsFront )
						{
							auto flags = renderPass.prepareFrontPipeline( pass->getColourBlendMode()
								, pass->getAlphaBlendMode()
								, pass->getAlphaFunc()
								, passFlags
								, textures
								, pass->getHeightTextureIndex()
								, programFlags
								, sceneFlags
								, submesh.getTopology()
								, submesh.getGeometryBuffers( material, instanceMult, textures ).layouts );
							nodes.addRenderNode( flags
								, animated
								, culledNode
								, instance
								, *pass
								, submesh
								, renderPass );
						}
					}
				}
			}

			for ( auto & culledNode : renderPass.getCuller().getAllBillboards( mode ).objects )
			{
				auto & billboard = culledNode.data;
				auto & pass = culledNode.pass;

				pass->prepareTextures();
				auto sceneFlags = scene.getFlags();
				auto passFlags = pass->getPassFlags();
				auto programFlags = billboard.getProgramFlags();
				auto textures = pass->getTextures( renderPass.getTexturesMask() );
				addFlag( programFlags, ProgramFlag::eBillboards );
				auto flags = renderPass.prepareBackPipeline( pass->getColourBlendMode()
					, pass->getAlphaBlendMode()
					, pass->getAlphaFunc()
					, passFlags
					, textures
					, pass->getHeightTextureIndex()
					, programFlags
					, sceneFlags
					, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
					, billboard.getGeometryBuffers().layouts );

				if ( isValidNodeForPass( passFlags, mode )
					&& !isShadowMapProgram( programFlags ) )
				{
					nodes.addRenderNode( flags
						, culledNode
						, *pass
						, billboard
						, renderPass );
				}
			}
		}

		//*****************************************************************************************

		template< typename MapType >
		void doInitialiseNodes( RenderPass & renderPass
			, MapType & pipelineNodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNode : pipelineNodes )
			{
				pipelineNode.first->createDescriptorPools( uint32_t( pipelineNode.second.size() ) );

				for ( auto & node : pipelineNode.second )
				{
					renderPass.initialiseUboDescriptor( pipelineNode.first->getDescriptorPool( 0u ), node.second );
					Pass & pass = node.second.passNode.pass;

					if ( pipelineNode.first->hasDescriptorPool( 1u ) )
					{
						renderPass.initialiseTextureDescriptor( pipelineNode.first->getDescriptorPool( 1u )
							, node.second
							, shadowMaps );
					}
				}
			}
		}

		template< typename MapType >
		void doInitialiseInstancedNodes( RenderPass & renderPass
			, MapType & pipelineNodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNode : pipelineNodes )
			{
				uint32_t size = 0u;
				RenderPipeline & pipeline = *pipelineNode.first;

				for ( auto & passNodes : pipelineNode.second )
				{
					size += uint32_t( passNodes.second.size() );
				}

				pipelineNode.first->createDescriptorPools( size );
				renderPass.initialiseUboDescriptor( pipeline.getDescriptorPool( 0u )
					, pipelineNode.second );

				if ( pipeline.hasDescriptorPool( 1u ) )
				{
					renderPass.initialiseTextureDescriptor( pipeline.getDescriptorPool( 1u )
						, pipelineNode.second
						, shadowMaps );
				}
			}
		}
	}

	//*************************************************************************************************

	void SceneRenderNodes::parse( RenderQueue const & queue
		, ShadowMapLightTypeArray & shadowMaps )
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

		castor3d::doSortRenderNodes( *queue.getOwner()
			, queue.getMode()
			, queue.getIgnoredNode()
			, *this
			, shadowMaps );

		auto & renderPass = *queue.getOwner();
		renderPass.getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [&renderPass, this, shadowMaps]( RenderDevice const & device )
			{
				doInitialiseNodes( renderPass, staticNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, staticNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.backCulled, shadowMaps );

				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.backCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.backCulled, shadowMaps );
			} ) );
		renderPass.getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [&renderPass, this, shadowMaps]( RenderDevice const & device )
			{
				doInitialiseNodes( renderPass, billboardNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, billboardNodes.backCulled, shadowMaps );
			} ) );
	}

	void SceneRenderNodes::addRenderNode( PipelineFlags const & flags
		, AnimatedObjects const & animated
		, CulledSubmesh const & culledNode
		, Geometry & instance
		, Pass & pass
		, Submesh & submesh
		, RenderPass & renderPass )
	{
		if ( instance.isShadowCaster()
			|| ( !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( animated.skeleton )
			{
				doAddSkinningNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, *animated.skeleton
					, culledNode
					, skinnedNodes
					, instancedSkinnedNodes );
			}
			else if ( animated.mesh )
			{
				doAddMorphingNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, *animated.mesh
					, culledNode
					, morphingNodes );
			}
			else
			{
				doAddStaticNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, culledNode
					, staticNodes
					, instancedStaticNodes );
			}
		}
	}

	void SceneRenderNodes::addRenderNode( PipelineFlags const & flags
		, CulledBillboard const & culledNode
		, Pass & pass
		, BillboardBase & billboard
		, RenderPass & renderPass )
	{
		doAddNode( renderPass
			, flags
			, pass
			, billboardNodes
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
			, culledNode
			, [&renderPass, &pass, &billboard]( RenderPipeline & pipeline )
			{
				return renderPass.createBillboardNode( pass
					, pipeline
					, billboard );
			} );
	}
}
