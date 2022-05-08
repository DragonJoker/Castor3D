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

namespace castor3d
{
	namespace shdpasspoint
	{
		static castor::String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm )
		{
			auto result = cuT( "PointSML" ) + castor::string::toString( index / 6u ) + "F" + castor::string::toString( index % 6u );

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
			, shdpasspoint::getPassName( index, needsVsm, needsRsm )
			, matrixUbo
			, culler
			, shadowMap
			, needsVsm
			, needsRsm }
	{
		log::trace << "Created " << m_name << std::endl;
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
			, m_projection );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
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

	PassFlags ShadowMapPassPoint::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags ShadowMapPassPoint::doAdjustProgramFlags( ProgramFlags flags )const
	{
		addFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::eShadowMapPoint );

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

	ShaderPtr ShadowMapPassPoint::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.submeshFlags, SubmeshFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.submeshFlags, SubmeshFlag::eMorphing ) ? 1 : 0 ) < 2
			&& "Can't have both instantiation and morphing yet." );
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Morphing( writer
			, GlobalBuffersIdx::eMorphingData
			, RenderPipeline::eBuffers
			, flags.submeshFlags );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, RenderPipeline::eBuffers
			, flags.submeshFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.submeshFlags
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
				auto ids = shader::getIds( c3d_objectIdsData
					, in
					, pipelineID
					, in.drawID
					, flags.submeshFlags );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( in.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( in.tangent, 0.0_f ) );
				out.texture0 = in.texture0;
				auto morphingData = writer.declLocale( "morphingData"
					, c3d_morphingData[ids.morphingId]
					, checkFlag( flags.submeshFlags, SubmeshFlag::eMorphing ) );
				in.morph( morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, out.texture0 );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[ids.nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( ids.nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getCurModelMtx( flags.submeshFlags
						, skinningData
						, ids.skinningId
						, in.boneIds0
						, in.boneIds1
						, in.boneWeights0
						, in.boneWeights1 ) );
				auto worldPos = writer.declLocale( "worldPos"
					, mtxModel * curPosition );
				out.worldPosition = worldPos;
				out.vtx.position = c3d_matrixData.worldToCurProj( worldPos );

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, modelData.getNormalMtx( flags.submeshFlags, mtxModel ) );
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
		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		shader::Utils utils{ writer, *getEngine() };

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
		auto lightingModel = shader::LightingModel::createModel( utils
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
			, hasTextures ) );

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
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1] );
				pxl_linear = 0.0_f;
				pxl_variance = vec2( 0.0_f );
				pxl_normal = vec4( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto texCoord = writer.declLocale( "texCoord"
					, in.texture0
					, hasTextures );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal )
					, m_needsRsm );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent )
					, m_needsRsm );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent )
					, m_needsRsm );
				auto material = materials.getMaterial( modelData.getMaterialId() );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive )
					, m_needsRsm );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity
					, ( checkFlag( flags.texturesFlags, TextureFlag::eOpacity )
						&& flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef
					, flags.alphaFunc != VK_COMPARE_OP_ALWAYS );
				auto lightMat = lightingModel->declMaterial( "lightMat"
					, m_needsRsm );
				lightMat->create( material );

				if ( hasTextures )
				{
					if ( m_needsRsm )
					{
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
					}
					else
					{
						textureConfigs.computeGeometryMapContributions( utils
							, flags.passFlags
							, flags.textures
							, textureAnims
							, c3d_maps
							, modelData.getTextures0()
							, modelData.getTextures1()
							, texCoord
							, alpha
							, in.tangentSpaceViewPosition
							, in.tangentSpaceFragPosition );
					}
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, alphaRef );

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
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto light = writer.declLocale( "light"
						, c3d_shadowMapData.getPointLight( *lightingModel ) );
					auto lightToVertex = writer.declLocale( "lightToVertex"
						, light.position - in.worldPosition.xyz() );
					auto distance = writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto attenuation = writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					pxl_flux.rgb() = ( lightMat->albedo
							* light.base.colour
							* light.base.intensity.x()
							* clamp( dot( lightToVertex / distance, normal ), 0.0_f, 1.0_f ) )
						/ attenuation;
					pxl_normal.xyz() = normal;
					pxl_position.xyz() = in.worldPosition.xyz();
				}
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
