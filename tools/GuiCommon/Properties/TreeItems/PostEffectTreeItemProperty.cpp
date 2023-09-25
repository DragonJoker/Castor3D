#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

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
			explicit PostEffectShaderGatherer( ShaderSources & sources )
				: castor3d::ConfigurationVisitor{ { true } }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( castor3d::PostEffect & postEffect )
			{
				ShaderSources result;
				PostEffectShaderGatherer vis{ result };
				postEffect.accept( vis );
				return result;
			}

			void visit( castor3d::ShaderModule const & module
				, bool forceShaderVisit )override
			{
				doGetSource( module.name ).sources[module.stage] = &module;
			}

		private:
			std::unique_ptr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				return std::unique_ptr< ConfigurationVisitorBase >( new PostEffectShaderGatherer{ m_sources } );
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
				m_sources.emplace_back( std::move( source ) );
				return m_sources.back();
			}

		private:
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

	void PostEffectTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_POST_EFFECT = _( "Post Effect: " );
		static wxString PROPERTY_POST_EFFECT_ENABLED = _( "Enabled" );
		static wxString PROPERTY_POST_EFFECT_SHADER = _( "Shader" );
		static wxString PROPERTY_POST_EFFECT_EDIT_SHADER = _( "View Shaders..." );

		addProperty( grid, PROPERTY_CATEGORY_POST_EFFECT + m_effect.getName() );
		addPropertyT( grid, PROPERTY_POST_EFFECT_ENABLED, m_effect.isEnabled(), &m_effect, &castor3d::PostEffect::enable );
		addProperty( grid, PROPERTY_POST_EFFECT_SHADER, editor
			, [this]( wxVariant const & var )
			{
				ShaderSources sources = PostEffectShaderGatherer::submit( m_effect );
				ShaderDialog * editor = new ShaderDialog{ m_effect.getRenderSystem()->getEngine()
					, std::move( sources )
					, m_effect.getFullName()
					, m_parent };
				editor->Show();
			} );
		TreeItemConfigurationBuilder::submit( grid, *this, m_effect );
	}
}
