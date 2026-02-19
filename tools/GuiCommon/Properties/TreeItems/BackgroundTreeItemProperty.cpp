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
			: public c3d::BackgroundVisitor
		{
		public:
			static void submit( c3d::SceneBackground & background
				, TreeItemProperty & property
				, wxPropertyGrid & grid )
			{
				BackgroundDisplayer vis{ property, grid };
				background.accept( vis );
			}

			explicit BackgroundDisplayer( TreeItemProperty & property
				, wxPropertyGrid & grid )
				: m_properties{ property }
				, m_grid{ grid }
			{
			}

		private:
			void visit( c3d::ColourBackground & background )override
			{
			}

			void visit( c3d::SkyboxBackground & background )override
			{
				m_properties.addPropertyT( &m_grid, _( "Orientation" ), background.getOrientation()
					, &background, &c3d::SkyboxBackground::setOrientation );

				if ( !background.getEquiTexturePath().empty() )
				{
					m_properties.addPropertyT( &m_grid, _( "Equirectangular Dimensions" ), background.getEquiSize().getWidth()
						, &background, &c3d::SkyboxBackground::setEquiSize );
					m_properties.addProperty( &m_grid, _( "Equirectangular Image" ), background.getEquiTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< c3d::Path >( var );
							if ( c3d::File::fileExists( path ) )
								background.setEquiTexture( c3d::Path{}, path, background.getEquiSize().getWidth() );
						} );
				}
				else if ( !background.getCrossTexturePath().empty() )
				{
					m_properties.addProperty( &m_grid, _( "Cross Image" ), background.getCrossTexturePath()
						, [&background]( wxVariant const & var )
						{
							auto path = variantCast< c3d::Path >( var );
							if ( c3d::File::fileExists( path ) )
								background.setCrossTexture( c3d::Path{}, path );
						} );
				}
				else
				{
					c3d::u32 face{};
					for ( auto const & layerTexture : background.getLayerTexturePath() )
					{
						if ( !layerTexture.empty() )
							m_properties.addProperty( &m_grid, wxString{} << c3d::getName( c3d::SkyboxFace( face ) ), layerTexture
								, [&background, face]( wxVariant const & var )
								{
									auto path = variantCast< c3d::Path >( var );
									if ( c3d::File::fileExists( path ) )
										background.setFaceTexture( c3d::Path{}, path, c3d::SkyboxFace( face ) );
								} );
						++face;
					}
				}
			}

			void visit( c3d::ImageBackground & background )override
			{
				CU_Require( background.getTexture().isStatic() );
				auto source = background.getTexture().getDefaultSourceString();
				m_properties.addProperty( &m_grid, _( "Image" ), source
					, [&background]( wxVariant const & var )
					{
						auto path = variantCast< c3d::Path >( var );
						if ( c3d::File::fileExists( path ) )
							background.setImage( c3d::Path{}, path );
					} );
			}

			void visit( c3d::String const & name
				, float & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point2f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point2i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point2ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4i & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4ui & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Matrix4x4f & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< float > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< int32_t > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< uint32_t > & value
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, float & value
				, c3d::Range< float > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, c3d::Range< int32_t > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, c3d::Range< uint32_t > const & range
				, ControlsList controls )override
			{
				m_properties.addPropertyT( &m_grid, name, &value, range, c3d::move( controls ) );
			}

			using ConfigurationVisitorBase::visit;

		private:
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				m_properties.addProperty( &m_grid, category );
				return c3d::makeRawUnique< BackgroundDisplayer >( m_properties, m_grid );
			}

		private:
			TreeItemProperty & m_properties;
			wxPropertyGrid & m_grid;
		};
	}

	BackgroundTreeItemProperty::BackgroundTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::SceneBackground & background )
		: TreeItemProperty{ background.getScene().getEngine(), imagesLoader, editable }
		, m_background{ background }
	{
		CreateTreeItemMenu();
	}

	void BackgroundTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BACKGROUND = _( "Background: " );
		static wxString PROPERTY_BACKGROUND_VISIBILITY = _( "Visibility" );
		static wxString PROPERTY_BACKGROUND_NOTVISIBLE = _( "Not Visible" );
		static wxString PROPERTY_BACKGROUND_IRRADIANCE = _( "Irradiance" );
		static wxString PROPERTY_BACKGROUND_FULL = _( "Full" );

		wxString selected;
		wxArrayString choices;
		choices.Add( PROPERTY_BACKGROUND_NOTVISIBLE );
		choices.Add( PROPERTY_BACKGROUND_IRRADIANCE );
		choices.Add( PROPERTY_BACKGROUND_FULL );

		if ( m_background.isIrradianceShown() )
		{
			selected = PROPERTY_BACKGROUND_IRRADIANCE;
		}
		else if ( m_background.isVisible() )
		{
			selected = PROPERTY_BACKGROUND_FULL;
		}
		else
		{
			selected = PROPERTY_BACKGROUND_NOTVISIBLE;
		}

		setPrefix( m_background.getName() );
		addProperty( grid
			, PROPERTY_CATEGORY_BACKGROUND );
		addProperty( grid, PROPERTY_BACKGROUND_VISIBILITY
			, choices, selected
			, PropertyChangeHandler( [this]( wxVariant const & value )
			{
				switch ( value.GetLong() )
				{
				case 0:
					m_background.showIrradiance( false );
					m_background.setVisible( false );
					break;
				case 1:
					m_background.showIrradiance( true );
					break;
				case 2:
					m_background.showIrradiance( false );
					m_background.setVisible( true );
					break;
				default:
					break;
				}
			} ) );
		BackgroundDisplayer::submit( m_background, *this, *grid );
	}
}
