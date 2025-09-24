#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Pass/Component/PassMapComponent.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/Animation/TextureAnimation.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <wx/propgrid/advprops.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

namespace GuiCommon
{
	//*********************************************************************************************

	namespace textp
	{
		static uint32_t getMask( bool enabled
			, long component
			, uint32_t componentsCount )
		{
			uint32_t result = 0u;

			if ( enabled )
			{
				if ( componentsCount == 1u )
				{
					switch ( component )
					{
					case 0:
						result = 0x000000FF;
						break;
					case 1:
						result = 0x0000FF00;
						break;
					case 2:
						result = 0x00FF0000;
						break;
					case 3:
						result = 0xFF000000;
						break;
					default:
						break;
					}
				}
				else if ( componentsCount == 2u )
				{
					switch ( component )
					{
					case 0:
						result = 0x0000FFFF;
						break;
					case 1:
						result = 0x00FFFF00;
						break;
					case 2:
						result = 0xFFFF0000;
						break;
					default:
						break;
					}
				}
				else
				{
					switch ( component )
					{
					case 0:
						result = 0x00FFFFFF;
						break;
					case 1:
						result = 0xFFFFFF00;
						break;
					default:
						break;
					}
				}
			}

			return result;
		}

		using onMaskChange = c3d::Function< void ( wxVariant const & var
				, c3d::PassComponentTextureFlag flag
				, uint32_t componentsCount ) >;
		using onEnabledChange = c3d::Function< void( wxVariant const & var
			, c3d::Pass & pass
			, c3d::String const & compName ) >;

		class UnitTreeGatherer
			: public c3d::ConfigurationVisitor
		{
		public:
			static TextureTreeItemProperty::PropertiesArray submit( c3d::Pass & pass
				, c3d::TextureConfiguration const & config
				, c3d::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, wxPGProperty * mainContainer
				, onEnabledChange const & onEnabled
				, onMaskChange const & onChange )
			{
				TextureTreeItemProperty::PropertiesArray result;
				UnitTreeGatherer vis{ pass, format, properties, grid, onEnabled, onChange };
				auto const & compsRegister = pass.getOwner()->getEngine()->getPassComponentsRegister();

				for ( auto & componentDesc : compsRegister )
				{
					if ( componentDesc.plugin
						&& componentDesc.plugin->isMapComponent() )
					{
						auto component = static_cast< c3d::PassMapComponentRPtr >( pass.getComponent( componentDesc.name ) );
						c3d::PassMapComponentUPtr ownComponent;

						if ( !component )
						{
							ownComponent = c3d::ptrCast< c3d::PassMapComponent >( componentDesc.plugin->createComponent( pass ) );

							if ( ownComponent )
							{
								component = ownComponent.get();
							}
						}

						if ( component
							&& ( !pass.hasComponent( componentDesc.name )
								|| hasAny( config.components, component->getTextureFlags() ) ) )
						{
							c3d::TextureConfiguration configuration;
							configuration.components[0].flag = component->getTextureFlags();
							configuration.components[0].startIndex = 0u;
							auto passCompProps = c3d::makeRawUnique< TextureTreeItemProperty::Properties >( component->getTextureFlags()
								, configuration.components[0]
								, c3d::move( ownComponent )
								, component );
							auto compProps = passCompProps.get();
							compProps->container = mainContainer;

							vis.m_compProps = compProps;
							vis.m_result = &compProps->properties;
							vis.m_enabled = pass.hasComponent( componentDesc.name )
								&& hasAny( config.components, component->getTextureFlags() );

							compProps->component->fillConfig( configuration, vis );
							compProps->components->Enable( vis.m_enabled );

							for ( auto & compProp : compProps->properties )
							{
								compProp->Enable( vis.m_enabled );
							}

							if ( compProps->container
								&& !vis.m_enabled )
							{
								compProps->container->SetExpanded( false );
							}

							result.emplace_back( c3d::move( passCompProps ) );
						}
					}
				}

				return result;
			}

			UnitTreeGatherer( c3d::Pass & pass
				, c3d::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onEnabledChange const & onEnabled
				, onMaskChange const & onChange )
				: c3d::ConfigurationVisitor{}
				, m_pass{ pass }
				, m_format{ format }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onEnabled{ onEnabled }
				, m_onChange{ onChange }
			{
			}

		private:
			void visit( c3d::String const & name
				, bool & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int16_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint16_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int64_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint64_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, float & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, double & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::Angle & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::LuminousIntensity & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::Illumination & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbColour & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbaColour & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< float > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< int32_t > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< uint32_t > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::PassComponentTextureFlag textureFlag
				, c3d::TextureFlagConfiguration & configuration
				, uint32_t componentsCount
				, ConfigurationVisitor::ControlsList controls )override
			{
				doSink( c3d::move( controls ) );
				doAddProperty( name + _( " Component" )
					, textureFlag
					, configuration
					, componentsCount );
			}

		private:
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				doVisit( category );
				return c3d::makeRawUnique< UnitTreeGatherer >( m_pass
					, m_format
					, m_properties
					, m_grid
					, m_onEnabled
					, m_onChange );
			}

