#include "Castor3D/Scene/Background/Colour.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

CU_ImplementSmartPtr( c3d, ColourBackground )

namespace c3d
{
	//************************************************************************************************

	namespace bgcolour
	{
		static uint32_t constexpr Dim = 16u;

		static CU_ImplementAttributeParserBlock( parserRoot, SceneContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->scene->setBackgroundColour( params[0]->get< RgbColour >() );
		}
		CU_EndAttribute()
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene
		, String const & name )
		: SceneBackground{ engine
			, scene
			, name + cuT( "Colour" )
			, cuT( "colour" )
			, true }
		, m_colour{ m_needsUpload }
	{
		m_hdr = false;
		m_textureId = { engine.getRenderSystem()->getRenderDevice()
			, getScene().getResources()
			, cuT( "ColourBackground_Colour" )
			, { ImageCreateFlags::eCubeCompatible
				, { bgcolour::Dim, bgcolour::Dim, 1u }, 6u, 1u
				, PixelFormat::eR32G32B32A32_SFLOAT
				, ImageUsageFlags::eTransferDst | ImageUsageFlags::eSampled }
			, {} };
		m_textureId.create();
		m_texture = makeUnique< TextureLayout >( *engine.getRenderSystem()
			, cuT( "ColourBackground_Colour" )
			, *m_textureId.image
			, m_textureId.getWholeViewId() );
		m_buffer = PxBufferBase::create( makeSize( m_textureId.getExtent() )
			, PixelFormat::eR32G32B32A32_SFLOAT );
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void ColourBackground::accept( ConfigurationVisitorBase & visitor )
	{
	}

	bool ColourBackground::write( String const & tabs
		, Path const & folder
		, StringStream & stream )const
	{
		return true;
	}

	void ColourBackground::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SceneContext > sceneCtx{ result, CSCNSection::eScene, CSCNSection::eRoot };

		sceneCtx.addParser( cuT( "background_colour" ), bgcolour::parserRoot, { makeParameter< ParameterType::eRgbColour >() } );
	}

	bool ColourBackground::doInitialise( RenderDevice const & device )
	{
		m_textureId.create();
		auto data = device.graphicsData();
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		m_needsUpload = true;
		return true;
	}

	void ColourBackground::doCleanup()
	{
		m_textureId.destroy();
	}

	void ColourBackground::doCpuUpdate( CpuUpdater & updater )const
	{
		static c3d::Point3f const Scale{ 1, -1, 1 };
		static c3d::Quaternion const Orientation{ c3d::Quaternion::identity() };

		auto const & camera = *updater.camera;
		auto node = camera.getParent();
		c3d::matrix::setTransform( updater.bgMtxModl
			, node->getDerivedPosition(), Scale, Orientation );

		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		auto & viewport = *updater.viewport;
		viewport.setPerspective( updater.camera->getViewport().getFovY()
			, updater.camera->getRatio()
			, 0.1f
			, 2.0f );
		viewport.update();
		updater.bgMtxView = updater.camera->getView();
		updater.bgMtxProj = updater.isSafeBanded
			? viewport.getSafeBandedProjection( updater.renderSize )
			: viewport.getProjection();
	}

	void ColourBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	void ColourBackground::doUpload( UploadData & uploader )
	{
		Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

		for ( auto & c : makeArrayView( reinterpret_cast< Point4f * >( m_buffer->getPtr() )
			, m_buffer->getSize() / sizeof( Point4f ) ) )
		{
			c = colour;
		}

		ImageSubresourceRange dstSubresource{ ImageAspectFlags::eColor, 0u, m_texture->getMipLevels(), 0u, 1u };

		for ( uint32_t i = 0; i < 6u; ++i )
		{
			uploader.pushUpload( m_buffer->getPtr()
				, m_buffer->getSize()
				, m_texture->getTexture()
				, ImageMemoryLayout{ ImageViewType::e2D, *m_buffer }
				, dstSubresource
				, ImageLayout::eShaderReadOnly
				, PipelineStageFlags::eFragmentShader );
			dstSubresource.baseArrayLayer++;
		}
	}

	void ColourBackground::doAddPassBindings( crg::FramePass & pass
		, Texture * targetImage
		, uint32_t & index )const
	{
		pass.addInputSampledImage( m_textureId.getWholeViewId()
			, index
			, crg::SamplerDesc{ FilterMode::eLinear
				, FilterMode::eLinear
				, MipmapMode::eLinear } );
		++index;
	}

	void ColourBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages ) );	// c3d_mapBackground
		++index;
	}

	void ColourBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, Texture * targetImage
		, uint32_t & index )const
	{
		bindTexture( m_textureId.getSampledView()
			, *m_textureId.sampler
			, descriptorWrites
			, index );
	}

	//************************************************************************************************
}
