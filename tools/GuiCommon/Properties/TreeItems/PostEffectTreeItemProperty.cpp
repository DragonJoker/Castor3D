#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/PostEffectTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <ShaderAST/Shader.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		class PostEffectShaderGatherer
			: public castor3d::PipelineVisitor
		{
		private:
			explicit PostEffectShaderGatherer( ShaderSources & sources )
				: castor3d::PipelineVisitor{ { false, true } }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( PostEffect & postEffect )
			{
				ShaderSources result;
				PostEffectShaderGatherer vis{ result };
				postEffect.accept( vis );
				return result;
			}

			void visit( castor3d::ShaderModule const & module )override
			{
				doGetSource( module.name ).sources[module.stage] = &module;
			}

		private:
			template< typename T >
			void doVisitUntrackedT( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, T & value )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
			}

			template< typename T >
			void doVisitUntrackedT( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< T > & value )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
			}

			template< typename T >
			void doVisitTrackedT( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, T & value
				, bool * control )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value, control ) );
			}

			template< typename T >
			void doVisitTrackedT( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< T > & value
				, bool * control )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value, control ) );
			}

			template< typename ValueT, typename ... ParamsT >
			void doVisitT( ValueT & value
				, bool * control
				, ParamsT ... params )
			{
				if ( control )
				{
					doVisitTrackedT< ValueT >( params..., value, control );
				}
				else
				{
					doVisitUntrackedT( params..., value );
				}
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, float & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, int32_t & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, uint32_t & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2f & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2i & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2ui & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3f & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3i & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3ui & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4f & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4i & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4ui & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Matrix4x4f & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< float > & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
				doVisitT( value, control, name, shaders, ubo, uniform );
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

			UniformBufferValues & doGetUbo( ShaderSource & source
				, VkShaderStageFlags stages
				, castor::String const & name )
			{
				auto it = std::find_if( source.ubos.begin()
					, source.ubos.end()
					, [&name, &stages]( UniformBufferValues const & lookup )
					{
						return lookup.name == name
							&& lookup.stages == stages;
					} );

				if ( it != source.ubos.end() )
				{
					return *it;
				}

				UniformBufferValues ubo{ name, stages };
				source.ubos.emplace_back( std::move( ubo ) );
				return source.ubos.back();
			}

		private:
			ShaderSources & m_sources;
		};

		struct PostEffectConfigurationBuilder
			: public castor3d::PipelineVisitor
		{
		private:
			explicit PostEffectConfigurationBuilder( wxPropertyGrid * grid
				, TreeItemProperty & prop )
				: castor3d::PipelineVisitor{ { false, false } }
				, m_grid{ grid }
				, m_prop{ prop }
			{
			}

		public:
			static void submit( wxPropertyGrid * grid
				, TreeItemProperty & prop
				, PostEffect & postEffect )
			{
				PostEffectConfigurationBuilder vis{ grid, prop };
				postEffect.accept( vis );
			}

			void visit( castor3d::ShaderModule const & module )override
			{
			}

		private:
			void visit( castor::String const & name
				, float & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int32_t & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2f & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2i & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point2ui & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3f & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3i & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point3ui & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4f & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4i & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Point4ui & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::Matrix4x4f & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
				m_prop.addPropertyT( m_grid, name, &value, control );
			}

		private:
			wxPropertyGrid * m_grid;
			TreeItemProperty & m_prop;
		};
	}

	PostEffectTreeItemProperty::PostEffectTreeItemProperty( bool editable
		, PostEffect & effect
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
		addPropertyT( grid, PROPERTY_POST_EFFECT_ENABLED, m_effect.isEnabled(), &m_effect, &PostEffect::enable );
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
		PostEffectConfigurationBuilder::submit( grid, *this, m_effect );
	}
}
