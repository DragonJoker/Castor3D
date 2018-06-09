#include "BackgroundTreeItemProperty.hpp"

#include <Render/RenderLoop.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Background/Colour.hpp>
#include <Scene/Background/Image.hpp>
#include <Scene/Background/Skybox.hpp>
#include <Scene/Background/Visitor.hpp>
#include <Texture/TextureLayout.hpp>

#include "AdditionalProperties.hpp"
#include "SizeProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_BACKGROUND = _( "Background: " );
		static wxString PROPERTY_BACKGROUND_COLOUR = _( "Colour" );
		static wxString PROPERTY_BACKGROUND_IMAGE = _( "Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX = _( "Skybox" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_TYPE = _( "Skybox Type" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR = _( "Equirectangular" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_CROSS = _( "Cross" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_FACES = _( "Faces" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE = _( "Equirectangular Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS = _( "Equirectangular Dimensions" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE = _( "Cross Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE = _( "Left Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE = _( "Right Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE = _( "Top Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE = _( "Bottom Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE = _( "Front Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE = _( "Back Image" );

		class BackgroundDisplayer
			: public castor3d::BackgroundVisitor
		{
		public:
			static wxPGProperty * submit( SceneBackground & background
				, wxPropertyGrid & grid )
			{
				BackgroundDisplayer vis{ grid };
				background.accept( vis );
				return vis.m_property;
			}

		private:
			explicit BackgroundDisplayer( wxPropertyGrid & grid )
				: m_grid{ grid }
			{
			}

			void visit( ColourBackground const & background )
			{
				m_property = m_grid.Append( new wxColourProperty( PROPERTY_BACKGROUND_COLOUR ) );
				m_property->SetValue( WXVARIANT( wxColour( toBGRPacked( background.getScene().getBackgroundColour() ) ) ) );
			}

			void visit( SkyboxBackground const & background )
			{
				wxString selected;
				wxPGChoices choices;
				choices.Add( PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR );
				choices.Add( PROPERTY_BACKGROUND_SKYBOX_CROSS );
				choices.Add( PROPERTY_BACKGROUND_SKYBOX_FACES );

				if ( !background.getEquiTexturePath().empty() )
				{
					selected = PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR;
				}
				else if ( !background.getCrossTexturePath().empty() )
				{
					selected = PROPERTY_BACKGROUND_SKYBOX_CROSS;
				}
				else
				{
					selected = PROPERTY_BACKGROUND_SKYBOX_FACES;
				}

				if ( !background.getEquiTexturePath().empty() )
				{
					m_grid.Append( new wxImageFileProperty{ PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE
						, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE
						, make_wxString( background.getEquiTexturePath() ) } );
					m_grid.Append( new wxUIntProperty{ PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS
						, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS
						, background.getEquiSize().getWidth() } );
				}
				else if ( !background.getCrossTexturePath().empty() )
				{
					m_grid.Append( new wxImageFileProperty{ PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE
						, PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE
						, make_wxString( background.getCrossTexturePath() ) } );
				}
				else
				{
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveX ) );
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeX ) );
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveY ) );
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeY ) );
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveZ ) );
					m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeZ ) );
				}

			}

			void visit( ImageBackground const & background )
			{
				m_property = m_grid.Append( doCreateTextureImageProperty( PROPERTY_BACKGROUND_IMAGE, background.getTexture() ) );
			}

			wxPGProperty * doCreateTextureImageProperty( wxString const & name
				, castor3d::TextureLayout const & texture )
			{
				REQUIRE( texture.getImage().isStaticSource() );
				auto source = texture.getImage().toString();
				return new wxImageFileProperty{ name
					, name
					, make_wxString( source ) };
			}

			wxPGProperty * doCreateTextureImageProperty( wxString const & name
				, castor3d::TextureLayout const & texture
				, castor3d::CubeMapFace face )
			{
				REQUIRE( texture.getImage( uint32_t( face ) ).isStaticSource() );
				auto source = texture.getImage( uint32_t( face ) ).toString();
				return new wxImageFileProperty{ name
					, name
					, make_wxString( source ) };
			}

		private:
			wxPropertyGrid & m_grid;
			wxPGProperty * m_property{ nullptr };
		};
	}

	BackgroundTreeItemProperty::BackgroundTreeItemProperty( wxWindow * parent
		, bool editable
		, SceneBackground & background )
		: TreeItemProperty{ background.getScene().getEngine(), editable, ePROPERTY_DATA_TYPE_BACKGROUND }
		, m_parent{ parent }
		, m_background{ background }
	{
		PROPERTY_CATEGORY_BACKGROUND = _( "Background: " );
		PROPERTY_BACKGROUND_COLOUR = _( "Colour" );
		PROPERTY_BACKGROUND_IMAGE = _( "Image" );
		PROPERTY_BACKGROUND_SKYBOX = _( "Skybox" );
		PROPERTY_BACKGROUND_SKYBOX_TYPE = _( "Skybox Type" );
		PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR = _( "Equirectangular" );
		PROPERTY_BACKGROUND_SKYBOX_CROSS = _( "Cross" );
		PROPERTY_BACKGROUND_SKYBOX_FACES = _( "Faces" );
		PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE = _( "Equirectangular Image" );
		PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS = _( "Equirectangular Dimensions" );
		PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE = _( "Cross Image" );
		PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE = _( "Left Image" );
		PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE = _( "Right Image" );
		PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE = _( "Top Image" );
		PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE = _( "Bottom Image" );
		PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE = _( "Front Image" );
		PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE = _( "Back Image" );

		CreateTreeItemMenu();
	}

	BackgroundTreeItemProperty::~BackgroundTreeItemProperty()
	{
	}

	void BackgroundTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		wxString selected;
		wxPGChoices choices;
		choices.Add( PROPERTY_BACKGROUND_COLOUR );
		choices.Add( PROPERTY_BACKGROUND_IMAGE );
		choices.Add( PROPERTY_BACKGROUND_SKYBOX );

		switch ( m_background.getType() )
		{
		case BackgroundType::eColour:
			selected = PROPERTY_BACKGROUND_COLOUR;
			break;

		case BackgroundType::eImage:
			selected = PROPERTY_BACKGROUND_IMAGE;
			break;

		case BackgroundType::eSkybox:
			selected = PROPERTY_BACKGROUND_SKYBOX;
			break;
		}

		grid->Append( new wxPropertyCategory( selected + wxT( " " ) + PROPERTY_CATEGORY_BACKGROUND ) );
		m_backgroundProperty = BackgroundDisplayer::submit( m_background, *grid );
	}

	void BackgroundTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_BACKGROUND_COLOUR )
			{
				wxColour colour;
				colour << property->GetValue();
				onBackgroundColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_IMAGE )
			{
				onBackgroundImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE )
			{
				onBackgroundLeftImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE )
			{
				onBackgroundTopImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE )
			{
				onBackgroundRightImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE )
			{
				onBackgroundBottomImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE )
			{
				onBackgroundFrontImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE )
			{
				onBackgroundBackImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS )
			{
				onBackgroundEquirectangularDimensionsChange( getValue< uint32_t >( property->GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE )
			{
				onBackgroundEquirectangularImageChange( make_String( property->GetValueAsString() ) );
			}
			else if ( property->GetName() == PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE )
			{
				onBackgroundCrossImageChange( make_String( property->GetValueAsString() ) );
			}
		}
	}

	wxPGProperty * BackgroundTreeItemProperty::doCreateTextureImageProperty( wxString const & name
		, castor3d::TextureLayout const & texture )
	{
		wxPGProperty * property = nullptr;

		if ( texture.getImage().isStaticSource() )
		{
			property = new wxImageFileProperty( name );
		}

		return property;
	}

	void BackgroundTreeItemProperty::onBackgroundColourChange( castor::RgbColour const & value )
	{
		doApplyChange( [value, this]()
			{
				m_background.getScene().setBackgroundColour( value );
			} );
	}

	void BackgroundTreeItemProperty::onBackgroundImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< ImageBackground & >( m_background ).loadImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundLeftImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadLeftImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundRightImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadRightImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundTopImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadTopImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundBottomImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadBottomImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundFrontImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadFrontImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundBackImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					static_cast< SkyboxBackground & >( m_background ).loadBackImage( Path{}, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundCrossImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					auto & background = static_cast< SkyboxBackground & >( m_background );
					background.loadCrossTexture( Path{}
						, Path{ value } );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}

	void BackgroundTreeItemProperty::onBackgroundEquirectangularDimensionsChange( uint32_t value )
	{
		doApplyChange( [this, value]()
			{
				static_cast< SkyboxBackground & >( m_background ).setEquiSize( value );
			} );
		m_background.notifyChanged();
	}

	void BackgroundTreeItemProperty::onBackgroundEquirectangularImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
				{
					auto & background = static_cast< SkyboxBackground & >( m_background );
					background.loadEquiTexture( Path{}
						, Path{ value }
						, background.getEquiSize().getWidth() );
				} );
			m_background.notifyChanged();
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}
}
