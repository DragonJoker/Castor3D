#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	String const OpaquePass::Output1 = "outData1";
	String const OpaquePass::Output2 = "outData2";
	String const OpaquePass::Output3 = "outData3";
	String const OpaquePass::Output4 = "outData4";
	String const OpaquePass::Output5 = "outData5";

	OpaquePass::OpaquePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, SceneRenderPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniquePass{ pass
			, context
			, graph
			, device
			, category
			, name
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	void OpaquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto flags = visitor.getFlags();
		auto shaderProgram = doGetProgram( flags );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	TextureFlags OpaquePass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	void OpaquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing );
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	void OpaquePass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void OpaquePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	void OpaquePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	ashes::PipelineColorBlendStateCreateInfo OpaquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 4u );
	}

	ShaderPtr OpaquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr OpaquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		auto textures = filterTexturesFlags( flags.textures );
		shader::PhongMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTextures )
				, RenderPipeline::eBuffers );
		}

		// UBOs
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

		// Fragment Inputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		// Fragment Outputs
		auto index = 0u;
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::PhongLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inSurface.material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto normal = writer.declLocale( "normal"
					, normalize( inSurface.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inSurface.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inSurface.bitangent ) );
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

				lightingModel.computeMapContributions( flags.passFlags
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
					, inSurface.tangentSpaceViewPosition
					, inSurface.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.alphaRef );
				outData2 = vec4( lightMat.albedo, lightMat.shininess );
				outData3 = vec4( lightMat.specular, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( inSurface.getVelocity(), writer.cast< Float >( inSurface.material ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		auto textures = filterTexturesFlags( flags.textures );
		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTextures )
				, RenderPipeline::eBuffers );
		}

		// UBOs
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

		// Fragment Inputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		// Fragment Outputs
		auto index = 0u;
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::PbrLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inSurface.material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto normal = writer.declLocale( "normal"
					, normalize( inSurface.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inSurface.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inSurface.bitangent ) );
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eMetallicRoughness >( material.albedo
					, material.metalness
					, material.roughness );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto occlusion = writer.declLocale( "ambientOcclusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				lightingModel.computeMapContributions( flags.passFlags
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
					, inSurface.tangentSpaceViewPosition
					, inSurface.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.alphaRef );
				outData2 = vec4( lightMat.albedo, lightMat.roughness );
				outData3 = vec4( lightMat.metalness, 0.0_f, 0.0_f, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( inSurface.getVelocity(), writer.cast< Float >( inSurface.material ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		auto textures = filterTexturesFlags( flags.textures );
		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTextures )
				, RenderPipeline::eBuffers );
		}

		// UBOs
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

		// Fragment Inputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		// Fragment Outputs
		auto index = 0u;
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::PbrLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inSurface.material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( inSurface.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inSurface.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inSurface.bitangent ) );
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eSpecularGlossiness >( material.albedo
					, material.specular
					, material.glossiness );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto occlusion = writer.declLocale( "ambientOcclusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				lightingModel.computeMapContributions( flags.passFlags
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
					, inSurface.tangentSpaceViewPosition
					, inSurface.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.alphaRef );
				outData2 = vec4( lightMat.albedo, 1.0_f - lightMat.roughness );
				outData3 = vec4( lightMat.specular, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( inSurface.getVelocity(), writer.cast< Float >( inSurface.material ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
