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
				: castor3d::PipelineVisitor{ false }
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

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, float & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, int32_t & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, uint32_t & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2f & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2i & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point2ui & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3f & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3i & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point3ui & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4f & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4i & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Point4ui & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::Matrix4x4f & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< float > & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< int32_t > & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::RangedValue< uint32_t > & value )override
			{
				doVisit( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< float > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< int32_t > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< uint32_t > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point2f > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point2i > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point2ui > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point3f > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point3i > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point3ui > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point4f > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point4i > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Point4ui > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::Matrix4x4f > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::RangedValue< float > > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::RangedValue< int32_t > > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
			}

			void visit( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value )override
			{
				doVisitTracked( name, shaders, ubo, uniform, value );
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

			template< typename T >
			void doVisit( castor::String const & name
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
			void doVisit( castor::String const & name
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
			void doVisitTracked( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< T > & value )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
			}

			template< typename T >
			void doVisitTracked( castor::String const & name
				, VkShaderStageFlags shaders
				, castor::String const & ubo
				, castor::String const & uniform
				, castor::ChangeTracked< castor::RangedValue< T > > & value )
			{
				auto & source = doGetSource( name );
				auto & uboValues = doGetUbo( source, shaders, ubo );
				uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
			}

		private:
			ShaderSources & m_sources;
		};
	}

	PostEffectTreeItemProperty::PostEffectTreeItemProperty( bool editable
		, PostEffect & effect
		, wxWindow * parent )
		: TreeItemProperty{ effect.getRenderSystem()->getEngine(), editable, ePROPERTY_DATA_TYPE_POST_EFFECT }
		, m_effect{ effect }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	PostEffectTreeItemProperty::~PostEffectTreeItemProperty()
	{
	}

	void PostEffectTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_POST_EFFECT = _( "Post Effect: " );
		static wxString PROPERTY_POST_EFFECT_SHADER = _( "Shader" );
		static wxString PROPERTY_POST_EFFECT_EDIT_SHADER = _( "View Shaders..." );

		addProperty( grid, PROPERTY_CATEGORY_POST_EFFECT + m_effect.getName() );
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
	}
}
