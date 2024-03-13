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
		using onEnabledChange = castor::Function< void( wxVariant const & var
			, castor3d::Pass & pass
			, castor::String const & compName ) >;

		class PassTreeGatherer
			: public castor3d::ConfigurationVisitor
		{
		public:
			static PassTreeItemProperty::PropertiesArray submit( castor3d::Pass & pass
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
						auto passCompProps = castor::make_unique< PassTreeItemProperty::Properties >();
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

							result.emplace_back( castor::move( passCompProps ) );
						}
					}
				}

				return result;
			}

		private:
			PassTreeGatherer( castor3d::Pass & pass
				, TreeItemProperty * properties
				, wxPropertyGrid * grid
				, onEnabledChange onEnabled )
				: castor3d::ConfigurationVisitor{}
				, m_pass{ pass }
				, m_properties{ properties }
				, m_grid{ grid }
				, m_onEnabled{ onEnabled }
			{
			}

			void visit( castor::String const & name
				, bool & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int16_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ));
			}

			void visit( castor::String const & name
				, uint32_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int64_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ));
			}

			void visit( castor::String const & name
				, uint64_t & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, float & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, double & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::Angle & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::HdrRgbColour & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::HdrRgbaColour & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int32_t & enumValue
				, castor::StringArray const & enumNames
				, castor3d::ConfigurationVisitor::OnSEnumValueChange onChange
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, castor::move( controls )
					, onChange ) );
			}

			void visit( castor::String const & name
				, uint32_t & enumValue
				, castor::StringArray const & enumNames
				, castor3d::ConfigurationVisitor::OnUEnumValueChange onChange
				, castor3d::ConfigurationVisitor::ControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, castor::move( controls )
					, onChange ) );
			}

			void visit( castor::String const & name
				, bool & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int16_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int32_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int64_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, uint64_t & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, float & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, double & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::HdrRgbColour & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::HdrRgbaColour & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyT( m_compProps->container, name, &value, castor::move( controls ) ) );
			}

			void visit( castor::String const & name
				, int32_t & enumValue
				, castor::StringArray const & enumNames
				, castor3d::ConfigurationVisitor::OnSEnumValueChange onChange
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, castor::move( controls )
					, onChange ) );
			}

			void visit( castor::String const & name
				, uint32_t & enumValue
				, castor::StringArray const & enumNames
				, castor3d::ConfigurationVisitor::OnUEnumValueChange onChange
				, castor3d::ConfigurationVisitor::AtomicControlsList controls )override
			{
				m_result->push_back( m_properties->addPropertyET( m_compProps->container
					, name
					, make_wxArrayString( enumNames )
					, &enumValue
					, castor::move( controls )
					, onChange ) );
			}

		private:
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				doVisit( category );
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new PassTreeGatherer{ m_pass
					, m_properties
					, m_grid
					, m_onEnabled } );
			}

			void doVisit( castor::String const & name )
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
			castor3d::Pass & m_pass;
			TreeItemProperty * m_properties;
			wxPropertyGrid * m_grid;
			onEnabledChange m_onEnabled;
			PropertyArray * m_result{};
			PassTreeItemProperty::Properties * m_compProps{};
			bool m_enabled{};
		};

		class PassShaderGatherer
			: public castor3d::RenderTechniqueVisitor
		{
		private:
			PassShaderGatherer( castor3d::PipelineFlags flags
				, castor3d::Scene const & scene
				, ShaderSources & sources )
				: castor3d::RenderTechniqueVisitor{ castor::move( flags ), scene, { true } }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( castor3d::Pass const & pass
				, castor3d::Scene const & scene )
			{
				ShaderSources result;
				PassShaderGatherer vis{ castor3d::PipelineFlags{ scene.getEngine()->getPassComponentsRegister().getPassComponentCombine( pass )
						, scene.getEngine()->getSubmeshComponentsRegister().getDefaultComponentCombine()
						, pass.getLightingModelId()
						, scene.getBackgroundModelId()
						, pass.getColourBlendMode()
						, pass.getAlphaBlendMode()
						, ( pass.getRenderPassInfo()? pass.getRenderPassInfo()->id: castor3d::RenderPassTypeID{} )
						, castor3d::ProgramFlag::eNone
						, scene.getFlags()
						, castor3d::ShaderFlag::eNone
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
						, 3u
						, pass.getAlphaFunc()
						, pass.getTexturesMask()
						, 0u }
					, scene
					, result };
				auto & engine = *pass.getOwner()->getEngine();
				engine.getRenderTargetCache().forEach( [&vis]( castor3d::RenderTarget const & target )
					{
						if ( target.isInitialised() )
						{
							target.getTechnique().accept( vis );
						}
					} );

				return result;
			}

		private:
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
					, castor3d::getEntryPointType( *getScene().getEngine()->getRenderDevice(), module.stage ) } );
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
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new PassShaderGatherer{ getFlags(), getScene(), m_sources } );
			}

		private:
			ShaderSources & m_sources;
		};
	}

	PassTreeItemProperty::PassTreeItemProperty( bool editable
		, castor3d::Pass & pass
		, castor3d::Scene & scene
		, wxWindow * parent )
		: TreeItemProperty{ pass.getOwner()->getEngine(), editable }
		, m_pass{ &pass }
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

		if ( auto pass = getPass() )
		{
			addProperty( grid, PROPERTY_CATEGORY_PASS + wxString( pass->getOwner()->getName() ) );
			auto mainContainer = addProperty( grid, PROPERTY_CATEGORY_BASE );
			addProperty( grid, PROPERTY_PASS_SHADER
				, [this]( wxVariant const & var )
				{
					auto lpass = getPass();
					auto sources = passtp::PassShaderGatherer::submit( *lpass, m_scene );
					auto editor = new ShaderDialog{ lpass->getOwner()->getEngine()
						, castor::move( sources )
						, lpass->getOwner()->getName() + castor::string::toString( lpass->getId() )
						, m_parent };
					editor->Show();
				} );
			m_properties = passtp::PassTreeGatherer::submit( *pass
				, this
				, grid
				, mainContainer
				, [this]( wxVariant const & value
					, castor3d::Pass & pass
					, castor::String const & compName )
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
							moveComponentsToPass( castor::move( compProps.ownComponent ) );
						}
					}
					else if ( pass.hasComponent( compProps.component->getType() ) )
					{
						auto removed = pass.removeComponent( compProps.component->getType() );
						compProps.ownComponent = castor::move( removed.back() );
						removed.pop_back();
						moveComponentsToProps( castor::move( removed ) );
						CU_Require( compProps.ownComponent );
					}
				} );
		}
	}

	void PassTreeItemProperty::moveComponentsToPass( castor3d::PassComponentUPtr component )
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
				moveComponentsToPass( castor::move( ( *it )->ownComponent ) );
			}
		}

		pass.addComponent( castor::move( component ) );
	}

	void PassTreeItemProperty::moveComponentsToProps( castor::Vector< castor3d::PassComponentUPtr > removed )
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
				( *it )->ownComponent = castor::move( rem );
			}
		}
	}
}
