#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shdpassspot
	{
		static castor::String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm )
		{
			auto result = cuT( "SpotSM" ) + castor::string::toString( index );

			if ( needsVsm )
			{
				result += "_VSM";
			}

			if ( needsRsm )
			{
				result += "_RSM";
			}

			return result;
		}
	}

	castor::String const ShadowMapPassSpot::Type = "c3d.shadows.spot";

	ShadowMapPassSpot::ShadowMapPassSpot( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, bool needsVsm
		, bool needsRsm )
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, Type
			, shdpassspot::getPassName( index, needsVsm, needsRsm )
			, matrixUbo
			, culler
			, shadowMap
			, needsVsm
			, needsRsm }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
		getCuller().getCamera().detach();
	}

	void ShadowMapPassSpot::update( CpuUpdater & updater )
	{
		getCuller().update( updater );
		m_outOfDate = m_outOfDate
			|| getCuller().areAnyChanged();
		RenderNodesPass::update( updater );
	}

	void ShadowMapPassSpot::doUpdateUbos( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto & myCamera = getCuller().getCamera();
		m_shadowType = light.getShadowType();
		m_shadowMapUbo.update( light
			, updater.index );
		m_matrixUbo.cpuUpdate( myCamera.getView()
			, myCamera.getProjection( false )
			, myCamera.getFrustum() );
		auto angle = light.getSpotLight()->getOuterCutOff().radians();

		if ( angle != m_angle )
		{
			m_outOfDate = true;
			m_angle = angle;
		}
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassSpot::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassSpot::doCreateBlendState( PipelineFlags const & flags )const
	{
		uint32_t result = 1u;

		if ( m_needsVsm )
		{
			++result;
		}

		if ( m_needsRsm )
		{
			result += 3;
		}

		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, result );
	}

	PassFlags ShadowMapPassSpot::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags ShadowMapPassSpot::doAdjustProgramFlags( ProgramFlags flags )const
	{
		addFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::eShadowMapSpot );

		if ( m_needsVsm )
		{
			addFlag( flags, ProgramFlag::eVsmShadowMap );
		}

		if ( m_needsRsm )
		{
			addFlag( flags, ProgramFlag::eRsmShadowMap );
		}

		return flags;
	}

	ShaderPtr ShadowMapPassSpot::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
			, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags.programFlags ) );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto curTangent = writer.declLocale( "curTangent"
					, in.tangent );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				out.colour = in.colour;
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				material.getPassMultipliers( flags.submeshFlags
					, in.passMasks
					, out.passMultipliers );
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getModelMtx() );

				if ( checkFlag( flags.submeshFlags, SubmeshFlag::eVelocity ) )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, c3d_matrixData.getCurViewCenter()
						, worldPos.xyz()
						, curNormal
						, curTangent );
				}
				else
				{
					auto worldPos = writer.declLocale( "worldPos"
						, mtxModel * curPosition );
					auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
						, modelData.getNormalMtx( flags.programFlags, mtxModel ) );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, c3d_matrixData.getCurViewCenter()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				out.vtx.position = c3d_matrixData.worldToCurProj( worldPos );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassSpot::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		shader::Utils utils{ writer };

		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers
			, ( m_needsRsm
				|| ( checkFlag( flags.texturesFlags, TextureFlag::eOpacity )
					&& flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) ) };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_ShadowMap( writer
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( *renderSystem.getEngine()
			, utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, LightType::eSpot
			, lightsIndex
			, RenderPipeline::eBuffers
			, false
			, shader::ShadowOptions{ SceneFlag::eNone, m_needsVsm, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		// Fragment Outputs
		uint32_t outIndex{};
		auto pxl_linear( writer.declOutput< Float >( "pxl_linear", outIndex++ ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", m_needsVsm ? outIndex++ : 0u, m_needsVsm ) );
		auto pxl_normal( writer.declOutput< Vec4 >( "pxl_normal", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				pxl_linear = 0.0_f;
				pxl_variance = vec2( 0.0_f );
				pxl_normal = vec4( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				shader::OpaqueBlendComponents components{ writer
					, "out"
					, in.texture0
					, in.texture1
					, in.texture2
					, in.texture3
					, { 1.0_f, ( checkFlag( flags.texturesFlags, TextureFlag::eOpacity ) && flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) }
					, { normalize( in.normal ), m_needsRsm }
					, normalize( in.tangent )
					, normalize( in.bitangent )
					, in.tangentSpaceViewPosition
					, in.tangentSpaceFragPosition
					, { 1.0_f, false }
					, { 1.0_f, false }
					, { vec3( 0.0_f ), m_needsRsm } };
				auto lightMat = materials.blendMaterials( utils
					, m_needsRsm
					, flags.alphaFunc
					, flags.passFlags
					, flags.submeshFlags
					, flags.textures
					, hasTextures
					, textureConfigs
					, textureAnims
					, *lightingModel
					, c3d_maps
					, modelData.getMaterialId()
					, in.passMultipliers
					, in.colour
					, components );

				auto depth = writer.declLocale( "depth"
					, c3d_shadowMapData.getLinearisedDepth( in.worldPosition.xyz() ) );
				pxl_linear = depth;

				if ( m_needsVsm )
				{
					pxl_variance.x() = depth;
					pxl_variance.y() = depth * depth;

					auto dx = writer.declLocale( "dx"
						, dFdx( depth ) );
					auto dy = writer.declLocale( "dy"
						, dFdy( depth ) );
					pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
				}

				if ( m_needsRsm )
				{
					auto light = writer.declLocale( "light"
						, c3d_shadowMapData.getSpotLight( *lightingModel ) );
					auto lightToVertex = writer.declLocale( "lightToVertex"
						, light.position - in.worldPosition.xyz() );
					auto distance = writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto attenuation = writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					auto lightDirection = writer.declLocale( "lightDirection"
						, lightToVertex / distance );
					auto spotFactor = writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.direction ) );
					spotFactor = max( 0.0_f
						, sdw::fma( ( spotFactor - 1.0_f )
							, 1.0_f / ( 1.0_f - light.outerCutOff )
							, 1.0_f ) );
					spotFactor = 1.0_f - step( spotFactor, 0.0_f );
					pxl_flux.rgb() = ( lightMat->albedo
							* light.base.colour
							* light.base.intensity.x()
							* clamp( dot( lightDirection, components.normal() ), 0.0_f, 1.0_f ) )
						/ attenuation;
					pxl_normal.xyz() = components.normal();
					pxl_position.xyz() = in.worldPosition.xyz();
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
