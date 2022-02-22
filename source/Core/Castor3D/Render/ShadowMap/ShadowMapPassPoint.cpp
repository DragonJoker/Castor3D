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
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
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
		castor::String getPassName( uint32_t index )
		{
			return cuT( "PointSML" ) + string::toString( index / 6u ) + "F" + string::toString( index % 6u );
		}
	}

	uint32_t const ShadowMapPassPoint::TextureSize = 512u;
	castor::String const ShadowMapPassPoint::Type = "c3d.shadows.point";

	ShadowMapPassPoint::ShadowMapPassPoint( crg::FramePass const & pass
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
			, Type
			, getPassName( index )
			, matrixUbo
			, culler
			, shadowMap }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::update( CpuUpdater & updater )
	{
		getCuller().compute();
		m_outOfDate = m_outOfDate
			|| getCuller().areAllChanged()
			|| getCuller().areCulledChanged();
		RenderNodesPass::update( updater );
	}

	void ShadowMapPassPoint::update( GpuUpdater & updater )
	{
		if ( m_initialised )
		{
			doUpdateNodes( m_renderQueue->getCulledRenderNodes() );
		}
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
			, m_projection );
	}

	void ShadowMapPassPoint::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		RenderNodesPass::doUpdate( nodes.instancedStaticNodes.backCulled );
		RenderNodesPass::doUpdate( nodes.staticNodes.backCulled );
		RenderNodesPass::doUpdate( nodes.skinnedNodes.backCulled );
		RenderNodesPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
		RenderNodesPass::doUpdate( nodes.morphingNodes.backCulled );
		RenderNodesPass::doUpdate( nodes.billboardNodes.backCulled );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
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
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_Morphing( writer
			, GlobalBuffersIdx::eMorphingData
			, RenderPipeline::ePass
			, flags.programFlags );

		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, uint32_t( NodeUboIdx::eSkinningBones )
			, RenderPipeline::ePass
			, RenderPipeline::eBuffers
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
			, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( in.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( in.tangent, 0.0_f ) );
				auto objectIdsData = writer.declLocale( "objectIdsData"
					, c3d_objectIdsData[pipelineID][customDrawID] );
				auto nodeId = writer.declLocale( "nodeId"
					, shader::ObjectsIds::getNodeId( objectIdsData ) );
				auto morphingId = writer.declLocale( "morphingId"
					, shader::ObjectsIds::getMorphingId( objectIdsData )
					, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
				auto skinningId = writer.declLocale( "skinningId"
					, shader::ObjectsIds::getSkinningId( objectIdsData )
					, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
				out.texture0 = in.texture0;
				auto morphingData = writer.declLocale( "morphingData"
					, c3d_morphingData[morphingId]
					, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
				in.morph( morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, out.texture0 );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId] );
				out.textures0 = modelData.getTextures0( flags.programFlags
					, in.textures0 );
				out.textures1 = modelData.getTextures1( flags.programFlags
					, in.textures1 );
				out.textures = modelData.getTextures( flags.programFlags
					, in.textures );
				out.material = modelData.getMaterialId( flags.programFlags
					, in.material );
				out.nodeId = writer.cast< sdw::Int >( nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getCurModelMtx( flags.programFlags
						, skinningData
						, skinningId
						, in.vertexIndex - in.baseVertex ) );
				auto worldPos = writer.declLocale( "worldPos"
					, mtxModel * curPosition );
				out.worldPosition = worldPos;
				out.vtx.position = c3d_matrixData.worldToCurProj( worldPos );

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, modelData.getNormalMtx( flags.programFlags, mtxModel ) );
				out.computeTangentSpace( flags.programFlags
					, c3d_matrixData.getCurViewCenter()
					, worldPos.xyz()
					, mtxNormal
					, v4Normal
					, v4Tangent );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassPoint::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		
		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		shader::Utils utils{ writer, *getEngine() };

		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::ePass };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::ePass
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::ePass
			, hasTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		UBO_SHADOWMAP( writer
			, index++
			, RenderPipeline::ePass );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, LightType::ePoint
			, lightsIndex
			, RenderPipeline::ePass
			, false
			, shader::ShadowOptions{ SceneFlag::eNone, false }
			, index
			, RenderPipeline::ePass
			, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_normalLinear( writer.declOutput< Vec4 >( "pxl_normalLinear", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

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
				pxl_normalLinear = vec4( 0.0_f );
				pxl_variance = vec2( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto texCoord = writer.declLocale( "texCoord"
					, in.texture0 );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );
				auto material = materials.getMaterial( in.material );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( material );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( hasTextures )
				{
					lightingModel->computeMapContributions( flags.passFlags
						, flags.textures
						, textureConfigs
						, textureAnims
						, c3d_maps
						, in.textures0
						, in.textures1
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
					, c3d_shadowMapData.getPointLight( *lightingModel ) );
				auto lightToVertex = writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - in.worldPosition.xyz() );
				auto distance = writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto attenuation = writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				pxl_flux.rgb() = ( lightMat->albedo
						* light.m_lightBase.m_colour
						* light.m_lightBase.m_intensity.x()
						* clamp( dot( lightToVertex / distance, normal ), 0.0_f, 1.0_f ) )
					/ attenuation;

				auto depth = writer.declLocale( "depth"
					, c3d_shadowMapData.getLinearisedDepth( in.worldPosition.xyz() ) );
				pxl_normalLinear.w() = depth;
				pxl_normalLinear.xyz() = normal;
				pxl_position.xyz() = in.worldPosition.xyz();

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
