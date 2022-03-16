#include "Castor3D/Render/Passes/PickingPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
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
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
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

	void PickingPass::updateArea( VkRect2D const & scissor )
	{
		ShadowMapLightTypeArray shadowMaps;
		m_renderQueue->update( shadowMaps, scissor );
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
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Morphing( writer
			, GlobalBuffersIdx::eMorphingData
			, RenderPipeline::eBuffers
			, flags.programFlags );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
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
					, in.drawID
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
					, c3d_modelsData[ids.nodeId - 1u] );
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

		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::Utils utils{ writer, *renderSystem.getEngine() };

		// UBOs
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< UVec4 >( "pxl_fragColor", 0 ) );

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
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( in.nodeId ) - 1u] );
				auto material = materials.getMaterial( modelData.getMaterialId() );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				auto textureFlags = merge( flags.textures );

				if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
				{
					for ( uint32_t index = 0u; index < flags.textures.size(); ++index )
					{
						auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
						auto id = writer.declLocale( "id" + name
							, shader::ModelIndices::getTexture( modelData.getTextures0()
								, modelData.getTextures1()
								, index ) );

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
									, c3d_maps[id - 1_u].sample( texCoord ) );
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
				pxl_fragColor = uvec4( ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) ? 2_u : 1_u )
					, writer.cast< sdw::UInt >( in.nodeId )
					, writer.cast< sdw::UInt >( in.primitiveID )
					, 0_u );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	PassFlags PickingPass::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags PickingPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		remFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::ePicking );
		return flags;
	}

	ashes::PipelineDepthStencilStateCreateInfo PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}
}
