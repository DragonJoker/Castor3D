#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

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
		CreateTreeItemMenu();
	}

	ToneMappingTreeItemProperty::~ToneMappingTreeItemProperty()
	{
	}

	void ToneMappingTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_TONE_MAPPING = _( "Tone Mapping: " );
		static wxString PROPERTY_TONE_MAPPING_SHADER = _( "Shader" );

		addProperty( grid, PROPERTY_CATEGORY_TONE_MAPPING + m_toneMapping.getName() );
		addProperty( grid, PROPERTY_TONE_MAPPING_SHADER, editor
			, [this]( wxVariant const & var ){ onEditShader( var ); } );
	}

	void ToneMappingTreeItemProperty::onEditShader( wxVariant const & var )
	{
		ShaderSources sources = ToneMappingShaderGatherer::submit( m_toneMapping );
		ShaderDialog * editor = new ShaderDialog{ m_toneMapping.getEngine()
			, std::move( sources )
			, m_toneMapping.getFullName()
			, m_parent };
		editor->Show();
	}
}
