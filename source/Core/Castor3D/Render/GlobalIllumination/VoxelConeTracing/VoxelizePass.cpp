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
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
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
				, nodeId{ getMember< sdw::Int >( "nodeId" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
				, SubmeshFlags submeshFlags )
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
						, ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) ? index++ : 0u )
						, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) );
					result->declMember( "texcoord2"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) ? index++ : 0u )
						, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) );
					result->declMember( "texcoord3"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) ? index++ : 0u )
						, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) );
					result->declMember( "colour"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, ( checkFlag( submeshFlags, SubmeshFlag::eColours ) ? index++ : 0u )
						, checkFlag( submeshFlags, SubmeshFlag::eColours ) );
					result->declMember( "nodeId"
						, sdw::type::Kind::eInt
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
			sdw::Int nodeId;
		};
	}

	//*********************************************************************************************

	castor::String const VoxelizePass::Type = "c3d.voxelize";

	VoxelizePass::VoxelizePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, VoxelizerUbo const & voxelizerUbo
		, ashes::Buffer< Voxel > const & voxels
		, VoxelSceneData const & voxelConfig )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, Type
			, "Voxelize"
			, "Voxelization"
			, RenderNodesPassDesc{ { voxelConfig.gridSize.value(), voxelConfig.gridSize.value(), 1u }, matrixUbo, culler, RenderMode::eBoth, true, true } }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.getCamera() }
		, m_voxels{ voxels }
		, m_voxelizerUbo{ voxelizerUbo }
		, m_voxelConfig{ voxelConfig }
	{
	}

	void VoxelizePass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
			auto flags = visitor.getFlags();
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
			static const castor::Matrix4x4f identity{ []()
				{
					castor::Matrix4x4f res;
					res.setIdentity();
					return res;
				}() };
			//Orthograhic projection
			auto sceneBoundingBox = m_scene.getBoundingBox();
			auto ortho = castor::matrix::ortho( sceneBoundingBox.getMin()->x
				, sceneBoundingBox.getMax()->x
				, sceneBoundingBox.getMin()->y
				, sceneBoundingBox.getMax()->y
				, -1.0f * sceneBoundingBox.getMin()->z
				, -1.0f * sceneBoundingBox.getMax()->z );
			auto jitterProjSpace = updater.jitter * 2.0f;
			jitterProjSpace[0] /= float( m_camera.getWidth() );
			jitterProjSpace[1] /= float( m_camera.getHeight() );
			m_matrixUbo.cpuUpdate( identity
				, ortho
				, jitterProjSpace );
		}
	}

	SubmeshFlags VoxelizePass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		remFlag( flags, SubmeshFlag::eTangents );
		return flags;
	}

	PassFlags VoxelizePass::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eReflection );
		remFlag( flags, PassFlag::eRefraction );
		remFlag( flags, PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags, PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags, PassFlag::eDistanceBasedTransmittance );
		remFlag( flags, PassFlag::eSubsurfaceScattering );
		remFlag( flags, PassFlag::eAlphaBlending );
		remFlag( flags, PassFlag::eAlphaTest );
		remFlag( flags, PassFlag::eBlendAlphaTest );
		return flags;
	}

	ProgramFlags VoxelizePass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		addFlag( flags, ProgramFlag::eHasGeometry );
		addFlag( flags, ProgramFlag::eLighting );
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
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_MorphTargets( writer
			, GlobalBuffersIdx::eMorphTargets
			, RenderPipeline::eBuffers
			, flags.morphFlags
			, flags.programFlags );
		C3D_MorphingWeights( writer
			, GlobalBuffersIdx::eMorphingWeights
			, RenderPipeline::eBuffers
			, flags.programFlags );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, vxlpass::SurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< vxlpass::SurfaceT >{ writer
				, flags.submeshFlags }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< vxlpass::SurfaceT > out )
			{
				auto ids = shader::getIds( c3d_objectIdsData
					, in
					, pipelineID
					, in.drawID
					, flags.programFlags );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( in.normal, 0.0_f ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[ids.nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( ids.nodeId );

				if ( hasTextures )
				{
					out.texture0 = in.texture0;
					out.texture1 = in.texture1;
					out.texture2 = in.texture2;
					out.texture3 = in.texture3;
				}

				auto morphingWeights = writer.declLocale( "morphingWeights"
					, c3d_morphingWeights[ids.morphingId] );
				morphingWeights.morph( c3d_morphTargets
					, writer.cast< UInt >( in.vertexIndex - in.baseVertex )
					, curPosition
					, v4Normal
					, out.texture0
					, out.texture1
					, out.texture2
					, out.texture3
					, out.colour );

				auto modelMtx = writer.declLocale< Mat4 >( "modelMtx"
					, modelData.getCurModelMtx( flags.programFlags
						, skinningData
						, ids.skinningId
						, in.boneIds0
						, in.boneIds1
						, in.boneWeights0
						, in.boneWeights1 ) );

				out.vtx.position = ( modelMtx * curPosition );
				out.viewPosition = c3d_matrixData.worldToCurView( out.vtx.position ).xyz();
				out.worldPosition = out.viewPosition;
				out.normal = normalize( mat3( transpose( inverse( modelMtx ) ) ) * v4Normal.xyz() );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		// Shader inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition", 0u );
		auto inTexcoord = writer.declInput< Vec2 >( "inTexcoord", 1u, hasTextures );
		auto inCenter = writer.declInput< Vec3 >( "inCenter", 2u );

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

		writer.implementMainT< VoidT, vxlpass::SurfaceT >( sdw::VertexInT< VoidT >{ writer }
			, sdw::VertexOutT< vxlpass::SurfaceT >{ writer
				, SubmeshFlags{} }
			,[&]( VertexIn in
			, VertexOutT< vxlpass::SurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, in.drawID ) );
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
					, billboardData.getCameraRight( flags.programFlags, c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( flags.programFlags, c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( flags.programFlags, c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( flags.programFlags, c3d_sceneData ) );

				if ( hasTextures )
				{
					out.texture0 = vec3( inTexcoord, 0.0_f );
				}

				out.vtx.position = vec4( curBbcenter
						+ right * inPosition.x() * width
						+ up * inPosition.y() * height
					, 1.0_f );
				auto viewPosition = writer.declLocale( "viewPosition"
					, c3d_matrixData.worldToCurView( out.vtx.position ) );
				out.viewPosition = viewPosition.xyz();
				out.worldPosition = out.viewPosition;
				out.normal = normalize( c3d_sceneData.getPosToCamera( curBbcenter ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetHullShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr VoxelizePass::doGetDomainShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr VoxelizePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		GeometryWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_Voxelizer( writer
			, uint32_t( GlobalBuffersIdx::eCount ) + 1u
			, RenderPipeline::eBuffers
			, true );

		writer.implementMainT< TriangleListT< vxlpass::SurfaceT >, TriangleStreamT< vxlpass::SurfaceT > >( TriangleListT< vxlpass::SurfaceT >{ writer
				, flags.submeshFlags }
			, TriangleStreamT< vxlpass::SurfaceT >{ writer
				, 3u
				, flags.submeshFlags }
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
					positions[i] = list[i].vtx.position.xyz() * c3d_voxelData.worldToGrid;

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

					if ( hasTextures )
					{
						out.texture0 = list[i].texture0;
						out.texture1 = list[i].texture1;
						out.texture2 = list[i].texture2;
						out.texture3 = list[i].texture3;
					}

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
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();
		shader::Utils utils{ writer, *getEngine() };

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
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };
		auto addIndex = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = addIndex++;
		C3D_Voxelizer( writer
			, addIndex++
			, RenderPipeline::eBuffers
			, true );
		auto output( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
			, addIndex++
			, RenderPipeline::eBuffers ) );
		auto lightingModel = shader::LightingModel::createDiffuseModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.sceneFlags, true, false }
			, addIndex
			, RenderPipeline::eBuffers
			, m_mode != RenderMode::eTransparentOnly );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		writer.implementMainT< vxlpass::SurfaceT, VoidT >( FragmentInT< vxlpass::SurfaceT >{ writer
				, FragmentOrigin::eUpperLeft
				, FragmentCenter::eHalfPixel
				, flags.submeshFlags }
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
						, c3d_modelsData[in.nodeId - 1] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );
					auto normal = writer.declLocale( "normal"
						, normalize( in.normal ) );
					auto lightMat = lightingModel->declMaterial( "lightMat" );
					lightMat->create( in.colour
						, material );
					auto emissive = writer.declLocale( "emissive"
						, vec3( material.emissive ) );
					auto alpha = writer.declLocale( "alpha"
						, material.opacity );
					auto occlusion = writer.declLocale( "occlusion"
						, 1.0_f );

					if ( hasTextures )
					{
						auto texCoord0 = writer.declLocale( "texCoord0"
							, in.texture0 );
						auto texCoord1 = writer.declLocale( "texCoord1"
							, in.texture1 );
						auto texCoord2 = writer.declLocale( "texCoord2"
							, in.texture2 );
						auto texCoord3 = writer.declLocale( "texCoord3"
							, in.texture3 );
						lightingModel->computeMapDiffuseContributions( flags.passFlags
							, flags.textures
							, textureConfigs
							, textureAnims
							, c3d_maps
							, modelData.getTextures0()
							, modelData.getTextures1()
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

					if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
					{
						auto worldEye = writer.declLocale( "worldEye"
							, c3d_sceneData.cameraPosition );
						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( in.fragCoord.xy(), in.viewPosition, in.worldPosition, normal );
						color += occlusion
							* lightMat->albedo
							* lightingModel->computeCombinedDiffuse( *lightMat
								, c3d_sceneData
								, surface
								, worldEye
								, modelData.isShadowReceiver() );
					}

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
