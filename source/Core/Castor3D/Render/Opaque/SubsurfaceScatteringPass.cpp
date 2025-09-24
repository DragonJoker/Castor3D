#include "Castor3D/Render/Opaque/SubsurfaceScatteringPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <random>

CU_ImplementSmartPtr( c3d, SubsurfaceScatteringPass )

namespace c3d
{
	namespace sssss
	{
		enum BlurId : size_t
		{
			BlurMaterialsUboId,
			BlurSssProfilesUboId,
			BlurModelsUboId,
			BlurCameraUboId,
			BlurSssUboId,
			BlurDepthObjImgId,
			BlurLgtDiffImgId,
		};

		enum CombId : size_t
		{
			CombMaterialsUboId,
			CombModelsUboId,
			CombDepthObjImgId,
			CombEmsTrnImgId,
			CombBlur1ImgId,
			CombBlur2ImgId,
			CombBlur3ImgId,
			CombLgtDiffImgId,
		};

		template< typename sdw::var::Flag FlagT >
		using VertexStructT = sdw::IOStructInstanceHelperT< FlagT
			, "Vertex"
			, sdw::IOVec2Field< "position", 0u >
			, sdw::IOVec2Field< "texcoord", 1u > >;

		template< typename sdw::var::Flag FlagT >
		struct VertexT
			: public VertexStructT< FlagT >
		{
			VertexT( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled = true )
				: VertexStructT< FlagT >{ writer, c3d::move( expr ), enabled }
			{
			}

			auto position()const { return this->template getMember< "position" >(); }
			auto texcoord()const { return this->template getMember< "texcoord" >(); }
		};

