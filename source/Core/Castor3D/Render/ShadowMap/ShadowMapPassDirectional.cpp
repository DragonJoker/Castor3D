#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/InstantiatedFrustumCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapDirectionalUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void fillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, Scene const & scene
			, ShadowMapDirectionalUbo const & shadowMapDirectionalUbo
			, ShadowMapUbo const & shadowMapUbo )
		{
			auto index = uint32_t( PassUboIdx::eCount );
			auto & flags = pipeline.getFlags();
			descriptorWrites.push_back( scene.getLightCache().getDescriptorWrite( index++ ) );
#if C3D_UseTiledDirectionalShadowMap
			descriptorWrites.push_back( shadowMapDirectionalUbo.getDescriptorWrite( index++ ) );
#else
			descriptorWrites.push_back( shadowMapUbo.getDescriptorWrite( index++ ) );
#endif
		}

		castor::String getPassName( uint32_t cascadeIndex )
		{
			return cuT( "DirectionalSMC" ) + string::toString( cascadeIndex );
		}
	}

	uint32_t const ShadowMapPassDirectional::TileSize = 2048u;
	uint32_t const ShadowMapPassDirectional::TileCountX = ShadowMapDirectionalTileCountX;
	uint32_t const ShadowMapPassDirectional::TileCountY = ShadowMapDirectionalTileCountY;

	ShadowMapPassDirectional::ShadowMapPassDirectional( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, uint32_t cascadeCount )
#if C3D_UseTiledDirectionalShadowMap
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, cuT( "DirectionalSM" )
			, matrixUbo
			, culler
			, shadowMap
			, createRenderPass( device, shadowMap, cuT( "DirectionalSM" ) )
			, cascadeCount }
#else
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, getPassName( cascadeCount )
			, matrixUbo
			, culler
			, shadowMap
			, 1u }
#endif
		, m_shadowMapDirectionalUbo{ device }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
		getCuller().getCamera().detach();
	}

	bool ShadowMapPassDirectional::update( CpuUpdater & updater )
	{
#if C3D_UseTiledDirectionalShadowMap
		auto & light = *updater.light;
		auto & camera = *updater.camera;
		auto & directional = *light.getDirectionalLight();
		auto & culler = static_cast< InstantiatedFrustumCuller & >( getCuller() );
		std::vector< Frustum > frustums;

		for ( uint32_t cascade = 0u; cascade < getInstanceMult(); ++cascade )
		{
			frustums.emplace_back( camera.getViewport() );
			frustums.back().update( directional.getProjMatrix( cascade )
				, directional.getViewMatrix( cascade ) );
		}

		culler.setFrustums( std::move( frustums ) );
		culler.compute();
		m_outOfDate = m_outOfDate
			|| culler.areAllChanged()
			|| culler.areCulledChanged();
#else
		getCuller().compute();
#endif
		m_outOfDate = true;
		SceneRenderPass::update( updater );
		return m_outOfDate;
	}

	void ShadowMapPassDirectional::update( GpuUpdater & updater )
	{
		if ( m_initialised )
		{
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	void ShadowMapPassDirectional::doUpdateUbos( CpuUpdater & updater )
	{
		SceneRenderPass::doUpdateUbos( updater );
#if C3D_UseTiledDirectionalShadowMap
		m_shadowMapDirectionalUbo.update( *updater.light->getDirectionalLight() );
#else
		m_shadowMapUbo.update( *updater.light, updater.index );
#endif
	}

	void ShadowMapPassDirectional::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( PassUboIdx::eCount );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
	}

	void ShadowMapPassDirectional::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( pipeline
			, descriptorWrites
			, getCuller().getScene()
			, m_shadowMapDirectionalUbo
			, m_shadowMapUbo );
	}

	void ShadowMapPassDirectional::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( pipeline
			, descriptorWrites
			, getCuller().getScene()
			, m_shadowMapDirectionalUbo
			, m_shadowMapUbo );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassDirectional::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassDirectional::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, uint32_t( SmTexture::eCount ) - 1u );
	}

	void ShadowMapPassDirectional::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::eShadowMapDirectional );
#if C3D_UseTiledDirectionalShadowMap
		addFlag( flags.programFlags, ProgramFlag::eInstanceMult );
#endif
	}

	ShaderPtr ShadowMapPassDirectional::doGetVertexShaderSource( PipelineFlags const & flags )const
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
#if C3D_UseTiledDirectionalShadowMap
		UBO_MODEL_INSTANCES( writer
			, uint32_t( NodeUboIdx::eModelInstances )
			, RenderPipeline::eBuffers );

		auto index = uint32_t( PassUboIdx::eCount ) + 1u;
		UBO_SHADOWMAP_DIRECTIONAL( writer
			, index++
			, RenderPipeline::eAdditional );
