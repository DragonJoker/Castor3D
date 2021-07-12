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
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, VoxelizePass )

namespace castor3d
{
	namespace
	{
		template< MaterialType MaterialT >
		ShaderPtr getPixelShaderSourceT( RenderSystem const & renderSystem
			, PipelineFlags const & flags
			, FilteredTextureFlags const & textures
			, bool isTransparentOnly
			, uint32_t voxelGridSize )
		{
			using MyTraits = shader::ShaderMaterialTraitsT< MaterialT >;
			using LightingModel = typename MyTraits::LightingModel;
			using LightMaterial = typename MyTraits::LightMaterial;

			using namespace sdw;
			FragmentWriter writer;
			bool hasTextures = !flags.textures.empty();

			shader::Utils utils{ writer };
			utils.declareApplyGamma();
			utils.declareRemoveGamma();
			utils.declareIsSaturated();
			utils.declareEncodeColor();
			utils.declareEncodeNormal();
			utils.declareFlatten();

			// Shader inputs
			auto index = 0u;
			auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition", index++ );
			auto inViewPosition = writer.declInput< Vec3 >( "inViewPosition", index++ );
			auto inNormal = writer.declInput< Vec3 >( "inNormal", index++ );
			auto inTexture = writer.declInput< Vec3 >( "inTexture", index++, hasTextures );
			auto inMaterial = writer.declInput< UInt >( "inMaterial", index++ );
			auto in = writer.getIn();

			shader::Materials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eMaterials )
				, RenderPipeline::eBuffers );
			shader::TextureConfigurations textureConfigs{ writer };

			if ( hasTextures )
			{
				textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
					, uint32_t( NodeUboIdx::eTextures )
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
			auto lighting = LightingModel::createDiffuseModel( writer
				, utils
				, lightsIndex
				, RenderPipeline::eAdditional
				, shader::ShadowOptions{ flags.sceneFlags, false }
				, addIndex
				, RenderPipeline::eAdditional
				, !isTransparentOnly );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto diff = writer.declLocale( "diff"
						, c3d_voxelData.worldToClip( inWorldPosition ) );
					auto uvw = writer.declLocale( "uvw"
						, diff * vec3( 0.5_f, -0.5f, 0.5f ) + 0.5f );

					IF( writer, utils.isSaturated( uvw ) )
					{
						auto material = writer.declLocale( "material"
							, materials.getMaterial( inMaterial ) );
						auto gamma = writer.declLocale( "gamma"
							, material.gamma );
						auto normal = writer.declLocale( "normal"
							, normalize( inNormal ) );
						auto lightMat = writer.declLocale< LightMaterial >( "lightMat" );
						lightMat.create( material );
						auto emissive = writer.declLocale( "emissive"
							, vec3( material.emissive ) );
						auto alpha = writer.declLocale( "alpha"
							, material.opacity );
						auto occlusion = writer.declLocale( "occlusion"
							, 1.0_f );

						if ( hasTextures )
						{
							auto texCoord = writer.declLocale( "texCoord"
								, inTexture );
							lighting->computeMapVoxelContributions( flags.passFlags
								, textures
								, gamma
								, textureConfigs
								, c3d_maps
								, texCoord
								, emissive
								, alpha
								, occlusion
								, lightMat );
						}

						if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
						{
							auto worldEye = writer.declLocale( "worldEye"
								, c3d_sceneData.getCameraPosition() );
							auto surface = writer.declLocale< shader::Surface >( "surface" );
							surface.create( in.fragCoord.xy(), inViewPosition, inWorldPosition, normal );
							auto color = writer.declLocale( "color"
								, lighting->computeCombinedDiffuse( worldEye
									, lightMat
									, c3d_modelData.isShadowReceiver()
									, c3d_sceneData
									, surface ) );
							color *= lightMat.albedo * occlusion;
							color += emissive;

							auto encodedColor = writer.declLocale( "encodedColor"
								, utils.encodeColor( vec4( color, alpha ) ) );
							auto encodedNormal = writer.declLocale( "encodedNormal"
								, utils.encodeNormal( normal ) );
							auto writecoord = writer.declLocale( "writecoord"
								, uvec3( floor( uvw * c3d_voxelData.clipToGrid ) ) );
							auto id = writer.declLocale( "id"
								, utils.flatten( writecoord, uvec3( sdw::UInt{ voxelGridSize } ) ) );
							atomicMax( output[id].colorMask, encodedColor );
							atomicMax( output[id].normalMask, encodedNormal );
						}
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

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

	VoxelizePass::~VoxelizePass()
	{
	}

	void VoxelizePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto flags = visitor.getFlags();
		auto shaderProgram = doGetProgram( flags );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_GEOMETRY_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
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
			auto & nodes = m_renderQueue.getCulledRenderNodes();

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
			jitterProjSpace[0] /= m_camera.getWidth();
			jitterProjSpace[1] /= m_camera.getHeight();
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
		remFlag( flags.passFlags, PassFlag::eReflection );
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
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
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
			descriptorWrites.push_back( scene.getLightCache().getDescriptorWrite( index++ ) );
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

		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		// Outputs
		uint32_t index = 0u;
		auto outViewPosition = writer.declOutput< Vec3 >( "outViewPosition", index++ );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal", index++ );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture", index++, hasTextures );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial", index++ );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inSurface.normal, 0.0_f ) );
				outTexture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, outTexture );

