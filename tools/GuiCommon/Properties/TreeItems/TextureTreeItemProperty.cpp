#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/Animation/TextureAnimation.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	//*********************************************************************************************

	namespace
	{
		uint32_t getMask( bool enabled
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

		using onMaskChange = std::function< void ( wxVariant const & var
				, castor3d::PassComponentTextureFlag flag
				, uint32_t componentsCount ) >;

		class UnitTreeGatherer
			: public castor3d::PassVisitor
		{
		public:
			static std::map< castor3d::PassComponentTextureFlag, TextureTreeItemProperty::PropertyPair > submit( castor3d::Pass & pass
				, castor3d::TextureConfiguration & config
				, castor::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onMaskChange onChange )
			{
				std::map< castor3d::PassComponentTextureFlag, TextureTreeItemProperty::PropertyPair > result;
				UnitTreeGatherer vis{ format, properties, grid, onChange, result };
				pass.fillConfig( config, vis );
				return result;
			}

		private:
			UnitTreeGatherer( castor::PixelFormat format
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onMaskChange onChange
				, std::map< castor3d::PassComponentTextureFlag, TextureTreeItemProperty::PropertyPair > & result )
				: castor3d::PassVisitor{ {} }
				, m_format{ format }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onChange{ onChange }
				, m_result{ result }
			{
			}

			void visit( castor::String const & name
				, bool & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, int16_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, int64_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, uint64_t & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, float & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, double & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor3d::BlendMode & value
				, PassVisitor::ControlsList controls )override
			{
				wxArrayString choices;
				choices.push_back( _( "No Blend" ) );
				choices.push_back( _( "Additive" ) );
				choices.push_back( _( "Multiplicative" ) );
				choices.push_back( _( "Interpolative" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor3d::ParallaxOcclusionMode & value
				, PassVisitor::ControlsList controls )override
			{
				wxArrayString choices;
				choices.push_back( _( "None" ) );
				choices.push_back( _( "One" ) );
				choices.push_back( _( "Repeat" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, VkCompareOp & value
				, PassVisitor::ControlsList controls )override
			{
				wxArrayString choices;
				choices.push_back( _( "Never" ) );
				choices.push_back( _( "Less" ) );
				choices.push_back( _( "Equal" ) );
				choices.push_back( _( "Less Equal" ) );
				choices.push_back( _( "Greater" ) );
				choices.push_back( _( "Not Equal" ) );
				choices.push_back( _( "Greater Equal" ) );
				choices.push_back( _( "Always" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, PassVisitor::ControlsList controls )override
			{
				m_properties->addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor3d::PassComponentTextureFlag textureFlag
				, castor3d::TextureFlagConfiguration & configuration
				, uint32_t componentsCount
				, PassVisitor::ControlsList controls )override
			{
				doAddProperty( name
					, name + _( " Map" )
					, name + _( " Component" )
					, textureFlag
					, configuration
					, componentsCount );
			}

		private:
			void doAddProperty( wxString const & flagName
				, wxString const & isName
				, wxString const & compName
				, castor3d::PassComponentTextureFlag flag
				, castor3d::TextureFlagConfiguration & configuration
				, uint32_t componentsCount )
			{
				auto onChange = m_onChange;
				static wxString PROPERTY_COMPONENT_A = wxT( "A" );
				static wxString PROPERTY_COMPONENT_R = wxT( "R" );
				static wxString PROPERTY_COMPONENT_G = wxT( "G" );
				static wxString PROPERTY_COMPONENT_B = wxT( "B" );
				static wxString PROPERTY_COMPONENT_RGB = wxT( "RGB" );
				static wxString PROPERTY_COMPONENT_GBA = wxT( "GBA" );

				m_grid->Append( new wxPropertyCategory( flagName ) );
				m_properties->addProperty( m_grid, flagName );
				auto isProp = m_properties->addProperty( m_grid
					, isName
					, configuration.componentsMask != 0
					, [onChange, flag, componentsCount]( wxVariant const & var )
					{
						onChange( var, flag, componentsCount );
					} );
				wxString name;
				wxString selected;
				wxArrayString choices;

				if ( componentsCount == 1u )
				{
					if ( isABGRFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_A );
						choices.Add( PROPERTY_COMPONENT_B );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_R );
					}
					else if ( isBGRAFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_B );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_R );
						choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( isRGBAFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_R );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_B );
						choices.Add( PROPERTY_COMPONENT_A );
					}
					else if ( isARGBFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_A );
						choices.Add( PROPERTY_COMPONENT_R );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_B );
					}
					else if ( isBGRFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_B );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_R );
					}
					else
					{
						choices.Add( PROPERTY_COMPONENT_R );
						choices.Add( PROPERTY_COMPONENT_G );
						choices.Add( PROPERTY_COMPONENT_B );
					}

					selected = choices[configuration.startIndex];
				}
				else
				{
					if ( isABGRFormat( m_format )
						|| isBGRAFormat( m_format )
						|| isARGBFormat( m_format )
						|| isRGBAFormat( m_format ) )
					{
						choices.Add( PROPERTY_COMPONENT_RGB );
						choices.Add( PROPERTY_COMPONENT_GBA );
					}
					else
					{
						choices.Add( PROPERTY_COMPONENT_RGB );
					}
					selected = choices[configuration.startIndex];
				}

				auto compProp = m_properties->addProperty( m_grid
					, compName
					, choices
					, selected
					, [onChange, flag, componentsCount]( wxVariant const & var )
					{
						onChange( var, flag, componentsCount );
					} );
				compProp->Enable( configuration.componentsMask != 0 );
				m_result.emplace( flag, TextureTreeItemProperty::PropertyPair{ isProp, compProp, configuration } );
			}

		private:
			castor::PixelFormat m_format;
			TextureTreeItemProperty * m_properties;
			wxPropertyGrid * m_grid;
			onMaskChange m_onChange;
			std::map< castor3d::PassComponentTextureFlag, TextureTreeItemProperty::PropertyPair > & m_result;
		};
	}

	//*********************************************************************************************

	TextureTreeItemProperty::TextureTreeItemProperty( bool editable
		, castor3d::Pass & pass
		, castor3d::TextureUnitSPtr texture )
		: TreeItemProperty( texture->getEngine(), editable )
		, m_pass{ pass }
		, m_texture{ *texture }
		, m_configuration{ texture->getConfiguration() }
	{
		CreateTreeItemMenu();
	}

	void TextureTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		auto unit = &m_texture;

		if ( unit )
		{
			static wxString const CATEGORY_TEXTURE = _( "Texture " );
			static wxString const PROPERTY_TEXTURE_IMAGE = _( "Image" );
			static wxString const PROPERTY_TEXTURE_TEXCOORDSET = _( "Texcoord Set" );

			grid->Append( new wxPropertyCategory( wxString{ CATEGORY_TEXTURE } << unit->getId() ) );

			if ( unit->getTexture()->isStatic() )
			{
				m_path = castor::Path{ unit->getTexture()->getPath() };
				addProperty( grid, PROPERTY_TEXTURE_IMAGE, m_path
					, [this]( wxVariant const & var )
					{
						onImageChange( var );
					} );
			}

			addPropertyT( grid, PROPERTY_TEXTURE_TEXCOORDSET, unit->getTexcoordSet(), unit, &castor3d::TextureUnit::setTexcoordSet );
			m_properties = UnitTreeGatherer::submit( m_pass
				, m_configuration
				, castor::PixelFormat( unit->getTexture()->getPixelFormat() )
				, this
				, grid
				, [this]( wxVariant const & var
					, castor3d::PassComponentTextureFlag flag
					, uint32_t componentsCount )
				{
					onChange( var, flag, componentsCount );
				} );

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
			grid->Append( new wxPropertyCategory( CATEGORY_TRANSFORM ) );
			addProperty( grid, PROPERTY_TRANSFORM_UV_TRANSLATE
				, m_translate
				, [this]( wxVariant const & value )
				{
					m_translate = variantCast< castor::Point2f >( value );
					auto transform = m_texture.getTransform();
					transform.translate->x = m_translate->x;
					transform.translate->y = m_translate->y;
					m_texture.setTransform( transform );
				} );
			addProperty( grid, PROPERTY_TRANSFORM_UV_ROTATE
				, m_rotate
				, [this]( wxVariant const & value )
				{
					m_rotate = variantCast< castor::Angle >( value );
					auto transform = m_texture.getTransform();
					transform.rotate = m_rotate;
					m_texture.setTransform( transform );
				} );
			addProperty( grid, PROPERTY_TRANSFORM_UV_SCALE
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
				grid->Append( new wxPropertyCategory( CATEGORY_ANIMATION ) );
				addPropertyT( grid, PROPERTY_ANIMATION_TRANSLATE, anim.getTranslateSpeed(), &anim, &castor3d::TextureAnimation::setTranslateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_ROTATE, anim.getRotateSpeed(), &anim, &castor3d::TextureAnimation::setRotateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_SCALE, anim.getScaleSpeed(), &anim, &castor3d::TextureAnimation::setScaleSpeed );
			}
		}
	}

	void TextureTreeItemProperty::onChange( wxVariant const & var
		, castor3d::PassComponentTextureFlag flag
		, uint32_t componentsCount )
	{
		auto unit = &m_texture;
		auto it = m_properties.find( flag );
		CU_Require( it != m_properties.end() );
		bool isMap = it->second.isMap->GetValue();
		long components = it->second.components->GetValue();
		it->second.components->Enable( isMap );
		it->second.configuration.componentsMask = getMask( isMap, components, componentsCount );
		m_pass.updateConfig( unit->getSourceInfo(), m_configuration );
	}

	void TextureTreeItemProperty::onImageChange( wxVariant const & var )
	{
		auto unit = &m_texture;
		castor::Path path{ make_String( var.GetString() ) };

		// Absolute path
		if ( castor::File::fileExists( path ) )
		{
			auto & sourceInfo = unit->getSourceInfo();
			m_pass.resetTexture( sourceInfo
				, castor3d::TextureSourceInfo{ sourceInfo.sampler()
				, path.getPath()
				, path.getFileName( true )
				, sourceInfo.config() } );
		}
	}

	//*********************************************************************************************
}
