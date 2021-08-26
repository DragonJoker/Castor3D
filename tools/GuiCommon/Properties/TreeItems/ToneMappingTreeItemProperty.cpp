#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

#include <ShaderAST/Shader.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		class ToneMappingShaderGatherer
			: public castor3d::ToneMappingVisitor
		{
		private:
			explicit ToneMappingShaderGatherer( ShaderSources & sources )
				: castor3d::ToneMappingVisitor{}
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( ToneMapping & toneMapping )
			{
				ShaderSources result;
				ToneMappingShaderGatherer vis{ result };
				toneMapping.accept( vis );
				return result;
			}

			void visit( castor3d::ShaderModule const & module )override
			{
				doGetSource( module.name ).sources[module.stage] = &module;
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, HdrConfig & value )override
			{
				auto & source = doGetSource( name );
				UniformBufferValues ubo{ wxT( "HdrConfig" ), VK_SHADER_STAGE_FRAGMENT_BIT };
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Exposure" ), value.exposure ) );
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Gamma" ), value.gamma ) );
				source.ubos.emplace_back( std::move( ubo ) );
			}

		private:
			ShaderSource & doGetSource( castor::String const & name )
			{
				auto it = std::find_if( m_sources.begin()
					, m_sources.end()
					, [&name]( ShaderSource const & lookup )
					{
						return lookup.name == name;
					} );

				if ( it != m_sources.end() )
				{
					return *it;
				}

				ShaderSource source{ name };
				m_sources.emplace_back( std::move( source ) );
				return m_sources.back();
			}

		private:
			ShaderSources & m_sources;
		};
	}

	ToneMappingTreeItemProperty::ToneMappingTreeItemProperty( bool editable
		, castor3d::RenderTarget & target
		, wxWindow * parent )
		: TreeItemProperty{ target.getEngine(), editable }
		, m_target{ target }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	ToneMappingTreeItemProperty::~ToneMappingTreeItemProperty()
	{
	}

	void ToneMappingTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_TONE_MAPPING = _( "Tone Mapping" );
		static wxString PROPERTY_TONE_MAPPING_TYPE = _( "Type" );
		static wxString PROPERTY_TONE_MAPPING_EXPOSURE = _( "Exposure" );
		static wxString PROPERTY_TONE_MAPPING_GAMMA = _( "Gamma" );
		static wxString PROPERTY_TONE_MAPPING_SHADER = _( "Shader" );

		addProperty( grid, PROPERTY_CATEGORY_TONE_MAPPING );

		m_choices.Clear();
		auto types = m_target.getEngine()->getToneMappingFactory().listRegisteredTypes();

		for ( auto & toneMapping : types )
		{
			auto name = make_wxString( toneMapping );
			m_nameToChoice[toneMapping] = m_choices.size();
			name.Replace( wxT( " Tone Mapping" ), wxEmptyString );
			m_choices.Add( name );
		}

		auto prop = addProperty( grid
			, PROPERTY_TONE_MAPPING_TYPE
			, m_choices
			, [this]( wxVariant const & var )
			{
				auto selected = uint32_t( variantCast< uint32_t >( var ) );
				m_target.setToneMappingType( make_String( m_choices[selected] ), {} );

			} );
		auto toneMapping = m_target.getToneMapping();

		if ( toneMapping )
		{
			prop->SetValue( m_choices[m_nameToChoice[m_target.getToneMapping()->getName()]] );
		}

		HdrConfig & hdrConfig = m_target.getHdrConfig();
		addPropertyT( grid, PROPERTY_TONE_MAPPING_EXPOSURE, &hdrConfig.exposure );
		addPropertyT( grid, PROPERTY_TONE_MAPPING_GAMMA, &hdrConfig.gamma );
		addProperty( grid, PROPERTY_TONE_MAPPING_SHADER, editor
			, [this]( wxVariant const & var ){ onEditShader( var ); } );
	}

	void ToneMappingTreeItemProperty::onEditShader( wxVariant const & var )
	{
		auto toneMapping = m_target.getToneMapping();

		if ( toneMapping )
		{
			ShaderSources sources = ToneMappingShaderGatherer::submit( *toneMapping );
			ShaderDialog * editor = new ShaderDialog{ toneMapping->getEngine()
				, std::move( sources )
				, toneMapping->getFullName()
				, m_parent };
			editor->Show();
		}
	}
}
