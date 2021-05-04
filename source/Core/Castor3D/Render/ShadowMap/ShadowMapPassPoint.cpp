#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doUpdateShadowMatrices( castor::Point3f const & position
			, std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices =
			{
				{
					castor::matrix::lookAt( position, position + castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Positive X
					castor::matrix::lookAt( position, position + castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Negative X
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ),// Positive Y
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ),// Negative Y
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Positive Z
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Negative Z
				}
			};
		}
	}

	uint32_t const ShadowMapPassPoint::TextureSize = 512u;

	ShadowMapPassPoint::ShadowMapPassPoint( RenderDevice const & device
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ device, cuT( "Point Layer " ) + string::toString( index / 6u ) + " Face " + string::toString( index % 6u ), matrixUbo, culler, shadowMap }
		, m_viewport{ *device.renderSystem.getEngine() }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	bool ShadowMapPassPoint::update( CpuUpdater & updater )
	{
		getCuller().compute();
		m_outOfDate = m_outOfDate
			|| getCuller().areAllChanged()
			|| getCuller().areCulledChanged();
		SceneRenderPass::update( updater );
		return m_outOfDate;
	}

	void ShadowMapPassPoint::update( GpuUpdater & updater )
	{
		if ( m_initialised )
		{
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	void ShadowMapPassPoint::doUpdateUbos( CpuUpdater & updater )
	{
		m_viewport.updateFar( updater.light->getFarPlane() );
		auto position = updater.light->getParent()->getDerivedPosition();
		doUpdateShadowMatrices( position, m_matrices );

		m_shadowMapUbo.update( *updater.light, updater.index );
		m_matrixUbo.cpuUpdate( m_matrices[updater.index], m_projection );
	}

	void ShadowMapPassPoint::doUpdateNodes( SceneCulledRenderNodes & nodes )
	{
		SceneRenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.staticNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.skinnedNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.morphingNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.billboardNodes.backCulled );
	}

	bool ShadowMapPassPoint::doInitialise( Size const & size )
	{
		float const aspect = float( size.getWidth() ) / size.getHeight();
		float const nearZ = 1.0f;
		float const farZ = 2000.0f;
		m_projection = getEngine()->getRenderSystem()->getPerspective( 90.0_degrees, aspect, nearZ, farZ );

		std::array< VkImageLayout, size_t( SmTexture::eCount ) > FinalLayouts
		{
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		};

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches;
		ashes::VkAttachmentReferenceArray references;
		uint32_t index = 0u;

		for ( auto & result : m_shadowMap.getShadowPassResult() )
		{
			attaches.push_back(
				{
					0u,
					result->getTexture()->getPixelFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					FinalLayouts[index],
				} );

			if ( index > 0 )
			{
				references.push_back( { index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			}

			++index;
		}

		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				references,
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = m_device->createRenderPass( m_name
			, std::move( createInfo ) );
		m_viewport.resize( size );
		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassPoint::doCleanup()
	{
		m_renderQueue.cleanup();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		m_shadowMapUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eShadow ) ) );
	}

	void ShadowMapPassPoint::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		m_shadowMapUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eShadow ) ) );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassPoint::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = SceneRenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eShadow )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
		return uboBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, uint32_t( SmTexture::eCount ) - 1u );
	}

	void ShadowMapPassPoint::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::eShadowMapPoint );
	}

	ShaderPtr ShadowMapPassPoint::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MATRIX( writer, uint32_t( NodeUboIdx::eMatrix ), 0 );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0 );
		auto skinningData = SkinningUbo::declare( writer, uint32_t( NodeUboIdx::eSkinning ), 0, flags.programFlags );
		UBO_MORPHING( writer, uint32_t( NodeUboIdx::eMorphing ), 0, flags.programFlags );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( "curPosition"
				, inSurface.position );
			auto v4Normal = writer.declLocale( "v4Normal"
				, vec4( inSurface.normal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( "v4Tangent"
				, vec4( inSurface.tangent, 0.0_f ) );
			outSurface.texture = inSurface.texture;
			inSurface.morph( c3d_morphingData
				, curPosition
				, v4Normal
				, v4Tangent
				, outSurface.texture );
			outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
				, inSurface.material );
			outSurface.instance = writer.cast< UInt >( in.instanceIndex );

			auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
				, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
			curPosition = mtxModel * curPosition;
			outSurface.worldPosition = curPosition.xyz();
			out.vtx.position = c3d_matrixData.worldToCurProj( curPosition );

			auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
				, c3d_modelData.getNormalMtx( flags.programFlags, mtxModel ) );
			outSurface.computeTangentSpace( flags.programFlags
				, c3d_matrixData.getCurViewCenter()
				, mtxNormal
				, v4Normal
				, v4Tangent );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassPoint::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PhongMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", uint32_t( NodeUboIdx::eLights ), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );
		UBO_SHADOWMAP( writer, uint32_t( NodeUboIdx::eShadow ), 0u );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
		{
			pxl_normalLinear = vec4( 0.0_f );
			pxl_variance = vec2( 0.0_f );
			pxl_position = vec4( 0.0_f );
			pxl_flux = vec4( 0.0_f );
			auto texCoord = writer.declLocale( "texCoord"
				, inSurface.texture );
			auto normal = writer.declLocale( "normal"
				, normalize( inSurface.normal ) );
			auto tangent = writer.declLocale( "tangent"
				, normalize( inSurface.tangent ) );
			auto bitangent = writer.declLocale( "bitangent"
				, normalize( inSurface.bitangent ) );
			auto material = materials.getMaterial( inSurface.material );
			auto gamma = writer.declLocale( "gamma"
				, material.m_gamma );
			auto diffuse = writer.declLocale( "diffuse"
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto specular = writer.declLocale( "specular"
				, material.m_specular );
			auto shininess = writer.declLocale( "shininess"
				, material.m_shininess );
			auto emissive = writer.declLocale( "emissive"
				, vec3( material.m_emissive ) );
			auto occlusion = writer.declLocale( "occlusion"
				, 1.0_f );
			auto transmittance = writer.declLocale( "transmittance"
				, 0.0_f );
			auto alpha = writer.declLocale( "alpha"
				, material.m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material.m_alphaRef );
			auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
				, inSurface.tangentSpaceViewPosition );
			auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
				, inSurface.tangentSpaceFragPosition );

			if ( hasTextures )
			{
				lighting->computeMapContributions( flags.passFlags
					, textures
					, gamma
					, textureConfigs
					, c3d_textureData.config
					, c3d_maps
					, texCoord
					, normal
					, tangent
					, bitangent
					, emissive
					, alpha
					, occlusion
					, transmittance
					, diffuse
					, specular
					, shininess
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
			}

			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, alphaRef );

			auto lightDiffuse = writer.declLocale( "lightDiffuse"
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( "lightSpecular"
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			auto light = writer.declLocale( "light"
				, c3d_shadowMapData.getPointLight( *lighting ) );
			auto lightToVertex = writer.declLocale( "lightToVertex"
				, light.m_position.xyz() - inSurface.worldPosition );
			auto distance = writer.declLocale( "distance"
				, length( lightToVertex ) );
			auto attenuation = writer.declLocale( "attenuation"
				, sdw::fma( light.m_attenuation.z()
					, distance * distance
					, sdw::fma( light.m_attenuation.y()
						, distance
						, light.m_attenuation.x() ) ) );
			pxl_flux.rgb() = ( diffuse
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					* clamp( dot( lightToVertex / distance, normal ), 0.0_f, 1.0_f ) )
				/ attenuation;

			auto depth = writer.declLocale( "depth"
				, c3d_shadowMapData.getLinearisedDepth( inSurface.worldPosition ) );
			pxl_normalLinear.w() = depth;
			pxl_normalLinear.xyz() = normal;
			pxl_position.xyz() = inSurface.worldPosition;

			pxl_variance.x() = depth;
			pxl_variance.y() = depth * depth;

			auto dx = writer.declLocale( "dx"
				, dFdx( depth ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( depth ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassPoint::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", uint32_t( NodeUboIdx::eLights ), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );
		UBO_SHADOWMAP( writer, uint32_t( NodeUboIdx::eShadow ), 0u );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
		{
			pxl_normalLinear = vec4( 0.0_f );
			pxl_variance = vec2( 0.0_f );
			pxl_position = vec4( 0.0_f );
			pxl_flux = vec4( 0.0_f );
			auto texCoord = writer.declLocale( "texCoord"
				, inSurface.texture );
			auto normal = writer.declLocale( "normal"
				, normalize( inSurface.normal ) );
			auto tangent = writer.declLocale( "tangent"
				, normalize( inSurface.tangent ) );
			auto bitangent = writer.declLocale( "bitangent"
				, normalize( inSurface.bitangent ) );
			auto material = materials.getMaterial( inSurface.material );
			auto metallic = writer.declLocale( "metallic"
				, material.m_metallic );
			auto roughness = writer.declLocale( "roughness"
				, material.m_roughness );
			auto gamma = writer.declLocale( "gamma"
				, material.m_gamma );
			auto albedo = writer.declLocale( "albedo"
				, utils.removeGamma( gamma, material.m_albedo ) );
			auto emissive = writer.declLocale( "emissive"
				, vec3( material.m_emissive ) );
			auto occlusion = writer.declLocale( "occlusion"
				, 1.0_f );
			auto transmittance = writer.declLocale( "transmittance"
				, 0.0_f );
			auto alpha = writer.declLocale( "alpha"
				, material.m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material.m_alphaRef );
			auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
				, inSurface.tangentSpaceViewPosition );
			auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
				, inSurface.tangentSpaceFragPosition );

			if ( hasTextures )
			{
				lighting->computeMapContributions( flags.passFlags
					, textures
					, gamma
					, textureConfigs
					, c3d_textureData.config
					, c3d_maps
					, texCoord
					, normal
					, tangent
					, bitangent
					, emissive
					, alpha
					, occlusion
					, transmittance
					, albedo
					, metallic
					, roughness
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
			}

			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, alphaRef );

			auto lightDiffuse = writer.declLocale( "lightDiffuse"
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( "lightSpecular"
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			auto light = writer.declLocale( "light"
				, c3d_shadowMapData.getPointLight( *lighting ) );
			auto lightToVertex = writer.declLocale( "lightToVertex"
				, light.m_position.xyz() - inSurface.worldPosition );
			auto distance = writer.declLocale( "distance"
				, length( lightToVertex ) );
			auto attenuation = writer.declLocale( "attenuation"
				, sdw::fma( light.m_attenuation.z()
					, distance * distance
					, sdw::fma( light.m_attenuation.y()
						, distance
						, light.m_attenuation.x() ) ) );
			pxl_flux.rgb() = ( albedo
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					* clamp( dot( lightToVertex / distance, normal ), 0.0_f, 1.0_f ) )
				/ attenuation;

			auto depth = writer.declLocale( "depth"
				, c3d_shadowMapData.getLinearisedDepth( inSurface.worldPosition ) );
			pxl_normalLinear.w() = depth;
			pxl_normalLinear.xyz() = normal;
			pxl_position.xyz() = inSurface.worldPosition;

			pxl_variance.x() = depth;
			pxl_variance.y() = depth * depth;

			auto dx = writer.declLocale( "dx"
				, dFdx( depth ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( depth ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassPoint::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", uint32_t( NodeUboIdx::eLights ), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );
		UBO_SHADOWMAP( writer, uint32_t( NodeUboIdx::eShadow ), 0u );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
		{
			pxl_normalLinear = vec4( 0.0_f );
			pxl_variance = vec2( 0.0_f );
			pxl_position = vec4( 0.0_f );
			pxl_flux = vec4( 0.0_f );
			auto texCoord = writer.declLocale( "texCoord"
				, inSurface.texture );
			auto normal = writer.declLocale( "normal"
				, normalize( inSurface.normal ) );
			auto tangent = writer.declLocale( "tangent"
				, normalize( inSurface.tangent ) );
			auto bitangent = writer.declLocale( "bitangent"
				, normalize( inSurface.bitangent ) );
			auto material = materials.getMaterial( inSurface.material );
			auto specular = writer.declLocale( "specular"
				, material.m_specular );
			auto glossiness = writer.declLocale( "glossiness"
				, material.m_glossiness );
			auto gamma = writer.declLocale( "gamma"
				, material.m_gamma );
			auto albedo = writer.declLocale( "albedo"
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto emissive = writer.declLocale( "emissive"
				, vec3( material.m_emissive ) );
			auto occlusion = writer.declLocale( "occlusion"
				, 1.0_f );
			auto transmittance = writer.declLocale( "transmittance"
				, 0.0_f );
			auto alpha = writer.declLocale( "alpha"
				, material.m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material.m_alphaRef );
			auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
				, inSurface.tangentSpaceViewPosition );
			auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
				, inSurface.tangentSpaceFragPosition );

			if ( hasTextures )
			{
				lighting->computeMapContributions( flags.passFlags
					, textures
					, gamma
					, textureConfigs
					, c3d_textureData.config
					, c3d_maps
					, texCoord
					, normal
					, tangent
					, bitangent
					, emissive
					, alpha
					, occlusion
					, transmittance
					, albedo
					, specular
					, glossiness
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
			}

			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, alphaRef );

			auto lightDiffuse = writer.declLocale( "lightDiffuse"
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( "lightSpecular"
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			auto light = writer.declLocale( "light"
				, c3d_shadowMapData.getPointLight( *lighting ) );
			auto lightToVertex = writer.declLocale( "lightToVertex"
				, light.m_position.xyz() - inSurface.worldPosition );
			auto distance = writer.declLocale( "distance"
				, length( lightToVertex ) );
			auto attenuation = writer.declLocale( "attenuation"
				, sdw::fma( light.m_attenuation.z()
					, distance * distance
					, sdw::fma( light.m_attenuation.y()
						, distance
						, light.m_attenuation.x() ) ) );
			pxl_flux.rgb() = ( albedo
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					* clamp( dot( lightToVertex / distance, normal ), 0.0_f, 1.0_f ) )
				/ attenuation;

			auto depth = writer.declLocale( "depth"
				, c3d_shadowMapData.getLinearisedDepth( inSurface.worldPosition ) );
			pxl_normalLinear.w() = depth;
			pxl_normalLinear.xyz() = normal;
			pxl_position.xyz() = inSurface.worldPosition;

			pxl_variance.x() = depth;
			pxl_variance.y() = depth * depth;

			auto dx = writer.declLocale( "dx"
				, dFdx( depth ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( depth ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
