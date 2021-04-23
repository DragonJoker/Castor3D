#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassSpot::TextureSize = 512u;

	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ cuT( "Spot " ) + string::toString( index ), engine, matrixUbo, culler, shadowMap }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	bool ShadowMapPassSpot::update( CpuUpdater & updater )
	{
		getCuller().compute();
		m_outOfDate = m_outOfDate
			|| getCuller().areAllChanged()
			|| getCuller().areCulledChanged();
		SceneRenderPass::update( updater );
		return m_outOfDate;
	}

	void ShadowMapPassSpot::update( GpuUpdater & updater )
	{
		if ( m_initialised )
		{
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	void ShadowMapPassSpot::doUpdateUbos( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto & myCamera = getCuller().getCamera();
		m_shadowType = light.getShadowType();
		light.getSpotLight()->updateShadow( myCamera
			, updater.index );
		m_shadowMapUbo.update( light, updater.index );
		m_matrixUbo.cpuUpdate( myCamera.getView(), myCamera.getProjection() );
	}

	bool ShadowMapPassSpot::doInitialise( RenderDevice const & device
		, Size const & size )
	{
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
		m_renderPass = device->createRenderPass( m_name
			, std::move( createInfo ) );

		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassSpot::doCleanup( RenderDevice const & device )
	{
		m_renderQueue.cleanup();
		getCuller().getCamera().detach();
	}

	void ShadowMapPassSpot::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		m_shadowMapUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eShadow ) ) );
	}

	void ShadowMapPassSpot::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		m_shadowMapUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eShadow ) ) );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassSpot::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = SceneRenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eShadow )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
		return uboBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassSpot::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassSpot::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, uint32_t( SmTexture::eCount ) - 1u );
	}

	void ShadowMapPassSpot::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::eShadowMapSpot );
	}

	ShaderPtr ShadowMapPassSpot::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		auto position = writer.declInput< Vec4 >( "position"
			, SceneRenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( "normal"
			, SceneRenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( "tangent"
			, SceneRenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( "uv"
			, SceneRenderPass::VertexInputs::TextureLocation
			, hasTextures );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, SceneRenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, SceneRenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, SceneRenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, SceneRenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( "transform"
			, SceneRenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( "material"
			, SceneRenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, SceneRenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inNormal2 = writer.declInput< Vec3 >( "inNormal2"
			, SceneRenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTangent2 = writer.declInput< Vec3 >( "inTangent2"
			, SceneRenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
			, SceneRenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) && hasTextures );
		auto in = writer.getIn();

		UBO_MATRIX( writer, uint32_t( NodeUboIdx::eMatrix ), 0 );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0 );
		auto skinningData = SkinningUbo::declare( writer, uint32_t( NodeUboIdx::eSkinning ), 0, flags.programFlags );
		UBO_MORPHING( writer, uint32_t( NodeUboIdx::eMorphing ), 0, flags.programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, SceneRenderPass::VertexOutputs::MaterialLocation );
		auto vtx_cameraPosition = writer.declOutput< Vec3 >( "vtx_cameraPosition"
			, 12u );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto vertexPosition = writer.declLocale( "vertexPosition"
				, vec4( position.xyz(), 1.0_f ) );
			auto v4Normal = writer.declLocale( "v4Normal"
				, vec4( normal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( "v4Tangent"
				, vec4( tangent, 0.0_f ) );
			vtx_texture = uv;

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, transform );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, c3d_curMtxModel );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, mat3( c3d_mtxNormal ) );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = writer.cast< UInt >( material );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				vertexPosition = vec4( sdw::mix( vertexPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				vtx_texture = sdw::mix( vtx_texture, inTexture2, vec3( c3d_time ) );
			}

			auto mtxModel = writer.getVariable< Mat4 >( "mtxModel" );
			auto mtxNormal = writer.getVariable< Mat3 >( "mtxNormal" );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_cameraPosition = c3d_matrixData.getCurViewCenter();
			vertexPosition = mtxModel * vertexPosition;
			vtx_worldPosition = vertexPosition.xyz();

			auto tbn = writer.declLocale( "tbn"
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
			vtx_tangentSpaceViewPosition = tbn * vtx_cameraPosition.xyz();

			out.vtx.position = c3d_matrixData.worldToCurProj( vertexPosition );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassSpot::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, SceneRenderPass::VertexOutputs::MaterialLocation );
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
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
			, LightType::eSpot
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				pxl_normalLinear = vec4( 0.0_f );
				pxl_variance = vec2( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
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
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );

				if ( hasTextures )
				{
					lighting->computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
					, lighting->getSpotLight( writer.cast< Int >( c3d_lightIndex ) ) );
				auto lightToVertex = writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - vtx_worldPosition );
				auto distance = writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto attenuation = writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( diffuse
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = depth * depth;

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassSpot::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, SceneRenderPass::VertexOutputs::MaterialLocation );
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
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
			, LightType::eSpot
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				pxl_normalLinear = vec4( 0.0_f );
				pxl_variance = vec2( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
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
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );

				if ( hasTextures )
				{
					lighting->computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
					, lighting->getSpotLight( writer.cast< Int >( c3d_lightIndex ) ) );
				auto lightToVertex = writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - vtx_worldPosition );
				auto distance = writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto attenuation = writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = depth * depth;

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassSpot::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, SceneRenderPass::VertexOutputs::MaterialLocation );
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
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
			, LightType::eSpot
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				pxl_normalLinear = vec4( 0.0_f );
				pxl_variance = vec2( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
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
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );

				if ( hasTextures )
				{
					lighting->computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
					, lighting->getSpotLight( writer.cast< Int >( c3d_lightIndex ) ) );
				auto lightToVertex = writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - vtx_worldPosition );
				auto distance = writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto attenuation = writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = depth * depth;

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
