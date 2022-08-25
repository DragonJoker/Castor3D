#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementCUSmartPtr( castor3d, VoxelizePass )

namespace castor3d
{
	//*********************************************************************************************

	namespace vxlpass
	{
		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
				, viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
				, normal{ getMember< sdw::Vec3 >( "normal" ) }
				, texture0{ getMember< sdw::Vec3 >( "texcoord0" ) }
				, texture1{ getMember< sdw::Vec3 >( "texcoord1" ) }
				, texture2{ getMember< sdw::Vec3 >( "texcoord2" ) }
				, texture3{ getMember< sdw::Vec3 >( "texcoord3" ) }
				, colour{ getMember< sdw::Vec3 >( "colour" ) }
				, nodeId{ getMember< sdw::UInt >( "nodeId" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
				, PipelineFlags const & flags )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "VoxelSurface"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "worldPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "viewPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "normal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "texcoord0"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "texcoord1"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( flags.enableTexcoord1() ? index++ : 0 )
						, flags.enableTexcoord1() );
					result->declMember( "texcoord2"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( flags.enableTexcoord2() ? index++ : 0 )
						, flags.enableTexcoord2() );
					result->declMember( "texcoord3"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( flags.enableTexcoord3() ? index++ : 0 )
						, flags.enableTexcoord3() );
					result->declMember( "colour"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( flags.enableColours() ? index++ : 0 )
						, flags.enableColours() );
					result->declMember( "nodeId"
						, sdw::type::Kind::eUInt
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			sdw::Vec3 worldPosition;
			sdw::Vec3 viewPosition;
			sdw::Vec3 normal;
			sdw::Vec3 texture0;
			sdw::Vec3 texture1;
			sdw::Vec3 texture2;
			sdw::Vec3 texture3;
			sdw::Vec3 colour;
			sdw::UInt nodeId;
		};
	}

	//*********************************************************************************************

	castor::String const VoxelizePass::Type = "c3d.voxelize";

