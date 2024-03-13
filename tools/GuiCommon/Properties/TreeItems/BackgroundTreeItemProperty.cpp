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
								background.setEquiTexture( castor::Path{}, path, background.getEquiSize().getWidth() );
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
								background.setCrossTexture( castor::Path{}, path );
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
						, castor3d::SkyboxFace::eLeft );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_LEFT_IMAGE
						, background.getTexture()
						, castor3d::SkyboxFace::eRight );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_BOTTOM_IMAGE
						, background.getTexture()
						, castor3d::SkyboxFace::eTop );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_TOP_IMAGE
						, background.getTexture()
						, castor3d::SkyboxFace::eBottom );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_BACK_IMAGE
						, background.getTexture()
						, castor3d::SkyboxFace::eFront );
					doCreateTextureImageProperty( background
						, PROPERTY_BACKGROUND_SKYBOX_FRONT_IMAGE
						, background.getTexture()
						, castor3d::SkyboxFace::eBack );
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
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point2f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point2i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point2ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Matrix4x4f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, float & value
				, castor::Range< float > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, castor::Range< int32_t > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, castor::move( controls ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, castor::Range< uint32_t > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, castor::move( controls ) );
			}

			void doCreateTextureImageProperty( castor3d::ImageBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture )
			{
				CU_Require( texture.isStatic() );
				auto source = texture.getDefaultSourceString();
				m_properties.addProperty( &m_grid, name, source
					, [&background]( wxVariant const & var )
					{
						auto path = variantCast< castor::Path >( var );

						if ( castor::File::fileExists( path ) )
						{
							background.setImage( castor::Path{}, path );
						}
					} );
			}

			void doCreateTextureImageProperty( castor3d::SkyboxBackground & background
				, wxString const & name
				, castor3d::TextureLayout const & texture
				, castor3d::SkyboxFace face )
			{
				CU_Require( texture.isStatic() );
				auto source = castor::Path{ texture.getLayerCubeFaceSourceString( 0u
					, castor3d::CubeMapFace( uint32_t( face ) ) ) };
				m_properties.addProperty( &m_grid, name, source
					, [&background, face]( wxVariant const & var )
					{
						auto path = variantCast< castor::Path >( var );

						if ( castor::File::fileExists( path ) )
						{
							background.setFaceTexture( castor::Path{}, path, face );
						}
					} );
			}

			using ConfigurationVisitorBase::visit;

		private:
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				m_properties.addProperty( &m_grid, category );
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new BackgroundDisplayer{ m_properties, m_grid } );
			}

		private:
			TreeItemProperty & m_properties;
			wxPropertyGrid & m_grid;
		};
	}

	BackgroundTreeItemProperty::BackgroundTreeItemProperty( bool editable
		, castor3d::SceneBackground & background )
		: TreeItemProperty{ background.getScene().getEngine(), editable }
		, m_background{ background }
	{
		CreateTreeItemMenu();
	}

	void BackgroundTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BACKGROUND = _( "Background: " );
		static wxString PROPERTY_BACKGROUND_VISIBLE = _( "Visible" );
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
		else if ( m_background.getType() == cuT( "c3d.atmosphere" ) )
		{
			selected = PROPERTY_BACKGROUND_ATMOSPHERE;
		}

		setPrefix( m_background.getName() );
		addProperty( grid
			, selected + wxT( " " ) + PROPERTY_CATEGORY_BACKGROUND );
		addPropertyT( grid, PROPERTY_BACKGROUND_VISIBLE
			, m_background.isVisible()
			, &m_background
			, &castor3d::SceneBackground::setVisible );
		addPropertyT( grid, PROPERTY_BACKGROUND_COLOUR
			, m_background.getScene().getBackgroundColour()
			, &m_background.getScene()
			, &castor3d::Scene::setBackgroundColour );
		BackgroundDisplayer::submit( m_background, *this, *grid );
	}
}
