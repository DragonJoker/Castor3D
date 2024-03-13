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
					}
				}
			}

			return result;
		}

		using onMaskChange = castor::Function< void ( wxVariant const & var
				, castor3d::PassComponentTextureFlag flag
				, uint32_t componentsCount ) >;
		using onEnabledChange = castor::Function< void( wxVariant const & var
			, castor3d::Pass & pass
			, castor::String const & compName ) >;

		class UnitTreeGatherer
			: public castor3d::ConfigurationVisitor
		{
		public:
			static TextureTreeItemProperty::PropertiesArray submit( castor3d::Pass & pass
				, castor3d::TextureConfiguration & config
				, castor::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, wxPGProperty * mainContainer
				, onEnabledChange onEnabled
				, onMaskChange onChange )
			{
				TextureTreeItemProperty::PropertiesArray result;
				UnitTreeGatherer vis{ pass, format, properties, grid, onEnabled, onChange };
				auto & compsRegister = pass.getOwner()->getEngine()->getPassComponentsRegister();

				for ( auto & componentDesc : compsRegister )
				{
					if ( componentDesc.plugin
						&& componentDesc.plugin->isMapComponent() )
					{
						auto component = static_cast< castor3d::PassMapComponentRPtr >( pass.getComponent( componentDesc.name ) );
						castor3d::PassMapComponentUPtr ownComponent;

						if ( !component )
						{
							ownComponent = castor::ptrCast< castor3d::PassMapComponent >( componentDesc.plugin->createComponent( pass ) );

							if ( ownComponent )
							{
								component = ownComponent.get();
							}
						}

						if ( component
							&& ( !pass.hasComponent( componentDesc.name )
								|| hasAny( config.components, component->getTextureFlags() ) ) )
						{
							castor3d::TextureConfiguration configuration;
							configuration.components[0].flag = component->getTextureFlags();
							configuration.components[0].startIndex = 0u;
							auto passCompProps = castor::make_unique< TextureTreeItemProperty::Properties >( component->getTextureFlags()
								, configuration.components[0]
								, castor::move( ownComponent )
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

							result.emplace_back( castor::move( passCompProps ) );
						}
					}
				}

				return result;
			}

		private:
			UnitTreeGatherer( castor3d::Pass & pass
				, castor::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onEnabledChange onEnabled
				, onMaskChange onChange )
				: castor3d::ConfigurationVisitor{}
				, m_pass{ pass }
				, m_format{ format }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onEnabled{ onEnabled }
				, m_onChange{ onChange }
			{
			}

			void visit( castor::String const & name
				, bool & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int16_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int64_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint64_t & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, float & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, double & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::Angle & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor3d::PassComponentTextureFlag textureFlag
				, castor3d::TextureFlagConfiguration & configuration
				, uint32_t componentsCount
				, ConfigurationVisitor::ControlsList controls )override
			{
				doAddProperty( name
					, name + _( " Map" )
					, name + _( " Component" )
					, textureFlag
					, configuration
					, componentsCount );
			}

		private:
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				doVisit( category );
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new UnitTreeGatherer{ m_pass
					, m_format
					, m_properties
					, m_grid
					, m_onEnabled
					, m_onChange } );
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

			void doAddProperty( wxString const & flagName
				, wxString const & isName
				, wxString const & compName
				, castor3d::PassComponentTextureFlag flag
				, castor3d::TextureFlagConfiguration const & configuration
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
					if ( isABGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( isBGRAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( isRGBAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( isARGBFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
					}
					else if ( isBGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( isRGFormat( m_format ) )
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
					if ( isABGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( isBGRAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( isRGBAFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
					}
					else if ( isARGBFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_A );
						m_compProps->choices.Add( PROPERTY_COMPONENT_R );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( isBGRFormat( m_format ) )
					{
						m_compProps->choices.Add( PROPERTY_COMPONENT_B );
						m_compProps->choices.Add( PROPERTY_COMPONENT_G );
					}
					else if ( isRGFormat( m_format ) )
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
					if ( isABGRFormat( m_format )
						|| isBGRAFormat( m_format )
						|| isARGBFormat( m_format )
						|| isRGBAFormat( m_format ) )
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
			castor3d::Pass & m_pass;
			castor::PixelFormat m_format;
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

	TextureTreeItemProperty::TextureTreeItemProperty( bool editable
		, castor3d::Pass & pass
		, castor3d::TextureUnit & texture )
		: TreeItemProperty( texture.getEngine(), editable )
		, m_pass{ pass }
		, m_texture{ texture }
		, m_configuration{ m_texture.getConfiguration() }
	{
		CreateTreeItemMenu();
	}

	void TextureTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString const CATEGORY_TEXTURE = _( "Texture " );
		static wxString const PROPERTY_TEXTURE_BASE = _( "Base" );
		static wxString const PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString const PROPERTY_TEXTURE_TEXCOORDSET = _( "Texcoord Set" );

		auto unit = &m_texture;
		m_configuration = unit->getConfiguration();
		addProperty( grid, wxString{ CATEGORY_TEXTURE } << unit->getId() );
		auto mainContainer = addProperty( grid, PROPERTY_TEXTURE_BASE );
		addPropertyT( mainContainer, PROPERTY_TEXTURE_TEXCOORDSET, unit->getTexcoordSet()
			, unit
			, &castor3d::TextureUnit::setTexcoordSet );

		if ( unit->isTextureStatic() )
		{
			m_path = castor::Path{ unit->getTexturePath() };
			addProperty( mainContainer, PROPERTY_TEXTURE_IMAGE, m_path
				, [this]( wxVariant const & var )
				{
					auto unit = &m_texture;
					castor::Path path{ make_String( var.GetString() ) };

					// Absolute path
					if ( castor::File::fileExists( path ) )
					{
						auto & sourceInfo = unit->getSourceInfo();
						m_pass.resetTexture( sourceInfo
							, castor3d::TextureSourceInfo{ path.getFileName( true )
								, sourceInfo.textureConfig()
								, path.getPath()
								, path.getFileName( true )
								, sourceInfo.loadConfig() } );
					}
				} );
		}

		auto & transform = unit->getTransform();
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
				m_translate = variantCast< castor::Point2f >( value );
				auto transform = m_texture.getTransform();
				transform.translate->x = m_translate->x;
				transform.translate->y = m_translate->y;
				m_texture.setTransform( transform );
			} );
		addProperty( transformCont, PROPERTY_TRANSFORM_UV_ROTATE
			, m_rotate
			, [this]( wxVariant const & value )
			{
				m_rotate = variantCast< castor::Angle >( value );
				auto transform = m_texture.getTransform();
				transform.rotate = m_rotate;
				m_texture.setTransform( transform );
			} );
		addProperty( transformCont, PROPERTY_TRANSFORM_UV_SCALE
			, m_scale
			, [this]( wxVariant const & value )
			{
				m_scale = variantCast< castor::Point2f >( value );
				auto transform = m_texture.getTransform();
				transform.scale->x = m_scale->x;
				transform.scale->y = m_scale->y;
				m_texture.setTransform( transform );
			} );

		if ( unit->hasAnimation() )
		{
			static wxString const CATEGORY_ANIMATION = _( "Animation" );
			static wxString const PROPERTY_ANIMATION_TRANSLATE = _( "Translate" );
			static wxString const PROPERTY_ANIMATION_ROTATE = _( "Rotate" );
			static wxString const PROPERTY_ANIMATION_SCALE = _( "Scale" );

			auto & anim = unit->getAnimation();
			auto animCont = addProperty( mainContainer, CATEGORY_ANIMATION );
			addPropertyT( animCont, PROPERTY_ANIMATION_TRANSLATE, anim.getTranslateSpeed(), &anim, &castor3d::TextureAnimation::setTranslateSpeed );
			addPropertyT( animCont, PROPERTY_ANIMATION_ROTATE, anim.getRotateSpeed(), &anim, &castor3d::TextureAnimation::setRotateSpeed );
			addPropertyT( animCont, PROPERTY_ANIMATION_SCALE, anim.getScaleSpeed(), &anim, &castor3d::TextureAnimation::setScaleSpeed );
		}

		m_properties = textp::UnitTreeGatherer::submit( m_pass
			, m_configuration
			, castor::PixelFormat( unit->getTexturePixelFormat() )
			, this
			, grid
			, mainContainer
			, [this]( wxVariant const & value
				, castor3d::Pass & pass
				, castor::String const & compName )
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
				auto unit = &m_texture;
				compProps.configuration.componentsMask = textp::getMask( enable, components, compProps.componentsCount );

				if ( enable )
				{
					if ( compProps.ownComponent )
					{
						try
						{
							addFlagConfiguration( m_configuration, compProps.configuration );
							moveComponentsToPass( castor::ptrRefCast< castor3d::PassComponent >( compProps.ownComponent ) );
							m_pass.updateConfig( unit->getSourceInfo(), m_configuration );
						}
						catch ( std::exception & exc )
						{
							castor3d::log::error << exc.what() << std::endl;
							enable = false;
							compProps.isSetting = true;
							compProps.isEnabled->SetValue( false );
							compProps.isSetting = false;
						}
					}
				}
				else if ( pass.hasComponent( compProps.component->getType() ) )
				{
					auto removed = pass.removeComponent( compProps.component->getType() );

					if ( !removed.empty() )
					{
						compProps.ownComponent = castor::ptrRefCast< castor3d::PassMapComponent >( removed.back() );
						removeFlagConfiguration( m_configuration, compProps.configuration );
						removed.pop_back();
						moveComponentsToProps( castor::move( removed ) );
					}

					CU_Require( compProps.ownComponent );
				}

				compProps.components->Enable( enable );

				for ( auto & prop : compProps.properties )
				{
					prop->Enable( enable );
				}

			}
			, [this]( wxVariant const & var
				, castor3d::PassComponentTextureFlag flag
				, uint32_t componentsCount )
			{
				auto unit = &m_texture;
				auto it = std::find_if( m_properties.begin()
					, m_properties.end()
					, [&flag]( auto & lookup )
					{
						return lookup->flag == flag;
					} );
				CU_Require( it != m_properties.end() );
				auto & props = *it;
				bool isEnabled = props->isEnabled->GetValue();
				long components = props->components->GetValue();
				props->configuration.componentsMask = textp::getMask( isEnabled, components, componentsCount );

				for ( auto prop : props->properties )
				{
					prop->Enable( isEnabled );
				}

				m_pass.updateConfig( unit->getSourceInfo(), m_configuration );
			} );
	}

	void TextureTreeItemProperty::moveComponentsToPass( castor3d::PassComponentUPtr component )
	{
		auto & pass = *component->getOwner();

		for ( auto dep : component->getDependencies() )
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
				moveComponentsToPass( castor::ptrRefCast< castor3d::PassComponent >( ( *it )->ownComponent ) );
			}
		}

		pass.addComponent( castor::move( component ) );
	}

	void TextureTreeItemProperty::moveComponentsToProps( castor::Vector< castor3d::PassComponentUPtr > removed )
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
				( *it )->ownComponent = castor::ptrRefCast< castor3d::PassMapComponent >( rem );
			}
		}
	}

	//*********************************************************************************************
}

#pragma GCC diagnostic pop
