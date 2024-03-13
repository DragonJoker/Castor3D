#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ShaderAST/Shader.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		class PostEffectShaderGatherer
			: public castor3d::ConfigurationVisitor
		{
		private:
			explicit PostEffectShaderGatherer( castor3d::RenderDevice const & device
				, ShaderSources & sources )
				: castor3d::ConfigurationVisitor{ { true } }
				, m_device{ device }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( castor3d::PostEffect & postEffect )
			{
				ShaderSources result;
				PostEffectShaderGatherer vis{ postEffect.getRenderSystem()->getRenderDevice(), result };
				postEffect.accept( vis );
				return result;
			}

			void visit( castor3d::ShaderModule const & module
				, bool forceProgramsVisit )override
			{
				if ( !module.shader
					&& module.source.empty()
					&& module.compiled.spirv.empty()
					&& module.compiled.text.empty() )
				{
					return;
				}

				doGetSource( module.name ).sources.push_back( { module.shader.get()
					, module.compiled
					, castor3d::getEntryPointType( m_device, module.stage ) } );
			}

			void visit( castor3d::ProgramModule const & module
				, ast::EntryPoint entryPoint
				, bool forceProgramsVisit )override
			{
				auto it = module.compiled.find( getShaderStage( entryPoint ) );

				if ( !module.shader
					&& ( it == module.compiled.end()
						|| ( it->second.text.empty()
							&& it->second.spirv.empty() ) ) )
				{
					return;
				}

				doGetSource( module.name ).sources.push_back( { module.shader.get()
					, it->second
					, entryPoint } );
			}

		private:
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new PostEffectShaderGatherer{ m_device, m_sources } );
			}

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
				m_sources.emplace_back( castor::move( source ) );
				return m_sources.back();
			}

		private:
			castor3d::RenderDevice const & m_device;
			ShaderSources & m_sources;
		};
	}

	PostEffectTreeItemProperty::PostEffectTreeItemProperty( bool editable
		, castor3d::PostEffect & effect
		, wxWindow * parent )
		: TreeItemProperty{ effect.getRenderSystem()->getEngine(), editable }
		, m_effect{ effect }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	void PostEffectTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_POST_EFFECT = _( "Post Effect: " );
		static wxString PROPERTY_POST_EFFECT_ENABLED = _( "Enabled" );
		static wxString PROPERTY_POST_EFFECT_SHADER = _( "Shader" );
		static wxString PROPERTY_POST_EFFECT_EDIT_SHADER = _( "View Shaders..." );

		addProperty( grid, PROPERTY_CATEGORY_POST_EFFECT + m_effect.getName() );
		addPropertyT( grid, PROPERTY_POST_EFFECT_ENABLED, m_effect.isEnabled(), &m_effect, &castor3d::PostEffect::enable );
		addProperty( grid, PROPERTY_POST_EFFECT_SHADER
			, [this]( wxVariant const & var )
			{
				ShaderSources sources = PostEffectShaderGatherer::submit( m_effect );
				ShaderDialog * editor = new ShaderDialog{ m_effect.getRenderSystem()->getEngine()
					, castor::move( sources )
					, m_effect.getFullName()
					, m_parent };
				editor->Show();
			} );
		TreeItemConfigurationBuilder::submit( grid, *this, m_effect );
	}
}
