#include "Castor3D/Overlay/Overlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, Overlay )
CU_ImplementSmartPtr( castor3d, OverlayContext )

namespace castor3d
{
	namespace overlay
	{
		static SceneRPtr getScene( castor::FileParserContext const & context
			, OverlayContext const * blockContext
			, OverlayContext::OverlayPtr const & overlay )
		{
			if ( overlay.rptr )
			{
				return overlay.rptr->getScene();
			}

			if ( !blockContext || !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene context in overlay context" ) );
				CU_Failure( "No scene context in overlay context" );
				CU_Exception( "No scene context in overlay context" );
			}

			return blockContext->scene->scene;
		}

		static CU_ImplementAttributeParserBlock( parserPosition, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setRelativePosition( params[0]->get< castor::Point2d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSize, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setRelativeSize( params[0]->get< castor::Point2d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPixelSize, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setPixelSize( params[0]->get< castor::Size >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPixelPosition, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setPixelPosition( params[0]->get< castor::Position >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaterial, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				blockContext->overlay.rptr->setMaterial( getEngine( *blockContext )->findMaterial( name ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPanelOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( name )
				: engine->tryFindOverlay( name );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::ePanel
					, getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( name )
				: engine->tryFindOverlay( name );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::eBorderPanel
					, getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserTextOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( name )
				: engine->tryFindOverlay( name );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::eText
					, getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserEnd, OverlayContext )
		{
			log::info << "Loaded overlay [" << blockContext->overlay.rptr->getName() << "]" << std::endl;

			if ( blockContext->overlay.rptr->getType() == OverlayType::eText )
			{
				auto textOverlay = blockContext->overlay.rptr->getTextOverlay();

				if ( auto fontTexture = textOverlay->getFontTexture() )
				{
					if ( fontTexture->getFont()->isSDF()
						&& textOverlay->getSDFHeight() == uint32_t{} )
					{
						blockContext->overlay.rptr->setVisible( false );
						CU_ParsingError( cuT( "TextOverlay's font is an SDF font, and the overlay text_height has not been set, the overlay will not be rendered" ) );
					}
					else
					{
						blockContext->overlay.rptr->setVisible( true );
					}
				}
				else
				{
					blockContext->overlay.rptr->setVisible( false );
					CU_ParsingError( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
				}
			}
			else
			{
				blockContext->overlay.rptr->setVisible( true );
			}

			if ( blockContext->overlay.uptr )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->addOverlay( blockContext->overlay.rptr->getName()
						, blockContext->overlay.uptr
						, true );
				}
				else
				{
					getEngine( *blockContext )->addOverlay( blockContext->overlay.rptr->getName()
						, blockContext->overlay.uptr
						, true );
				}
			}

			CU_Require( !blockContext->parentOverlays.empty() );
			blockContext->overlay = castor::move( blockContext->parentOverlays.back() );
			blockContext->parentOverlays.pop_back();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserPanelOverlayUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::ePanel )
			{
				overlay->getPanelOverlay()->setUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlaySizes, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setRelativeBorderSize( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayPixelSizes, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setPixelBorderSize( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayMaterial, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				overlay->getBorderPanelOverlay()->setBorderMaterial( getEngine( *blockContext )->findMaterial( name ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayPosition, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderPosition( BorderPosition( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayCenterUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayOuterUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderOuterUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayInnerUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderInnerUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayFont, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				castor::String name;

				if ( getEngine( *blockContext )->hasFont( params[0]->get( name ) ) )
				{
					overlay->getTextOverlay()->setFont( name );
				}
				else
				{
					CU_ParsingError( cuT( "Unknown font" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTextHeight, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setSDFHeight( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTextWrapping, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setTextWrappingMode( TextWrappingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayVerticalAlign, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setVAlign( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayHorizontalAlign, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setHAlign( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTexturingMode, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setTexturingMode( TextTexturingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayLineSpacingMode, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setLineSpacingMode( TextLineSpacingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayText, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				auto strParams = params[0]->get< castor::String >();
				castor::string::replace( strParams, cuT( "\\a" ), cuT( "\a" ) );
				castor::string::replace( strParams, cuT( "\\b" ), cuT( "\b" ) );
				castor::string::replace( strParams, cuT( "\\f" ), cuT( "\f" ) );
				castor::string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
				castor::string::replace( strParams, cuT( "\\r" ), cuT( "\r" ) );
				castor::string::replace( strParams, cuT( "\\t" ), cuT( "\t" ) );
				castor::string::replace( strParams, cuT( "\\v" ), cuT( "\v" ) );
				overlay->getTextOverlay()->setCaption( castor::toUtf8U32String( strParams ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()
	}

	Overlay::Overlay( castor::String const & name
		, Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent
		, uint32_t level )
		: OwnedBy< Engine >{ engine }
		, m_name{ name }
		, m_parent{ parent }
		, m_category{ scene
			? scene->getOverlayCache().getFactory().create( type )
			: engine.getOverlayCache().getFactory().create( type ) }
		, m_scene{ scene }
		, m_renderSystem{ engine.getRenderSystem() }
	{
		m_category->setOverlay( this );
		m_category->setOrder( level, 0u );
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent
		, uint32_t level )
		: Overlay{ castor::String{}
			, engine
			, type
			, scene
			, parent
			, level }
	{
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type
		, uint32_t level )
		: Overlay{ engine
			, type
			, nullptr
			, nullptr
			, level }
	{
	}

	uint32_t Overlay::getChildrenCount( uint32_t level )const
	{
		uint32_t result{ 0 };

		if ( level == getLevel() )
		{
			result = 1u;
		}
		else if ( level == getLevel() + 1u )
		{
			result = uint32_t( m_children.size() );
		}
		else if ( level > getLevel() )
		{
			for ( auto overlay : m_children )
			{
				result += overlay->getChildrenCount( level );
			}
		}

		return result;
	}

	PanelOverlayRPtr Overlay::getPanelOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::ePanel )
		{
			CU_Failure( "This overlay is not a panel." );
			return nullptr;
		}

		return &static_cast< PanelOverlay & >( *m_category );
	}

	BorderPanelOverlayRPtr Overlay::getBorderPanelOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::eBorderPanel )
		{
			CU_Failure( "This overlay is not a border panel." );
			return nullptr;
		}

		return &static_cast< BorderPanelOverlay & >( *m_category );
	}

	TextOverlayRPtr Overlay::getTextOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::eText )
		{
			CU_Failure( "This overlay is not a text." );
			return nullptr;
		}

		return &static_cast< TextOverlay & >( *m_category );
	}

	bool Overlay::isVisible()const
	{
		bool result = m_category->isVisible();

		if ( result && getParent() )
		{
			result = getParent()->isVisible();
		}

		return result;
	}

	bool Overlay::isDisplayable()const
	{
		bool result = m_category->isDisplayable();

		if ( result && getParent() )
		{
			result = getParent()->isDisplayable();
		}

		return result;
	}

	uint32_t Overlay::computeLevel()const
	{
		if ( !m_parent )
		{
			return m_category->getLevel();
		}

		return 1u + m_parent->computeLevel();
	}

	void Overlay::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< OverlayContext > panelCtx{ result, CSCNSection::ePanelOverlay };
		BlockParserContextT< OverlayContext > borderCtx{ result, CSCNSection::eBorderPanelOverlay };
		BlockParserContextT< OverlayContext > textCtx{ result, CSCNSection::eTextOverlay };

		panelCtx.addParser( cuT( "material" ), overlay::parserMaterial, { makeParameter< ParameterType::eName >() } );
		panelCtx.addParser( cuT( "position" ), overlay::parserPosition, { makeParameter< ParameterType::ePoint2D >() } );
		panelCtx.addParser( cuT( "size" ), overlay::parserSize, { makeParameter< ParameterType::ePoint2D >() } );
		panelCtx.addParser( cuT( "pxl_size" ), overlay::parserPixelSize, { makeParameter< ParameterType::eSize >() } );
		panelCtx.addParser( cuT( "pxl_position" ), overlay::parserPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		panelCtx.addParser( cuT( "uv" ), overlay::parserPanelOverlayUvs, { makeParameter< ParameterType::ePoint4D >() } );
		panelCtx.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, overlay::parserPanelOverlay, { makeParameter< ParameterType::eName >() } );
		panelCtx.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, overlay::parserBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		panelCtx.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, overlay::parserTextOverlay, { makeParameter< ParameterType::eName >() } );
		panelCtx.addPopParser( cuT( "}" ), overlay::parserEnd );

		borderCtx.addParser( cuT( "material" ), overlay::parserMaterial, { makeParameter< ParameterType::eName >() } );
		borderCtx.addParser( cuT( "position" ), overlay::parserPosition, { makeParameter< ParameterType::ePoint2D >() } );
		borderCtx.addParser( cuT( "size" ), overlay::parserSize, { makeParameter< ParameterType::ePoint2D >() } );
		borderCtx.addParser( cuT( "pxl_size" ), overlay::parserPixelSize, { makeParameter< ParameterType::eSize >() } );
		borderCtx.addParser( cuT( "pxl_position" ), overlay::parserPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		borderCtx.addParser( cuT( "border_material" ), overlay::parserBorderPanelOverlayMaterial, { makeParameter< ParameterType::eName >() } );
		borderCtx.addParser( cuT( "border_size" ), overlay::parserBorderPanelOverlaySizes, { makeParameter< ParameterType::ePoint4D >() } );
		borderCtx.addParser( cuT( "pxl_border_size" ), overlay::parserBorderPanelOverlayPixelSizes, { makeParameter< ParameterType::ePoint4U >() } );
		borderCtx.addParser( cuT( "border_position" ), overlay::parserBorderPanelOverlayPosition, { makeParameter< ParameterType::eCheckedText, BorderPosition >() } );
		borderCtx.addParser( cuT( "center_uv" ), overlay::parserBorderPanelOverlayCenterUvs, { makeParameter< ParameterType::ePoint4D >() } );
		borderCtx.addParser( cuT( "border_inner_uv" ), overlay::parserBorderPanelOverlayInnerUvs, { makeParameter< ParameterType::ePoint4D >() } );
		borderCtx.addParser( cuT( "border_outer_uv" ), overlay::parserBorderPanelOverlayOuterUvs, { makeParameter< ParameterType::ePoint4D >() } );
		borderCtx.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, overlay::parserPanelOverlay, { makeParameter< ParameterType::eName >() } );
		borderCtx.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, overlay::parserBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		borderCtx.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, overlay::parserTextOverlay, { makeParameter< ParameterType::eName >() } );
		borderCtx.addPopParser( cuT( "}" ), overlay::parserEnd );

		textCtx.addParser( cuT( "material" ), overlay::parserMaterial, { makeParameter< ParameterType::eName >() } );
		textCtx.addParser( cuT( "position" ), overlay::parserPosition, { makeParameter< ParameterType::ePoint2D >() } );
		textCtx.addParser( cuT( "size" ), overlay::parserSize, { makeParameter< ParameterType::ePoint2D >() } );
		textCtx.addParser( cuT( "pxl_size" ), overlay::parserPixelSize, { makeParameter< ParameterType::eSize >() } );
		textCtx.addParser( cuT( "pxl_position" ), overlay::parserPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		textCtx.addParser( cuT( "font" ), overlay::parserTextOverlayFont, { makeParameter< ParameterType::eName >() } );
		textCtx.addParser( cuT( "text_height" ), overlay::parserTextOverlayTextHeight, { makeParameter< ParameterType::eUInt32 >() } );
		textCtx.addParser( cuT( "text" ), overlay::parserTextOverlayText, { makeParameter< ParameterType::eText >() } );
		textCtx.addParser( cuT( "text_wrapping" ), overlay::parserTextOverlayTextWrapping, { makeParameter< ParameterType::eCheckedText, TextWrappingMode >() } );
		textCtx.addParser( cuT( "vertical_align" ), overlay::parserTextOverlayVerticalAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
		textCtx.addParser( cuT( "horizontal_align" ), overlay::parserTextOverlayHorizontalAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
		textCtx.addParser( cuT( "texturing_mode" ), overlay::parserTextOverlayTexturingMode, { makeParameter< ParameterType::eCheckedText, TextTexturingMode >() } );
		textCtx.addParser( cuT( "line_spacing_mode" ), overlay::parserTextOverlayLineSpacingMode, { makeParameter< ParameterType::eCheckedText, TextLineSpacingMode >() } );
		textCtx.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, overlay::parserPanelOverlay, { makeParameter< ParameterType::eName >() } );
		textCtx.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, overlay::parserBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		textCtx.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, overlay::parserTextOverlay, { makeParameter< ParameterType::eName >() } );
		textCtx.addPopParser( cuT( "}" ), overlay::parserEnd );
	}

	void Overlay::addChild( OverlayRPtr overlay )
	{
		m_children.push_back( overlay );
		overlay->m_parent = this;
	}

	void Overlay::removeChild( OverlayRPtr overlay )
	{
		if ( auto it = std::find( m_children.begin()
				, m_children.end()
				, overlay );
			it != m_children.end() )
		{
			( *it )->m_parent = nullptr;
			m_children.erase( it );
		}
	}

	void Overlay::clear()noexcept
	{
		for ( auto child : m_children )
		{
			child->m_parent = nullptr;
		}

		m_children.clear();
	}

	castor::String getPrefix( OverlayContext const & context )
	{
		return context.scene
			? getPrefix( *context.scene )
			: getPrefix( *context.root );
	}

	Engine * getEngine( OverlayContext const & context )
	{
		return getEngine( *context.root );
	}
}
