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
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		castor::String getPassName( uint32_t index )
		{
			return cuT( "SpotSM" ) + string::toString( index );
		}
	}

	uint32_t const ShadowMapPassSpot::TextureSize = 512u;

	ShadowMapPassSpot::ShadowMapPassSpot( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, getPassName( index )
			, matrixUbo
			, culler
			, shadowMap }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
		getCuller().getCamera().detach();
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
		auto & spotLight = *light.getSpotLight();
		auto & myCamera = getCuller().getCamera();
		auto & aabb = light.getScene()->getBoundingBox();
		auto farPlane = light.getFarPlane();
		myCamera.getViewport().setPerspective( spotLight.getCutOff()
			, 1.0f
			, ( std::min( double( farPlane ), castor::point::length( aabb.getDimensions() ) ) > 1000.0
				? 1.0f
				: 0.1f )
			, farPlane );
		m_shadowType = light.getShadowType();
		light.getSpotLight()->updateShadow( myCamera
			, updater.index );
		m_shadowMapUbo.update( light, updater.index );
		m_matrixUbo.cpuUpdate( myCamera.getView()
			, myCamera.getProjection( false ) );
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

	ShaderPtr ShadowMapPassSpot::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
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
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, LightType::eSpot
			, lightsIndex
			, RenderPipeline::eAdditional
			, false
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
					, material.gamma );
				auto lightMat = writer.declLocale< shader::PhongLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::ePhong >( material.diffuse
					, material.gamma
					, material.specular
					, material.shininess );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef );
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
						, lightMat
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
					, c3d_shadowMapData.getSpotLight( *lighting ) );
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
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( lightMat.albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
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

	ShaderPtr ShadowMapPassSpot::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
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
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, LightType::eSpot
			, lightsIndex
			, RenderPipeline::eAdditional
			, false
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
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eMetallicRoughness >( material.albedo
					, material.metalness
					, material.roughness );
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( hasTextures )
				{
					lighting->computeMapContributions( flags.passFlags
						, MaterialType::eMetallicRoughness
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
						, lightMat
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
					, c3d_shadowMapData.getSpotLight( *lighting ) );
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
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( lightMat.albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
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

	ShaderPtr ShadowMapPassSpot::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
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
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, LightType::eSpot
			, lightsIndex
			, RenderPipeline::eAdditional
			, false
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
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eSpecularGlossiness >( material.albedo
					, material.specular
					, material.glossiness );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( hasTextures )
				{
					lighting->computeMapContributions( flags.passFlags
						, MaterialType::eSpecularGlossiness
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
						, lightMat
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
					, c3d_shadowMapData.getSpotLight( *lighting ) );
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
				auto lightDirection = writer.declLocale( "lightDirection"
					, lightToVertex / distance );
				auto spotFactor = writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );
				spotFactor = max( 0.0_f
					, sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f ) );
				spotFactor = 1.0_f - step( spotFactor, 0.0_f );
				pxl_flux.rgb() = ( lightMat.albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightDirection, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = spotFactor * normal;
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
