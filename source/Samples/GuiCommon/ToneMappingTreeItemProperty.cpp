#include "GuiCommon/ToneMappingTreeItemProperty.hpp"

#include "GuiCommon/ShaderDialog.hpp"
#include "GuiCommon/AdditionalProperties.hpp"

#include <Castor3D/HDR/ToneMapping.hpp>

#include <ShaderWriter/Shader.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_TONE_MAPPING = _( "Tone Mapping: " );
		static wxString PROPERTY_TONE_MAPPING_SHADER = _( "Shader" );
		static wxString PROPERTY_TONE_MAPPING_EDIT_SHADER = _( "View Shaders..." );

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

			void visit( castor::String const & name
				, VkShaderStageFlagBits type
				, sdw::Shader const & shader )override
			{
				doGetSource( name ).sources[type] = glsl::compileGlsl( shader
					, ast::SpecialisationInfo{}
					, glsl::GlslConfig
					{
						convert( type ),
						430,
						false,
						false,
						true,
						true,
						true,
						true,
					} );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, HdrConfig & value )override
			{
				auto & source = doGetSource( name );
				UniformBufferValues ubo{ wxT( "HdrConfig" ), VK_SHADER_STAGE_FRAGMENT_BIT };
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Exposure" ), value.getExposure() ) );
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Gamma" ), value.getGamma() ) );
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
		, castor3d::ToneMapping & toneMapping
		, wxWindow * parent )
		: TreeItemProperty{ toneMapping.getEngine(), editable, ePROPERTY_DATA_TYPE_POST_EFFECT }
		, m_toneMapping{ toneMapping }
		, m_parent{ parent }
	{
		PROPERTY_CATEGORY_TONE_MAPPING = _( "Tone Mapping: " );
		PROPERTY_TONE_MAPPING_SHADER = _( "Shader" );
		PROPERTY_TONE_MAPPING_EDIT_SHADER = _( "View Shaders..." );

		CreateTreeItemMenu();
	}

	ToneMappingTreeItemProperty::~ToneMappingTreeItemProperty()
	{
	}

	void ToneMappingTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TONE_MAPPING + m_toneMapping.getName() ) );
		grid->Append( CreateProperty( PROPERTY_TONE_MAPPING_SHADER
			, PROPERTY_TONE_MAPPING_EDIT_SHADER
			, static_cast< ButtonEventMethod >( &ToneMappingTreeItemProperty::onEditShader ), this, editor ) );
	}

	void ToneMappingTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
		}
	}

	bool ToneMappingTreeItemProperty::onEditShader( wxPGProperty * property )
	{
		ShaderSources sources = ToneMappingShaderGatherer::submit( m_toneMapping );
		ShaderDialog * editor = new ShaderDialog{ m_toneMapping.getEngine()
			, std::move( sources )
			, m_toneMapping.getFullName()
			, m_parent };
		editor->Show();
		return false;
	}
}
