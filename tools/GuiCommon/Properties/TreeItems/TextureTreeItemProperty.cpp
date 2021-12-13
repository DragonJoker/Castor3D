#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

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
		uint32_t getComponent( castor::Point2ui const & flag )
		{
			for ( uint32_t i = 0u; i < 4u; ++i )
			{
				if ( ( flag[0] >> ( i * 8u ) ) & 0x01 )
				{
					return i;
				}
			}

			return 0u;
		}

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
				, castor3d::TextureFlag flag
				, uint32_t componentsCount ) >;

		class UnitTreeGatherer
			: public castor3d::PassVisitor
		{
		public:
			static std::map< castor3d::TextureFlag, TextureTreeItemProperty::PropertyPair > submit( castor3d::Pass & pass
				, castor3d::TextureConfiguration & config
				, TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onMaskChange onChange )
			{
				std::map< castor3d::TextureFlag, TextureTreeItemProperty::PropertyPair > result;
				UnitTreeGatherer vis{ properties, grid, onChange, result };
				pass.fillConfig( config, vis );
				return result;
			}

		private:
			UnitTreeGatherer( TextureTreeItemProperty * properties
				, wxPropertyGrid * grid
				, onMaskChange onChange
				, std::map< castor3d::TextureFlag, TextureTreeItemProperty::PropertyPair > & result )
				: castor3d::PassVisitor{ {} }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onChange{ onChange }
				, m_result{ result }
			{
			}

			void visit( castor::String const & name
				, bool & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int16_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int32_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int64_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint64_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, float & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, double & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor3d::BlendMode & value
				, bool * control )override
			{
				wxArrayString choices;
				choices.push_back( _( "No Blend" ) );
				choices.push_back( _( "Additive" ) );
				choices.push_back( _( "Multiplicative" ) );
				choices.push_back( _( "Interpolative" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, castor3d::ParallaxOcclusionMode & value
				, bool * control )override
			{
				wxArrayString choices;
				choices.push_back( _( "None" ) );
				choices.push_back( _( "One" ) );
				choices.push_back( _( "Repeat" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, VkCompareOp & value
				, bool * control )override
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
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor3d::TextureFlag textureFlag
				, castor::Point2ui & mask
				, uint32_t componentsCount
				, bool * control )override
			{
				doAddProperty( name
					, name + _( " Map" )
					, name + _( " Component" )
					, textureFlag
					, mask
					, componentsCount );
			}

		private:
			void doAddProperty( wxString const & flagName
				, wxString const & isName
				, wxString const & compName
				, castor3d::TextureFlag flag
				, castor::Point2ui & mask
				, uint32_t componentsCount )
			{
				auto onChange = m_onChange;
				static wxString PROPERTY_COMPONENT_R = wxT( "R" );
				static wxString PROPERTY_COMPONENT_G = wxT( "G" );
				static wxString PROPERTY_COMPONENT_B = wxT( "B" );
				static wxString PROPERTY_COMPONENT_A = wxT( "A" );
				static wxString PROPERTY_COMPONENT_RGB = wxT( "RGB" );
				static wxString PROPERTY_COMPONENT_GBA = wxT( "GBA" );

				m_grid->Append( new wxPropertyCategory( flagName ) );
				m_properties->addProperty( m_grid, flagName );
				auto isProp = m_properties->addProperty( m_grid
					, isName
					, mask[0] != 0
					, [onChange, flag, componentsCount]( wxVariant const & var )
					{
						onChange( var, flag, componentsCount );
					} );
				wxString name;
				wxString selected;
				wxArrayString choices;

				if ( componentsCount == 1u )
				{
					choices.Add( PROPERTY_COMPONENT_R );
					choices.Add( PROPERTY_COMPONENT_G );
					choices.Add( PROPERTY_COMPONENT_B );
					choices.Add( PROPERTY_COMPONENT_A );
					selected = choices[getComponent( mask )];
				}
				else
				{
					choices.Add( PROPERTY_COMPONENT_RGB );
					choices.Add( PROPERTY_COMPONENT_GBA );
					selected = choices[getComponent( mask )];
				}

				auto compProp = m_properties->addProperty( m_grid
					, compName
					, choices
					, selected
					, [onChange, flag, componentsCount]( wxVariant const & var )
					{
						onChange( var, flag, componentsCount );
					} );
				compProp->Enable( mask[0] != 0 );
				m_result.emplace( flag, TextureTreeItemProperty::PropertyPair{ isProp, compProp, mask } );
			}

		private:
			TextureTreeItemProperty * m_properties;
			wxPropertyGrid * m_grid;
			onMaskChange m_onChange;
			std::map< castor3d::TextureFlag, TextureTreeItemProperty::PropertyPair > & m_result;
		};
	}

	//*********************************************************************************************

	TextureTreeItemProperty::TextureTreeItemProperty( bool editable
		, castor3d::Pass & pass
		, castor3d::TextureUnitSPtr texture )
		: TreeItemProperty( texture->getEngine(), editable )
		, m_pass{ pass }
		, m_texture{ texture }
		, m_configuration{ texture->getConfiguration() }
	{
		CreateTreeItemMenu();
	}

	void TextureTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString CATEGORY_TEXTURE = _( "Texture" );
		static wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString PROPERTY_FLAG_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_FLAG_SPECULAR = _( "Specular" );
		static wxString PROPERTY_FLAG_METALLIC = _( "Metallic" );
		static wxString PROPERTY_FLAG_ALBEDO = _( "Albedo" );
		static wxString PROPERTY_FLAG_SHININESS = _( "Shininess" );
		static wxString PROPERTY_FLAG_GLOSSINESS = _( "Glossiness" );
		static wxString PROPERTY_FLAG_ROUGHNESS = _( "Roughness" );
		static wxString PROPERTY_FLAG_NORMAL = _( "Normal" );
		static wxString PROPERTY_FLAG_OPACITY = _( "Opacity" );
		static wxString PROPERTY_FLAG_HEIGHT = _( "Height" );
		static wxString PROPERTY_FLAG_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_FLAG_OCCLUSION = _( "Occlusion" );
		static wxString PROPERTY_FLAG_TRANSMITTANCE = _( "Transmittance" );
		static wxString PROPERTY_IS_DIFFUSE = _( "Diffuse Map" );
		static wxString PROPERTY_IS_ALBEDO = _( "Albedo Map" );
		static wxString PROPERTY_IS_SPECULAR = _( "Specular Map" );
		static wxString PROPERTY_IS_METALLIC = _( "Metallic Map" );
		static wxString PROPERTY_IS_SHININESS = _( "Shininess Map" );
		static wxString PROPERTY_IS_GLOSSINESS = _( "Glossiness Map" );
		static wxString PROPERTY_IS_ROUGHNESS = _( "Roughness Map" );
		static wxString PROPERTY_IS_NORMAL = _( "Normal Map" );
		static wxString PROPERTY_IS_OPACITY = _( "Opacity Map" );
		static wxString PROPERTY_IS_HEIGHT = _( "Height Map" );
		static wxString PROPERTY_IS_EMISSIVE = _( "Emissive Map" );
		static wxString PROPERTY_IS_OCCLUSION = _( "Occlusion Map" );
		static wxString PROPERTY_IS_TRANSMITTANCE = _( "Transmittance Map" );
		static wxString PROPERTY_COMP_DIFFUSE = _( "Diffuse Components" );
		static wxString PROPERTY_COMP_ALBEDO = _( "Albedo Components" );
		static wxString PROPERTY_COMP_SPECULAR = _( "Specular Components" );
		static wxString PROPERTY_COMP_METALLIC = _( "Metallic Component" );
		static wxString PROPERTY_COMP_SHININESS = _( "Shininess Component" );
		static wxString PROPERTY_COMP_GLOSSINESS = _( "Glossiness Component" );
		static wxString PROPERTY_COMP_ROUGHNESS = _( "Roughness Component" );
		static wxString PROPERTY_COMP_NORMAL = _( "Normal Components" );
		static wxString PROPERTY_COMP_OPACITY = _( "Opacity Component" );
		static wxString PROPERTY_COMP_HEIGHT = _( "Height Component" );
		static wxString PROPERTY_COMP_EMISSIVE = _( "Emissive Components" );
		static wxString PROPERTY_COMP_OCCLUSION = _( "Occlusion Component" );
		static wxString PROPERTY_COMP_TRANSMITTANCE = _( "Transmittance Component" );
		static wxString PROPERTY_COMPONENT = _( "Component" );
		static wxString PROPERTY_COMPONENTS = _( "Components" );
		static wxString PROPERTY_FACTOR_NORMAL = _( "Normal Factor" );
		static wxString PROPERTY_FACTOR_HEIGHT = _( "Height Factor" );
		static wxString PROPERTY_DIRECTX_NORMAL = _( "Normal DirectX" );

		auto unit = getTexture();

		if ( unit )
		{
			grid->Append( new wxPropertyCategory( CATEGORY_TEXTURE ) );

			if ( unit->getTexture()->isStatic() )
			{
				m_path = castor::Path{ unit->getTexture()->getPath() };
				addProperty( grid, PROPERTY_TEXTURE_IMAGE, m_path
					, [this]( wxVariant const & var )
					{
						onImageChange( var );
					} );
			}

			m_properties = UnitTreeGatherer::submit( m_pass
				, m_configuration
				, this
				, grid
				, [this]( wxVariant const & var
					, castor3d::TextureFlag flag
					, uint32_t componentsCount )
				{
					onChange( var, flag, componentsCount );
				} );

			if ( unit->hasAnimation() )
			{
				static wxString CATEGORY_ANIMATION = _( "Animation" );
				static wxString PROPERTY_ANIMATION_TRANSLATE = _( "Translate" );
				static wxString PROPERTY_ANIMATION_ROTATE = _( "Rotate" );
				static wxString PROPERTY_ANIMATION_SCALE = _( "Scale" );

				auto & anim = unit->getAnimation();
				grid->Append( new wxPropertyCategory( CATEGORY_ANIMATION ) );
				addPropertyT( grid, PROPERTY_ANIMATION_TRANSLATE, anim.getTranslateSpeed(), &anim, &castor3d::TextureAnimation::setTranslateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_ROTATE, anim.getRotateSpeed(), &anim, &castor3d::TextureAnimation::setRotateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_SCALE, anim.getScaleSpeed(), &anim, &castor3d::TextureAnimation::setScaleSpeed );
			}
		}
	}

	void TextureTreeItemProperty::onChange( wxVariant const & var
		, castor3d::TextureFlag flag
		, uint32_t componentsCount )
	{
		auto unit = getTexture();
		auto it = m_properties.find( flag );
		CU_Require( it != m_properties.end() );
		bool isMap = it->second.isMap->GetValue();
		long components = it->second.components->GetValue();
		it->second.components->Enable( isMap );
		it->second.mask[0] = getMask( isMap, components, componentsCount );
		m_pass.updateConfig( unit->getSourceInfo(), m_configuration );
	}

	void TextureTreeItemProperty::onImageChange( wxVariant const & var )
	{
		auto unit = getTexture();
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
