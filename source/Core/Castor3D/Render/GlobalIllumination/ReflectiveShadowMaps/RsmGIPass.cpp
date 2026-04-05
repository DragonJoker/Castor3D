#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapping.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <random>

CU_ImplementSmartPtr( c3d, RsmGIPass )

namespace c3d
{
	namespace rsmgi
	{
		enum class Idx : uint32_t
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

			writer.implementMain( [&vtx_texture, &uv, &position]( sdw::VertexIn const &
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
			C3D_RsmConfig( writer, Idx::RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, uint32_t( Idx::RsmSamplesIdx )
				, 0u
				, true };
			C3D_Camera( writer, Idx::CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", Idx::DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", Idx::NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormal ), Idx::RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition ), Idx::RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux ), Idx::RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, uint32_t( Idx::ShadowsIdx )
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
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
			C3D_RsmConfig( writer, Idx::RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, uint32_t( Idx::RsmSamplesIdx )
				, 0u
				, true };
			C3D_Camera( writer, Idx::CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", Idx::DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", Idx::NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormal ), Idx::RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition ), Idx::RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux ), Idx::RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, uint32_t( Idx::ShadowsIdx )
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
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
			C3D_RsmConfig( writer, Idx::RsmCfgUboIdx, 0u );
			sdw::ArrayStorageBufferT< sdw::Vec4 > c3d_rsmSamples{ writer
				, "c3d_rsmSamples"
				, writer.getTypesCache().getVec4F()
				, sdw::type::MemoryLayout::eStd430
				, uint32_t( Idx::RsmSamplesIdx )
				, 0u
				, true };
			C3D_Camera( writer, Idx::CameraUboIdx, 0u );
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", Idx::DepthObjMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", Idx::NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormal ), Idx::RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition ), Idx::RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux ), Idx::RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );
			auto pxl_rsmNormal = writer.declOutput< sdw::Vec3 >( "pxl_rsmNormal", 1 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, uint32_t( Idx::ShadowsIdx )
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
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
					pxl_rsmGI = rsm.point( shadowData.position().xyz()
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
		auto rng = c3d::getEngine( device ).createRandomEngine();
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
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		m_rsmConfigUbo.createPassBinding( pass, rsmgi::Idx::RsmCfgUboIdx );
		pass.addInputStorageBufferT( m_rsmSamplesSsbo.getBuffer().bufferViewId, rsmgi::Idx::RsmSamplesIdx );
		cameraUbo.createPassBinding( pass, rsmgi::Idx::CameraUboIdx );
		shadowBuffer.createPassBinding( pass, rsmgi::Idx::ShadowsIdx );
		pass.addInputSampledImageT( depthObj.getSampledViewId(), rsmgi::Idx::DepthObjMapIdx );
		pass.addInputSampledImageT( nmlOcc.getSampledViewId(), rsmgi::Idx::NmlOccMapIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::eNormal ), rsmgi::Idx::RsmNormalsIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::ePosition ), rsmgi::Idx::RsmPositionIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::eFlux ), rsmgi::Idx::RsmFluxIdx );

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
