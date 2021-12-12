#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/RenderPassTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/Technique/RenderTechniquePass.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <ShaderAST/Shader.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		//class ShaderGatherer
		//	: public castor3d::RenderTechniqueVisitor
		//{
		//private:
		//	explicit ShaderGatherer( castor3d::PipelineFlags flags
		//		, castor3d::Scene const & scene
		//		, ShaderSources & sources )
		//		: castor3d::RenderTechniqueVisitor{ std::move( flags ), scene, { false, true } }
		//		, m_sources{ sources }
		//	{
		//	}

		//public:
		//	static ShaderSources submit( castor3d::RenderTechniquePass & renderPass )
		//	{
		//		ShaderSources result;
		//		ShaderGatherer vis{ result };
		//		renderPass.accept( vis );
		//		return result;
		//	}

		//	void visit( castor3d::ShaderModule const & module )override
		//	{
		//		doGetSource( module.name ).sources[module.stage] = &module;
		//	}

		//private:
		//	template< typename T >
		//	void doVisitUntrackedT( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, T & value )
		//	{
		//		auto & source = doGetSource( name );
		//		auto & uboValues = doGetUbo( source, shaders, ubo );
		//		uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
		//	}

		//	template< typename T >
		//	void doVisitUntrackedT( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::RangedValue< T > & value )
		//	{
		//		auto & source = doGetSource( name );
		//		auto & uboValues = doGetUbo( source, shaders, ubo );
		//		uboValues.uniforms.emplace_back( makeUniformValue( uniform, value ) );
		//	}

		//	template< typename T >
		//	void doVisitTrackedT( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, T & value
		//		, bool * control )
		//	{
		//		auto & source = doGetSource( name );
		//		auto & uboValues = doGetUbo( source, shaders, ubo );
		//		uboValues.uniforms.emplace_back( makeUniformValue( uniform, value, control ) );
		//	}

		//	template< typename T >
		//	void doVisitTrackedT( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::RangedValue< T > & value
		//		, bool * control )
		//	{
		//		auto & source = doGetSource( name );
		//		auto & uboValues = doGetUbo( source, shaders, ubo );
		//		uboValues.uniforms.emplace_back( makeUniformValue( uniform, value, control ) );
		//	}

		//	template< typename ValueT, typename ... ParamsT >
		//	void doVisitT( ValueT & value
		//		, bool * control
		//		, ParamsT ... params )
		//	{
		//		if ( control )
		//		{
		//			doVisitTrackedT< ValueT >( params..., value, control );
		//		}
		//		else
		//		{
		//			doVisitUntrackedT( params..., value );
		//		}
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, float & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, int32_t & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, uint32_t & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point2f & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point2i & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point2ui & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point3f & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point3i & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point3ui & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point4f & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point4i & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Point4ui & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::Matrix4x4f & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::RangedValue< float > & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::RangedValue< int32_t > & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//	void visit( castor::String const & name
		//		, VkShaderStageFlags shaders
		//		, castor::String const & ubo
		//		, castor::String const & uniform
		//		, castor::RangedValue< uint32_t > & value
		//		, bool * control )override
		//	{
		//		doVisitT( value, control, name, shaders, ubo, uniform );
		//	}

		//private:
		//	ShaderSource & doGetSource( castor::String const & name )
		//	{
		//		auto it = std::find_if( m_sources.begin()
		//			, m_sources.end()
		//			, [&name]( ShaderSource const & lookup )
		//			{
		//				return lookup.name == name;
		//			} );

		//		if ( it != m_sources.end() )
		//		{
		//			return *it;
		//		}

		//		ShaderSource source{ name };
		//		m_sources.emplace_back( std::move( source ) );
		//		return m_sources.back();
		//	}

		//	UniformBufferValues & doGetUbo( ShaderSource & source
		//		, VkShaderStageFlags stages
		//		, castor::String const & name )
		//	{
		//		auto it = std::find_if( source.ubos.begin()
		//			, source.ubos.end()
		//			, [&name, &stages]( UniformBufferValues const & lookup )
		//			{
		//				return lookup.name == name
		//					&& lookup.stages == stages;
		//			} );

		//		if ( it != source.ubos.end() )
		//		{
		//			return *it;
		//		}

		//		UniformBufferValues ubo{ name, stages };
		//		source.ubos.emplace_back( std::move( ubo ) );
		//		return source.ubos.back();
		//	}

		//private:
		//	ShaderSources & m_sources;
		//};

		struct RenderPassConfigurationBuilder
			: public castor3d::RenderTechniqueVisitor
		{
		private:
			explicit RenderPassConfigurationBuilder( castor3d::PipelineFlags flags
				, castor3d::Scene const & scene
				, wxPropertyGrid * grid
				, TreeItemProperty & prop )
				: castor3d::RenderTechniqueVisitor{ std::move( flags ), scene, { false, true } }
				, m_grid{ grid }
				, m_prop{ prop }
			{
			}

		public:
			static void submit( wxPropertyGrid * grid
				, TreeItemProperty & prop
				, castor3d::RenderTechniquePass & renderPass )
			{
				auto & engine = *renderPass.getEngine();
				RenderPassConfigurationBuilder vis{ castor3d::PipelineFlags{ castor3d::BlendMode::eNoBlend
						, castor3d::BlendMode::eNoBlend
						, castor3d::PassFlag::eNone
						, engine.getRenderPassTypeID( renderPass.getTypeName() )
						, 0u
						, castor3d::InvalidIndex
						, castor3d::ProgramFlag::eNone
						, castor3d::SceneFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, VK_COMPARE_OP_ALWAYS
						, VK_COMPARE_OP_ALWAYS
						, castor3d::TextureFlagsArray{} }
					, renderPass.getScene()
					, grid
					, prop };
				renderPass.accept( vis );
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

	RenderPassTreeItemProperty::RenderPassTreeItemProperty( bool editable
		, castor3d::RenderTechniquePass & renderPass
		, wxWindow * parent )
		: TreeItemProperty{ renderPass.getEngine(), editable }
		, m_renderPass{ renderPass }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	void RenderPassTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_PASS = _( "Render pass: " );
		static wxString PROPERTY_RENDER_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_PASS_EDIT_SHADER = _( "View Shaders..." );

		addProperty( grid, PROPERTY_CATEGORY_RENDER_PASS + m_renderPass.getName() );
		//addProperty( grid, PROPERTY_RENDER_PASS_SHADER, editor
		//	, [this]( wxVariant const & var )
		//	{
		//		ShaderSources sources = ShaderGatherer::submit( m_renderPass );
		//		ShaderDialog * editor = new ShaderDialog{ m_renderPass.getEngine()
		//			, std::move( sources )
		//			, m_renderPass.getName()
		//			, m_parent };
		//		editor->Show();
		//	} );
		fillRenderPassConfiguration( grid, *this, m_renderPass);
	}

	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, castor3d::RenderTechniquePass & renderPass )
	{
		RenderPassConfigurationBuilder::submit( grid, properties, renderPass );
	}
}
