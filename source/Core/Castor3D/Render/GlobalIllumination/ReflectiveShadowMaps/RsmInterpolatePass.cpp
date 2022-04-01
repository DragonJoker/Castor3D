#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmInterpolatePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapping.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementSmartPtr( castor3d, RsmInterpolatePass )

namespace castor3d
{
	namespace rsminterp
	{
		enum Idx : uint32_t
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

			writer.implementMain( [&]( sdw::VertexIn in
				, sdw::VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getDirectionalPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height
			, RenderSystem const & renderSystem )
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
			C3D_Camera( writer, CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", GiMapIdx, 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", NmlMapIdx, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", DepthMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );

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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.demote();
					}
					FI;

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

					IF( writer, areEqual.x() && areEqual.y() && areEqual.z() )
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
					ELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getDirectionalShadows() );
						pxl_rsmGI = rsm.directional( shadowData
							, vsPosition
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getSpotPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height
			, RenderSystem const & renderSystem )
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
			C3D_Camera( writer, CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", GiMapIdx, 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", NmlMapIdx, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", DepthMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );

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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.demote();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					IF( writer, dot( wsNormal, wsNormal ) == 0.0f )
					{
						writer.demote();
					}
					FI;

					auto giNormal = writer.declLocale( "giNormal"
						, c3d_mapNml.lod( texCoord, 0.0_f ).xyz() );

					IF( writer, all( giNormal == wsNormal ) )
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
					ELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getSpotShadows( 0_i ) );
						pxl_rsmGI = rsm.spot( shadowData
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPointPixelShaderSource( LightType lightType
			, uint32_t width
			, uint32_t height
			, RenderSystem const & renderSystem )
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
			C3D_Camera( writer, CameraIdx, 0u );
			auto c3d_mapGi = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapGi", GiMapIdx, 0u );
			auto c3d_mapNml = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNml", NmlMapIdx, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", DepthMapIdx, 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNmlOcc", NmlOccMapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eNormal ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );

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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
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

					IF( writer, areEqual.x() && areEqual.y() && areEqual.z() )
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
					ELSE
					{
						auto shadowData = writer.declLocale( "shadowData"
							, shadows.getPointShadows( 0_i ) );
						pxl_rsmGI = rsm.point( shadowData
							, shadowData.position().xyz()
							, wsPosition
							, wsNormal
							, c3d_rsmConfigData );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram( LightType lightType
			, uint32_t width
			, uint32_t height
			, RenderSystem const & renderSystem )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return getDirectionalPixelShaderSource( lightType, width, height, renderSystem );
			case LightType::eSpot:
				return getSpotPixelShaderSource( lightType, width, height, renderSystem );
			case LightType::ePoint:
				return getPointPixelShaderSource( lightType, width, height, renderSystem );
			default:
				CU_Failure( "Unexpected LightType" );
				return nullptr;
			}
		}

		static crg::rq::Config getConfig( VkExtent2D const & renderSize
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
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, VkExtent3D const & size
		, CameraUbo const & cameraUbo
		, crg::ImageViewId const & depthObj
		, crg::ImageViewId const & nmlOcc
		, ShadowMapResult const & smResult
		, RsmConfigUbo const & rsmConfigUbo
		, GpuBufferOffsetT< castor::Point4f > const & rsmSamplesSsbo
		, Texture const & gi
		, Texture const & nml
		, Texture const & dst )
		: castor::Named{ "RsmInterpolate" }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), rsminterp::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), rsminterp::getPixelProgram( lightType, gi.getExtent().width, gi.getExtent().height , device.renderSystem) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto & pass = graph.createPass( getName()
			, [this, &device, size]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::RenderQuad >( pass
					, context
					, graph
					, crg::ru::Config{ 1u, false }
					, rsminterp::getConfig( { size.width, size.height }
						, m_stages ) );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/RsmInterpolate"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addUniformBuffer( { rsmConfigUbo.getUbo().getBuffer(), "RsmConfig" }
			, rsminterp::RsmCfgUboIdx
			, rsmConfigUbo.getUbo().getByteOffset()
			, rsmConfigUbo.getUbo().getByteRange() );
		pass.addInputStorageBuffer( { rsmSamplesSsbo.getBuffer(), "RsmSample" }
			, rsminterp::RsmSamplesIdx
			, rsmSamplesSsbo.getOffset()
			, rsmSamplesSsbo.getSize() );
		cameraUbo.createPassBinding( pass
			, rsminterp::CameraIdx );
		shadowBuffer.createPassBinding( pass
			, rsminterp::ShadowsIdx );
		pass.addSampledView( gi.sampledViewId
			, rsminterp::GiMapIdx );
		pass.addSampledView( nml.sampledViewId
			, rsminterp::NmlMapIdx );
		pass.addSampledView( depthObj
			, rsminterp::DepthMapIdx );
		pass.addSampledView( nmlOcc
			, rsminterp::NmlOccMapIdx );
		pass.addSampledView( smResult[SmTexture::eNormal].sampledViewId
			, rsminterp::RsmNormalsIdx );
		pass.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, rsminterp::RsmPositionIdx );
		pass.addSampledView( smResult[SmTexture::eFlux].sampledViewId
			, rsminterp::RsmFluxIdx );
		pass.addInOutColourView( dst.targetViewId
			, VkPipelineColorBlendAttachmentState{ VK_TRUE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT } );
		m_pass = &pass;
	}

	void RsmInterpolatePass::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}
