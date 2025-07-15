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
			: public c3d::ConfigurationVisitor
		{
		private:
			explicit PostEffectShaderGatherer( c3d::RenderDevice const & device
				, ShaderSources & sources )
				: c3d::ConfigurationVisitor{ { true } }
				, m_device{ device }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( c3d::PostEffect & postEffect )
			{
				ShaderSources result;
				PostEffectShaderGatherer vis{ postEffect.getRenderSystem()->getRenderDevice(), result };
				postEffect.accept( vis );
				return result;
			}

			void visit( c3d::ShaderModule const & module
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
					, c3d::getEntryPointType( m_device, module.stage ) } );
			}

			void visit( c3d::ProgramModule const & module
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
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				return c3d::RawUniquePtr< ConfigurationVisitorBase >( new PostEffectShaderGatherer{ m_device, m_sources } );
			}

			ShaderSource & doGetSource( c3d::String const & name )
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
				m_sources.emplace_back( c3d::move( source ) );
				return m_sources.back();
			}

		private:
			c3d::RenderDevice const & m_device;
			ShaderSources & m_sources;
		};
	}

	PostEffectTreeItemProperty::PostEffectTreeItemProperty( bool editable
		, c3d::PostEffect & effect
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
		addPropertyT( grid, PROPERTY_POST_EFFECT_ENABLED, m_effect.isEnabled(), &m_effect, &c3d::PostEffect::enable );
		addProperty( grid, PROPERTY_POST_EFFECT_SHADER
			, [this]( wxVariant const & var )
			{
				ShaderSources sources = PostEffectShaderGatherer::submit( m_effect );
				ShaderDialog * editor = new ShaderDialog{ m_effect.getRenderSystem()->getEngine()
					, c3d::move( sources )
					, m_effect.getFullName()
					, m_parent };
				editor->Show();
			} );
		TreeItemConfigurationBuilder::submit( grid, *this, m_effect );
	}
}