		static ShaderPtr getBlurProgram( Engine & engine
			, bool isVertic )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			// Shader inputs
			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders, BlurMaterialsUboId, 0u };
			shader::SssProfiles sssProfiles{ writer, BlurSssProfilesUboId, 0u };
			C3D_ModelsData( writer, BlurModelsUboId, 0u );
			C3D_Camera( writer, BlurCameraUboId, 0u );
			auto config = writer.declUniformBuffer( SubsurfaceScatteringPass::Config, BlurSssUboId, 0u );
			auto c3d_pixelSize = config.declMember< sdw::Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< sdw::Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", BlurDepthObjImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", BlurLgtDiffImgId, 0u );

			writer.implementEntryPointT< VertexT, VertexT >( []( sdw::VertexInT< VertexT > const & in
				, sdw::VertexOutT< VertexT > out )
				{
					out.texcoord() = in.texcoord();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< VertexT, shader::Colour4FT >( [&writer, &c3d_mapDepthObj, &c3d_modelsData, &c3d_mapLightDiffuse, &c3d_cameraData, &c3d_pixelSize, &c3d_correction, &utils, &materials, &sssProfiles, isVertic]
				( sdw::FragmentInT< VertexT > const & in
					, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( in.texcoord(), 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					sdwIF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					sdwFI

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto materialId = writer.declLocale( "materialId"
						, modelData.getMaterialId() );
					auto material = materials.getMaterial( materialId );
					auto sssProfileIndex = material.getMember< sdw::UInt >( "sssProfileIndex", true );

					sdwIF( writer, sssProfileIndex == 0_u )
					{
						writer.demote();
					}
					sdwFI

					auto sssProfile = writer.declLocale( "sssProfile"
						, sssProfiles.getProfile( sssProfileIndex ) );

					// Fetch color and linear depth for current pixel:
					auto colorM = writer.declLocale( "colorM"
						, c3d_mapLightDiffuse.lod( in.texcoord(), 0.0_f ) );
					auto depthM = writer.declLocale( "depthM"
						, depthObj.x() );
					depthM = c3d_cameraData.projToView( utils, in.texcoord(), depthM ).z();

					// Accumulate center sample, multiplying it with its gaussian weight:
					out.colour() = vec4( colorM.rgb(), 1.0_f );
					out.colour().rgb() *= 0.382_f;

					if ( isVertic )
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 0.0_f, 1.0_f ) );
					}
					else
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 1.0_f, 0.0_f ) );
					}

					auto step = writer.getVariable< sdw::Vec2 >( "step" );

					// Calculate the step that we will use to fetch the surrounding pixels,
					// where "step" is:
					//     step = sssStrength * gaussianWidth * pixelSize * dir
					// The closer the pixel, the stronger the effect needs to be, hence
					// the factor 1.0 / depthM.
					auto translucency = writer.declLocale( "translucency"
						, colorM.a() );
					auto finalStep = writer.declLocale( "finalStep"
						, translucency * step * sssProfile.subsurfaceScatteringStrength() * sssProfile.gaussianWidth() / depthM );

					auto offset = writer.declLocale< sdw::Vec2 >( "offset" );
					auto color = writer.declLocale< sdw::Vec3 >( "color" );
					auto depth = writer.declLocale< sdw::Float >( "depth" );
					auto s = writer.declLocale< sdw::Float >( "s" );

					// Gaussian weights for the six samples around the current pixel:
					//   -3 -2 -1 +1 +2 +3
					auto w = writer.declLocaleArray( "w"
						, 6u
						, Vector< sdw::Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
					auto o = writer.declLocaleArray( "o"
						, 6u
						, Vector< sdw::Float >{ { -1.0_f, -0.666666667_f, -0.333333333_f, 0.333333333_f, 0.666666667_f, 1.0_f } } );

					// Accumulate the other samples:
					sdwFOR( writer, sdw::Int, i, 0_i, i < 6_i, ++i )
					{
						// Fetch color and depth for current sample:
						offset = sdw::fma( vec2( o[i] ), finalStep, in.texcoord() );
						color = c3d_mapLightDiffuse.lod( offset, 0.0_f ).rgb();
						offset = sdw::fma( vec2( o[i] ), finalStep, in.texcoord() );
						depth = c3d_mapDepthObj.lod( offset, 0.0_f ).x();
						depth = c3d_cameraData.projToView( utils, in.texcoord(), depth ).z();

						// If the difference in depth is huge, we lerp color back to "colorM":
						s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
						color = mix( color, colorM.rgb(), vec3( s ) );

						// Accumulate:
						out.colour().rgb() += w[i] * color;
					}
					sdwROF
				} );
			return makeRawUnique< sdw::Shader >( c3d::move( writer.getShader() ) );
		}

		static ShaderPtr getCombineProgram( Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			// Shader inputs
			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders, CombMaterialsUboId, 0u };
			C3D_ModelsData( writer, CombModelsUboId, 0u );

			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", CombDepthObjImgId, 0u );
			auto c3d_mapBlur1 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur1", CombBlur1ImgId, 0u );
			auto c3d_mapBlur2 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur2", CombBlur2ImgId, 0u );
			auto c3d_mapBlur3 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur3", CombBlur3ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", CombLgtDiffImgId, 0u );

			writer.implementEntryPointT< VertexT, VertexT >( []( sdw::VertexInT< VertexT > const & in
				, sdw::VertexOutT< VertexT > out )
				{
					out.texcoord() = in.texcoord();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< VertexT, shader::Colour4FT >( [&writer, &c3d_mapDepthObj, &c3d_mapBlur1, &c3d_mapBlur2, &c3d_mapBlur3, &c3d_mapLightDiffuse, &c3d_modelsData, &materials]
				( sdw::FragmentInT< VertexT > const & in
					, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( in.texcoord(), 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					sdwIF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					sdwFI

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto original = writer.declLocale( "original"
						, c3d_mapLightDiffuse.lod( in.texcoord(), 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, modelData.getMaterialId() );
					auto material = materials.getMaterial( materialId );
					auto sssProfileIndex = material.getMember< sdw::UInt >( "sssProfileIndex", true );

					sdwIF( writer, sssProfileIndex == 0_u )
					{
						out.colour() = vec4( original.rgb(), 1.0_f );
					}
					sdwELSE
					{
						auto originalWeight = writer.declLocale< sdw::Vec4 >( "originalWeight"
							, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
						auto blurWeights = writer.declLocaleArray< sdw::Vec4 >( "blurWeights"
							, 3u
							, {
								vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
								vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
								vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
							} );
						auto blurVariances = writer.declLocaleArray< sdw::Float >( "blurVariances"
							, 3u
							, {
								0.0516_f,
								0.2719_f,
								2.0062_f
							} );
						auto blur1 = writer.declLocale( "blur1"
							, c3d_mapBlur1.lod( in.texcoord(), 0.0_f ) );
						auto blur2 = writer.declLocale( "blur2"
							, c3d_mapBlur2.lod( in.texcoord(), 0.0_f ) );
						auto blur3 = writer.declLocale( "blur3"
							, c3d_mapBlur3.lod( in.texcoord(), 0.0_f ) );
						auto translucency = writer.declLocale( "translucency"
							, original.a() );
						original.a() = 1.0_f;
						out.colour() = original * originalWeight
							+ blur1 * blurWeights[0]
							+ blur2 * blurWeights[1]
							+ blur3 * blurWeights[2];
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}

		static Texture doCreateImage( crg::ResourcesCache & resources
			, RenderDevice const & device
			, Size const & size
			, PixelFormat format
			, String const & name )
		{
			return Texture{ device
				, resources
				, name
				, { ImageCreateFlags::eNone
					, makeExtent3D( size ), 1u, 1u
					, format
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eStorage
						| ImageUsageFlags::eTransferDst ) }
				, { BorderColour::eFloatTransparentBlack } };
		}

		static crg::rq::Config createConfig( Size const & size
			, ashes::PipelineShaderStageCreateInfoArray const & shaderStages
			, bool const * enabled
			, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		{
			crg::rq::Config result;
			result.texcoordConfig( crg::Texcoord{} );
			result.renderSize( makeExtent2D( size ) );
			result.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( shaderStages ) );
			result.isEnabled( crg::RunnablePass::IsEnabledCallback( [enabled, isEnabled](){ return *enabled && isEnabled(); } ) );
			return result;
		}
	}

	//*********************************************************************************************

	MbString const SubsurfaceScatteringPass::Config = "Config";
	MbString const SubsurfaceScatteringPass::Step = "c3d_step";
	MbString const SubsurfaceScatteringPass::Correction = "c3d_correction";
	MbString const SubsurfaceScatteringPass::PixelSize = "c3d_pixelSize";
	MbString const SubsurfaceScatteringPass::Weights = "c3d_weights";
	MbString const SubsurfaceScatteringPass::Offsets = "c3d_offsets";

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene const & scene
		, CameraUbo const & cameraUbo
		, Texture const & depthObj
		, Texture const & diffuse
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_cameraUbo{ cameraUbo }
		, m_diffuse{ diffuse }
		, m_scene{ scene }
		, m_group{ graph.createPassGroup( "SSSSS" ) }
		, m_enabled{ m_scene.needsSubsurfaceScattering() }
		, m_size{ makeSize( m_diffuse.getExtent() ) }
		, m_intermediate{ sssss::doCreateImage( *depthObj.resources, m_device, m_size, m_diffuse.getFormat(), cuT( "SSSIntermediate" ) ) }
		, m_blurImages{ sssss::doCreateImage( *depthObj.resources, m_device, m_size, m_intermediate.getFormat(),cuT( "SSSBlur0" ) )
			, sssss::doCreateImage( *depthObj.resources, m_device, m_size, m_intermediate.getFormat(),cuT( "SSSBlur1" ) )
			, sssss::doCreateImage( *depthObj.resources, m_device, m_size, m_intermediate.getFormat(), cuT( "SSSBlur2" ) ) }
		, m_result{ sssss::doCreateImage( *depthObj.resources, m_device, m_size, m_intermediate.getFormat(), cuT( "SSSResult" ) ) }
		, m_blurCfgUbo{ m_device.uboPool->getBuffer< BlurConfiguration >( MemoryPropertyFlags::eNone ) }
		, m_blurWgtUbo{ m_device.uboPool->getBuffer< BlurWeights >( MemoryPropertyFlags::eNone ) }
		, m_blurHorizProgram{ cuT( "SSSBlurX" ), sssss::getBlurProgram( *device.renderSystem.getEngine(), false ) }
		, m_blurXShader{ makeProgramStates( m_device, m_blurHorizProgram ) }
		, m_blurVerticProgram{ cuT( "SSSBlurY" ), sssss::getBlurProgram( *device.renderSystem.getEngine(), true ) }
		, m_blurYShader{ makeProgramStates( m_device, m_blurVerticProgram ) }
		, m_combineProgram{ cuT( "SSSCombine" ), sssss::getCombineProgram( *device.renderSystem.getEngine() ) }
		, m_combineShader{ makeProgramStates( m_device, m_combineProgram ) }
	{
		auto & configuration = m_blurCfgUbo.getData();
		configuration.blurCorrection = 1.0f;
		configuration.blurPixelSize = Point2f{ 1.0f / float( m_size.getWidth() ), 1.0f / float( m_size.getHeight() ) };

		auto & weights = m_blurWgtUbo.getData();
		weights.originalWeight = Point4f{ 0.2406f, 0.4475f, 0.6159f, 0.25f };
		weights.blurWeights[0] = Point4f{ 0.1158, 0.3661, 0.3439, 0.25 };
		weights.blurWeights[1] = Point4f{ 0.1836, 0.1864, 0.0, 0.25 };
		weights.blurWeights[2] = Point4f{ 0.46, 0.0, 0.0402, 0.25 };
		weights.blurVariance = Point4f{ 0.0516, 0.2719, 2.0062 };
		auto blurXSource = &m_diffuse;
		stepProgressBarLocal( progress, cuT( "Creating SSSSS Blur passes" ) );
		auto & modelBuffer = scene.getModelBuffer();

		for ( uint32_t i = 0u; i < PassCount; ++i )
		{
			auto blurYDestination = &m_blurImages[i];
			auto & blurX = m_group.createPass( "BlurX" + string::toMbString( i )
				, [this, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnable )
				{
					auto result = makeRawUnique< crg::RenderQuad >( framePass
						, context
						, runnable
						, crg::ru::Config{}
						, sssss::createConfig( m_size, m_blurXShader, &m_enabled, isEnabled ) );
					getEngine()->registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurX, sssss::BlurMaterialsUboId );
			getEngine()->getMaterialCache().getSssProfileBuffer().createPassBinding( blurX, sssss::BlurSssProfilesUboId );
			blurX.addInputStorage( *modelBuffer.getLastAttach(), uint32_t( sssss::BlurModelsUboId ) );
			m_cameraUbo.createPassBinding( blurX, sssss::BlurCameraUboId );
			m_blurCfgUbo.createPassBinding( blurX, sssss::BlurSssUboId );
			blurX.addInputSampled( *depthObj.getSampledLastAttach(), sssss::BlurDepthObjImgId );
			blurX.addInputSampled( *blurXSource->getSampledLastAttach(), sssss::BlurLgtDiffImgId );
			m_intermediate.setLastAttach( blurX.addOutputColourTarget( m_intermediate.getTargetViewId() ) );

			auto & blurY = m_group.createPass( "BlurY" + string::toMbString( i )
				, [this, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnable )
				{
					auto result = makeRawUnique< crg::RenderQuad >( framePass
						, context
						, runnable
						, crg::ru::Config{}
						, sssss::createConfig( m_size, m_blurYShader, &m_enabled, isEnabled ) );
					getEngine()->registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurY, sssss::BlurMaterialsUboId );
			getEngine()->getMaterialCache().getSssProfileBuffer().createPassBinding( blurY, sssss::BlurSssProfilesUboId );
			blurY.addInputStorage( *modelBuffer.getLastAttach(), uint32_t( sssss::BlurModelsUboId ) );
			m_cameraUbo.createPassBinding( blurY, sssss::BlurCameraUboId );
			m_blurCfgUbo.createPassBinding( blurY, sssss::BlurSssUboId );
			blurY.addInputSampled( *depthObj.getSampledLastAttach(), sssss::BlurDepthObjImgId );
			blurY.addInputSampled( *m_intermediate.getSampledLastAttach(), sssss::BlurLgtDiffImgId );
			blurYDestination->setLastAttach( blurY.addOutputColourTarget( blurYDestination->getTargetViewId() ) );

			blurXSource = blurYDestination;
		}

		stepProgressBarLocal( progress, cuT( "Creating SSSSS combine pass" ) );
		auto & pass = m_group.createPass("Combine"
			, [this, progress, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnable )
			{
				stepProgressBarLocal( progress, cuT( "Initialising SSSSS combine pass" ) );
				auto extent = m_result.getExtent();
				auto ruConfig = crg::ru::Config{}
					.implicitAction( m_result.getWholeViewId()
						, crg::RecordContext::copyImage( m_diffuse.getWholeViewId()
							, m_result.getWholeViewId()
							, { extent.width, extent.height } ) );
				auto rqConfig = sssss::createConfig( m_size, m_combineShader, &m_enabled, isEnabled );
				auto result = makeRawUnique< crg::RenderQuad >( framePass
					, context
					, runnable
					, ruConfig
					, rqConfig );
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		getEngine()->getMaterialCache().getPassBuffer().createPassBinding( pass, sssss::CombMaterialsUboId );
		pass.addInputStorage( *modelBuffer.getLastAttach(), uint32_t( sssss::CombModelsUboId ) );
		pass.addInputSampled( *depthObj.getSampledLastAttach(), sssss::CombDepthObjImgId );
		pass.addInputSampled( *m_blurImages[0].getSampledLastAttach(), sssss::CombBlur1ImgId );
		pass.addInputSampled( *m_blurImages[1].getSampledLastAttach(), sssss::CombBlur2ImgId );
		pass.addInputSampled( *m_blurImages[2].getSampledLastAttach(), sssss::CombBlur3ImgId );
		pass.addInputSampled( *m_diffuse.getSampledLastAttach(), sssss::CombLgtDiffImgId );
		m_result.setLastAttach( pass.addOutputColourTarget( m_result.getTargetViewId() ) );

		m_result.create();
		m_intermediate.create();

		for ( auto & texture : m_blurImages )
		{
			texture.create();
		}
	}

	SubsurfaceScatteringPass::~SubsurfaceScatteringPass()noexcept
	{
		for ( auto & texture : m_blurImages )
		{
			texture.destroy();
		}

		m_intermediate.destroy();
		m_result.destroy();
		m_device.uboPool->putBuffer( m_blurCfgUbo );
		m_device.uboPool->putBuffer( m_blurWgtUbo );
	}

	void SubsurfaceScatteringPass::update( CpuUpdater const & )
	{
		m_enabled = m_scene.needsSubsurfaceScattering();
	}

	void SubsurfaceScatteringPass::accept( ConfigurationVisitorBase & visitor )
	{
		for ( size_t i{ 0u }; i < m_blurImages.size(); ++i )
		{
			visitor.visit( cuT( "SSSSS Blur " ) + string::toString( i )
				, m_blurImages[i]
				, ImageLayout::eShaderReadOnly
				, TextureFactors{}.invert( true ) );
		}

		visitor.visit( cuT( "SSSSS Result" )
			, m_result
			, ImageLayout::eShaderReadOnly
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_blurHorizProgram, ast::EntryPoint::eVertex );
		visitor.visit( m_blurHorizProgram, ast::EntryPoint::eFragment );

		visitor.visit( m_blurVerticProgram, ast::EntryPoint::eVertex );
		visitor.visit( m_blurVerticProgram, ast::EntryPoint::eFragment );

		visitor.visit( m_combineProgram, ast::EntryPoint::eVertex );
		visitor.visit( m_combineProgram, ast::EntryPoint::eFragment );
	}
}
