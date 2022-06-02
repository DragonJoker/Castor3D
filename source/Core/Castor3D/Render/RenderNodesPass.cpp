#include "Castor3D/Render/RenderNodesPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		struct Meshlet
			: public sdw::StructInstance
		{
			Meshlet( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, m_vertices{ getMemberArray< sdw::UVec4 >( "vertices" ) }
				, m_primitives{ getMemberArray< sdw::UVec4 >( "primitives" ) }
				, m_counts{ getMember< sdw::UVec4 >( "counts" ) }
				, vertexCount{ m_counts.x() }
				, primitiveCount{ m_counts.y() }
			{
			}

			SDW_DeclStructInstance( , Meshlet );

			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "C3D_Meshlet" );

				if ( result->empty() )
				{
					result->declMember( "vertices"
						, sdw::type::Kind::eVec4U
						, 64u / 4u );
					result->declMember( "primitives"
						, sdw::type::Kind::eVec4U
						, 124u * 3u / 4u );
					result->declMember( "counts"
						, sdw::type::Kind::eVec4U
						, sdw::type::NotArray );
				}

				return result;
			}

			sdw::UInt getVertexIndex( sdw::UInt const & i )
			{
				return m_vertices[i / 4u][i % 4u];
			}

			sdw::UInt getPrimitiveIndex( sdw::UInt const & i )
			{
				return m_primitives[i / 4u][i % 4u];
			}

		private:
			sdw::Array< sdw::UVec4 > m_vertices;
			sdw::Array< sdw::UVec4 > m_primitives;
			sdw::UVec4 m_counts;

		public:
			sdw::UInt vertexCount;
			sdw::UInt primitiveCount;
		};
	}

	//*********************************************************************************************

	namespace rendndpass
	{
		template< typename PipelineContT >
		static auto findPipeline( PipelineFlags const & flags
			, PipelineContT & pipelines )
		{
			return std::find_if( pipelines.begin()
				, pipelines.end()
				, [&flags]( auto & lookup )
				{
					return lookup->getFlags() == flags;
				} );
		}

		static size_t makeHash( ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			auto nodeType = getRenderNodeType( programFlags );
			return size_t( nodeType )
				| ( size_t( sceneFlags ) << 8u );
		}
	}

	//*********************************************************************************************

	RenderNodesPass::RenderNodesPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, castor::String const & category
		, castor::String const & name
		, RenderNodesPassDesc const & desc )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, castor::Named{ name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); }
				, GetSubpassContentsCallback( [](){ return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS; } )
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return isPassEnabled(); } ) }
			, makeExtent2D( desc.m_size )
			, desc.m_ruConfig }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_matrixUbo{ desc.m_matrixUbo }
		, m_culler{ desc.m_culler }
		, m_typeName{ typeName }
		, m_typeID{ getEngine()->getRenderPassTypeID( m_typeName ) }
		, m_renderQueue{ castor::makeUnique< RenderQueue >( *this, desc.m_mode, desc.m_ignored ) }
		, m_category{ category }
		, m_size{ desc.m_size.width, desc.m_size.height }
		, m_mode{ desc.m_mode }
		, m_oit{ desc.m_oit }
		, m_forceTwoSided{ desc.m_forceTwoSided }
		, m_safeBand{ desc.m_safeBand }
		, m_meshShading{ desc.m_meshShading && device.hasMeshAndTaskShaders() }
		, m_sceneUbo{ m_device }
		, m_index{ desc.m_index }
	{
		m_sceneUbo.setWindowSize( m_size );
		m_culler.registerRenderPass( *this );
	}

	RenderNodesPass::~RenderNodesPass()
	{
		m_renderQueue->cleanup();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderNodesPass::setIgnoredNode( SceneNode const & node )
	{
		m_renderQueue->setIgnoredNode( node );
	}

	void RenderNodesPass::update( CpuUpdater & updater )
	{
		doUpdate( *updater.queues );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	ShaderPtr RenderNodesPass::getTaskShaderSource( PipelineFlags const & flags )const
	{
		return doGetTaskShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getMeshShaderSource( PipelineFlags const & flags )const
	{
		return doGetMeshShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getVertexShaderSource( PipelineFlags const & flags )const
	{
		ShaderPtr result;

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			result = doGetBillboardShaderSource( flags );
		}
		else
		{
			result = doGetVertexShaderSource( flags );
		}

		return result;
	}

	ShaderPtr RenderNodesPass::getHullShaderSource( PipelineFlags const & flags )const
	{
		return doGetHullShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getDomainShaderSource( PipelineFlags const & flags )const
	{
		return doGetDomainShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	SubmeshFlags RenderNodesPass::adjustFlags( SubmeshFlags flags )const
	{
		return doAdjustSubmeshFlags( flags );
	}

	PassFlags RenderNodesPass::adjustFlags( PassFlags flags )const
	{
		return doAdjustPassFlags( flags );
	}

	ProgramFlags RenderNodesPass::adjustFlags( ProgramFlags flags )const
	{
		if ( !m_meshShading )
		{
			remFlag( flags, ProgramFlag::eHasMesh );
		}
		else
		{
			remFlag( flags, ProgramFlag::eInstantiation );
		}

		return doAdjustProgramFlags( flags );
	}

	SceneFlags RenderNodesPass::adjustFlags( SceneFlags flags )const
	{
		return doAdjustSceneFlags( flags );
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, PassFlags passFlags
		, RenderPassTypeID renderPassTypeID
		, PassTypeID passTypeID
		, uint32_t heightTextureIndex
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureFlagsArray const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )
	{
		auto result = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, renderPassTypeID
			, passTypeID
			, heightTextureIndex
			, submeshFlags
			, programFlags
			, sceneFlags
			, topology
			, 3u
			, alphaFunc
			, blendAlphaFunc
			, textures
			, {}
			, morphTargets.getOffset() };

		if ( isFrontCulled )
		{
			addFlag( result.programFlags, ProgramFlag::eInvertNormals );
		}

		doUpdateFlags( result );
		return result;
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( Pass const & pass
		, TextureFlagsArray const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )
	{
		return createPipelineFlags( pass.getColourBlendMode()
			, pass.getAlphaBlendMode()
			, pass.getPassFlags()
			, ( pass.getRenderPassInfo()
				? pass.getRenderPassInfo()->id
				: RenderPassTypeID{} )
			, pass.getTypeID()
			, pass.getHeightTextureIndex()
			, pass.getAlphaFunc()
			, pass.getBlendAlphaFunc()
			, textures
			, submeshFlags
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, morphTargets );
	}

	RenderPipeline & RenderNodesPass::prepareBackPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, std::move( pipelineFlags )
			, VK_CULL_MODE_BACK_BIT );
	}

	RenderPipeline & RenderNodesPass::prepareFrontPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, std::move( pipelineFlags )
			, VK_CULL_MODE_FRONT_BIT );
	}

	FilteredTextureFlags RenderNodesPass::filterTexturesFlags( TextureFlagsArray const & textures )const
	{
		return castor3d::filterTexturesFlags( textures
			, getTexturesMask() );
	}

	ashes::PipelineColorBlendStateCreateInfo RenderNodesPass::createBlendState( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, uint32_t attachesCount )
	{
		VkPipelineColorBlendAttachmentState attach{};

		switch ( colourBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		default:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;
		}

		switch ( alphaBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.alphaBlendOp = VK_BLEND_OP_ADD;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = VK_TRUE;
			attach.alphaBlendOp = VK_BLEND_OP_ADD;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = VK_TRUE;
			attach.alphaBlendOp = VK_BLEND_OP_ADD;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = VK_TRUE;
			attach.alphaBlendOp = VK_BLEND_OP_ADD;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		default:
			attach.blendEnable = VK_TRUE;
			attach.alphaBlendOp = VK_BLEND_OP_ADD;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;
		}

		attach.colorWriteMask = defaultColorWriteMask;

		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			ashes::VkPipelineColorBlendAttachmentStateArray{ size_t( attachesCount ), attach },
		};
	}

	TextureFlags RenderNodesPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	bool RenderNodesPass::isValidPass( Pass const & pass )const
	{
		return doIsValidPass( pass );
	}

	bool RenderNodesPass::isValidRenderable( RenderedObject const & object )const
	{
		return doIsValidRenderable( object );
	}

	bool RenderNodesPass::hasNodes()const
	{
		return m_renderQueue->hasNodes();
	}

	bool RenderNodesPass::isPassEnabled()const
	{
		return hasNodes();
	}

	Scene & RenderNodesPass::getScene()const
	{
		return getCuller().getScene();
	}

	SceneNode const * RenderNodesPass::getIgnoredNode()const
	{
		return m_renderQueue->getIgnoredNode();
	}

	void RenderNodesPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ShadowMapLightTypeArray const & shadowMaps
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )
	{
		auto programFlags = pipeline.getFlags().programFlags;
		programFlags = doAdjustProgramFlags( programFlags );
		auto sceneFlags = doAdjustSceneFlags( pipeline.getFlags().sceneFlags );
		auto descLayoutIt = m_additionalDescriptors.emplace( rendndpass::makeHash( programFlags, sceneFlags )
			, PassDescriptors{} ).first;
		auto & descriptors = descLayoutIt->second;

		if ( !descriptors.set )
		{
			auto & scene = getCuller().getScene();
			descriptors.set = descriptors.pool->createDescriptorSet( getName() + "_Add"
				, RenderPipeline::eBuffers );
			ashes::WriteDescriptorSetArray descriptorWrites;
			descriptorWrites.push_back( m_matrixUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eMatrix ) ) );
			descriptorWrites.push_back( m_sceneUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eScene ) ) );

			auto & nodesIds = getCuller().getNodesIds( *this );
			auto nodesIdsWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eObjectsNodeID )
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			nodesIdsWrite.bufferInfo.push_back( { nodesIds.getBuffer()
				, 0u
				, nodesIds.getBuffer().getSize() } );
			descriptorWrites.push_back( nodesIdsWrite );

			auto & modelBuffer = scene.getModelBuffer();
			auto modelDataWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eModelsData )
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			modelDataWrite.bufferInfo.push_back( { modelBuffer.getBuffer()
				, 0u
				, modelBuffer.getBuffer().getSize() } );
			descriptorWrites.push_back( modelDataWrite );
			auto & matCache = getOwner()->getMaterialCache();
			descriptorWrites.push_back( matCache.getPassBuffer().getBinding( uint32_t( GlobalBuffersIdx::eMaterials ) ) );
			descriptorWrites.push_back( matCache.getSssProfileBuffer().getBinding( uint32_t( GlobalBuffersIdx::eSssProfiles ) ) );
			descriptorWrites.push_back( matCache.getTexConfigBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexConfigs ) ) );
			descriptorWrites.push_back( matCache.getTexAnimBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexAnims ) ) );

			if ( checkFlag( programFlags, ProgramFlag::eBillboards ) )
			{
				auto & billboardDatas = scene.getBillboardsBuffer();
				auto write = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eBillboardsData )
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				write.bufferInfo.push_back( { billboardDatas.getBuffer()
					, 0u
					, billboardDatas.getBuffer().getSize() } );
				descriptorWrites.push_back( write );
			}

			doFillAdditionalDescriptor( descriptorWrites
				, shadowMaps );
			descriptors.set->setBindings( descriptorWrites );
			descriptors.set->update();
		}

		pipeline.setAdditionalDescriptorSet( *descriptors.set );
	}

	void RenderNodesPass::doSubInitialise()
	{
		m_renderQueue->initialise();
	}

	void RenderNodesPass::doSubRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkCommandBuffer secondary = m_renderQueue->initCommandBuffer();
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
	}

	void RenderNodesPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( *m_renderQueue );
	}

	void RenderNodesPass::doUpdateUbos( CpuUpdater & updater )
	{
		auto & camera = *updater.camera;
		auto jitterProjSpace = updater.jitter * 2.0f;
		jitterProjSpace[0] /= float( camera.getWidth() );
		jitterProjSpace[1] /= float( camera.getHeight() );
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection( m_safeBand != 0u )
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( *camera.getScene(), &camera );
	}

	bool RenderNodesPass::doIsValidPass( Pass const & pass )const
	{
		return ( pass.getRenderPassInfo() == nullptr || pass.getRenderPassInfo()->create == nullptr )
			&& doAreValidPassFlags( pass.getPassFlags() );
	}

	bool RenderNodesPass::doIsValidRenderable( RenderedObject const & object )const
	{
		return true;
	}

	SubmeshFlags RenderNodesPass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		return flags;
	}

	PassFlags RenderNodesPass::doAdjustPassFlags( PassFlags flags )const
	{
		return flags;
	}

	ProgramFlags RenderNodesPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags RenderNodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return flags;
	}

	bool RenderNodesPass::doAreValidPassFlags( PassFlags const & passFlags )const
	{
		if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) )
		{
			if ( checkFlag( passFlags, PassFlag::eAlphaTest ) )
			{
				return true;
			}

			return m_mode == RenderMode::eBoth
				|| m_mode == RenderMode::eTransparentOnly;
		}

		return m_mode == RenderMode::eBoth
			|| m_mode == RenderMode::eOpaqueOnly;
	}

	ShaderProgramSPtr RenderNodesPass::doGetProgram( PipelineFlags const & flags
		, VkCullModeFlags cullMode )
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderNodesPass::doCreateAdditionalBindings( PipelineFlags const & flags )const
	{
		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		if ( checkFlag( flags.programFlags, ProgramFlag::eHasMesh ) )
		{
			stageFlags |= VK_SHADER_STAGE_MESH_BIT_NV;

			if ( checkFlag( flags.programFlags, ProgramFlag::eHasTask ) )
			{
				stageFlags |= VK_SHADER_STAGE_TASK_BIT_NV;
			}
		}
		else
		{
			stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;

			if ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry ) )
			{
				stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eHasTessellation ) )
			{
				stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}
		}

		ashes::VkDescriptorSetLayoutBindingArray addBindings;
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eScene )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eObjectsNodeID )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eModelsData )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		auto & matCache = getOwner()->getMaterialCache();
		addBindings.emplace_back( matCache.getPassBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eMaterials ) ) );
		addBindings.emplace_back( matCache.getSssProfileBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eSssProfiles ) ) );
		addBindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexConfigs ) ) );
		addBindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexAnims ) ) );

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eBillboardsData )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stageFlags ) );
		}

		doFillAdditionalBindings( addBindings );
		return addBindings;
	}

	std::vector< RenderPipelineUPtr > & RenderNodesPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > & RenderNodesPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	std::vector< RenderPipelineUPtr > const & RenderNodesPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > const & RenderNodesPass::doGetBackPipelines()const
	{
		return m_backPipelines;
	}

	RenderPipeline & RenderNodesPass::doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout
		, PipelineFlags flags
		, VkCullModeFlags cullMode )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		RenderPipeline * result{};
		auto program = doGetProgram( flags, cullMode );

		if ( doAreValidPassFlags( flags.passFlags )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			auto & pipelines = castor::checkFlag( cullMode, VK_CULL_MODE_FRONT_BIT )
				? doGetFrontPipelines()
				: doGetBackPipelines();
			auto it = rendndpass::findPipeline( flags, pipelines );

			if ( it == pipelines.end() )
			{
				auto pipeline = castor::makeUnique< RenderPipeline >( *this
					, renderSystem
					, doCreateDepthStencilState( flags )
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, cullMode }
					, doCreateBlendState( flags )
					, ashes::PipelineMultisampleStateCreateInfo{}
					, program
					, flags );
				pipeline->setViewport( makeViewport( m_size ) );

				if ( !checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
				{
					pipeline->setScissor( makeScissor( m_size ) );
				}

				auto addDescLayoutIt = m_additionalDescriptors.emplace( rendndpass::makeHash( flags.programFlags, flags.sceneFlags )
					, PassDescriptors{} ).first;
				auto & addDescriptors = addDescLayoutIt->second;

				if ( !addDescriptors.layout )
				{
					auto bindings = doCreateAdditionalBindings( flags );
					addDescriptors.layout = device->createDescriptorSetLayout( getName() + "Add"
						, std::move( bindings ) );
					addDescriptors.pool = addDescriptors.layout->createPool( 1u );
				}

				pipeline->setAdditionalDescriptorSetLayout( *addDescriptors.layout );

				if ( checkFlag( flags.programFlags, ProgramFlag::eHasMesh )
					&& meshletDescriptorLayout )
				{
					pipeline->setMeshletDescriptorSetLayout( *meshletDescriptorLayout );
					pipeline->setPushConstantRanges( { { VK_SHADER_STAGE_MESH_BIT_NV, 0u, 4u } } );
				}
				else
				{
					pipeline->setVertexLayouts( vertexLayouts );
					pipeline->setPushConstantRanges( { { VK_SHADER_STAGE_VERTEX_BIT, 0u, 4u } } );
				}

				pipeline->initialise( device
					, getRenderPass() );
				pipelines.emplace_back( std::move( pipeline ) );
				it = std::next( pipelines.begin()
					, ptrdiff_t( pipelines.size() - 1u ) );
			}

			result = it->get();
		}

		return *result;
	}

	void RenderNodesPass::doUpdateFlags( PipelineFlags & flags )const
	{
		if ( m_mode != RenderMode::eTransparentOnly )
		{
			flags.alphaBlendMode = BlendMode::eNoBlend;
		}

		flags.submeshFlags = adjustFlags( flags.submeshFlags );
		flags.programFlags = adjustFlags( flags.programFlags );
		flags.passFlags = adjustFlags( flags.passFlags );
		flags.sceneFlags = adjustFlags( flags.sceneFlags );
		auto textureFlags = filterTexturesFlags( flags.textures );

		if ( textureFlags.empty()
			&& !checkFlag( flags.programFlags, ProgramFlag::eForceTexCoords ) )
		{
			remFlag( flags.submeshFlags, SubmeshFlag::eTexcoords0 );
			remFlag( flags.submeshFlags, SubmeshFlag::eTexcoords1 );
			remFlag( flags.submeshFlags, SubmeshFlag::eTexcoords2 );
			remFlag( flags.submeshFlags, SubmeshFlag::eTexcoords3 );
		}

		doAdjustFlags( flags );
	}

	void RenderNodesPass::doAdjustFlags( PipelineFlags & flags )const
	{
	}

	ShaderPtr RenderNodesPass::doGetTaskShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr RenderNodesPass::doGetMeshShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		MeshWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

