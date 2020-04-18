#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Shader/ShaderDialog.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;
using namespace GuiCommon;

namespace GuiCommon
{
	namespace
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

		class PassShaderGatherer
			: public castor3d::RenderTechniqueVisitor
		{
		private:
			PassShaderGatherer( PipelineFlags flags
				, Scene const & scene
				, ShaderSources & sources )
				: castor3d::RenderTechniqueVisitor{ std::move( flags )
					, scene }
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
						pass.getTextures(),
						pass.getNonEnvTextureUnitsCount(),
						pass.getHeightTextureIndex(),
						ProgramFlags{},
						scene.getFlags(),
						VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
						pass.getAlphaFunc(),
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
			void visit( castor::String const & name
				, VkShaderStageFlagBits type
				, castor::String const & shader )override
			{
				doGetSource( name ).sources[type] = shader;
			}

			void visit( castor::String const & name
				, VkShaderStageFlagBits type
				, ast::Shader const & shader )override
			{
				doGetSource( name ).sources[type] = glsl::compileGlsl( shader
					, ast::SpecialisationInfo{}
					, glsl::GlslConfig
					{
						convert( type ),
						430,
						false,
						false,
						true,
						true,
						true,
						true,
					} );
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
		PROPERTY_CATEGORY_PASS = _( "Pass: " );
		PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		PROPERTY_PASS_SPECULAR = _( "Specular" );
		PROPERTY_PASS_AMBIENT = _( "Ambient" );
		PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		PROPERTY_PASS_EXPONENT = _( "Exponent" );
		PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		PROPERTY_PASS_OPACITY = _( "Opacity" );
		PROPERTY_PASS_ALBEDO = _( "Albedo" );
		PROPERTY_PASS_ROUGHNESS = _( "Roughness" );
		PROPERTY_PASS_METALLIC = _( "Metallic" );
		PROPERTY_PASS_SHADER = _( "Shader" );
		PROPERTY_PASS_GLOSSINESS = _( "Glossiness" );
		PROPERTY_PASS_EDIT_SHADER = _( "View Shaders..." );
		PROPERTY_PASS_BWACCUM = _( "BW Accumulator" );

		CreateTreeItemMenu();
	}

	PassTreeItemProperty::~PassTreeItemProperty()
	{
	}

	void PassTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		PassSPtr pass = getPass();

		if ( pass )
		{
			grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( pass->getOwner()->getName() ) ) );

