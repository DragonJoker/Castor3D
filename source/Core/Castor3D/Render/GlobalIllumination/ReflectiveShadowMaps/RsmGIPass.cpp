#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
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

CU_ImplementSmartPtr( castor3d, RsmGIPass )

namespace castor3d
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

			writer.implementMain( [&]( sdw::VertexIn in
				, sdw::VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getDirectionalPixelShaderSource( uint32_t width
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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI

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

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getSpotPixelShaderSource( uint32_t width
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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, nmlOcc.xyz() );

					IF( writer, dot( wsNormal, wsNormal ) == 0.0f )
					{
						writer.demote();
					}
					FI;

					auto shadowData = writer.declLocale( "shadowData"
						, shadows.getSpotShadows( c3d_rsmConfigData.index ) );
					pxl_rsmGI = rsm.spot( shadowData
						, wsPosition
						, wsNormal
						, c3d_rsmConfigData );
					pxl_rsmNormal = wsNormal;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPointPixelShaderSource( uint32_t width
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

			writer.implementMain( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI

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
				return getDirectionalPixelShaderSource( width, height, renderSystem );
			case LightType::eSpot:
				return getSpotPixelShaderSource( width, height, renderSystem );
			case LightType::ePoint:
				return getPointPixelShaderSource( width, height, renderSystem );
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

	RsmGIPass::RsmGIPass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, VkExtent3D const & size
		, CameraUbo const & cameraUbo
		, crg::ImageViewId const & depthObj
		, crg::ImageViewId const & nmlOcc
		, ShadowMapResult const & smResult
		, TextureArray const & result )
		: castor::Named{ castor3d::getName( lightType ) + "Rsm" }
		, m_rsmConfigUbo{ device }
		, m_rsmSamplesSsbo{ device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, MaxRsmRange
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), rsmgi::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), rsmgi::getPixelProgram( lightType, size.width, size.height, device.renderSystem ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		std::random_device rd;
		std::mt19937 rng( rd() );
		std::uniform_real_distribution< float > dist( 0.0f, 1.0f );

		for ( auto & point : m_rsmSamplesSsbo.getData() )
		{
			auto xi2 = dist( rng );
			auto twoPIy = castor::PiMult2< float > *xi2;
			auto xi1 = dist( rng );
			point[0] = float( xi1 * sin( twoPIy ) );
			point[1] = float( xi1 * cos( twoPIy ) );
			point[2] = float( xi1 );
			point[3] = float( xi2 );
		}

		m_rsmSamplesSsbo.markDirty( VK_ACCESS_UNIFORM_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

		auto & pass = graph.createPass( getName()
			, [this, &device, size]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = std::make_unique< crg::RenderQuad >( framePass
					, context
					, runnableGraph
					, crg::ru::Config{ 1u, false }
					, rsmgi::getConfig( { size.width, size.height }
						, m_stages ) );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		pass.addUniformBuffer( { m_rsmConfigUbo.getUbo().getBuffer(), "RsmConfig" }
			, rsmgi::RsmCfgUboIdx
			, m_rsmConfigUbo.getUbo().getByteOffset()
			, m_rsmConfigUbo.getUbo().getByteRange() );
		pass.addInputStorageBuffer( { m_rsmSamplesSsbo.getBuffer(), "RsmSample" }
			, rsmgi::RsmSamplesIdx
			, m_rsmSamplesSsbo.getOffset()
			, m_rsmSamplesSsbo.getSize() );
		cameraUbo.createPassBinding( pass
			, rsmgi::CameraUboIdx );
		shadowBuffer.createPassBinding( pass
			, rsmgi::ShadowsIdx );
		pass.addSampledView( depthObj
			, rsmgi::DepthObjMapIdx );
		pass.addSampledView( nmlOcc
			, rsmgi::NmlOccMapIdx );
		pass.addSampledView( smResult[SmTexture::eNormal].sampledViewId
			, rsmgi::RsmNormalsIdx );
		pass.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, rsmgi::RsmPositionIdx );
		pass.addSampledView( smResult[SmTexture::eFlux].sampledViewId
			, rsmgi::RsmFluxIdx );
		pass.addOutputColourView( result[0].targetViewId
			, transparentBlackClearColor );
		pass.addOutputColourView( result[1].targetViewId
			, transparentBlackClearColor );
		m_pass = &pass;
	}

	void RsmGIPass::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void RsmGIPass::update( Light const & light )
	{
		m_rsmConfigUbo.cpuUpdate( light.getRsmConfig()
			, uint32_t( light.getShadowMapIndex() ) );
	}
}