#define DeclareSsbo( Name, Type, Binding, Enable )\
		sdw::Ssbo Name##Buffer{ writer\
			, #Name + std::string{ "Buffer" }\
			, uint32_t( Binding )\
			, RenderPipeline::eMeshBuffers\
			, ast::type::MemoryLayout::eStd430\
			, Enable };\
		auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
		Name##Buffer.end()

		// Inputs
		auto c3d_meshlets = writer.declArrayStorageBuffer< shader::Meshlet >( "c3d_meshlets"
			, uint32_t( MeshBuffersIdx::eMeshlets )
			, RenderPipeline::eMeshBuffers );
		DeclareSsbo( c3d_position
			, sdw::Vec4
			, MeshBuffersIdx::ePosition
			, checkFlag( flags.submeshFlags, SubmeshFlag::ePositions ) );
		DeclareSsbo( c3d_normal
			, sdw::Vec4
			, MeshBuffersIdx::eNormal
			, checkFlag( flags.submeshFlags, SubmeshFlag::eNormals ) );
		DeclareSsbo( c3d_tangent
			, sdw::Vec4
			, MeshBuffersIdx::eTangent
			, checkFlag( flags.submeshFlags, SubmeshFlag::eTangents ) );
		DeclareSsbo( c3d_texcoord0
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord0
			, checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords0 ) );
		DeclareSsbo( c3d_texcoord1
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord1
			, checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords1 ) );
		DeclareSsbo( c3d_texcoord2
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord2
			, checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords2 ) );
		DeclareSsbo( c3d_texcoord3
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord3
			, checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords3 ) );
		DeclareSsbo( c3d_colour
			, sdw::Vec4
			, MeshBuffersIdx::eColour
			, checkFlag( flags.submeshFlags, SubmeshFlag::eColours ) );
		DeclareSsbo( c3d_velocity
			, sdw::Vec4
			, MeshBuffersIdx::eVelocity
			, checkFlag( flags.submeshFlags, SubmeshFlag::eVelocity ) );

