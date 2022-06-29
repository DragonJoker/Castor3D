#include "GuiCommon/Properties/TreeItems/BackgroundTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"
#include "GuiCommon/Shader/ShaderSource.hpp"

#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Colour.hpp>
#include <Castor3D/Scene/Background/Image.hpp>
#include <Castor3D/Scene/Background/Skybox.hpp>
#include <Castor3D/Scene/Background/Visitor.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		class BackgroundDisplayer
			: public castor3d::BackgroundVisitor
		{
		public:
			static void submit( castor3d::SceneBackground & background
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

			void visit( castor3d::ColourBackground & background )override
			{
				static wxString PROPERTY_BACKGROUND_COLOUR_COLOUR = _( "Colour" );

				m_properties.addPropertyT( &m_grid, PROPERTY_BACKGROUND_COLOUR_COLOUR, background.getScene().getBackgroundColour()
					, &background.getScene(), &castor3d::Scene::setBackgroundColour );
			}

			void visit( castor3d::SkyboxBackground & background )override
			{
				if ( !background.getEquiTexturePath().empty() )
				{
					static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE = _( "Equirectangular Image" );
					static wxString PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS = _( "Equirectangular Dimensions" );

					m_properties.addPropertyT( &m_grid, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_DIMENSIONS, background.getEquiSize().getWidth()
						, &background, &castor3d::SkyboxBackground::setEquiSize );
					m_properties.addProperty( &m_grid, PROPERTY_BACKGROUND_SKYBOX_EQUIRECTANGULAR_IMAGE, background.getEquiTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< castor::Path >( var );

							if ( castor::File::fileExists( path ) )
							{
								background.loadEquiTexture( castor::Path{}, path, background.getEquiSize().getWidth() );
							}
						} );
				}
				else if ( !background.getCrossTexturePath().empty() )
				{
					static wxString PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE = _( "Cross Image" );

					m_properties.addProperty( &m_grid, PROPERTY_BACKGROUND_SKYBOX_CROSS_IMAGE, background.getCrossTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< castor::Path >( var );

							if ( castor::File::fileExists( path ) )
							{
								background.loadCrossTexture( castor::Path{}, path );
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

			void visit( castor3d::ImageBackground & background )override
			{
				static wxString PROPERTY_BACKGROUND_IMAGE_IMAGE = _( "Image" );

				doCreateTextureImageProperty( background
					, PROPERTY_BACKGROUND_IMAGE_IMAGE
					, background.getTexture() );
			}

			void visit( castor::String const & name
				, float & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int32_t & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2f & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2i & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2ui & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3f & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3i & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3ui & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4f & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4i & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4ui & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Matrix4x4f & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, control );
			}

			void doCreateTextureImageProperty( castor3d::ImageBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture )
			{
				CU_Require( texture.isStatic() );
				auto source = texture.getDefaultView().toString();
				m_properties.addProperty( &m_grid, name, source
					, [&background]( wxVariant const & var )
					{
						auto path = variantCast< castor::Path >( var );

						if ( castor::File::fileExists( path ) )
						{
							background.loadImage( castor::Path{}, path );
						}
					} );
			}

			void doCreateTextureImageProperty( castor3d::SkyboxBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture
				, castor3d::CubeMapFace face )
			{
				CU_Require( texture.isStatic() );
				auto source = castor::Path{ texture.getLayerCubeFaceView( 0u, face ).toString() };
				m_properties.addProperty( &m_grid, name, source
					, [&background, face]( wxVariant const & var )
					{
						auto path = variantCast< castor::Path >( var );

						if ( castor::File::fileExists( path ) )
						{
							background.loadFaceImage( castor::Path{}, path, face );
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
		, castor3d::SceneBackground & background )
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
		static wxString PROPERTY_BACKGROUND_ATMOSPHERE = _( "Atmosphere" );

		wxString selected;
		wxPGChoices choices;
		choices.Add( PROPERTY_BACKGROUND_COLOUR );
		choices.Add( PROPERTY_BACKGROUND_IMAGE );
		choices.Add( PROPERTY_BACKGROUND_SKYBOX );
		choices.Add( PROPERTY_BACKGROUND_ATMOSPHERE );

		if ( m_background.getType() == cuT( "colour" ) )
		{
			selected = PROPERTY_BACKGROUND_COLOUR;
		}
		else if ( m_background.getType() == cuT( "image" ) )
		{
			selected = PROPERTY_BACKGROUND_IMAGE;
		}
		else if ( m_background.getType() == cuT( "skybox" ) )
		{
			selected = PROPERTY_BACKGROUND_SKYBOX;
		}
		else if ( m_background.getType() == cuT( "skybox" ) )
		{
			selected = PROPERTY_BACKGROUND_ATMOSPHERE;
		}

		addProperty( grid, selected + wxT( " " ) + PROPERTY_CATEGORY_BACKGROUND );
		BackgroundDisplayer::submit( m_background, *this, *grid );
	}
}
