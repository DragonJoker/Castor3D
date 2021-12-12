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
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
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
				, material{ getMember< sdw::UInt >( "material" ) }
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
					result->declMember( "material"
						, sdw::type::Kind::eUInt
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			sdw::Vec3 worldPosition;
			sdw::Vec3 viewPosition;
			sdw::Vec3 normal;
			sdw::Vec3 texture;
			sdw::UInt material;
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
		: SceneRenderPass{ pass
			, context
			, graph
			, device
			, Type
			, "Voxelize"
			, "Voxelization"
			, SceneRenderPassDesc{ { voxelConfig.gridSize.value(), voxelConfig.gridSize.value(), 1u }, matrixUbo, culler, RenderMode::eBoth, true, true } }
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
			SceneRenderPass::update( updater );
		}
	}

	void VoxelizePass::update( GpuUpdater & updater )
	{
		if ( m_voxelConfig.enabled )
		{
			auto & nodes = m_renderQueue->getCulledRenderNodes();

			if ( nodes.hasNodes() )
			{
				SceneRenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
				SceneRenderPass::doUpdate( nodes.staticNodes.frontCulled );
				SceneRenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
				SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
				SceneRenderPass::doUpdate( nodes.morphingNodes.frontCulled );
				SceneRenderPass::doUpdate( nodes.billboardNodes.frontCulled );

				SceneRenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, updater.info );
				SceneRenderPass::doUpdate( nodes.staticNodes.backCulled, updater.info );
				SceneRenderPass::doUpdate( nodes.skinnedNodes.backCulled, updater.info );
				SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, updater.info );
				SceneRenderPass::doUpdate( nodes.morphingNodes.backCulled, updater.info );
				SceneRenderPass::doUpdate( nodes.billboardNodes.backCulled, updater.info );
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

		remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eFogLinear );
		remFlag( flags.sceneFlags, SceneFlag::eFogExponential );
		remFlag( flags.sceneFlags, SceneFlag::eFogSquaredExponential );

		remFlag( flags.passFlags, PassFlag::eReflection );
		remFlag( flags.passFlags, PassFlag::eRefraction );
		remFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags.passFlags, PassFlag::eDistanceBasedTransmittance );
		remFlag( flags.passFlags, PassFlag::eSubsurfaceScattering );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.passFlags, PassFlag::eAlphaTest );
		remFlag( flags.passFlags, PassFlag::eBlendAlphaTest );
	}

	void VoxelizePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
		m_sceneUbo.cpuUpdate( m_scene, &m_camera );
	}

	void VoxelizePass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
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
			if ( checkFlag( flags.sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << j ) ) )
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
		return SceneRenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
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

		void fillAdditionalDescriptor( crg::RunnableGraph & graph
			, RenderPipeline const & pipeline
			, Scene const & scene
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels )
		{
			auto index = uint32_t( PassUboIdx::eCount );
			auto & flags = pipeline.getFlags();
			descriptorWrites.push_back( scene.getLightCache().getBinding( index++ ) );
			descriptorWrites.push_back( voxelizerUbo.getDescriptorWrite( index++ ) );
			descriptorWrites.push_back( getDescriptorWrite( voxels, index++ ) );
			bindShadowMaps( graph
				, flags
				, shadowMaps
				, descriptorWrites
				, index );
		}
	}

	void VoxelizePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, m_scene
			, descriptorWrites
			, shadowMaps
			, m_voxelizerUbo
			, m_voxels );
	}

	void VoxelizePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, m_scene
			, descriptorWrites
			, shadowMaps
			, m_voxelizerUbo
			, m_voxels );
	}

	ShaderPtr VoxelizePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		// Inputs
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

				if ( hasTextures )
				{
					out.texture = in.texture0;
				}

				in.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, out.texture );

				auto modelMtx = writer.declLocale< Mat4 >( "modelMtx"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, in ) );
				out.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, in.material );

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

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		UBO_BILLBOARD( writer
			, uint32_t( NodeUboIdx::eBillboard )
			, RenderPipeline::eBuffers );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );
		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );

		writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
			, VertexOutT< SurfaceT > out )
			{
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, c3d_modelData.modelToCurWorld( vec4( inCenter, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto right = writer.declLocale( "right"
					, c3d_billboardData.getCameraRight( flags.programFlags, c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, c3d_billboardData.getCameraUp( flags.programFlags, c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, c3d_billboardData.getWidth( flags.programFlags, c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, c3d_billboardData.getHeight( flags.programFlags, c3d_sceneData ) );

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
				out.material = c3d_modelData.getMaterialIndex();
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
			, RenderPipeline::eAdditional
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

		// Shader inputs
		shader::Materials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		shader::TextureAnimations textureAnims{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexConfigs )
				, RenderPipeline::eBuffers );
			textureAnims.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexAnims )
				, RenderPipeline::eBuffers );
		}

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );
		auto addIndex = uint32_t( PassUboIdx::eCount );
		auto lightsIndex = addIndex++;
		UBO_VOXELIZER( writer
			, addIndex++
			, RenderPipeline::eAdditional
			, true );
		auto output( writer.declArrayShaderStorageBuffer< shader::Voxel >( "voxels"
			, addIndex++
			, RenderPipeline::eAdditional ) );
		auto lightingModel = shader::LightingModel::createDiffuseModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, addIndex
			, RenderPipeline::eAdditional
			, m_mode != RenderMode::eTransparentOnly
			, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

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
							, filterTexturesFlags( flags.textures )
							, textureConfigs
							, textureAnims
							, c3d_maps
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
								, c3d_modelData.isShadowReceiver() );
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
