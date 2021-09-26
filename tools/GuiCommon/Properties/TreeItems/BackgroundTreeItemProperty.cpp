#include "GuiCommon/Properties/TreeItems/BackgroundTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Colour.hpp>
#include <Castor3D/Scene/Background/Image.hpp>
#include <Castor3D/Scene/Background/Skybox.hpp>
#include <Castor3D/Scene/Background/Visitor.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		class BackgroundDisplayer
			: public castor3d::BackgroundVisitor
		{
		public:
			static void submit( SceneBackground & background
				, TreeItemProperty & property
				, wxPropertyGrid & grid )
			{
				BackgroundDisplayer vis{ property, grid };
				background.accept( vis );
			}

		private:
			explicit BackgroundDisplayer( TreeItemProperty & property
				, wxPropertyGrid & grid )
				: m_properties{ property }
				, m_grid{ grid }
			{
			}

			void visit( ColourBackground & background )override
			{
				static wxString PROPERTY_BACKGROUND_COLOUR_COLOUR = _( "Colour" );

				m_properties.addPropertyT( &m_grid, PROPERTY_BACKGROUND_COLOUR_COLOUR, background.getScene().getBackgroundColour()
					, &background.getScene(), &Scene::setBackgroundColour );
			}

			void visit( SkyboxBackground & background )override
			{
				if ( !background.getEquiTexturePath().empty() )
				{
					static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE = _( "Equirectangular Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS = _( "Equirectangular Dimensions" );

					m_properties.addPropertyT( &m_grid, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS, background.getEquiSize().getWidth()
						, &background, &SkyboxBackground::setEquiSize );
					m_properties.addProperty( &m_grid, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE, background.getEquiTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< Path >( var );

							if ( File::fileExists( path ) )
							{
								background.loadEquiTexture( Path{}, path, background.getEquiSize().getWidth() );
							}
						} );
				}
				else if ( !background.getCrossTexturePath().empty() )
				{
					static wxString PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE = _( "Cross Image" );

					m_properties.addProperty( &m_grid, PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE, background.getCrossTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< Path >( var );

							if ( File::fileExists( path ) )
							{
								background.loadCrossTexture( Path{}, path );
							}
						} );
				}
				else
				{
					static wxString PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE = _( "Left Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE = _( "Right Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE = _( "Top Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE = _( "Bottom Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE = _( "Front Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE = _( "Back Image" );

					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_RIGHT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveX );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeX );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveY );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeY );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::ePositiveZ );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE
						, background.getTexture()
						, castor3d::CubeMapFace::eNegativeZ );
				}

			}

			void visit( ImageBackground & background )override
			{
				static wxString PROPERTY_BACKGROUND_IMAGE_IMAGE = _( "Image" );

				doCreateTextureImageProperty( background
					, PROPERTY_BACKGROUND_IMAGE_IMAGE
					, background.getTexture() );
			}

			void doCreateTextureImageProperty( ImageBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture )
			{
				CU_Require( texture.isStatic() );
				auto source = texture.getDefaultView().toString();
				m_properties.addProperty( &m_grid, name, source
					, [&background]( wxVariant const & var )
					{
						auto path = variantCast< Path >( var );

						if ( File::fileExists( path ) )
						{
							background.loadImage( Path{}, path );
						}
					} );
			}

			void doCreateTextureImageProperty( SkyboxBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture
				, castor3d::CubeMapFace face )
			{
				CU_Require( texture.isStatic() );
				auto source = castor::Path{ texture.getLayerCubeFaceView( 0u, face ).toString() };
				m_properties.addProperty( &m_grid, name, source
					, [&background, face]( wxVariant const & var )
					{
						auto path = variantCast< Path >( var );

						if ( File::fileExists( path ) )
						{
							background.loadFaceImage( Path{}, path, face );
						}
					} );
			}

		private:
			TreeItemProperty & m_properties;
			wxPropertyGrid & m_grid;
		};
	}

	BackgroundTreeItemProperty::BackgroundTreeItemProperty( wxWindow * parent
		, bool editable
		, SceneBackground & background )
		: TreeItemProperty{ background.getScene().getEngine(), editable }
		, m_background{ background }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	void BackgroundTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BACKGROUND = _( "Background: " );
		static wxString PROPERTY_BACKGROUND_COLOUR = _( "Colour" );
		static wxString PROPERTY_BACKGROUND_IMAGE = _( "Image" );
		static wxString PROPERTY_BACKGROUND_SKYBOX = _( "Skybox" );

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

		default:
			break;
		}

		addProperty( grid, selected + wxT( " " ) + PROPERTY_CATEGORY_BACKGROUND );
		BackgroundDisplayer::submit( m_background, *this, *grid );
	}
}
