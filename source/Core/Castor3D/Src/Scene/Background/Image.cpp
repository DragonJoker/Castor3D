#include "Scene/Background/Image.hpp"

#include "Engine.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "RenderToTexture/EquirectangularToCube.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	ImageBackground::ImageBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eImage }
	{
	}

	ImageBackground::~ImageBackground()
	{
	}

	bool ImageBackground::loadImage( Path const & folder, Path const & relative )
	{
		bool result = false;

		try
		{
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.flags = 0u;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst;
			auto texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			texture->setSource( folder, relative );
			m_texture = texture;
			notifyChanged();
			result = true;
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( p_exc.what() );
		}

		return result;
	}

	void ImageBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	ashes::ShaderStageStateArray ImageBackground::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0u, 0u );
			UBO_MODEL_MATRIX( writer, 1u, 0u );

			// Outputs
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				out.gl_Position() = writer.paren( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0 ) ).xyww();
			} );

			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer, 2u, 0u );
			glsl::Ubo sizeUbo{ writer, cuT( "SizeUbo" ), 3u, 0u };
			auto c3d_size = sizeUbo.declMember< Vec2 >( cuT( "c3d_size" ) );
			sizeUbo.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 4u, 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );
			glsl::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto texcoord = writer.declLocale( cuT( "texcoord" )
					, writer.ashesBottomUpToTopDown( gl_FragCoord.xy() / c3d_size ) );
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( c3d_mapDiffuse, texcoord ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( utils.removeGamma( c3d_gamma, colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = colour;
				}
			} );

			pxl = writer.finalise();
		}

		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, vtx.getSource() ) );
		program[1].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, pxl.getSource() ) );
		return program;
	}

	bool ImageBackground::doInitialise( ashes::RenderPass const & renderPass )
	{
		auto & device = getCurrentDevice( *this );
		m_sizeUbo = ashes::makeUniformBuffer< Point2f >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );
		m_hdr = m_texture->getPixelFormat() == ashes::Format::eR32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16B16A16_SFLOAT;
		return m_texture->initialise();
	}

	void ImageBackground::doCleanup()
	{
		m_texture->cleanup();
		m_sizeUbo.reset();
	}

	void ImageBackground::doUpdate( Camera const & camera )
	{
		m_sizeUbo->getData() = Point2f{ camera.getWidth()
			, camera.getHeight() };
		m_sizeUbo->upload();
		m_matrixUbo.update( camera.getView()
			, camera.getProjection() );
	}

	void ImageBackground::doInitialiseDescriptorLayout()
	{
		auto & device = getCurrentDevice( *this );
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },			// Matrix UBO
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },			// Model Matrix UBO
			{ 2u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },			// HDR Config UBO
			{ 3u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },			// Size UBO
			{ 4u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// Image Map
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	void ImageBackground::initialiseDescriptorSet( MatrixUbo const & matrixUbo
		, ModelMatrixUbo const & modelMatrixUbo
		, HdrConfigUbo const & hdrConfigUbo
		, ashes::DescriptorSet & descriptorSet )const
	{
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 0u )
			, matrixUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 1u )
			, modelMatrixUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 2u )
			, hdrConfigUbo.getUbo() );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 3u )
			, *m_sizeUbo );
		descriptorSet.createBinding( m_descriptorLayout->getBinding( 4u )
			, m_texture->getDefaultView()
			, m_sampler.lock()->getSampler() );
	}
}