				auto modelMtx = writer.declLocale< Mat4 >( "modelMtx"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				outMaterial = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );

				out.vtx.position = ( modelMtx * curPosition );
				outViewPosition = c3d_matrixData.worldToCurView( out.vtx.position ).xyz();
				outNormal = normalize( mat3( transpose( inverse( modelMtx ) ) ) * v4Normal.xyz() );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Shader inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition", 0u );
		auto center = writer.declInput< Vec3 >( "center", 2u );

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

		auto in = writer.getIn();

		// Outputs
		uint32_t index = 0u;
		auto outViewPosition = writer.declOutput< Vec3 >( "outViewPosition", index++ );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal", index++ );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture", index++, hasTextures );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial", index++ );
		auto out = writer.getOut();

		writer.implementFunction< Void >( "main"
			, [&]()
			{
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, c3d_modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
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

				out.vtx.position = vec4( curBbcenter
						+ right * inPosition.x() * width
						+ up * inPosition.y() * height
					, 1.0_f );
				auto viewPosition = writer.declLocale( "viewPosition"
					, c3d_matrixData.worldToCurView( out.vtx.position ) );
				outViewPosition = viewPosition.xyz();
				outNormal = normalize( c3d_sceneData.getPosToCamera( curBbcenter ) );
				outMaterial = c3d_modelData.getMaterialIndex();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		GeometryWriter writer;
		bool hasTextures = !flags.textures.empty();

		writer.inputLayout( ast::stmt::InputLayout::eTriangleList );
		writer.outputLayout( ast::stmt::OutputLayout::eTriangleStrip, 3u );

		UBO_VOXELIZER( writer
			, uint32_t( PassUboIdx::eCount ) + 1u
			, RenderPipeline::eAdditional
			, true );

		// Shader inputs
		uint32_t index = 0u;
		auto inViewPosition = writer.declInputArray< Vec3 >( "inViewPosition", index++, 3u );
		auto inNormal = writer.declInputArray< Vec3 >( "inNormal", index++, 3u );
		auto inTexture = writer.declInputArray< Vec3 >( "inTexture", index++, 3u, hasTextures );
		auto inMaterial = writer.declInputArray< UInt >( "inMaterial", index++, 3u );
		auto in = writer.getIn();

		// Outputs
		index = 0u;
		auto outWorldPosition = writer.declOutput< Vec3 >( "outWorldPosition", index++ );
		auto outViewPosition = writer.declOutput< Vec3 >( "outViewPosition", index++ );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal", index++ );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture", index++, hasTextures );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial", index++ );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto facenormal = writer.declLocale( "facenormal"
					, abs( inNormal[0] + inNormal[1] + inNormal[2] ) );
				auto maxi = writer.declLocale( "maxi"
					, writer.ternary( facenormal[1] > facenormal[0], 1_u, 0_u ) );
				maxi = writer.ternary( facenormal[2] > facenormal[maxi], 2_u, maxi );
				auto positions = writer.declLocaleArray< Vec3 >( "positions", 3u );

				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
					positions[i] = in.vtx[i].position.xyz() * c3d_voxelData.worldToGrid;

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
					outWorldPosition = in.vtx[i].position.xyz();
					outViewPosition = inViewPosition[i];
					outNormal = inNormal[i];
					outMaterial = inMaterial[i];
					outTexture = inTexture[i];
					out.vtx.position = vec4( positions[i], 1.0f );

					EmitVertex( writer );
				}
				ROF;

				EndPrimitive( writer );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::ePhong >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, m_mode == RenderMode::eTransparentOnly
			, m_voxelConfig.gridSize.value() );
	}

	ShaderPtr VoxelizePass::doGetPbrPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::eMetallicRoughness >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, m_mode == RenderMode::eTransparentOnly
			, m_voxelConfig.gridSize.value() );
	}
}