			switch ( pass->getType() )
			{
			case MaterialType::ePhong:
				{
					auto phong = std::static_pointer_cast< PhongPass >( pass );
					grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( phong->getDiffuse() ) ) ) );
					grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( phong->getSpecular() ) ) ) );
					grid->Append( new wxFloatProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( phong->getAmbient() );
					grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( phong->getShininess() );
				}
				break;

			case MaterialType::eMetallicRoughness:
				{
					auto pbr = std::static_pointer_cast< MetallicRoughnessPbrPass >( pass );
					grid->Append( new wxColourProperty( PROPERTY_PASS_ALBEDO ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getAlbedo() ) ) ) );
					grid->Append( new wxFloatProperty( PROPERTY_PASS_ROUGHNESS ) )->SetValue( pbr->getRoughness() );
					grid->Append( new wxFloatProperty( PROPERTY_PASS_METALLIC ) )->SetValue( pbr->getMetallic() );
				}
				break;

			case MaterialType::eSpecularGlossiness:
				{
					auto pbr = std::static_pointer_cast< SpecularGlossinessPbrPass >( pass );
					grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getDiffuse() ) ) ) );
					grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( pbr->getSpecular() ) ) ) );
					grid->Append( new wxFloatProperty( PROPERTY_PASS_GLOSSINESS ) )->SetValue( pbr->getGlossiness() );
				}
				break;
			}

			grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( pass->IsTwoSided() );
			grid->Append( new wxFloatProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( pass->getEmissive() );
			grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( pass->getOpacity() );
			grid->Append( new wxUIntProperty( PROPERTY_PASS_BWACCUM ) )->SetValue( int( pass->getBWAccumulationOperator() ) );

			if ( checkFlag( pass->getTextures(), TextureFlag::eRefraction ) )
			{
				grid->Append( new wxFloatProperty( PROPERTY_PASS_REFRACTION ) )->SetValue( pass->getRefractionRatio() );
			}

			grid->Append( CreateProperty( PROPERTY_PASS_SHADER, PROPERTY_PASS_EDIT_SHADER, static_cast< ButtonEventMethod >( &PassTreeItemProperty::OnEditShader ), this, editor ) );
		}
	}

	void PassTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		PassSPtr pass = getPass();
		wxPGProperty * property = event.GetProperty();

		if ( property && pass )
		{
			wxColour colour;

			if ( property->GetName() == PROPERTY_PASS_DIFFUSE )
			{
				colour << property->GetValue();
				OnDiffuseColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_SPECULAR )
			{
				colour << property->GetValue();
				OnSpecularColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_AMBIENT )
			{
				OnAmbientChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_EMISSIVE )
			{
				OnEmissiveChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_EXPONENT )
			{
				OnExponentChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_TWO_SIDED )
			{
				OnTwoSidedChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_OPACITY )
			{
				OnOpacityChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_BWACCUM )
			{
				OnBWAccumulatorChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_REFRACTION )
			{
				OnRefractionRatioChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_ALBEDO )
			{
				OnAlbedoChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_PASS_ROUGHNESS )
			{
				OnRoughnessChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_GLOSSINESS )
			{
				OnGlossinessChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_PASS_METALLIC )
			{
				OnMetallicChange( property->GetValue() );
			}
		}
	}

	void PassTreeItemProperty::OnDiffuseColourChange( RgbColour const & value )
	{
		auto pass = m_pass.lock();

		switch ( pass->getType() )
		{
		case MaterialType::ePhong:
			{
				auto phong = getTypedPass< MaterialType::ePhong >();
				doApplyChange( [value, phong]()
				{
						phong->setDiffuse( value );
				} );
			}
			break;

		case MaterialType::eSpecularGlossiness:
			{
				auto pbr = getTypedPass< MaterialType::eSpecularGlossiness >();
				doApplyChange( [value, pbr]()
				{
					pbr->setDiffuse( value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnSpecularColourChange( RgbColour const & value )
	{
		auto pass = m_pass.lock();

		switch ( pass->getType() )
		{
		case MaterialType::ePhong:
			{
				auto phong = getTypedPass< MaterialType::ePhong >();
				doApplyChange( [value, phong]()
				{
						phong->setSpecular( value );
				} );
			}
			break;

		case MaterialType::eSpecularGlossiness:
			{
				auto pbr = getTypedPass< MaterialType::eSpecularGlossiness >();
				doApplyChange( [value, pbr]()
				{
					pbr->setSpecular( value );
				} );
			}
			break;
		}
	}

	void PassTreeItemProperty::OnAmbientChange (double value)
	{
		auto pass = getTypedPass< MaterialType::ePhong >();

		doApplyChange( [value, pass]()
		{
			pass->setAmbient( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnEmissiveChange( double value )
	{
		auto pass = getPass();

		doApplyChange( [value, pass]()
		{
			pass->setEmissive( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnExponentChange( double value )
	{
		auto pass = getTypedPass< MaterialType::ePhong >();

		doApplyChange( [value, pass]()
		{
			pass->setShininess( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnTwoSidedChange( bool value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [value, pass]()
		{
			pass->setTwoSided( value );
		} );
	}

	void PassTreeItemProperty::OnOpacityChange( double value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [value, pass]()
		{
			pass->setOpacity( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnBWAccumulatorChange( long value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [value, pass]()
		{
			pass->setBWAccumulationOperator( uint32_t( value ) );
		} );
	}

	void PassTreeItemProperty::OnRefractionRatioChange( double value )
	{
		PassSPtr pass = getPass();

		doApplyChange( [value, pass]()
		{
			pass->setRefractionRatio( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnAlbedoChange( castor::RgbColour const & value )
	{
		auto pass = getTypedPass< MaterialType::eMetallicRoughness >();

		doApplyChange( [value, pass]()
		{
			pass->setAlbedo( value );
		} );
	}

	void PassTreeItemProperty::OnRoughnessChange( double value )
	{
		auto pass = getTypedPass< MaterialType::eMetallicRoughness >();

		doApplyChange( [value, pass]()
		{
			pass->setRoughness( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnMetallicChange( double value )
	{
		auto pass = getTypedPass< MaterialType::eMetallicRoughness >();

		doApplyChange( [value, pass]()
		{
			pass->setMetallic( float( value ) );
		} );
	}

	void PassTreeItemProperty::OnGlossinessChange( double value )
	{
		auto pass = getTypedPass< MaterialType::eSpecularGlossiness >();

		doApplyChange( [value, pass]()
		{
			pass->setGlossiness( float( value ) );
		} );
	}

	bool PassTreeItemProperty::OnEditShader( wxPGProperty * p_property )
	{
		PassSPtr pass = getPass();
		ShaderSources sources = PassShaderGatherer::submit( *pass, m_scene );
		ShaderDialog * editor = new ShaderDialog{ pass->getOwner()->getEngine()
			, std::move( sources )
			, pass->getOwner()->getName() + string::toString( pass->getId(), 10, std::locale{ "C" } )
			, m_parent };
		editor->Show();
		return false;
	}
}