#else
		auto index = uint32_t( PassUboIdx::eCount ) + 1u;
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
#endif

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

#if C3D_UseTiledDirectionalShadowMap
			auto ti = writer.declLocale( "tileIndex"
				, c3d_shadowMapDirectionalData.getTileIndex( c3d_modelInstancesData, in ) );
			auto tileMin = writer.declLocale( "tileMin"
				, c3d_shadowMapDirectionalData.getTileMin( ti ) );
			auto tileMax = writer.declLocale( "tileMax"
				, c3d_shadowMapDirectionalData.getTileMax( tileMin ) );

			curPosition = c3d_shadowMapDirectionalData.worldToView( ti, curPosition );
			curPosition = c3d_shadowMapDirectionalData.viewToProj( ti, curPosition );
			out.vtx.position = curPosition;

			out.vtx.clipDistance[0] = dot( vec4( 1.0_f, 0.0_f, 0.0_f, -tileMin.x() ), curPosition );
			out.vtx.clipDistance[1] = dot( vec4( -1.0_f, 0.0_f, 0.0_f, tileMax.x() ), curPosition );
			out.vtx.clipDistance[2] = dot( vec4( 0.0_f, -1.0_f, 0.0_f, -tileMin.y() ), curPosition );
			out.vtx.clipDistance[3] = dot( vec4( 0.0_f, 1.0_f, 0.0_f, tileMax.y() ), curPosition );
#else
			curPosition = c3d_shadowMapData.worldToView( curPosition );
			out.vtx.position = c3d_shadowMapData.viewToProj( curPosition );
#endif

			auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
				, c3d_modelData.getNormalMtx( flags.programFlags, mtxModel ) );
			outSurface.computeTangentSpace( flags.programFlags
				, vec3( 0.0_f )
				, mtxNormal
				, v4Normal
				, v4Tangent );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassDirectional::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
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
		auto lightsIndex = index++;
#if C3D_UseTiledDirectionalShadowMap
		UBO_SHADOWMAP_DIRECTIONAL( writer
			, index++
			, RenderPipeline::eAdditional );
#else
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
#endif
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, LightType::eDirectional
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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
#if C3D_UseTiledDirectionalShadowMap
				auto light = writer.declLocale( "light"
					, c3d_shadowMapDirectionalData.getDirectionalLight( *lighting ) );
#else
				auto light = writer.declLocale( "light"
					, c3d_shadowMapData.getDirectionalLight( *lighting ) );
#endif
				pxl_flux.rgb() = diffuse
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					/** clamp( dot( normalize( light.m_direction ), normal ), 0.0_f, 1.0_f )*/;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
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
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassDirectional::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
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
		auto lightsIndex = index++;
#if C3D_UseTiledDirectionalShadowMap
		UBO_SHADOWMAP_DIRECTIONAL( writer
			, index++
			, RenderPipeline::eAdditional );
#else
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
#endif
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, LightType::eDirectional
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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
#if C3D_UseTiledDirectionalShadowMap
				auto light = writer.declLocale( "light"
					, c3d_shadowMapDirectionalData.getDirectionalLight( *lighting ) );
#else
				auto light = writer.declLocale( "light"
					, c3d_shadowMapData.getDirectionalLight( *lighting ) );
#endif
				pxl_flux.rgb() = albedo
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					/** clamp( dot( normalize( light.m_direction ), normal ), 0.0_f, 1.0_f )*/;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
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
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassDirectional::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
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
		auto lightsIndex = index++;
#if C3D_UseTiledDirectionalShadowMap
		UBO_SHADOWMAP_DIRECTIONAL( writer
			, index++
			, RenderPipeline::eAdditional );
#else
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
#endif
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, LightType::eDirectional
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::eAdditional );

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
#if C3D_UseTiledDirectionalShadowMap
				auto light = writer.declLocale( "light"
					, c3d_shadowMapDirectionalData.getDirectionalLight( *lighting ) );
#else
				auto light = writer.declLocale( "light"
					, c3d_shadowMapData.getDirectionalLight( *lighting ) );
#endif
				pxl_flux.rgb() = albedo
					* light.m_lightBase.m_colour
					* light.m_lightBase.m_intensity.x()
					/** clamp( dot( normalize( light.m_direction ), normal ), 0.0_f, 1.0_f )*/;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
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
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
