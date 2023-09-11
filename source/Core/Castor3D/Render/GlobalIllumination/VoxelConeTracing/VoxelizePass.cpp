#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, VoxelizePass )

namespace castor3d
{
	//*********************************************************************************************

	namespace vxlzpass
	{
		static RenderNodesPassDesc buildDesc( VoxelSceneData const & voxelConfig
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler
			, bool isStatic )
		{
			RenderNodesPassDesc result{ { voxelConfig.gridSize.value(), voxelConfig.gridSize.value(), 1u }
				, cameraUbo
				, sceneUbo
				, culler
				, RenderFilter::eNone
				, true
				, true };
			result.isStatic( isStatic );
			result.componentModeFlags( ComponentModeFlag::eOcclusion
				| ComponentModeFlag::eColour
				| ComponentModeFlag::eDiffuseLighting
				| ComponentModeFlag::eSpecularLighting
				| ComponentModeFlag::eNormals );
			return result;
		}
	}

	//*********************************************************************************************

	castor::String const VoxelizePass::Type = "c3d.voxelize";

	VoxelizePass::VoxelizePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, Camera const & camera
		, SceneCuller & culler
		, VoxelizerUbo const & voxelizerUbo
		, ashes::Buffer< Voxel > const & voxels
		, VoxelSceneData const & voxelConfig
		, bool isStatic )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, Type
			, {}
			, {}
			, vxlzpass::buildDesc( voxelConfig
				, cameraUbo
				, sceneUbo
				, culler
				, isStatic ) }
		, m_scene{ *camera.getScene() }
		, m_voxels{ voxels }
		, m_voxelizerUbo{ voxelizerUbo }
		, m_voxelConfig{ voxelConfig }
	{
	}

	void VoxelizePass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID
			&& visitor.config.allowProgramsVisit )
		{
			auto flags = visitor.getFlags();
			doUpdateFlags( flags );
			auto shaderProgram = doGetProgram( flags );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_GEOMETRY_BIT ) );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
	}

	void VoxelizePass::update( CpuUpdater & updater )
	{
		if ( m_voxelConfig.enabled )
		{
			auto sceneIt = updater.dirtyScenes.find( &getScene() );

			if ( sceneIt != updater.dirtyScenes.end() )
			{
				auto & sceneObjs = sceneIt->second;
				m_outOfDate = m_outOfDate
					|| !sceneObjs.dirtyLights.empty();

				if ( !m_outOfDate )
				{
					auto it = std::find_if( sceneObjs.dirtyNodes.begin()
						, sceneObjs.dirtyNodes.end()
						, [this]( SceneNode * node )
						{
							return node->isStatic() == filtersNonStatic();
						} );
					m_outOfDate = m_outOfDate
						|| it != sceneObjs.dirtyNodes.end();
				}
			}

			RenderNodesPass::update( updater );
			m_outOfDate = m_outOfDate
				//|| filtersStatic()
				|| getCuller().areAnyChanged();
		}
	}

	bool VoxelizePass::isPassEnabled()const
	{
		return m_voxelConfig.enabled
			&& RenderNodesPass::isPassEnabled()
			&& ( m_renderQueue->isOutOfDate() || m_outOfDate );
	}

	ShaderFlags VoxelizePass::getShaderFlags()const
	{
		return ShaderFlag::eWorldSpace
			| ShaderFlag::eNormal
			| ShaderFlag::eLighting
			| ShaderFlag::eOpacity
			| ShaderFlag::eGeometry
			| ShaderFlag::eColour;
	}

	void VoxelizePass::setUpToDate()
	{
		m_outOfDate = m_renderQueue->isOutOfDate();
	}

	SubmeshFlags VoxelizePass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		remFlag( flags, SubmeshFlag::eTangents );
		return flags;
	}

	ProgramFlags VoxelizePass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags VoxelizePass::doAdjustSceneFlags( SceneFlags flags )const
	{
		remFlag( flags, SceneFlag::eLpvGI );
		remFlag( flags, SceneFlag::eLayeredLpvGI );
		remFlag( flags, SceneFlag::eFogLinear );
		remFlag( flags, SceneFlag::eFogExponential );
		remFlag( flags, SceneFlag::eFogSquaredExponential );
		return flags;
	}

	void VoxelizePass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		doAddShadowBindings( m_scene, flags, bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
	}

	ashes::PipelineDepthStencilStateCreateInfo VoxelizePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_FALSE
			, VK_FALSE
			, VK_COMPARE_OP_GREATER };
	}

	ashes::PipelineColorBlendStateCreateInfo VoxelizePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	void VoxelizePass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_voxelizerUbo.getDescriptorWrite( index++ ) );
		bindBuffer( m_voxels.getBuffer(), descriptorWrites, index );
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
	}

	ShaderPtr VoxelizePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, ComponentModeFlag::eNone
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::VoxelSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< shader::VoxelSurfaceT >{ writer
				, flags }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< shader::VoxelSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags ) );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				out.nodeId = nodeId;
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				material.getPassMultipliers( flags
					, in.passMasks
					, out.passMultipliers );
				auto modelMtx = writer.declLocale< Mat4 >( "modelMtx"
					, modelData.getModelMtx() );

				if ( flags.hasWorldPosInputs() )
				{
					out.vtx.position = curPosition;
					out.normal = curNormal;
				}
				else
				{
					out.vtx.position = ( modelMtx * curPosition );
					out.normal = normalize( mat3( transpose( inverse( modelMtx ) ) ) * curNormal );
				}

				out.worldPosition = out.vtx.position;
				out.viewPosition = c3d_cameraData.worldToCurView( out.vtx.position );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition", 0u );
		auto inTexcoord = writer.declInput< Vec2 >( "inTexcoord", 1u, flags.enableTexcoords() );
		auto inCenter = writer.declInput< Vec3 >( "inCenter", 2u );

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, shader::VoxelSurfaceT >( sdw::VertexInT< VoidT >{ writer }
			, sdw::VertexOutT< shader::VoxelSurfaceT >{ writer
				, flags }
			,[&]( VertexIn in
			, VertexOutT< shader::VoxelSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID ) ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( inCenter, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_cameraData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_cameraData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_cameraData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_cameraData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_cameraData ) );

				out.texture0 = vec3( inTexcoord, 0.0_f );
				out.vtx.position = vec4( curBbcenter
					+ right * inPosition.x() * width
					+ up * inPosition.y() * height
					, 1.0_f );
				out.worldPosition = out.vtx.position;
				auto viewPosition = writer.declLocale( "viewPosition"
					, c3d_cameraData.worldToCurView( out.vtx.position ) );
				out.viewPosition = viewPosition;
				out.normal = normalize( c3d_cameraData.getPosToCamera( curBbcenter ) );
				auto passMultipliers = std::vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f ) };
				out.passMultipliers[0] = passMultipliers[0];
				out.passMultipliers[1] = passMultipliers[1];
				out.passMultipliers[2] = passMultipliers[2];
				out.passMultipliers[3] = passMultipliers[3];
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		GeometryWriter writer;

		C3D_Voxelizer( writer
			, uint32_t( GlobalBuffersIdx::eCount ) + 1u
			, RenderPipeline::eBuffers
			, true );

		writer.implementMainT< TriangleListT< shader::VoxelSurfaceT >, TriangleStreamT< shader::VoxelSurfaceT > >( TriangleListT< shader::VoxelSurfaceT >{ writer
				, flags }
			, TriangleStreamT< shader::VoxelSurfaceT >{ writer
				, 3u
				, flags }
			, [&]( GeometryIn in
				, TriangleListT< shader::VoxelSurfaceT > list
				, TriangleStreamT< shader::VoxelSurfaceT > out )
			{
				auto facenormal = writer.declLocale( "facenormal"
					, abs( list[0].normal + list[1].normal + list[2].normal ) );
				auto maxi = writer.declLocale( "maxi"
					, writer.ternary( facenormal[1] > facenormal[0], 1_u, 0_u ) );
				maxi = writer.ternary( facenormal[2] > facenormal[maxi], 2_u, maxi );
				auto positions = writer.declLocaleArray< Vec3 >( "positions", 3u );

				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
					positions[i] = list[i].worldPosition.xyz() * c3d_voxelData.worldToGrid;

					// Project onto dominant axis:
					IF( writer, maxi == 0_u )
					{
						positions[i] = positions[i].zyx();
					}
					ELSEIF( maxi == 1_u )
					{
						positions[i] = positions[i].xzy();
					}
					FI;

					positions[i].xy() *= c3d_voxelData.gridToClip;
					positions[i].z() = 1.0_f;
				}
				ROF;

				IF( writer, c3d_voxelData.enableConservativeRasterization != 0_u )
				{
					auto side0N = writer.declLocale( "side0N"
						, normalize( positions[1].xy() - positions[0].xy() ) );
					auto side1N = writer.declLocale( "side1N"
						, normalize( positions[2].xy() - positions[1].xy() ) );
					auto side2N = writer.declLocale( "side2N"
						, normalize( positions[0].xy() - positions[2].xy() ) );
					positions[0].xy() += normalize( side2N - side0N ) * c3d_voxelData.gridToClip;
					positions[1].xy() += normalize( side0N - side1N ) * c3d_voxelData.gridToClip;
					positions[2].xy() += normalize( side1N - side2N ) * c3d_voxelData.gridToClip;
				}
				FI;

				// Output
				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
					out.worldPosition = list[i].vtx.position;
					out.viewPosition = list[i].viewPosition;
					out.normal = list[i].normal;
					out.nodeId = list[i].nodeId;
					out.vtx.position = vec4( positions[i], 1.0f );
					out.texture0 = list[i].texture0;
					out.texture1 = list[i].texture1;
					out.texture2 = list[i].texture2;
					out.texture3 = list[i].texture3;
					out.passMultipliers = list[i].passMultipliers;

					out.append();
				}
				ROF;

				out.restartStrip();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		bool enableTextures = flags.enableTextures();
		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		auto addIndex = uint32_t( GlobalBuffersIdx::eCount );

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };
		auto lightsIndex = addIndex++;
		C3D_Voxelizer( writer
			, addIndex++
			, RenderPipeline::eBuffers
			, true );
		auto output( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
			, addIndex++
			, RenderPipeline::eBuffers ) );
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, addIndex /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, false /* enableVolumetric */ };
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, makeExtent2D( m_size )
			, true
			, addIndex
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		writer.implementMainT< shader::VoxelSurfaceT, VoidT >( FragmentInT< shader::VoxelSurfaceT >{ writer
				, FragmentOrigin::eUpperLeft
				, FragmentCenter::eHalfPixel
				, flags }
			, FragmentOutT< VoidT >{ writer }
			, [&]( FragmentInT< shader::VoxelSurfaceT > in
				, FragmentOut out )
			{
				auto diff = writer.declLocale( "diff"
					, c3d_voxelData.worldToClip( in.worldPosition.xyz() ) );
				auto uvw = writer.declLocale( "uvw"
					, diff * vec3( 0.5_f, -0.5f, 0.5f ) + 0.5f );

				IF( writer, utils.isSaturated( uvw ) )
				{
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[in.nodeId - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ in } );
					surface.clipPosition = in.fragCoord.xyz();
					auto components = writer.declLocale( "components"
						, shader::BlendComponents{ materials
							, material
							, surface } );
					materials.blendMaterials( flags
						, textureConfigs
						, textureAnims
						, c3d_maps
						, material
						, modelData.getMaterialId()
						, in.passMultipliers
						, components );
					auto color = writer.declLocale( "color"
						, vec4( components.emissiveColour * components.emissiveFactor, 1.0f ) );
					shader::DebugOutput debugOutput{ getScene().getDebugConfig()
						, cuT( "Default" )
						, c3d_cameraData.debugIndex()
						, color
						, true };

					IF( writer, material.lighting != 0_u )
					{
						auto combined = writer.declLocale( "combined"
							, vec3( 0.0_f ) );
						auto lightSurface = shader::LightSurface::create( writer
							, utils
							, "lightSurface"
							, c3d_cameraData.position()
							, in.worldPosition.xyz()
							, in.worldPosition.xyz()
							, in.fragCoord.xyz()
							, normalize( components.normal )
							, components.f0
							, components
							, true, true, false );
						lights.computeCombinedDif( components
							, *backgroundModel
							, lightSurface
							, modelData.isShadowReceiver()
							, debugOutput
							, combined );
						color.xyz() += vec3( components.occlusion )
							* components.colour
							* combined;
					}
					FI;

					auto encodedColor = writer.declLocale( "encodedColor"
						, utils.encodeColor( vec4( color.xyz(), components.opacity ) ) );
					auto encodedNormal = writer.declLocale( "encodedNormal"
						, utils.encodeNormal( components.normal ) );
					auto writecoord = writer.declLocale( "writecoord"
						, uvec3( floor( uvw * c3d_voxelData.clipToGrid ) ) );
					auto id = writer.declLocale( "id"
						, utils.flatten( writecoord, uvec3( sdw::UInt{ m_voxelConfig.gridSize.value() } ) ) );
					atomicMax( output[id].colorMask, encodedColor );
					atomicMax( output[id].normalMask, encodedNormal );
				}
				FI;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//*********************************************************************************************
}
