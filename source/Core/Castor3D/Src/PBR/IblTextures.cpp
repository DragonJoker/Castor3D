#include "IblTextures.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace
	{
		renderer::TexturePtr doCreatePrefilteredBrdf( RenderSystem const & renderSystem
			, Size const & size )
		{
			auto & device = *renderSystem.getCurrentDevice();
			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32A32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			return device.createTexture( image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
		}

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr result;
			auto name = cuT( "IblTexturesBRDF" );

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().create( name );
				result->setMinFilter( renderer::Filter::eLinear );
				result->setMagFilter( renderer::Filter::eLinear );
				result->setWrapS( renderer::WrapMode::eClampToEdge );
				result->setWrapT( renderer::WrapMode::eClampToEdge );
				result->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise();
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, renderer::Texture const & source )
		: OwnedBy< Scene >{ scene }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( *scene.getEngine()->getRenderSystem(), Size{ 512u, 512u } ) }
		, m_prefilteredBrdfView{ m_prefilteredBrdf->createView( renderer::TextureViewType::e2D, m_prefilteredBrdf->getFormat() ) }
		, m_sampler{ doCreateSampler( *scene.getEngine() ) }
		, m_radianceComputer{ *scene.getEngine(), Size{ 32u, 32u }, source }
		, m_environmentPrefilter{ *scene.getEngine(), Size{ 128u, 128u }, source }
	{
		BrdfPrefilter filter{ *scene.getEngine()
			, { m_prefilteredBrdf->getDimensions().width, m_prefilteredBrdf->getDimensions().height }
			, *m_prefilteredBrdfView };
		filter.render();
	}

	IblTextures::~IblTextures()
	{
		m_prefilteredBrdfView.reset();
		m_prefilteredBrdf.reset();
	}

	void IblTextures::update()
	{
		m_radianceComputer.render();
		m_environmentPrefilter.render();
	}

	void IblTextures::debugDisplay( Size const & renderSize )const
	{
		int width = int( m_prefilteredBrdf->getDimensions().width );
		int height = int( m_prefilteredBrdf->getDimensions().height );
		int left = 0u;
		int top = renderSize.getHeight() - height;
		auto size = Size( width, height );
		//auto & context = *getScene()->getEngine()->getRenderSystem()->getCurrentContext();
		//context.renderTexture( Position{ left, top }
		//	, size
		//	, *m_prefilteredBrdf.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_prefilteredEnvironment.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_radianceTexture.getTexture() );
	}
}
