#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/RenderPassTreeItemProperty.hpp"

#include "GuiCommon/Shader/ShaderDialog.hpp"
#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		struct RenderPassConfigurationBuilder
			: public c3d::RenderTechniqueVisitor
		{
		private:
			explicit RenderPassConfigurationBuilder( c3d::PipelineFlags flags
				, c3d::Scene const & scene
				, wxPropertyGrid * grid
				, TreeItemProperty & prop )
				: c3d::RenderTechniqueVisitor{ c3d::move( flags ), scene, { false } }
				, m_grid{ grid }
				, m_prop{ prop }
			{
			}

		public:
			static void submit( wxPropertyGrid * grid
				, TreeItemProperty & prop
				, c3d::RenderTechniqueNodesPass & renderPass )
			{
				auto & scene = renderPass.getScene();
				auto & engine = *renderPass.getEngine();
				RenderPassConfigurationBuilder vis{ renderPass.createPipelineFlags( c3d::PassComponentCombine{}
						, c3d::SubmeshComponentCombine{}
						, c3d::BlendMode::eNoBlend
						, c3d::BlendMode::eNoBlend
						, engine.getRenderPassTypeID( renderPass.getTypeName() )
						, scene.getDefaultLightingModel()
						, scene.getBackgroundModelId()
						, c3d::ComparisonFunc::eAlways
						, c3d::ComparisonFunc::eAlways
						, c3d::TextureCombine{}
						, c3d::ProgramFlag::eNone
						, c3d::SceneFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, false
						, 0u
						, {}
						, nullptr
						, 0u )
					, renderPass.getScene()
					, grid
					, prop };
				renderPass.accept( vis );
			}

		private:
			void visit( c3d::String const & name
				, float & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, int32_t & enumValue
				, c3d::StringArray const & enumNames
				, OnSEnumValueChange onChange
				, ControlsList controls )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, c3d::move( controls ), onChange );
			}

			void visit( c3d::String const & name
				, uint32_t & enumValue
				, c3d::StringArray const & enumNames
				, OnUEnumValueChange onChange
				, ControlsList controls )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, c3d::move( controls ), onChange );
			}

			void visit( c3d::String const & name
				, c3d::Point2f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point2i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point2ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point3ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Point4ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::Matrix4x4f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< float > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< int32_t > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< uint32_t > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}

		private:
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				return c3d::RawUniquePtr< ConfigurationVisitorBase >( new RenderPassConfigurationBuilder{ getFlags(), getScene(), m_grid, m_prop } );
			}

		private:
			wxPropertyGrid * m_grid;
			TreeItemProperty & m_prop;
		};
	}

	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, c3d::RenderTechniqueNodesPass & renderPass )
	{
		RenderPassConfigurationBuilder::submit( grid, properties, renderPass );
	}
}
