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
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
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

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, ShadowMap const & shadowMap
			, castor::String const & name )
		{
			std::array< VkImageLayout, size_t( SmTexture::eCount ) > FinalLayouts{ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

			// Create the render pass.
			ashes::VkAttachmentDescriptionArray attaches;
			ashes::VkAttachmentReferenceArray references;
			uint32_t index = 0u;

			for ( auto & result : shadowMap.getShadowPassResult() )
			{
				attaches.push_back( { 0u
					, result->getTexture()->getPixelFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, FinalLayouts[index] } );

				if ( index > 0 )
				{
					references.push_back( { index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
				}

				++index;
			}

			ashes::SubpassDescriptionArray subpasses;
			subpasses.push_back( { 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, references
				, {}
				, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, {} } );
			ashes::VkSubpassDependencyArray dependencies
			{
				{ VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				,std::move( attaches )
				,std::move( subpasses )
				,std::move( dependencies ) };
			return device->createRenderPass( name
				, std::move( createInfo ) );
		}

		castor::String getPassName( uint32_t index )
		{
			return cuT( "Point Layer " ) + string::toString( index / 6u ) + " Face " + string::toString( index % 6u );
		}
	}

	uint32_t const ShadowMapPassPoint::TextureSize = 512u;

	ShadowMapPassPoint::ShadowMapPassPoint( RenderDevice const & device
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ device
			, getPassName( index )
			, matrixUbo
			, culler
			, shadowMap
			, createRenderPass( device, shadowMap, getPassName( index ) ) }
		, m_viewport{ *device.renderSystem.getEngine() }
	{
		float const aspect = float( ShadowMapPassPoint::TextureSize ) / ShadowMapPassPoint::TextureSize;
		float const nearZ = 1.0f;
		float const farZ = 2000.0f;
		m_projection = device.renderSystem.getPerspective( 90.0_degrees, aspect, nearZ, farZ );
		m_viewport.resize( { ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } );
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
		m_onNodeChanged.disconnect();
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

	void ShadowMapPassPoint::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		SceneRenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.staticNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.skinnedNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.morphingNodes.backCulled );
		SceneRenderPass::doUpdate( nodes.billboardNodes.backCulled );
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

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PhongMaterials materials{ writer };
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

		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		auto index = uint32_t( PassUboIdx::eCount );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PbrMRMaterials materials{ writer };
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

		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		auto index = uint32_t( PassUboIdx::eCount );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		shader::PbrSGMaterials materials{ writer };
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

		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		auto index = uint32_t( PassUboIdx::eCount );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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
