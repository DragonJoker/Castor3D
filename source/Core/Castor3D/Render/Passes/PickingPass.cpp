#include "Castor3D/Render/Passes/PickingPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Picking.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, PickingPass )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static int constexpr PickingOffset = int( Picking::PickingWidth / 2 );
		static int constexpr BufferOffset = ( PickingOffset * Picking::PickingWidth ) + PickingOffset - 1;

		template< typename NodeT, typename FuncT >
		inline void traverseNodes( PickingPass & pass
			, std::unordered_map< SubmeshRenderNode const *, UniformBufferOffsetT< PickingUboConfiguration > > & ubos
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes
			, PickNodeType type
			, FuncT function )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						if ( !itSubmeshes.second.empty() )
						{
							auto renderNode = itSubmeshes.second.front();
							auto it = ubos.find( renderNode );
							CU_Require( it != ubos.end() );
							PickingUbo::update( it->second.getData()
								, drawIndex
								, index++ );
							function( *itPipelines.first
								, *itPass.first
								, *itSubmeshes.first
								, itSubmeshes.first->getInstantiation()
								, itSubmeshes.second );
						}
					}
				}

				count++;
			}
		}

		template< typename NodeT >
		inline void updateNonInstanced( PickingPass & pass
			, std::unordered_map< NodeT const *, UniformBufferOffsetT< PickingUboConfiguration > > & ubos
			, PickNodeType type
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					auto it = ubos.find( renderNode );
					CU_Require( it != ubos.end() );
					PickingUbo::update( it->second.getData()
						, drawIndex
						, index++ );
				}

				count++;
			}
		}

		std::vector< VkBufferImageCopy > createPickDisplayRegions()
		{
			std::vector< VkBufferImageCopy > result;

			for ( int i = 0; i < PickingPass::PickingWidth; ++i )
			{
				for ( int j = 0; j < PickingPass::PickingWidth; ++j )
				{
					result.push_back( { BufferOffset * ashes::getMinimalSize( VK_FORMAT_R32G32B32A32_SFLOAT )
						, 0u
						, 0u
						, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
						, { int( PickingPass::PickingWidth ) * 2 + i, j, 0 }
						, { 1u, 1u, 1u } } );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	uint32_t const PickingPass::UboBindingPoint = 7u;

	PickingPass::PickingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: SceneRenderPass{ pass
			, context
			, graph
			, device
			, cuT( "Picking" )
			, cuT( "Picking" )
			, SceneRenderPassDesc{ { size.getWidth(), size.getHeight(), 1u }, matrixUbo, culler, RenderMode::eBoth, true, false } }
	{
	}

	PickingPass::~PickingPass()
	{
		for ( auto & ubo : m_submeshBuffers )
		{
			m_device.uboPools->putBuffer( ubo.second );
		}

		for ( auto & ubo : m_billboardBuffers )
		{
			m_device.uboPools->putBuffer( ubo.second );
		}

		m_submeshBuffers.clear();
		m_billboardBuffers.clear();
	}

	void PickingPass::addScene( Scene & scene, Camera & camera )
	{
	}

	bool PickingPass::updateNodes( VkRect2D const & scissor )
	{
		ShadowMapLightTypeArray shadowMaps;
		m_renderQueue->update( shadowMaps, scissor );
		bool result = m_renderQueue->getCulledRenderNodes().hasNodes();

		if ( result )
		{
			doUpdateNodes( m_renderQueue->getCulledRenderNodes() );
		}

		return result;
	}

	QueueCulledRenderNodes const & PickingPass::getCulledRenderNodes()const
	{
		return m_renderQueue->getCulledRenderNodes();
	}

	TextureFlags PickingPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity };
	}

	bool PickingPass::doIsValidPass( PassFlags const & passFlags )const
	{
		if ( !checkFlag( passFlags, PassFlag::ePickable ) )
		{
			return false;
		}

		return SceneRenderPass::doIsValidPass( passFlags );
	}

	void PickingPass::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		auto & myCamera = getCuller().getCamera();
		auto & myScene = getCuller().getScene();
		m_matrixUbo.cpuUpdate( myCamera.getView()
			, myCamera.getProjection( false ) );
		doUpdate( nodes.instancedStaticNodes.backCulled );
		doUpdate( nodes.staticNodes.backCulled );
		doUpdate( nodes.skinnedNodes.backCulled );
		doUpdate( nodes.instancedSkinnedNodes.backCulled );
		doUpdate( nodes.morphingNodes.backCulled );
		doUpdate( nodes.billboardNodes.backCulled );
	}

	void PickingPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes )
	{
		updateNonInstanced( *this
			, m_submeshBuffers
			, PickNodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		updateNonInstanced( *this
			, m_billboardBuffers
			, PickNodeType::eBillboard
			, nodes );
	}

	void PickingPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( *this
			, m_submeshBuffers
			, nodes
			, PickNodeType::eInstantiatedStatic
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = component.find( pass.getOwner() );

				if ( it != component.end()
					&& it->second[0].buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second[0].data );

					if ( renderNodes.front()->skeleton )
					{
						auto & instantiatedBones = submesh.getInstantiatedBones();

						if ( instantiatedBones.hasInstancedBonesBuffer() )
						{
							doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
						}
					}
				}
			} );
	}

	void PickingPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PassUboIdx::eCount )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	void PickingPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto ires = m_billboardBuffers.insert( { &node, {} } );

		if ( ires.second )
		{
			ires.first->second = m_device.uboPools->getBuffer< PickingUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		descriptorWrites.push_back( ires.first->second.getDescriptorWrite( uint32_t( PassUboIdx::eCount ) ) );
	}

	void PickingPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto ires = m_submeshBuffers.insert( { &node, {} } );

		if ( ires.second )
		{
			ires.first->second = m_device.uboPools->getBuffer< PickingUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		descriptorWrites.push_back( ires.first->second.getDescriptorWrite( uint32_t( PassUboIdx::eCount ) ) );
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_UnusedParam( queues ) )
	{
	}

	ShaderPtr PickingPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( NodeUboIdx::eSkinningUbo )
			, uint32_t( NodeUboIdx::eSkinningSsbo )
			, RenderPipeline::eBuffers
			, flags.programFlags );
		UBO_MORPHING( writer
			, uint32_t( NodeUboIdx::eMorphing )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			,[&] ()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				outSurface.texture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, outSurface.texture );
				outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );
				outSurface.nodeId = c3d_modelData.getNodeId( flags.programFlags
					, inSurface.nodeId );
				outSurface.instance = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				curPosition = mtxModel * curPosition;
				out.vtx.position = c3d_matrixData.worldToCurProj( curPosition );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr PickingPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );

		// UBOs
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTextures )
				, RenderPipeline::eBuffers );
		}

		UBO_PICKING( writer
			, uint32_t( PassUboIdx::eCount )
			, RenderPipeline::eAdditional );

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		shader::Utils utils{ writer, *renderSystem.getEngine() };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = materials->getMaterial( inSurface.material );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				utils.computeOpacityMapContribution( textures
					, textureConfigs
					, c3d_maps
					, inSurface.texture
					, opacity );
				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, material.alphaRef );
				pxl_fragColor = c3d_pickingData.getIndex( inSurface.instance
					, in.primitiveID );

#if C3D_DebugPicking
				pxl_fragColor /= 255.0_f;
#endif
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	void PickingPass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	//*********************************************************************************************
}