	VoxelizePass::VoxelizePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, SceneCuller & culler
		, VoxelizerUbo const & voxelizerUbo
		, ashes::Buffer< Voxel > const & voxels
		, VoxelSceneData const & voxelConfig )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, Type
			, nullptr
			, RenderNodesPassDesc{ { voxelConfig.gridSize.value(), voxelConfig.gridSize.value(), 1u }, matrixUbo, sceneUbo, culler, RenderFilter::eNone, true, true } }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.getCamera() }
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
			getCuller().update( updater );
			RenderNodesPass::update( updater );
		}
	}

	SubmeshFlags VoxelizePass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		remFlag( flags, SubmeshFlag::eTangents );
		return flags;
	}

	PassFlags VoxelizePass::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags, PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags, PassFlag::eDistanceBasedTransmittance );
		remFlag( flags, PassFlag::eSubsurfaceScattering );
		remFlag( flags, PassFlag::eAlphaBlending );
		remFlag( flags, PassFlag::eAlphaTest );
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

	void VoxelizePass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto sceneFlags = doAdjustSceneFlags( m_scene.getFlags() );
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			if ( checkFlag( sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << j ) ) )
			{
				// Depth
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				// Variance
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
		}
	}

	ashes::PipelineDepthStencilStateCreateInfo VoxelizePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_FALSE,
		};
	}

	ashes::PipelineColorBlendStateCreateInfo VoxelizePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	namespace
	{
		ashes::WriteDescriptorSet getDescriptorWrite( ashes::Buffer< Voxel > const & voxels
			, uint32_t dstBinding )
		{
			auto & buffer = voxels.getBuffer();
			auto result = ashes::WriteDescriptorSet{ dstBinding
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			result.bufferInfo.push_back( { buffer
				, 0u
				, buffer.getSize() } );
			return result;
		}
	}

	void VoxelizePass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto sceneFlags = doAdjustSceneFlags( m_scene.getFlags() );
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_voxelizerUbo.getDescriptorWrite( index++ ) );
		descriptorWrites.push_back( getDescriptorWrite( m_voxels, index++ ) );
		bindShadowMaps( m_graph
			, sceneFlags
			, shadowMaps
			, descriptorWrites
			, index );
	}

	ShaderPtr VoxelizePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, vxlpass::SurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< vxlpass::SurfaceT >{ writer
				, flags }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< vxlpass::SurfaceT > out )
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
				out.nodeId = writer.cast< sdw::Int >( nodeId );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
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

				out.viewPosition = c3d_matrixData.worldToCurView( out.vtx.position ).xyz();
				out.worldPosition = out.viewPosition;
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

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
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

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, vxlpass::SurfaceT >( sdw::VertexInT< VoidT >{ writer }
			, sdw::VertexOutT< vxlpass::SurfaceT >{ writer
				, flags }
			,[&]( VertexIn in
			, VertexOutT< vxlpass::SurfaceT > out )
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
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_sceneData ) );

				if ( flags.enableTextures() )
				{
					out.texture0 = vec3( inTexcoord, 0.0_f );
				}

				out.vtx.position = vec4( curBbcenter
					+ right * inPosition.x() * width
					+ up * inPosition.y() * height
					, 1.0_f );
				out.worldPosition = out.vtx.position.xyz();
				auto viewPosition = writer.declLocale( "viewPosition"
					, c3d_matrixData.worldToCurView( out.vtx.position ) );
				out.viewPosition = viewPosition.xyz();
				out.normal = normalize( c3d_sceneData.getPosToCamera( curBbcenter ) );
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

		writer.implementMainT< TriangleListT< vxlpass::SurfaceT >, TriangleStreamT< vxlpass::SurfaceT > >( TriangleListT< vxlpass::SurfaceT >{ writer
				, flags }
			, TriangleStreamT< vxlpass::SurfaceT >{ writer
				, 3u
				, flags }
			, [&]( GeometryIn in
				, TriangleListT< vxlpass::SurfaceT > list
				, TriangleStreamT< vxlpass::SurfaceT > out )
			{
				auto facenormal = writer.declLocale( "facenormal"
					, abs( list[0].normal + list[1].normal + list[2].normal ) );
				auto maxi = writer.declLocale( "maxi"
					, writer.ternary( facenormal[1] > facenormal[0], 1_u, 0_u ) );
				maxi = writer.ternary( facenormal[2] > facenormal[maxi], 2_u, maxi );
				auto positions = writer.declLocaleArray< Vec3 >( "positions", 3u );

				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
					positions[i] = list[i].worldPosition * c3d_voxelData.worldToGrid;

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
					out.worldPosition = list[i].vtx.position.xyz();
					out.viewPosition = list[i].viewPosition;
					out.normal = list[i].normal;
					out.nodeId = list[i].nodeId;
					out.vtx.position = vec4( positions[i], 1.0f );
					out.texture0 = list[i].texture0;
					out.texture1 = list[i].texture1;
					out.texture2 = list[i].texture2;
					out.texture3 = list[i].texture3;

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

		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
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
		auto addIndex = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = addIndex++;
		C3D_Voxelizer( writer
			, addIndex++
			, RenderPipeline::eBuffers
			, true );
		auto output( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
			, addIndex++
			, RenderPipeline::eBuffers ) );
		auto lightingModel = shader::LightingModel::createDiffuseModel( *getEngine()
			, utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, addIndex
			, RenderPipeline::eBuffers
			, false );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		writer.implementMainT< vxlpass::SurfaceT, VoidT >( FragmentInT< vxlpass::SurfaceT >{ writer
				, FragmentOrigin::eUpperLeft
				, FragmentCenter::eHalfPixel
				, flags }
			, FragmentOutT< VoidT >{ writer }
			, [&]( FragmentInT< vxlpass::SurfaceT > in
				, FragmentOut out )
			{
				auto diff = writer.declLocale( "diff"
					, c3d_voxelData.worldToClip( in.worldPosition ) );
				auto uvw = writer.declLocale( "uvw"
					, diff * vec3( 0.5_f, -0.5f, 0.5f ) + 0.5f );

				IF( writer, utils.isSaturated( uvw ) )
				{
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[in.nodeId - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );
					auto normal = writer.declLocale( "normal"
						, normalize( in.normal ) );
					auto lightMat = lightingModel->declMaterial( "lightMat" );
					lightMat->create( in.colour
						, material );
					auto emissive = writer.declLocale( "emissive"
						, vec3( material.emissive() ) );
					auto alpha = writer.declLocale( "alpha"
						, material.opacity() );
					auto occlusion = writer.declLocale( "occlusion"
						, 1.0_f );

					if ( c3d_maps.isEnabled() )
					{
						auto texCoord0 = writer.declLocale( "texCoord0"
							, in.texture0 );
						auto texCoord1 = writer.declLocale( "texCoord1"
							, in.texture1 );
						auto texCoord2 = writer.declLocale( "texCoord2"
							, in.texture2 );
						auto texCoord3 = writer.declLocale( "texCoord3"
							, in.texture3 );
						lightingModel->computeMapDiffuseContributions( flags
							, textureConfigs
							, textureAnims
							, c3d_maps
							, material
							, texCoord0
							, texCoord1
							, texCoord2
							, texCoord3
							, emissive
							, alpha
							, occlusion
							, *lightMat );
					}

					auto color = writer.declLocale( "color"
						, emissive );

					IF( writer, material.lighting() != 0_u )
					{
						auto worldEye = writer.declLocale( "worldEye"
							, c3d_sceneData.cameraPosition );
						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( in.fragCoord.xyz()
							, in.viewPosition
							, in.worldPosition
							, normal );
						color += occlusion
							* lightMat->albedo
							* lightingModel->computeCombinedDiffuse( *lightMat
								, c3d_sceneData
								, surface
								, worldEye
								, modelData.isShadowReceiver() );
					}
					FI;

					auto encodedColor = writer.declLocale( "encodedColor"
						, utils.encodeColor( vec4( color, alpha ) ) );
					auto encodedNormal = writer.declLocale( "encodedNormal"
						, utils.encodeNormal( normal ) );
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
