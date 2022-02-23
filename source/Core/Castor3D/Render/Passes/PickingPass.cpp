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
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
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
		template< typename NodeT, typename FuncT >
		inline void traverseNodes( PickingPass & pass
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes
			, PickNodeType type
			, FuncT function )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						if ( !itSubmeshes.second.empty() )
						{
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
			, PickNodeType type
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				count++;
			}
		}
	}

	//*********************************************************************************************

	uint32_t const PickingPass::UboBindingPoint = 7u;
	castor::String const PickingPass::Type = "c3d.pick";

	PickingPass::PickingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, Type
			, cuT( "Picking" )
			, cuT( "Picking" )
			, RenderNodesPassDesc{ { size.getWidth(), size.getHeight(), 1u }, matrixUbo, culler, RenderMode::eBoth, true, false } }
	{
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

	bool PickingPass::doIsValidPass( Pass const & pass )const
	{
		if ( !checkFlag( pass.getPassFlags(), PassFlag::ePickable) )
		{
			return false;
		}

		return RenderNodesPass::doAreValidPassFlags( pass.getPassFlags() );
	}

	void PickingPass::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		auto & myCamera = getCuller().getCamera();
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
			, PickNodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		updateNonInstanced( *this
			, PickNodeType::eBillboard
			, nodes );
	}

	void PickingPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( *this
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
					&& it->second.buffer )
				{
					doCopyNodesIds( renderNodes
						, it->second.data );
				}
			} );
	}

	void PickingPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void PickingPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_UnusedParam( queues ) )
	{
	}

	ShaderPtr PickingPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_Morphing( writer
			, GlobalBuffersIdx::eMorphingData
			, RenderPipeline::ePass
			, flags.programFlags );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, RenderPipeline::ePass
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
			, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto ids = shader::getIds( c3d_objectIdsData
					, in
					, pipelineID
					, customDrawID
					, flags.programFlags );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				out.texture0 = in.texture0;
				auto morphingData = writer.declLocale( "morphingData"
					, c3d_morphingData[ids.morphingId]
					, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
				in.morph( morphingData
					, curPosition
					, out.texture0 );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[ids.nodeId] );
				out.textures0 = modelData.getTextures0();
				out.textures1 = modelData.getTextures1();
				out.textures = modelData.getTextures();
				out.material = modelData.getMaterialId();
				out.nodeId = writer.cast< sdw::Int >( ids.nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getCurModelMtx( flags.programFlags
						, skinningData
						, ids.skinningId
						, in.boneIds0
						, in.boneIds1
						, in.boneWeights0
						, in.boneWeights1 ) );
				curPosition = mtxModel * curPosition;
				out.vtx.position = c3d_matrixData.worldToCurProj( curPosition );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr PickingPass::doGetHullShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetDomainShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		// UBOs
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::ePass };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::ePass
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::ePass
			, hasTextures };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		shader::Utils utils{ writer, *renderSystem.getEngine() };

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
			, FragmentOut out )
			{
				auto material = materials.getMaterial( in.material );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				auto textureFlags = merge( flags.textures );

				if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
				{
					for ( uint32_t index = 0u; index < flags.textures.size(); ++index )
					{
						auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
						auto id = writer.declLocale( "id" + name
							, shader::ModelIndices::getTexture( in.textures0, in.textures1, index ) );

						IF( writer, id > 0_u )
						{
							auto config = writer.declLocale( "config" + name
								, textureConfigs.getTextureConfiguration( id ) );

							IF( writer, config.isOpacity() )
							{
								auto anim = writer.declLocale( "anim" + name
									, textureAnims.getTextureAnimation( id ) );
								auto texCoord = writer.declLocale( "texCoord" + name
									, in.texture0.xy() );
								config.transformUV( anim, texCoord );
								auto sampledOpacity = writer.declLocale< sdw::Vec4 >( "sampled" + name
									, c3d_maps[nonuniform( id - 1_u )].sample( texCoord ) );
								opacity = config.getOpacity( sampledOpacity, opacity );
							}
							FI;
						}
						FI;
					}
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, material.alphaRef );
				pxl_fragColor = vec4( in.nodeId, in.instanceId, in.primitiveID, 0.0_f );

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
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	//*********************************************************************************************
}
