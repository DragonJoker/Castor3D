#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace passtp
	{
		using onEnabledChange = c3d::Function< void( wxVariant const & var
			, c3d::Pass & pass
			, c3d::String const & compName ) >;

		class PassTreeGatherer
			: public c3d::ConfigurationVisitor
		{
		public:
			static PassTreeItemProperty::PropertiesArray submit( c3d::Pass & pass
				, TreeItemProperty * properties
				, wxPropertyGrid * grid
				, wxPGProperty * mainContainer
				, onEnabledChange onEnabled )
			{
				PassTreeItemProperty::PropertiesArray result;
				PassTreeGatherer vis{ pass, properties, grid, onEnabled };
				auto & compsRegister = pass.getOwner()->getEngine()->getPassComponentsRegister();

				for ( auto & componentDesc : compsRegister )
				{
					if ( componentDesc.plugin
						&& !componentDesc.plugin->isMapComponent() )
					{
						auto passCompProps = c3d::makeRawUnique< PassTreeItemProperty::Properties >();
						auto compProps = passCompProps.get();
						compProps->container = mainContainer;
						compProps->component = pass.getComponent( componentDesc.name );

						if ( !compProps->component )
						{
							compProps->ownComponent = componentDesc.plugin->createComponent( pass );

							if ( compProps->ownComponent )
							{
								compProps->component = compProps->ownComponent.get();
							}
						}

						if ( compProps->component )
						{
							vis.m_compProps = compProps;
							vis.m_result = &compProps->properties;
							vis.m_enabled = pass.hasComponent( componentDesc.name );

							compProps->component->accept( vis );

							for ( auto & compProp : compProps->properties )
							{
								compProp->Enable( vis.m_enabled );
							}

							if ( compProps->container
								&& !vis.m_enabled )
							{
								compProps->container->SetExpanded( false );
							}

							result.emplace_back( c3d::move( passCompProps ) );
						}
					}
				}

				return result;
			}

		private:
			PassTreeGatherer( c3d::Pass & pass
				, TreeItemProperty * properties
				, wxPropertyGrid * grid
				, onEnabledChange onEnabled )
				: c3d::ConfigurationVisitor{}
				, m_pass{ pass }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onEnabled{ onEnabled }
			{
			}

			void visit( c3d::String const & name
				, bool & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int16_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint16_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ));
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int64_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ));
			}

			void visit( c3d::String const & name
				, uint64_t & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, float & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, double & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::Angle & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::LuminousIntensity & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::Illumination & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbColour & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbaColour & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::HdrRgbColour & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::HdrRgbaColour & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< float > & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< int32_t > & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< uint32_t > & value
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int32_t & enumValue
				, c3d::StringArray const & enumNames
				, c3d::ConfigurationVisitor::OnSEnumValueChange onChange
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, c3d::move( controls )
					, onChange ) );
			}

			void visit( c3d::String const & name
				, uint32_t & enumValue
				, c3d::StringArray const & enumNames
				, c3d::ConfigurationVisitor::OnUEnumValueChange onChange
				, c3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, c3d::move( controls )
					, onChange ) );
			}

			void visit( c3d::String const & name
				, bool & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int16_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint16_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int32_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint32_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int64_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, uint64_t & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, float & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, double & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbColour & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RgbaColour & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::HdrRgbColour & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::HdrRgbaColour & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< float > & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< int32_t > & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, c3d::RangedValue< uint32_t > & value
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, c3d::move( controls ) ) );
			}

			void visit( c3d::String const & name
				, int32_t & enumValue
				, c3d::StringArray const & enumNames
				, c3d::ConfigurationVisitor::OnSEnumValueChange onChange
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, c3d::move( controls )
					, onChange ) );
			}

			void visit( c3d::String const & name
				, uint32_t & enumValue
				, c3d::StringArray const & enumNames
				, c3d::ConfigurationVisitor::OnUEnumValueChange onChange
				, c3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, c3d::move( controls )
					, onChange ) );
			}

		private:
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				doVisit( category );
				return c3d::RawUniquePtr< ConfigurationVisitorBase >( new PassTreeGatherer{ m_pass
					, m_properties
					, m_grid
					, m_onEnabled } );
			}

			void doVisit( c3d::String const & name )
			{
				m_properties->setPrefix( make_String( name ) );
				m_compProps->container = m_properties->addProperty( m_grid, name );
				auto pass = &m_pass;
				auto compName = m_compProps->component->getType();
				auto onEnabled = m_onEnabled;
				auto prop = m_properties->addProperty( m_compProps->container
					, _( "Enabled" )
					, m_enabled
					, [onEnabled, pass, compName]( wxVariant const & value )
					{
						onEnabled( value, *pass, compName );
					} );
				prop->SetAttribute( wxPG_BOOL_USE_CHECKBOX, true );
			}

		private:
			c3d::Pass & m_pass;
			TreeItemProperty * m_properties;
			wxPropertyGrid * m_grid;
			onEnabledChange m_onEnabled;
			PropertyArray * m_result{};
			PassTreeItemProperty::Properties * m_compProps{};
			bool m_enabled{};
		};

		class PassShaderGatherer
			: public c3d::RenderTechniqueVisitor
		{
		private:
			PassShaderGatherer( c3d::PipelineFlags flags
				, c3d::Scene const & scene
				, ShaderSources & sources )
				: c3d::RenderTechniqueVisitor{ c3d::move( flags ), scene, { true } }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( c3d::Pass const & pass
				, c3d::Scene const & scene )
			{
				ShaderSources result;
				PassShaderGatherer vis{ c3d::PipelineFlags{ scene.getEngine()->getPassComponentsRegister().getPassComponentCombine( pass )
						, scene.getEngine()->getSubmeshComponentsRegister().getDefaultComponentCombine()
						, pass.getLightingModelId()
						, scene.getBackgroundModelId()
						, pass.getColourBlendMode()
						, pass.getAlphaBlendMode()
						, ( pass.getRenderPassInfo()? pass.getRenderPassInfo()->id: c3d::RenderPassTypeID{} )
						, c3d::ProgramFlag::eNone
						, scene.getFlags()
						, c3d::ShaderFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, 3u
						, pass.getAlphaFunc()
						, pass.getTexturesMask()
						, 0u }
					, scene
					, result };
				auto & engine = *pass.getOwner()->getEngine();
				engine.getRenderTargetCache().forEach( [&vis]( c3d::RenderTarget const & target )
					{
						if ( target.isInitialised() )
						{
							target.getTechnique().accept( vis );
						}
					} );

				return result;
			}

		private:
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

				doGetSource( shaderModule.name ).sources.push_back( { shaderModule.shader.get()
					, shaderModule.compiled
					, c3d::getEntryPointType( *getScene().getEngine()->getRenderDevice(), shaderModule.stage ) } );
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

				doGetSource( shaderModule.name ).sources.push_back( { shaderModule.shader.get()
					, it->second
					, entryPoint } );
			}

		private:
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
			c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
			{
				return c3d::RawUniquePtr< ConfigurationVisitorBase >( new PassShaderGatherer{ getFlags(), getScene(), m_sources } );
			}

		private:
			ShaderSources & m_sources;
		};
	}

	PassTreeItemProperty::PassTreeItemProperty( bool editable
		, c3d::Scene & scene
		, wxWindow * parent )
		: TreeItemProperty{ scene.getEngine(), editable }
		, m_scene{ scene }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	void PassTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static wxString PROPERTY_CATEGORY_BASE = _( "Base" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "View Shaders..." );

		if ( !m_pass )
		{
			return;
		}

		addProperty( grid, PROPERTY_CATEGORY_PASS + wxString( m_pass->getOwner()->getName() ) );
		auto mainContainer = addProperty( grid, PROPERTY_CATEGORY_BASE );
		addProperty( grid, PROPERTY_PASS_SHADER
			, [this]( wxVariant const & var )
			{
				auto sources = passtp::PassShaderGatherer::submit( *m_pass, m_scene );
				auto editor = new ShaderDialog{ m_pass->getOwner()->getEngine()
					, c3d::move( sources )
					, m_pass->getOwner()->getName() + c3d::string::toString( m_pass->getId() )
					, m_parent };
				editor->Show();
			} );
		m_properties = passtp::PassTreeGatherer::submit( *m_pass
			, this
			, grid
			, mainContainer
			, [this]( wxVariant const & value
				, c3d::Pass & pass
				, c3d::String const & compName )
			{
				auto it = std::find_if( m_properties.begin()
					, m_properties.end()
					, [&compName]( PropertiesPtr const & lookup )
					{
						return lookup->component->getType() == compName;
					} );

				if ( it == m_properties.end() )
				{
					return;
				}

				auto & compProps = **it;

				if ( !compProps.component )
				{
					return;
				}

				auto enable = value.GetBool();

				for ( auto & prop : compProps.properties )
				{
					prop->Enable( enable );
				}

				if ( enable )
				{
					if ( compProps.ownComponent )
					{
						moveComponentsToPass( c3d::move( compProps.ownComponent ) );
					}
				}
				else if ( pass.hasComponent( compProps.component->getType() ) )
				{
					auto removed = pass.removeComponent( compProps.component->getType() );
					compProps.ownComponent = c3d::move( removed.back() );
					removed.pop_back();
					moveComponentsToProps( c3d::move( removed ) );
					CU_Require( compProps.ownComponent );
				}
			} );
	}

	void PassTreeItemProperty::moveComponentsToPass( c3d::PassComponentUPtr component )
	{
		auto & pass = *component->getOwner();

		for ( auto dep : component->getDependencies() )
		{
			auto it = std::find_if( m_properties.begin()
				, m_properties.end()
				, [&dep]( PropertiesPtr const & lookup )
				{
					return lookup->ownComponent
						&& lookup->ownComponent->getType() == dep;
				} );

			if ( it != m_properties.end() )
			{
				moveComponentsToPass( c3d::move( ( *it )->ownComponent ) );
			}
		}

		pass.addComponent( c3d::move( component ) );
	}

	void PassTreeItemProperty::moveComponentsToProps( c3d::Vector< c3d::PassComponentUPtr > removed )
	{
		for ( auto & rem : removed )
		{
			auto it = std::find_if( m_properties.begin()
				, m_properties.end()
				, [&rem]( PropertiesPtr const & lookup )
				{
					return lookup->component->getType() == rem->getType();
				} );

			if ( it != m_properties.end() )
			{
				( *it )->ownComponent = c3d::move( rem );
			}
		}
	}
}
