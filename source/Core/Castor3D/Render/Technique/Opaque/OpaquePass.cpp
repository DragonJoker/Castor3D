#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static String const Output1 = "outData1";
		static String const Output2 = "outData2";
		static String const Output3 = "outData3";
		static String const Output4 = "outData4";
		static String const Output5 = "outData5";
	}

	castor::String const OpaquePass::Type = "c3d.deferred.geometry";

	OpaquePass::OpaquePass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniquePass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, category
			, name
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void OpaquePass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
			auto flags = visitor.getFlags();
			auto shaderProgram = doGetProgram( flags );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
	}

	TextureFlags OpaquePass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	ShaderFlags OpaquePass::getShaderFlags()const
	{
		return ShaderFlag::eTangentSpace
			| ShaderFlag::eVelocity;
	}

	ProgramFlags OpaquePass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		remFlag( flags, ProgramFlag::eLighting );
		return flags;
	}

	SceneFlags OpaquePass::doAdjustSceneFlags( SceneFlags flags )const
	{
		remFlag( flags, SceneFlag::eLpvGI );
		remFlag( flags, SceneFlag::eLayeredLpvGI );
		remFlag( flags, SceneFlag::eVoxelConeTracing );
		return flags;
	}

	void OpaquePass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void OpaquePass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	ashes::PipelineColorBlendStateCreateInfo OpaquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 4u );
	}

	ShaderPtr OpaquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr OpaquePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

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

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		auto index = 0u;
		auto outData2 = writer.declOutput< Vec4 >( Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( Output5, index++ );

		shader::Utils utils{ writer, *renderSystem.getEngine() };
		auto lightingModel = utils.createLightingModel( shader::getLightingModelName( *getEngine(), flags.passType )
			, {}
			, nullptr
			, true );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
			, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto texCoord = writer.declLocale( "texCoord"
					, in.texture0 );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( material );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				lightingModel->computeMapContributions( flags.passFlags
					, flags.textures
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getTextures0()
					, modelData.getTextures1()
					, texCoord
					, normal
					, tangent
					, bitangent
					, emissive
					, alpha
					, occlusion
					, transmittance
					, *lightMat
					, in.tangentSpaceViewPosition
					, in.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.alphaRef );
				lightMat->output( outData2, outData3 );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( in.getVelocity(), 0.0_f, occlusion );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
