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
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
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

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	castor::String const ShadowMapPassPoint::Type = "c3d.shadows.point";

	ShadowMapPassPoint::ShadowMapPassPoint( crg::FramePass const & pass
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
			, matrixUbo
			, culler
			, shadowMap
			, needsVsm
			, needsRsm }
	{
		log::trace << "Created " << getName() << std::endl;
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::update( CpuUpdater & updater )
	{
		getCuller().update( updater );
		m_outOfDate = ++m_count < 3u
			|| m_outOfDate
			|| getCuller().areAnyChanged();
		m_count = std::min( m_count, 2u );
		RenderNodesPass::update( updater );
	}

	void ShadowMapPassPoint::updateFrustum( castor::Matrix4x4f const & viewMatrix )
	{
		static_cast< FrustumCuller & >( getCuller() ).updateFrustum( m_projection, viewMatrix );
	}

	void ShadowMapPassPoint::doUpdateUbos( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto & aabb = light.getScene()->getBoundingBox();
		auto farPlane = light.getFarPlane();
		m_shadowMapUbo.update( light, updater.index );
		auto & pointLight = *light.getPointLight();
		m_projection = m_device.renderSystem.getPerspective( 90.0_degrees
			, 1.0f
			, ( std::min( double( farPlane ), castor::point::length( aabb.getDimensions() ) ) > 1000.0
				? 1.0f
				: 0.1f )
			, farPlane );
		m_matrixUbo.cpuUpdate( pointLight.getViewMatrix( CubeMapFace( updater.index ) )
			, m_projection
			, updater.camera->getFrustum() );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
	{
		uint32_t result = 1u;
		auto needsVsm = flags.writeShadowVSM();
		auto needsRsm = flags.writeShadowRSM();

		if ( needsVsm )
		{
			++result;
		}

		if ( needsRsm )
		{
			result += 3;
		}

		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, result );
	}

	ProgramFlags ShadowMapPassPoint::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	ShaderPtr ShadowMapPassPoint::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, ComponentModeFlag::eNone
			, utils };

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( VertexInT< shader::VertexSurfaceT > in
			, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags ) );
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
				material.getPassMultipliers( flags
					, in.passMasks
					, out.passMultipliers );
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getModelMtx() );

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags
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
						, modelData.getNormalMtx( flags, mtxModel ) );
					out.computeTangentSpace( flags
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

	ShaderPtr ShadowMapPassPoint::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		bool enableTextures = flags.enableTextures();
		auto needsVsm = flags.writeShadowVSM();
		auto needsRsm = flags.writeShadowRSM();

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers
			, needsRsm || passShaders.enableOpacity() };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_ShadowMap( writer
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( *getEngine()
			, materials
			, utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, LightType::ePoint
			, lightsIndex
			, RenderPipeline::eBuffers
			, false
			, shader::ShadowOptions{ SceneFlag::eNone, true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		// Fragment Outputs
		uint32_t outIndex{};
		auto pxl_linear( writer.declOutput< Float >( "pxl_linear", outIndex++ ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", needsVsm ? outIndex++ : 0u, needsVsm ) );
		auto pxl_normal( writer.declOutput< Vec4 >( "pxl_normal", needsRsm ? outIndex++ : 0u, needsRsm ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", needsRsm ? outIndex++ : 0u, needsRsm ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", needsRsm ? outIndex++ : 0u, needsRsm ) );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
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
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, in } );
				materials.blendMaterials( flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				auto depth = writer.declLocale( "depth"
					, c3d_shadowMapData.getNormalisedDepth( in.worldPosition.xyz() ) );
				pxl_linear = depth;

				if ( needsVsm )
				{
					pxl_variance.x() = depth;
					pxl_variance.y() = depth * depth;

					auto dx = writer.declLocale( "dx"
						, dFdx( depth ) );
					auto dy = writer.declLocale( "dy"
						, dFdy( depth ) );
					pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
				}

				if ( needsRsm )
				{
					auto light = writer.declLocale( "light"
						, c3d_shadowMapData.getPointLight( *lightingModel ) );
					auto lightToVertex = writer.declLocale( "lightToVertex"
						, light.position - in.worldPosition.xyz() );
					auto distance = writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto attenuation = writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					components.colour *= in.colour;
					pxl_flux.rgb() = ( components.colour
							* light.base.colour
							* light.base.intensity.x()
							* clamp( dot( lightToVertex / distance, components.normal ), 0.0_f, 1.0_f ) )
						/ attenuation;
					pxl_normal.xyz() = components.normal;
					pxl_position.xyz() = in.worldPosition.xyz();
				}
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
