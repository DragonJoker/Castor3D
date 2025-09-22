#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapping.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmConfig.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>
#include <random>

CU_ImplementSmartPtr( c3d, RsmGIPass )

namespace c3d
{
	namespace rsmgi
	{
		enum Idx : uint32_t
		{
			RsmCfgUboIdx,
			RsmSamplesIdx,
			CameraUboIdx,
			ShadowsIdx,
			DepthObjMapIdx,
			NmlOccMapIdx,
			RsmNormalsIdx,
			RsmPositionIdx,
			RsmFluxIdx,
		};

		static std::unique_ptr< ast::Shader > getVertexProgram()
		{
			sdw::VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< sdw::Vec2 >( "position", 0u );
			auto uv = writer.declInput< sdw::Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< sdw::Vec2 >( "vtx_texture", 0u );

			writer.implementMain( [&vtx_texture, &uv, &position]( sdw::VertexIn const & in
				, sdw::VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getDirectionalPixelShaderSource()
		{
			sdw::FragmentWriter writer;

			// Shader inputs
			C3D_RsmConfig( writer, RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			C3D_Camera( writer, CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux ), RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, Idx::ShadowsIdx
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					sdwIF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					sdwFI

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_cameraData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, nmlOcc.xyz() );
					auto shadowData = writer.declLocale( "shadowData"
						, shadows.getDirectionalShadows() );
					pxl_rsmGI = rsm.directional( shadowData
						, vsPosition
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getSpotPixelShaderSource()
		{
			sdw::FragmentWriter writer;

			// Shader inputs
			C3D_RsmConfig( writer, RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			C3D_Camera( writer, CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux ), RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, Idx::ShadowsIdx
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					sdwIF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					sdwFI

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, nmlOcc.xyz() );

					sdwIF( writer, dot( wsNormal, wsNormal ) == 0.0f )
					{
						writer.demote();
					}
					sdwFI

					auto shadowData = writer.declLocale( "shadowData"
						, shadows.getSpotShadows( c3d_rsmConfigData.index ) );
					pxl_rsmGI = rsm.spot( shadowData
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPointPixelShaderSource()
		{
			sdw::FragmentWriter writer;

			// Shader inputs
			C3D_RsmConfig( writer, RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, RsmSamplesIdx
				, 0u
				, true };
			C3D_Camera( writer, CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux ), RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, Idx::ShadowsIdx
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					sdwIF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					sdwFI

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, nmlOcc.xyz() );
					auto shadowData = writer.declLocale( "shadowData"
						, shadows.getPointShadows( c3d_rsmConfigData.index ) );
					pxl_rsmGI = rsm.point( shadowData
						, shadowData.position().xyz()
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram( LightType lightType )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return getDirectionalPixelShaderSource();
			case LightType::eSpot:
				return getSpotPixelShaderSource();
			case LightType::ePoint:
				return getPointPixelShaderSource();
			default:
				CU_Failure( "Unexpected LightType" );
				return nullptr;
			}
		}

		static crg::rq::Config getConfig( Extent2D const & renderSize
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			crg::rq::Config result;
			result.texcoordConfig( {} );
			result.renderSize( renderSize );
			result.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) );
			return result;
		}
	}

	//*********************************************************************************************

	RsmGIPass::RsmGIPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, Extent3D const & size
		, CameraUbo const & cameraUbo
		, Texture const & depthObj
		, Texture const & nmlOcc
		, ShadowMapResult const & smResult
		, Texture & gi
		, Texture & nml )
		: Named{ c3d::getName( lightType ) + "Rsm" }
		, m_rsmConfigUbo{ device }
		, m_rsmSamplesSsbo{ device.bufferPool->getBuffer< Point4f >( BufferUsageFlags::eStorageBuffer
			, MaxRsmRange
			, MemoryPropertyFlags::eHostVisible ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), rsmgi::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), rsmgi::getPixelProgram( lightType ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto rng = device.renderSystem.getEngine()->createRandomEngine();
		std::uniform_real_distribution< float > dist( 0.0f, 1.0f );

		for ( auto & point : m_rsmSamplesSsbo.getData() )
		{
			auto xi2 = dist( rng );
			auto twoPIy = PiMult2< float > *xi2;
			auto xi1 = dist( rng );
			point[0] = float( xi1 * sin( twoPIy ) );
			point[1] = float( xi1 * cos( twoPIy ) );
			point[2] = float( xi1 );
			point[3] = float( xi2 );
		}

		{
			InstantDirectUploadData uploader{ *device.transferQueue
				, device, cuT( "RSMSamplesUpload" ), *device.transferCommandPool };
			m_rsmSamplesSsbo.upload( uploader, VertexUniformReadState );
		}

		auto & pass = graph.createPass( getName()
			, [this, &device, size]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = makeRawUnique< crg::RenderQuad >( framePass
					, context
					, runnableGraph
					, crg::ru::Config{ 1u, false }
					, rsmgi::getConfig( { size.width, size.height }
						, m_stages ) );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		m_rsmConfigUbo.createPassBinding( pass, rsmgi::RsmCfgUboIdx );
		pass.addInputStorageBuffer( m_rsmSamplesSsbo.getBuffer().bufferViewId, rsmgi::RsmSamplesIdx );
		cameraUbo.createPassBinding( pass, rsmgi::CameraUboIdx );
		shadowBuffer.createPassBinding( pass, rsmgi::ShadowsIdx );
		pass.addInputSampledImage( depthObj.getSampledViewId(), rsmgi::DepthObjMapIdx );
		pass.addInputSampledImage( nmlOcc.getSampledViewId(), rsmgi::NmlOccMapIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eNormal ), rsmgi::RsmNormalsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::ePosition ), rsmgi::RsmPositionIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eFlux ), rsmgi::RsmFluxIdx );

		gi.setLastAttach( pass.addOutputColourTarget( gi.getTargetViewId(), transparentBlackClearColor ) );
		nml.setLastAttach( pass.addOutputColourTarget( nml.getTargetViewId(), transparentBlackClearColor ) );
	}

	void RsmGIPass::accept( ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void RsmGIPass::update( LightInstance const & light )
	{
		m_rsmConfigUbo.cpuUpdate( light.getRsmConfig()
			, uint32_t( light.getShadowMapIndex() ) );
	}
}
