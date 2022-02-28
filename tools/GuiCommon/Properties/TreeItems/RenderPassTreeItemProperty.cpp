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
				RenderPassConfigurationBuilder vis{ renderPass.createPipelineFlags( castor3d::BlendMode::eNoBlend
						, castor3d::BlendMode::eNoBlend
						, castor3d::PassFlag::eNone
						, engine.getRenderPassTypeID( renderPass.getTypeName() )
						, 0u
						, castor3d::InvalidIndex
						, VK_COMPARE_OP_ALWAYS
						, VK_COMPARE_OP_ALWAYS
						, castor3d::TextureFlagsArray{}
						, castor3d::ProgramFlag::eNone
						, castor3d::SceneFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, false )
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
				, int32_t & enumValue
				, castor::StringArray const & enumNames
				, bool * control )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, control );
			}

			void visit( castor::String const & name
				, uint32_t & enumValue
				, castor::StringArray const & enumNames
				, bool * control )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, control );
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

	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, castor3d::RenderTechniquePass & renderPass )
	{
		RenderPassConfigurationBuilder::submit( grid, properties, renderPass );
	}
}
