#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/ToneMappingTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ShaderAST/Shader.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		class ToneMappingShaderGatherer
			: public c3d::ToneMappingVisitor
		{
		public:
			ToneMappingShaderGatherer( c3d::RenderDevice const & device
				, ShaderSources & sources )
				: c3d::ToneMappingVisitor{ { true } }
				, m_device{ device }
				, m_sources{ sources }
			{
			}

			static ShaderSources submit( c3d::ToneMapping & toneMapping )
			{
				ShaderSources result;
				ToneMappingShaderGatherer vis{ *toneMapping.getEngine()->getRenderDevice(), result };
				toneMapping.accept( vis );
				return result;
			}

			void visit( c3d::ShaderModule const & shaderModule
				, bool forceProgramsVisit )override
			{
				if ( !shaderModule.shader
					&& shaderModule.source.empty()
					&& shaderModule.compiled.spirv.empty()
					&& shaderModule.compiled.text.empty() )
				{
					return;
				}

				doGetSource( shaderModule.name ).sources.emplace_back( shaderModule.shader.get()
					, shaderModule.compiled
					, c3d::getEntryPointType( m_device, shaderModule.stage ) );
			}

			void visit( c3d::ProgramModule const & shaderModule
				, ast::EntryPoint entryPoint
				, bool forceProgramsVisit )override
			{
				auto it = shaderModule.compiled.find( getShaderStage( entryPoint ) );

				if ( !shaderModule.shader
					&& ( it == shaderModule.compiled.end()
						|| ( it->second.text.empty()
							&& it->second.spirv.empty() ) ) )
				{
					return;
				}

				doGetSource( shaderModule.name ).sources.emplace_back( shaderModule.shader.get()
					, it->second
					, entryPoint );
			}

			void visit( c3d::String const & name
				, VkShaderStageFlags shaders
				, c3d::HdrConfig & value )override
			{
				auto & source = doGetSource( name );
				UniformBufferValues ubo{ make_String( wxT( "HdrConfig" ) ), VK_SHADER_STAGE_FRAGMENT_BIT };
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Exposure" ), value.exposure ) );
				ubo.uniforms.emplace_back( makeUniformValue( wxT( "Gamma" ), value.gamma ) );
				source.ubos.emplace_back( c3d::move( ubo ) );
			}

		private:
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				return c3d::makeRawUnique< ToneMappingShaderGatherer >( m_device, m_sources );
			}

			ShaderSource & doGetSource( c3d::String const & name )
			{
				if ( auto it = std::find_if( m_sources.begin()
					, m_sources.end()
					, [&name]( ShaderSource const & lookup )
					{
						return lookup.name == name;
					} );
					it != m_sources.end() )
				{
					return *it;
				}

				ShaderSource source{ name };
				m_sources.emplace_back( c3d::move( source ) );
				return m_sources.back();
			}

		private:
			c3d::RenderDevice const & m_device;
			ShaderSources & m_sources;
		};
	}

	ToneMappingTreeItemProperty::ToneMappingTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::RenderTarget & target
		, wxWindow * parent )
		: TreeItemProperty{ target.getEngine(), imagesLoader, editable }
		, m_target{ target }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	void ToneMappingTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_TONE_MAPPING = _( "Tone Mapping" );
		static wxString PROPERTY_TONE_MAPPING_TYPE = _( "Type" );
		static wxString PROPERTY_TONE_MAPPING_EXPOSURE = _( "Exposure" );
		static wxString PROPERTY_TONE_MAPPING_GAMMA = _( "Gamma" );
		static wxString PROPERTY_TONE_MAPPING_SHADER = _( "Shader" );

		addProperty( grid, PROPERTY_CATEGORY_TONE_MAPPING );

		m_choices.Clear();
		auto types = m_target.getEngine()->getToneMappingFactory().listRegisteredTypes();

		for ( auto const & [key, id] : types )
		{
			auto name = make_wxString( key );
			m_nameToChoice[key] = uint32_t( m_choices.size() );
			name.Replace( wxT( " Tone Mapping" ), wxEmptyString );
			m_choices.Add( name );
		}

		auto prop = addProperty( grid
			, PROPERTY_TONE_MAPPING_TYPE
			, m_choices
			, [this]( wxVariant const & var )
			{
				auto selected = variantCast< uint32_t >( var );
				m_target.setToneMappingType( make_String( m_choices[selected] ) );

			} );

		if ( auto toneMapping = m_target.getToneMapping() )
		{
			prop->SetValue( m_choices[m_nameToChoice[m_target.getToneMapping()->getName()]] );
		}

		c3d::HdrConfig & hdrConfig = m_target.getHdrConfig();
		addPropertyT( grid, PROPERTY_TONE_MAPPING_EXPOSURE, &hdrConfig.exposure );
		addPropertyT( grid, PROPERTY_TONE_MAPPING_GAMMA, &hdrConfig.gamma );
		addProperty( grid, PROPERTY_TONE_MAPPING_SHADER
			, [this]( wxVariant const & var ){ onEditShader( var ); } );
	}

	void ToneMappingTreeItemProperty::onEditShader( wxVariant const & )
	{
		if ( auto toneMapping = m_target.getToneMapping() )
		{
			ShaderSources sources = ToneMappingShaderGatherer::submit( *toneMapping );
			ShaderDialog editor{ toneMapping->getEngine()
				, m_imagesLoader
				, c3d::move( sources )
				, toneMapping->getFullName()
				, m_parent };
			editor.Show();
		}
	}
}
