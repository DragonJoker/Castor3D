#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;
using namespace GuiCommon;

namespace GuiCommon
{
	namespace
	{
		class PassTreeGatherer
			: public castor3d::PassVisitor
		{
		public:
			static void submit( Pass & pass
				, TreeItemProperty * properties
				, wxPGEditor * editor
				, wxPropertyGrid * grid )
			{
				PassTreeGatherer vis{ properties, editor, grid };
				pass.accept( vis );
			}

		private:
			PassTreeGatherer( TreeItemProperty * properties
				, wxPGEditor * editor
				, wxPropertyGrid * grid )
				: castor3d::PassVisitor{ {} }
				, m_properties{ properties }
				, m_editor{ editor }
				, m_grid{ grid }
			{
			}

			void visit( castor::String const & name
				, bool & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int16_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint16_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int32_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint32_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, int64_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, uint64_t & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, float & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, double & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor3d::BlendMode & value
				, bool * control )override
			{
				wxArrayString choices;
				choices.push_back( _( "No Blend" ) );
				choices.push_back( _( "Additive" ) );
				choices.push_back( _( "Multiplicative" ) );
				choices.push_back( _( "Interpolative" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, castor3d::ParallaxOcclusionMode & value
				, bool * control )override
			{
				wxArrayString choices;
				choices.push_back( _( "None" ) );
				choices.push_back( _( "One" ) );
				choices.push_back( _( "Repeat" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, VkCompareOp & value
				, bool * control )override
			{
				wxArrayString choices;
				choices.push_back( _( "Never" ) );
				choices.push_back( _( "Less" ) );
				choices.push_back( _( "Equal" ) );
				choices.push_back( _( "Less Equal" ) );
				choices.push_back( _( "Greater" ) );
				choices.push_back( _( "Not Equal" ) );
				choices.push_back( _( "Greater Equal" ) );
				choices.push_back( _( "Always" ) );
				m_properties->addPropertyET( m_grid, name, choices, &value, control );
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
				m_properties->addPropertyT( m_grid, name, &value, control );
			}

		private:
			TreeItemProperty * m_properties;
			wxPGEditor * m_editor;
			wxPropertyGrid * m_grid;
		};

		class PassShaderGatherer
			: public castor3d::RenderTechniqueVisitor
		{
		private:
			PassShaderGatherer( PipelineFlags flags
				, Scene const & scene
				, ShaderSources & sources )
				: castor3d::RenderTechniqueVisitor{ std::move( flags ), scene }
				, m_sources{ sources }
			{
			}

		public:
			static ShaderSources submit( Pass const & pass
				, Scene const & scene )
			{
				ShaderSources result;
				PassShaderGatherer vis{
					{
						pass.getColourBlendMode(),
						pass.getAlphaBlendMode(),
						pass.getPassFlags(),
						pass.getTypeID(),
						pass.getHeightTextureIndex(),
						ProgramFlags{},
						scene.getFlags(),
						VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
						pass.getAlphaFunc(),
						pass.getBlendAlphaFunc(),
						pass.getTexturesMask(),
					}
					, scene
					, result };
				auto & engine = *pass.getOwner()->getEngine();
				engine.getRenderTargetCache().forEach( [&vis]( RenderTarget const & target )
					{
						if ( target.isInitialised() )
						{
							target.getTechnique().accept( vis );
						}
					} );

				return result;
			}

		private:
			void visit( castor3d::ShaderModule const & module )override
			{
				doGetSource( module.name ).sources[module.stage] = &module;
			}

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
	}

	PassTreeItemProperty::PassTreeItemProperty( bool editable
		, PassSPtr pass
		, Scene & scene
		, wxWindow * parent )
		: TreeItemProperty{ pass->getOwner()->getEngine(), editable }
		, m_pass{ pass }
		, m_scene{ scene }
		, m_parent{ parent }
	{
		CreateTreeItemMenu();
	}

	PassTreeItemProperty::~PassTreeItemProperty()
	{
	}

	void PassTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "View Shaders..." );

		PassSPtr pass = getPass();

		if ( pass )
		{
			addProperty( grid, PROPERTY_CATEGORY_PASS + wxString( pass->getOwner()->getName() ) );
			PassTreeGatherer::submit( *pass, this, editor, grid );
			addProperty( grid, PROPERTY_PASS_SHADER, editor
				, [this]( wxVariant const & var )
				{
					PassSPtr pass = getPass();
					ShaderSources sources = PassShaderGatherer::submit( *pass, m_scene );
					ShaderDialog * editor = new ShaderDialog{ pass->getOwner()->getEngine()
						, std::move( sources )
						, pass->getOwner()->getName() + string::toString( pass->getId(), 10, std::locale{ "C" } )
						, m_parent };
					editor->Show();
				} );
		}
	}
}