#undef DeclareSsbo

		writer.implementMainT< VoidT, shader::FragmentSurfaceT, VoidT >( 32u
			, sdw::TaskPayloadIn{ writer }
			, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer
				, MaxMeshletVertexCount
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::TrianglesMeshPrimitiveListOutT< VoidT >{ writer
				, MaxMeshletTriangleCount }
			, [&]( sdw::MeshIn in
				, sdw::TaskPayloadInT< VoidT > payload
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > vtxOut
				, sdw::TrianglesMeshPrimitiveListOutT< VoidT > primOut )
			{
				auto ti = writer.declLocale( "ti"
					, in.localInvocationID );
				auto meshletIndex = writer.declLocale( "meshletIndex"
					, in.workGroupID );
				auto meshlet = writer.declLocale( "meshlet"
					, c3d_meshlets[meshletIndex] );
				auto indexCount = writer.declLocale( "indexCount"
					, meshlet.primitiveCount );
				auto vertexCount = writer.declLocale( "vertexCount"
					, meshlet.vertexCount );

				primOut.setMeshOutputCounts( vertexCount, indexCount );

				FOR( writer, sdw::UInt, i, ti, i < indexCount, i += 32u )
				{
					primOut[i].primitiveIndex = uvec3( meshlet.getPrimitiveIndex( i * 3u + 0u )
						, meshlet.getPrimitiveIndex( i * 3u + 1u )
						, meshlet.getPrimitiveIndex( i * 3u + 2u ) );
				}
				ROF;

				FOR( writer, sdw::UInt, i, ti, i < vertexCount, i += 32u )
				{
					auto vertexIndex = writer.declLocale( "vertexIndex", meshlet.getVertexIndex( i ) );

					auto nodeId = shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, in.drawID );
					auto curPosition = writer.declLocale( "curPosition"
						, c3d_position[vertexIndex] );
					auto curNormal = writer.declLocale( "curNormal"
						, c3d_normal[vertexIndex].xyz() );
					auto curTangent = writer.declLocale( "curTangent"
						, c3d_tangent[vertexIndex].xyz() );
					vtxOut[i].texture0 = c3d_texcoord0[vertexIndex].xyz();
					vtxOut[i].texture1 = c3d_texcoord1[vertexIndex].xyz();
					vtxOut[i].texture2 = c3d_texcoord2[vertexIndex].xyz();
					vtxOut[i].texture3 = c3d_texcoord3[vertexIndex].xyz();
					vtxOut[i].colour = c3d_colour[vertexIndex].xyz();
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					vtxOut[i].nodeId = writer.cast< Int >( nodeId );

					auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
						, modelData.getModelMtx() );
					auto prvPosition = writer.declLocale( "prvPosition"
						, curPosition );
					prvPosition.xyz() += c3d_velocity[vertexIndex].xyz();

					if ( checkFlag( flags.submeshFlags, SubmeshFlag::eVelocity ) )
					{
						auto worldPos = writer.declLocale( "worldPos"
							, curPosition );
						vtxOut[i].computeTangentSpace( flags.submeshFlags
							, flags.programFlags
							, c3d_sceneData.cameraPosition
							, worldPos.xyz()
							, curNormal
							, curTangent );
					}
					else
					{
						auto prvMtxModel = writer.declLocale( "prvMtxModel"
							, modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
						prvPosition = c3d_matrixData.worldToPrvProj( prvMtxModel * prvPosition );
						auto worldPos = writer.declLocale( "worldPos"
							, curMtxModel * curPosition );
						auto mtxNormal = writer.declLocale( "mtxNormal"
							, modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
						vtxOut[i].computeTangentSpace( flags.submeshFlags
							, flags.programFlags
							, c3d_sceneData.cameraPosition
							, worldPos.xyz()
							, mtxNormal
							, curNormal
							, curTangent );
					}

					auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
					vtxOut[i].worldPosition = worldPos;
					vtxOut[i].viewPosition = c3d_matrixData.worldToCurView( worldPos );
					curPosition = c3d_matrixData.worldToCurProj( worldPos );
					vtxOut[i].computeVelocity( c3d_matrixData
						, curPosition
						, prvPosition );
					vtxOut[i].position = curPosition;
				}
				ROF;
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = shader::getNodeId( c3d_objectIdsData
					, in
					, pipelineID
					, in.drawID
					, flags.programFlags );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto curTangent = writer.declLocale( "curTangent"
					, in.tangent );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				out.colour = in.colour;
				out.nodeId = writer.cast< Int >( nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += in.velocity;

				if ( checkFlag( flags.submeshFlags, SubmeshFlag::eVelocity ) )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, c3d_sceneData.cameraPosition
						, worldPos.xyz()
						, curNormal
						, curTangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
					prvPosition = c3d_matrixData.worldToPrvProj( prvMtxModel * prvPosition );
					auto worldPos = writer.declLocale( "worldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, c3d_sceneData.cameraPosition
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				out.viewPosition = c3d_matrixData.worldToCurView( worldPos );
				curPosition = c3d_matrixData.worldToCurProj( worldPos );
				out.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u, hasTextures );
		auto center = writer.declInput< Vec3 >( "center", 2u );

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::eBuffers );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, shader::FragmentSurfaceT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< VoidT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					,  shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, in.drawID ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( flags.programFlags, c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( flags.programFlags, c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( flags.programFlags, c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( flags.programFlags, c3d_sceneData ) );
				auto scaledRight = writer.declLocale( "scaledRight"
					, right * position.x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * position.y() * height );
				auto worldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				out.worldPosition = worldPos;

				if ( hasTextures )
				{
					out.texture0 = vec3( uv, 0.0_f );
				}

				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_matrixData.worldToPrvProj( vec4( prvBbcenter + scaledRight + scaledUp, 1.0_f ) ) );
				auto curPosition = writer.declLocale( "curPosition"
					, c3d_matrixData.worldToCurProj( worldPos ) );
				out.viewPosition = c3d_matrixData.worldToCurView( worldPos );
				out.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				out.computeTangentSpace( flags.submeshFlags
					, c3d_sceneData.cameraPosition
					, worldPos.xyz()
					, curToCamera
					, up
					, right );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
