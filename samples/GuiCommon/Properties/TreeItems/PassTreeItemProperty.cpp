#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
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
						pass.getHeightTextureIndex(),
						ProgramFlags{},
						scene.getFlags(),
						VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
						pass.getAlphaFunc(),
						pass.getTextures( castor3d::TextureFlag::eAll ),
					}
					, scene
					, result };
				auto & engine = *pass.getOwner()->getEngine();
				auto lock( castor::makeUniqueLock( engine.getRenderWindowCache() ) );
				auto it = engine.getRenderWindowCache().begin();

				if ( it != engine.getRenderWindowCache().end()
					&& it->second->getRenderTarget() )
				{
					RenderTechniqueSPtr technique = it->second->getRenderTarget()->getTechnique();

					if ( technique )
					{
						technique->accept( vis );
					}
				}

				return result;
			}

		private:
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

	PassTreeItemProperty::PassTreeItemProperty( bool editable
		, PassSPtr pass
		, Scene & scene
		, wxWindow * parent )
		: TreeItemProperty{ pass->getOwner()->getEngine(), editable, ePROPERTY_DATA_TYPE_PASS }
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
		static wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static wxString PROPERTY_PASS_ALBEDO = _( "Albedo" );
		static wxString PROPERTY_PASS_ROUGHNESS = _( "Roughness" );
		static wxString PROPERTY_PASS_METALLIC = _( "Metallic" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_REFRACTION = _( "Refraction" );
		static wxString PROPERTY_PASS_GLOSSINESS = _( "Glossiness" );
		static wxString PROPERTY_PASS_BWACCUM = _( "BW Accumulator" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "View Shaders..." );

		PassSPtr pass = getPass();

		if ( pass )
		{
			addProperty( grid, PROPERTY_CATEGORY_PASS + wxString( pass->getOwner()->getName() ) );

			switch ( pass->getType() )
			{
			case MaterialType::ePhong:
				{
					auto & spec = static_cast< PhongPass & >( *pass );
					addPropertyT( grid, PROPERTY_PASS_DIFFUSE, spec.getDiffuse(), &spec, &PhongPass::setDiffuse );
					addPropertyT( grid, PROPERTY_PASS_SPECULAR, spec.getSpecular(), &spec, &PhongPass::setSpecular );
					addPropertyT( grid, PROPERTY_PASS_AMBIENT, spec.getAmbient(), &spec, &PhongPass::setAmbient );
					addPropertyT( grid, PROPERTY_PASS_EXPONENT, spec.getShininess(), &spec, &PhongPass::setShininess );
				}
				break;

			case MaterialType::eMetallicRoughness:
				{
					auto & spec = static_cast< MetallicRoughnessPbrPass & >( *pass );
					addPropertyT( grid, PROPERTY_PASS_ALBEDO, spec.getAlbedo(), &spec, &MetallicRoughnessPbrPass::setAlbedo );
					addPropertyT( grid, PROPERTY_PASS_ROUGHNESS, spec.getRoughness(), &spec, &MetallicRoughnessPbrPass::setRoughness );
					addPropertyT( grid, PROPERTY_PASS_METALLIC, spec.getMetallic(), &spec, &MetallicRoughnessPbrPass::setMetallic );
				}
				break;

			case MaterialType::eSpecularGlossiness:
				{
				auto & spec = static_cast< SpecularGlossinessPbrPass & >( *pass );
					addPropertyT( grid, PROPERTY_PASS_DIFFUSE, spec.getDiffuse(), &spec, &SpecularGlossinessPbrPass::setDiffuse );
					addPropertyT( grid, PROPERTY_PASS_SPECULAR, spec.getSpecular(), &spec, &SpecularGlossinessPbrPass::setSpecular );
					addPropertyT( grid, PROPERTY_PASS_GLOSSINESS, spec.getGlossiness(), &spec, &SpecularGlossinessPbrPass::setGlossiness );
				}
				break;

			default:
				CU_Failure( "Unsupported MaterialType" );
				break;
			}

			addPropertyT( grid, PROPERTY_PASS_TWO_SIDED, pass->IsTwoSided(), pass.get(), &Pass::setTwoSided );
			addPropertyT( grid, PROPERTY_PASS_EMISSIVE, pass->getEmissive(), pass.get(), &Pass::setEmissive );
			addPropertyT( grid, PROPERTY_PASS_OPACITY, pass->getOpacity(), pass.get(), &Pass::setOpacity );
			addPropertyT( grid, PROPERTY_PASS_BWACCUM, pass->getBWAccumulationOperator(), pass.get(), &Pass::setBWAccumulationOperator );

			if ( pass->hasRefraction() )
			{
				addPropertyT( grid, PROPERTY_PASS_REFRACTION, pass->getRefractionRatio(), pass.get(), &Pass::setRefractionRatio );
			}

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
