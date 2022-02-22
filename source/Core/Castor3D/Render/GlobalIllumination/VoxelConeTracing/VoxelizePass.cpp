#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
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
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
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

	namespace
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
				, texture{ getMember< sdw::Vec3 >( "texcoord" ) }
				, textures0{ getMember< sdw::UVec4 >( "textures0" ) }
				, textures1{ getMember< sdw::UVec4 >( "textures1" ) }
				, textures{ getMember< sdw::Int >( "textures" ) }
				, material{ getMember< sdw::UInt >( "material" ) }
				, nodeId{ getMember< sdw::Int >( "nodeId" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "Surface"
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
					result->declMember( "texcoord"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "textures0"
						, sdw::type::Kind::eVec4U
						, sdw::type::NotArray
						, index++ );
					result->declMember( "textures1"
						, sdw::type::Kind::eVec4U
						, sdw::type::NotArray
						, index++ );
					result->declMember( "textures"
						, sdw::type::Kind::eInt
						, sdw::type::NotArray
						, index++ );
					result->declMember( "material"
						, sdw::type::Kind::eUInt
						, sdw::type::NotArray
						, index++ );
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
			sdw::Vec3 texture;
			sdw::UVec4 textures0;
			sdw::UVec4 textures1;
			sdw::Int textures;
			sdw::UInt material;
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
			getCuller().compute();
			RenderNodesPass::update( updater );
		}
	}

	void VoxelizePass::update( GpuUpdater & updater )
	{
		if ( m_voxelConfig.enabled )
		{
			auto & nodes = m_renderQueue->getCulledRenderNodes();

			if ( nodes.hasNodes() )
			{
				RenderNodesPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
				RenderNodesPass::doUpdate( nodes.staticNodes.frontCulled );
				RenderNodesPass::doUpdate( nodes.skinnedNodes.frontCulled );
				RenderNodesPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
				RenderNodesPass::doUpdate( nodes.morphingNodes.frontCulled );
				RenderNodesPass::doUpdate( nodes.billboardNodes.frontCulled );

				RenderNodesPass::doUpdate( nodes.instancedStaticNodes.backCulled, updater.info );
				RenderNodesPass::doUpdate( nodes.staticNodes.backCulled, updater.info );
				RenderNodesPass::doUpdate( nodes.skinnedNodes.backCulled, updater.info );
				RenderNodesPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, updater.info );
				RenderNodesPass::doUpdate( nodes.morphingNodes.backCulled, updater.info );
				RenderNodesPass::doUpdate( nodes.billboardNodes.backCulled, updater.info );
			}

			static const castor::Matrix4x4f identity
			{
				[]()
				{
					castor::Matrix4x4f res;
					res.setIdentity();
					return res;
			}( )
			};
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

	void VoxelizePass::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eHasGeometry );
		addFlag( flags.programFlags, ProgramFlag::eLighting );


		remFlag( flags.passFlags, PassFlag::eReflection );
		remFlag( flags.passFlags, PassFlag::eRefraction );
		remFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags.passFlags, PassFlag::eDistanceBasedTransmittance );
		remFlag( flags.passFlags, PassFlag::eSubsurfaceScattering );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.passFlags, PassFlag::eAlphaTest );
		remFlag( flags.passFlags, PassFlag::eBlendAlphaTest );
		flags.sceneFlags = doAdjustFlags( flags.sceneFlags );
	}

	SceneFlags VoxelizePass::doAdjustFlags( SceneFlags flags )const
	{
		remFlag( flags, SceneFlag::eLpvGI );
		remFlag( flags, SceneFlag::eLayeredLpvGI );
		remFlag( flags, SceneFlag::eFogLinear );
		remFlag( flags, SceneFlag::eFogExponential );
		remFlag( flags, SceneFlag::eFogSquaredExponential );
		return flags;
	}

	void VoxelizePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
		m_sceneUbo.cpuUpdate( m_scene, &m_camera );
	}

	void VoxelizePass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto sceneFlags = doAdjustFlags( m_scene.getFlags() );
		auto index = uint32_t( PassUboIdx::eCount );
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
		auto sceneFlags = doAdjustFlags( m_scene.getFlags() );
		auto index = uint32_t( PassUboIdx::eCount );
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
		bool hasTextures = !flags.textures.empty();

		C3D_Matrix( writer
			, PassUboIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, PassUboIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, PassUboIdx::eObjectsNodeID
			, RenderPipeline::ePass );

		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( NodeUboIdx::eSkinningSsbo )
			, uint32_t( NodeUboIdx::eSkinningBones )
			, RenderPipeline::eBuffers
			, flags.programFlags );
		C3D_Morphing( writer
			, NodeUboIdx::eMorphing
			, RenderPipeline::eBuffers
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, SurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< SurfaceT >{ writer }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< SurfaceT > out )
			{
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( in.normal, 0.0_f ) );
				auto nodeId = writer.declLocale( "nodeId"
					, c3d_objectIdsData[pipelineID].getNodeId( customDrawID ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId] );
				out.textures0 = modelData.getTextures0( flags.programFlags
					, in.textures0 );
				out.textures1 = modelData.getTextures1( flags.programFlags
					, in.textures1 );
				out.textures = modelData.getTextures( flags.programFlags
					, in.textures );
				out.material = modelData.getMaterialId( flags.programFlags
					, in.material );
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				if ( hasTextures )
				{
					out.texture = in.texture0;
				}

				in.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, out.texture );

				auto modelMtx = writer.declLocale< Mat4 >( "modelMtx"
					, modelData.getCurModelMtx( flags.programFlags
						, skinningData
						, in.instanceIndex - in.baseInstance
						, in.vertexIndex - in.baseVertex ) );

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
		bool hasTextures = !flags.textures.empty();

		// Shader inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition", 0u );
		auto inTexcoord = writer.declInput< Vec2 >( "inTexcoord", 1u, hasTextures );
		auto inCenter = writer.declInput< Vec3 >( "inCenter", 2u );

		C3D_Matrix( writer
			, PassUboIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_Scene( writer
			, PassUboIdx::eScene
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, PassUboIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_Billboard( writer
			, PassUboIdx::eBillboardsData
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, PassUboIdx::eObjectsNodeID
			, RenderPipeline::ePass );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
			, VertexOutT< SurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, c3d_objectIdsData[pipelineID].getNodeId( customDrawID ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId] );
				out.textures0 = modelData.getTextures0();
				out.textures1 = modelData.getTextures1();
				out.textures = modelData.getTextures();
				out.material = modelData.getMaterialId();
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( inCenter, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId] );
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
					out.texture = vec3( inTexcoord, 0.0_f );
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
		bool hasTextures = !flags.textures.empty();

		UBO_VOXELIZER( writer
			, uint32_t( PassUboIdx::eCount ) + 1u
			, RenderPipeline::ePass
			, true );

		writer.implementMainT< 3u, TriangleListT< SurfaceT >, TriangleStreamT< SurfaceT > >( [&]( GeometryIn in
			, TriangleListT< SurfaceT > list
			, TriangleStreamT< SurfaceT > out )
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
					out.textures0 = list[i].textures0;
					out.textures1 = list[i].textures1;
					out.nodeId = list[i].nodeId;
					out.textures = list[i].textures;
					out.material = list[i].material;
					out.vtx.position = vec4( positions[i], 1.0f );

					if ( hasTextures )
					{
						out.texture = list[i].texture;
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
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		bool hasTextures = !flags.textures.empty();
		shader::Utils utils{ writer, *getEngine() };

		C3D_Scene( writer
			, PassUboIdx::eScene
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, PassUboIdx::eModelsData
			, RenderPipeline::ePass );
		shader::Materials materials{ writer
			, uint32_t( PassUboIdx::eMaterials )
			, RenderPipeline::ePass };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( PassUboIdx::eTexConfigs )
			, RenderPipeline::ePass
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( PassUboIdx::eTexAnims )
			, RenderPipeline::ePass
			, hasTextures };
		auto addIndex = uint32_t( PassUboIdx::eCount );
		auto lightsIndex = addIndex++;
		UBO_VOXELIZER( writer
			, addIndex++
			, RenderPipeline::ePass
			, true );
		auto output( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
			, addIndex++
			, RenderPipeline::ePass ) );
		auto lightingModel = shader::LightingModel::createDiffuseModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::ePass
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, addIndex
			, RenderPipeline::ePass
			, m_mode != RenderMode::eTransparentOnly
			, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		writer.implementMainT< SurfaceT, VoidT >( [&]( FragmentInT< SurfaceT > in
			, FragmentOut out )
			{
				auto diff = writer.declLocale( "diff"
					, c3d_voxelData.worldToClip( in.worldPosition ) );
				auto uvw = writer.declLocale( "uvw"
					, diff * vec3( 0.5_f, -0.5f, 0.5f ) + 0.5f );

				IF( writer, utils.isSaturated( uvw ) )
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( in.material ) );
					auto normal = writer.declLocale( "normal"
						, normalize( in.normal ) );
					auto lightMat = lightingModel->declMaterial( "lightMat" );
					lightMat->create( material );
					auto emissive = writer.declLocale( "emissive"
						, vec3( material.emissive ) );
					auto alpha = writer.declLocale( "alpha"
						, material.opacity );
					auto occlusion = writer.declLocale( "occlusion"
						, 1.0_f );

					if ( hasTextures )
					{
						auto texCoord = writer.declLocale( "texCoord"
							, in.texture );
						lightingModel->computeMapDiffuseContributions( flags.passFlags
							, flags.textures
							, textureConfigs
							, textureAnims
							, c3d_maps
							, in.textures0
							, in.textures1
							, texCoord
							, emissive
							, alpha
							, occlusion
							, *lightMat );
					}

					auto color = writer.declLocale( "color"
						, emissive );

					if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
					{
						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[in.nodeId] );
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
