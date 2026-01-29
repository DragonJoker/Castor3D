#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmInterpolatePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapping.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, RsmInterpolatePass )

namespace c3d
{
	namespace rsminterp
	{
		enum class Idx : uint32_t
		{
			RsmCfgUboIdx,
			RsmSamplesIdx,
			CameraIdx,
			ShadowsIdx,
			GiMapIdx,
			NmlMapIdx,
			DepthMapIdx,
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

		static ShaderPtr getDirectionalPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height )
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
			C3D_Camera( writer, Idx::CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", Idx::GiMapIdx, 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", Idx::NmlMapIdx, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", Idx::DepthMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", Idx::NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), Idx::RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), Idx::RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), Idx::RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, Idx::ShadowsIdx
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );

					sdwIF( writer, depth == 1.0_f )
					{
						writer.demote();
					}
					sdwFI

					auto data1 = writer.declLocale( "data1"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_cameraData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					auto giNormal = writer.declLocale( "giNormal"
						, c3d_mapNml.lod( texCoord, 0.0_f ).xyz() );
					auto areEqual = writer.declLocale( "areEqual"
						, giNormal == wsNormal );

					sdwIF( writer, areEqual.x() && areEqual.y() && areEqual.z() )
					{
						auto offset = writer.declLocale( "offset"
							, vec2( 1.0_f / float( width ), 1.0_f / float( height ) ) );
						pxl_rsmGI = c3d_mapGi.lod( texCoord, 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), -offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), -offset.y() ), 0.0_f ).xyz();
						pxl_rsmGI /= 5.0_f;
					}
					sdwELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getDirectionalShadows() );
						pxl_rsmGI = rsm.directional( shadowData
							, vsPosition
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					sdwFI
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getSpotPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height )
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
			C3D_Camera( writer, Idx::CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", Idx::GiMapIdx, 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", Idx::NmlMapIdx, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", Idx::DepthMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", Idx::NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), Idx::RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), Idx::RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), Idx::RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );

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
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );

					sdwIF( writer, depth == 1.0_f )
					{
						writer.demote();
					}
					sdwFI

					auto data1 = writer.declLocale( "data1"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					sdwIF( writer, dot( wsNormal, wsNormal ) == 0.0f )
					{
						writer.demote();
					}
					sdwFI

					auto giNormal = writer.declLocale( "giNormal"
						, c3d_mapNml.lod( texCoord, 0.0_f ).xyz() );

					sdwIF( writer, all( giNormal == wsNormal ) )
					{
						auto offset = writer.declLocale( "offset"
							, vec2( 1.0_f / float( width ), 1.0_f / float( height ) ) );
						pxl_rsmGI = c3d_mapGi.lod( texCoord, 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), -offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), -offset.y() ), 0.0_f ).xyz();
						pxl_rsmGI /= 5.0_f;
					}
					sdwELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getSpotShadows( 0_i ) );
						pxl_rsmGI = rsm.spot( shadowData
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					sdwFI
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPointPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height )
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
			C3D_Camera( writer, Idx::CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", uint32_t( Idx::GiMapIdx ), 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", uint32_t( Idx::NmlMapIdx ), 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", uint32_t( Idx::DepthMapIdx ), 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", uint32_t( Idx::NmlOccMapIdx ), 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), uint32_t( Idx::RsmNormalsIdx ), 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), uint32_t( Idx::RsmPositionIdx ), 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), uint32_t( Idx::RsmFluxIdx ), 0u );

			auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< sdw::Vec3 >( "pxl_rsmGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };

			shader::ShadowsBuffer shadows{ writer
				, Idx::ShadowsIdx
				, 0u };
			ReflectiveShadowMapping rsm{ writer
				, c3d_rsmSamples };

			writer.implementMain( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto data1 = writer.declLocale( "data1"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					auto giNormal = writer.declLocale( "giNormal"
						, c3d_mapNml.lod( texCoord, 0.0_f ).xyz() );
					auto areEqual = writer.declLocale( "areEqual"
						, giNormal == wsNormal );

					sdwIF( writer, areEqual.x() && areEqual.y() && areEqual.z() )
					{
						auto offset = writer.declLocale( "offset"
							, vec2( 1.0_f / float( width ), 1.0_f / float( height ) ) );
						pxl_rsmGI = c3d_mapGi.lod( texCoord, 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( offset.x(), -offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), offset.y() ), 0.0_f ).xyz()
							+ c3d_mapGi.lod( texCoord + vec2( -offset.x(), -offset.y() ), 0.0_f ).xyz();
						pxl_rsmGI /= 5.0_f;
					}
					sdwELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getPointShadows( 0_i ) );
						pxl_rsmGI = rsm.point( shadowData.position().xyz()
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					sdwFI
				} );

			return makeRawUnique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram( LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return getDirectionalPixelShaderSource( lightType, width, height );
			case LightType::eSpot:
				return getSpotPixelShaderSource( lightType, width, height );
			case LightType::ePoint:
				return getPointPixelShaderSource( lightType, width, height );
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

	RsmInterpolatePass::RsmInterpolatePass( crg::FrameGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, Extent3D const & size
		, CameraUbo const & cameraUbo
		, Texture const & depthObj
		, Texture const & nmlOcc
		, ShadowMapResult const & smResult
		, RsmConfigUbo const & rsmConfigUbo
		, GpuBufferOffsetT< Point4f > const & rsmSamplesSsbo
		, Texture const & gi
		, Texture const & nml
		, Texture & dst )
		: Named{ "RsmInterpolate" }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), rsminterp::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), rsminterp::getPixelProgram( lightType, gi.getExtent().width, gi.getExtent().height ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto & pass = graph.createPass( getName()
			, [this, &device, size]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< crg::RenderQuad >( framePass
					, context
					, runGraph
					, crg::ru::Config{ 1u, false }
					, rsminterp::getConfig( { size.width, size.height }
						, m_stages ) );
				c3d::getEngine( device ).registerTimer( runGraph.getName() + "/RsmInterpolate"
					, result->getTimer() );
				return result;
			} );
		rsmConfigUbo.createPassBinding( pass, rsminterp::Idx::RsmCfgUboIdx );
		pass.addInputStorageBufferT( rsmSamplesSsbo.getBuffer().bufferViewId, rsminterp::Idx::RsmSamplesIdx );
		cameraUbo.createPassBinding( pass, rsminterp::Idx::CameraIdx );
		shadowBuffer.createPassBinding( pass, rsminterp::Idx::ShadowsIdx );
		pass.addInputSampledT( *gi.getSampledLastAttach(), rsminterp::Idx::GiMapIdx );
		pass.addInputSampledT( *nml.getSampledLastAttach(), rsminterp::Idx::NmlMapIdx );
		pass.addInputSampledImageT( depthObj.getSampledViewId(), rsminterp::Idx::DepthMapIdx );
		pass.addInputSampledImageT( nmlOcc.getSampledViewId(), rsminterp::Idx::NmlOccMapIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::eNormal ), rsminterp::Idx::RsmNormalsIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::ePosition ), rsminterp::Idx::RsmPositionIdx );
		pass.addInputSampledImageT( smResult.getSampledViewId( SmTexture::eFlux ), rsminterp::Idx::RsmFluxIdx );
		dst.setLastAttach( pass.addInOutColourTarget( *dst.getLastAttach()
			, crg::PipelineColorBlendAttachmentState{ VK_TRUE
			, BlendFactor::eOne, BlendFactor::eOne, BlendOp::eAdd
			, BlendFactor::eOne, BlendFactor::eOne, BlendOp::eAdd
			, ColorComponentFlags::eR | ColorComponentFlags::eG | ColorComponentFlags::eB | ColorComponentFlags::eA } ) );
	}

	void RsmInterpolatePass::accept( ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}