			void doVisit( wxString const & name )
			{
				m_properties->setPrefix( make_String( name ) );
				m_compProps->container = m_properties->addProperty( m_grid, name );
				auto pass = &m_pass;
				auto compName = m_compProps->component->getType();
				auto onEnabled = m_onEnabled;
				m_compProps->isEnabled = m_properties->addProperty( m_compProps->container
					, _( "Enabled" )
					, m_enabled
					, [onEnabled, pass, compName]( wxVariant const & value )
					{
						onEnabled( value, *pass, compName );
					} );
				m_compProps->isEnabled->SetAttribute( wxPG_BOOL_USE_CHECKBOX, true );
			}

			void doAddProperty( wxString const & compName
				, c3d::PassComponentTextureFlag flag
				, c3d::TextureFlagConfiguration const & configuration
				, uint32_t componentsCount )
			{
				static wxString PROPERTY_COMPONENT_A = wxT( "A" );
				static wxString PROPERTY_COMPONENT_R = wxT( "R" );
				static wxString PROPERTY_COMPONENT_G = wxT( "G" );
				static wxString PROPERTY_COMPONENT_B = wxT( "B" );
				static wxString PROPERTY_COMPONENT_RGB = wxT( "RGB" );
				static wxString PROPERTY_COMPONENT_GBA = wxT( "GBA" );

				wxString selected;

				if ( componentsCount == 1u )
				{
					if ( c3d::isABGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( c3d::isBGRAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( c3d::isRGBAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( c3d::isARGBFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
					}
					else if ( c3d::isBGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( c3d::isRGFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
					}
				}
				else if ( componentsCount == 2u )
				{
					if ( c3d::isABGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( c3d::isBGRAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( c3d::isRGBAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
					}
					else if ( c3d::isARGBFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( c3d::isBGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( c3d::isRGFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
				}
				else
				{
					if ( c3d::isABGRFormat( m_format )
						|| c3d::isBGRAFormat( m_format )
						|| c3d::isARGBFormat( m_format )
						|| c3d::isRGBAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_RGB );
						m_compProps->choices.Add( PROPERTY_COMPONENT_GBA );
					}
					else
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_RGB );
					}
				}

				selected = m_compProps->choices[configuration.startIndex];
				auto onChange = m_onChange;
				m_compProps->componentsCount = componentsCount;
				m_compProps->components = m_properties->addProperty( m_grid
					, compName
					, m_compProps->choices
					, selected
					, [onChange, flag, componentsCount]( wxVariant const & var )
					{
						onChange( var, flag, componentsCount );
					} );
			}

		private:
			c3d::Pass & m_pass;
			c3d::PixelFormat m_format;
			TextureTreeItemProperty * m_properties;
			wxPropertyGrid * m_grid;
			onEnabledChange m_onEnabled;
			onMaskChange m_onChange;
			PropertyArray * m_result{};
			bool m_enabled{};
			TextureTreeItemProperty::Properties * m_compProps{};
		};
	}

	//*********************************************************************************************

	TextureTreeItemProperty::TextureTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void TextureTreeItemProperty::setData( c3d::Pass & pass
		, c3d::TextureUnit & texture )noexcept
	{
		clearProperties();
		m_pass = &pass;
		m_texture = &texture;
		m_configuration = texture.getConfiguration();
	}

	void TextureTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString const CATEGORY_TEXTURE = _( "Texture " );
		static wxString const PROPERTY_TEXTURE_BASE = _( "Base" );
		static wxString const PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString const PROPERTY_TEXTURE_TEXCOORDSET = _( "Texcoord Set" );

		if ( !m_pass || !m_texture )
		{
			return;
		}

		m_configuration = m_texture->getConfiguration();
		addProperty( grid, wxString{ CATEGORY_TEXTURE } << m_texture->getId() );
		auto mainContainer = addProperty( grid, PROPERTY_TEXTURE_BASE );
		addPropertyT( mainContainer, PROPERTY_TEXTURE_TEXCOORDSET, m_texture->getTexcoordSet()
			, m_texture
			, &c3d::TextureUnit::setTexcoordSet );

		if ( m_texture->isTextureStatic() )
		{
			m_textureSource = m_texture->getSourceInfo();
			addProperty( mainContainer, PROPERTY_TEXTURE_IMAGE, &m_textureSource
				, [this]( wxVariant const & )
				{
					auto & sourceInfo = m_texture->getSourceInfo();
					m_pass->resetTexture( sourceInfo, m_textureSource );
				} );
		}

		auto & transform = m_texture->getTransform();
		m_translate->x = transform.translate->x;
		m_translate->y = transform.translate->y;
		m_scale->x = transform.scale->x;
		m_scale->y = transform.scale->y;
		m_rotate = transform.rotate;
		static wxString const CATEGORY_TRANSFORM = _( "Transform" );
		static wxString const PROPERTY_TRANSFORM_UV_TRANSLATE = _( "UV Translate" );
		static wxString const PROPERTY_TRANSFORM_UV_ROTATE = _( "UV Rotate" );
		static wxString const PROPERTY_TRANSFORM_UV_SCALE = _( "UV Scale" );
		auto transformCont = addProperty( mainContainer, CATEGORY_TRANSFORM );
		addProperty( transformCont, PROPERTY_TRANSFORM_UV_TRANSLATE
			, m_translate
			, [this]( wxVariant const & value )
			{
				m_translate = variantCast< c3d::Point2f >( value );
				auto transform = m_texture->getTransform();
				transform.translate->x = m_translate->x;
				transform.translate->y = m_translate->y;
				m_texture->setTransform( transform );
			} );
		addProperty( transformCont, PROPERTY_TRANSFORM_UV_ROTATE
			, m_rotate
			, [this]( wxVariant const & value )
			{
				m_rotate = variantCast< c3d::Angle >( value );
				auto transform = m_texture->getTransform();
				transform.rotate = m_rotate;
				m_texture->setTransform( transform );
			} );
		addProperty( transformCont, PROPERTY_TRANSFORM_UV_SCALE
			, m_scale
			, [this]( wxVariant const & value )
			{
				m_scale = variantCast< c3d::Point2f >( value );
				auto transform = m_texture->getTransform();
				transform.scale->x = m_scale->x;
				transform.scale->y = m_scale->y;
				m_texture->setTransform( transform );
			} );

		if ( m_texture->hasAnimation() )
		{
			static wxString const CATEGORY_ANIMATION = _( "Animation" );
			static wxString const PROPERTY_ANIMATION_TRANSLATE = _( "Translate" );
			static wxString const PROPERTY_ANIMATION_ROTATE = _( "Rotate" );
			static wxString const PROPERTY_ANIMATION_SCALE = _( "Scale" );

			auto & anim = m_texture->getAnimation();
			auto animCont = addProperty( mainContainer, CATEGORY_ANIMATION );
			addPropertyT( animCont, PROPERTY_ANIMATION_TRANSLATE, anim.getTranslateSpeed(), &anim, &c3d::TextureAnimation::setTranslateSpeed );
			addPropertyT( animCont, PROPERTY_ANIMATION_ROTATE, anim.getRotateSpeed(), &anim, &c3d::TextureAnimation::setRotateSpeed );
			addPropertyT( animCont, PROPERTY_ANIMATION_SCALE, anim.getScaleSpeed(), &anim, &c3d::TextureAnimation::setScaleSpeed );
		}

		m_properties = textp::UnitTreeGatherer::submit( *m_pass
			, m_configuration
			, c3d::PixelFormat( m_texture->getTexturePixelFormat() )
			, this
			, grid
			, mainContainer
			, [this]( wxVariant const & value
				, c3d::Pass & pass
				, c3d::String const & compName )
			{
				auto it = std::find_if( m_properties.begin()
					, m_properties.end()
					, [&compName]( PropertiesPtr const & lookup )
					{
						return lookup->component->getType() == compName;
					} );

				if ( it == m_properties.end() )
				{
					return;
				}

				auto & compProps = **it;

				if ( !compProps.component
					|| compProps.isSetting )
				{
					return;
				}

				auto enable = value.GetBool();
				long components = compProps.components->GetValue();
				compProps.configuration.componentsMask = textp::getMask( enable, components, compProps.componentsCount );

				if ( enable )
				{
					if ( compProps.ownComponent )
					{
						try
						{
							addFlagConfiguration( m_configuration, compProps.configuration );
							moveComponentsToPass( c3d::ptrRefCast< c3d::PassComponent >( compProps.ownComponent ) );
							m_pass->updateConfig( m_texture->getSourceInfo(), m_configuration );
						}
						catch ( std::exception & exc )
						{
							c3d::log::error << exc.what() << std::endl;
							enable = false;
							compProps.isSetting = true;
							compProps.isEnabled->SetValue( false );
							compProps.isSetting = false;
						}
					}
				}
				else if ( pass.hasComponent( compProps.component->getType() ) )
				{
					if ( auto removed = pass.removeComponent( compProps.component->getType() );
						!removed.empty() )
					{
						compProps.ownComponent = c3d::ptrRefCast< c3d::PassMapComponent >( removed.back() );
						removeFlagConfiguration( m_configuration, compProps.configuration );
						removed.pop_back();
						moveComponentsToProps( c3d::move( removed ) );
					}

					CU_Require( compProps.ownComponent );
				}

				compProps.components->Enable( enable );

				for ( auto & prop : compProps.properties )
				{
					prop->Enable( enable );
				}

			}
			, [this]( wxVariant const &
				, c3d::PassComponentTextureFlag flag
				, uint32_t componentsCount )
			{
				auto it = std::find_if( m_properties.begin()
					, m_properties.end()
					, [&flag]( auto & lookup )
					{
						return lookup->flag == flag;
					} );
				CU_Require( it != m_properties.end() );
				auto const & props = *it;
				bool isEnabled = props->isEnabled->GetValue();
				long components = props->components->GetValue();
				props->configuration.componentsMask = textp::getMask( isEnabled, components, componentsCount );

				for ( auto prop : props->properties )
				{
					prop->Enable( isEnabled );
				}

				m_pass->updateConfig( m_texture->getSourceInfo(), m_configuration );
			} );
	}

	void TextureTreeItemProperty::moveComponentsToPass( c3d::PassComponentUPtr component )
	{
		auto & pass = *component->getOwner();

		for ( auto const & dep : component->getDependencies() )
		{
			auto it = std::find_if( m_properties.begin()
				, m_properties.end()
				, [&dep]( PropertiesPtr const & lookup )
				{
					return lookup->ownComponent
						&& lookup->ownComponent->getType() == dep;
				} );

			if ( it != m_properties.end() )
			{
				moveComponentsToPass( c3d::ptrRefCast< c3d::PassComponent >( ( *it )->ownComponent ) );
			}
		}

		pass.addComponent( c3d::move( component ) );
	}

	void TextureTreeItemProperty::moveComponentsToProps( c3d::Vector< c3d::PassComponentUPtr > removed )
	{
		for ( auto & rem : removed )
		{
			auto it = std::find_if( m_properties.begin()
				, m_properties.end()
				, [&rem]( PropertiesPtr const & lookup )
				{
					return lookup->component->getType() == rem->getType();
				} );

			if ( it != m_properties.end() )
			{
				( *it )->ownComponent = c3d::ptrRefCast< c3d::PassMapComponent >( rem );
			}
		}
	}

	//*********************************************************************************************
}

#pragma GCC diagnostic pop
