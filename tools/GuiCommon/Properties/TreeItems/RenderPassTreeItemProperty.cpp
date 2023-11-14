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
			: public castor3d::RenderTechniqueVisitor
		{
		private:
			explicit RenderPassConfigurationBuilder( castor3d::PipelineFlags flags
				, castor3d::Scene const & scene
				, wxPropertyGrid * grid
				, TreeItemProperty & prop )
				: castor3d::RenderTechniqueVisitor{ std::move( flags ), scene, { false } }
				, m_grid{ grid }
				, m_prop{ prop }
			{
			}

		public:
			static void submit( wxPropertyGrid * grid
				, TreeItemProperty & prop
				, castor3d::RenderTechniqueNodesPass & renderPass )
			{
				auto & scene = renderPass.getScene();
				auto & engine = *renderPass.getEngine();
				RenderPassConfigurationBuilder vis{ renderPass.createPipelineFlags( castor3d::PassComponentCombine{}
						, castor3d::SubmeshComponentCombine{}
						, castor3d::BlendMode::eNoBlend
						, castor3d::BlendMode::eNoBlend
						, engine.getRenderPassTypeID( renderPass.getTypeName() )
						, scene.getDefaultLightingModel()
						, scene.getBackgroundModelId()
						, VK_COMPARE_OP_ALWAYS
						, VK_COMPARE_OP_ALWAYS
						, castor3d::TextureCombine{}
						, castor3d::ProgramFlag::eNone
						, castor3d::SceneFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, false
						, 0u
						, {} )
					, renderPass.getScene()
					, grid
					, prop };
				renderPass.accept( vis );
			}

		private:
			void visit( castor::String const & name
				, float & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, int32_t & enumValue
				, castor::StringArray const & enumNames
				, OnSEnumValueChange onChange
				, ControlsList controls )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
			}

			void visit( castor::String const & name
				, uint32_t & enumValue
				, castor::StringArray const & enumNames
				, OnUEnumValueChange onChange
				, ControlsList controls )override
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
			}

			void visit( castor::String const & name
				, castor::Point2f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point2i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point2ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point3ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4i & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Point4ui & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::Matrix4x4f & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, ControlsList controls )override
			{
				m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
			}

		private:
			std::unique_ptr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				return std::unique_ptr< ConfigurationVisitorBase >( new RenderPassConfigurationBuilder{ getFlags(), getScene(), m_grid, m_prop } );
			}

		private:
			wxPropertyGrid * m_grid;
			TreeItemProperty & m_prop;
		};
	}

	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, castor3d::RenderTechniqueNodesPass & renderPass )
	{
		RenderPassConfigurationBuilder::submit( grid, properties, renderPass );
	}
}
