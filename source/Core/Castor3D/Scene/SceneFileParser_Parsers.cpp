#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

using castor::operator<<;

namespace castor3d
{
	namespace scnprs
	{
		static CU_ImplementAttributeParserBlock( parserFontFile, FontContext )
		{
			params[0]->get( blockContext->path );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFontHeight, FontContext )
		{
			params[0]->get( blockContext->height );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFontEnd, FontContext )
		{
			if ( !blockContext->name.empty() && !blockContext->path.empty() )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->getFontView().add( blockContext->name
						, uint32_t( blockContext->height )
						, context.file.getPath() / blockContext->path );
				}
				else
				{
					getEngine( *blockContext )->addNewFont( blockContext->name
						, uint32_t( blockContext->height )
						, context.file.getPath() / blockContext->path );
				}

				log::info << "Loaded font [" << blockContext->name << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSdfFontFile, FontContext )
		{
			params[0]->get( blockContext->path );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSdfFontEnd, FontContext )
		{
			if ( !blockContext->name.empty() && !blockContext->path.empty() )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->getFontView().add( blockContext->name
						, context.file.getPath() / blockContext->path );
				}
				else
				{
					getEngine( *blockContext )->addNewSdfFont( blockContext->name
						, context.file.getPath() / blockContext->path );
				}

				log::info << "Loaded font [" << blockContext->name << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static void addFontParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			BlockParserContextT< FontContext > context{ result, CSCNSection::eFont };

			context.addParser( cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
			context.addPopParser( cuT( "}" ), parserFontEnd );
		}

		static void addSdfFontParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			BlockParserContextT< FontContext > context{ result, CSCNSection::eSdfFont };

			context.addParser( cuT( "file" ), parserSdfFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addPopParser( cuT( "}" ), parserSdfFontEnd );
		}
	}

	void fillMeshImportParameters( castor::FileParserContext & context
		, castor::String const & meshParams
		, Parameters & parameters )
	{
		for ( auto const & param : castor::string::split( meshParams, cuT( "-" ), 20, false ) )
		{
			if ( param.find( cuT( "smooth_normals" ) ) == 0 )
			{
				castor::String strNml = cuT( "smooth" );
				parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
			}
			else if ( param.find( cuT( "flat_normals" ) ) == 0 )
			{
				castor::String strNml = cuT( "flat" );
				parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
			}
			else if ( param.find( cuT( "tangent_space" ) ) == 0 )
			{
				parameters.add( cuT( "tangent_space" ), true );
			}
			else if ( param.find( cuT( "pitch" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					float value;
					castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "pitch" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -pitch=<degrees>." ) );
				}
			}
			else if ( param.find( cuT( "yaw" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					float value;
					castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "yaw" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -yaw=<degrees>." ) );
				}
			}
			else if ( param.find( cuT( "roll" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					float value;
					castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "roll" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -roll=<degrees>." ) );
				}
			}
			else if ( param.find( cuT( "emissive_mult" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					float value;
					castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "emissive_mult" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -emissive_mult=<float>." ) );
				}
			}
			else if ( param.find( cuT( "split_mesh" ) ) == 0 )
			{
				parameters.add( cuT( "split_mesh" ), true );
			}
			else if ( param.find( cuT( "rescale" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					float value;
					castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "rescale" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -rescale=<float>." ) );
				}
			}
			else if ( param.find( cuT( "prefix" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					if ( castor::String value = param.substr( eqIndex + 1 );
						value.size() > 2 && value.front() == '\"' && value.back() == '\"' )
					{
						castor::String prefix;
						if ( parameters.get( cuT( "prefix" ), prefix ) )
						{
							parameters.add( cuT( "prefix" ), prefix + value.substr( 1, value.size() - 2 ) );
						}
						else
						{
							parameters.add( cuT( "prefix" ), value.substr( 1, value.size() - 2 ) );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -prefix=\"name\"." ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -prefix=\"name\"." ) );
				}
			}
			else if ( param.find( cuT( "no_optimisations" ) ) == 0 )
			{
				parameters.add( cuT( "no_optimisations" ), true );
			}
			else if ( param.find( cuT( "invert_normals" ) ) == 0 )
			{
				parameters.add( cuT( "invert_normals" ), true );
			}
			else if ( param.find( cuT( "preferred_importer" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != castor::String::npos )
				{
					if ( castor::String value = param.substr( eqIndex + 1 );
						value.size() > 2 && value.front() == '\"' && value.back() == '\"' )
					{
						parameters.add( cuT( "preferred_importer" ), value.substr( 1, value.size() - 2 ) );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -preferred_importer=\"name\"." ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -preferred_importer=\"name\"." ) );
				}
			}
		}
	}

	castor::AdditionalParsers createSceneFileParsers( Engine const & engine )
	{
		castor::AttributeParsers parsers;
		scnprs::addFontParsers( parsers );
		scnprs::addSdfFontParsers( parsers );
		Engine::addParsers( parsers );
		Scene::addParsers( parsers );
		RenderWindow::addParsers( parsers );
		Sampler::addParsers( parsers );
		Camera::addParsers( parsers );
		Light::addParsers( parsers );
		SceneNode::addParsers( parsers );
		Geometry::addParsers( parsers );
		Overlay::addParsers( parsers );
		RenderTarget::addParsers( parsers );
		ShaderProgram::addParsers( parsers );
		BillboardList::addParsers( parsers );
		SkyboxBackground::addParsers( parsers );
		ParticleSystem::addParsers( parsers );
		Mesh::addParsers( parsers );
		AnimatedObjectGroup::addParsers( parsers );
		Skeleton::addParsers( parsers );
		ClustersConfig::addParsers( parsers );
		HdrConfig::addParsers( parsers );
		ShadowConfig::addParsers( parsers );
		SsaoConfig::addParsers( parsers );
		VctConfig::addParsers( parsers );
		Material::addParsers( parsers, engine.getPassComponentsRegister().getTextureChannels() );

		return { parsers
			, registerSceneFileSections()
			, nullptr };
	}

	castor::StrUInt32Map registerSceneFileSections()
	{
		return { { uint32_t( CSCNSection::eRoot ), castor::String{} }
			, { uint32_t( CSCNSection::eScene ), cuT( "scene" ) }
			, { uint32_t( CSCNSection::eWindow ), cuT( "window" ) }
			, { uint32_t( CSCNSection::eSampler ), cuT( "sampler" ) }
			, { uint32_t( CSCNSection::eCamera ), cuT( "camera" ) }
			, { uint32_t( CSCNSection::eViewport ), cuT( "viewport" ) }
			, { uint32_t( CSCNSection::eLight ), cuT( "light" ) }
			, { uint32_t( CSCNSection::eNode ), cuT( "scene_node" ) }
			, { uint32_t( CSCNSection::eObject ), cuT( "object" ) }
			, { uint32_t( CSCNSection::eObjectMaterials ), cuT( "materials" ) }
			, { uint32_t( CSCNSection::eFont ), cuT( "font" ) }
			, { uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ) }
			, { uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ) }
			, { uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ) }
			, { uint32_t( CSCNSection::eMesh ), cuT( "mesh" ) }
			, { uint32_t( CSCNSection::eSubmesh ), cuT( "submesh" ) }
			, { uint32_t( CSCNSection::eMaterial ), cuT( "material" ) }
			, { uint32_t( CSCNSection::ePass ), cuT( "pass" ) }
			, { uint32_t( CSCNSection::eTextureUnit ), cuT( "texture_unit" ) }
			, { uint32_t( CSCNSection::eRenderTarget ), cuT( "render_target" ) }
			, { uint32_t( CSCNSection::eShaderProgram ), cuT( "shader_program" ) }
			, { uint32_t( CSCNSection::eShaderStage ), cuT( "shader_object" ) }
			, { uint32_t( CSCNSection::eUBOVariable ), cuT( "variable" ) }
			, { uint32_t( CSCNSection::eBillboard ), cuT( "billboard" ) }
			, { uint32_t( CSCNSection::eBillboardList ), cuT( "positions" ) }
			, { uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object_group" ) }
			, { uint32_t( CSCNSection::eAnimation ), cuT( "animation" ) }
			, { uint32_t( CSCNSection::eSkybox ), cuT( "skybox" ) }
			, { uint32_t( CSCNSection::eParticleSystem ), cuT( "particle_system" ) }
			, { uint32_t( CSCNSection::eParticle ), cuT( "particle" ) }
			, { uint32_t( CSCNSection::eSsao ), cuT( "ssao" ) }
			, { uint32_t( CSCNSection::eHdrConfig ), cuT( "hdr_config" ) }
			, { uint32_t( CSCNSection::eShadows ), cuT( "shadows" ) }
			, { uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "default_materials" ) }
			, { uint32_t( CSCNSection::eLpv ), cuT( "lpv_config" ) }
			, { uint32_t( CSCNSection::eRaw ), cuT( "raw_config" ) }
			, { uint32_t( CSCNSection::ePcf ), cuT( "pcf_config" ) }
			, { uint32_t( CSCNSection::eVsm ), cuT( "vsm_config" ) }
			, { uint32_t( CSCNSection::eRsm ), cuT( "rsm_config" ) }
			, { uint32_t( CSCNSection::eTextureAnimation ), cuT( "texture_animation" ) }
			, { uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_cone_tracing" ) }
			, { uint32_t( CSCNSection::eTextureTransform ), cuT( "texture_transform" ) }
			, { uint32_t( CSCNSection::eSceneImport ), cuT( "import" ) }
			, { uint32_t( CSCNSection::eSkeleton ), cuT( "skeleton" ) }
			, { uint32_t( CSCNSection::eMorphAnimation ), cuT( "morph_animation" ) }
			, { uint32_t( CSCNSection::eTextureRemapChannel ), cuT( "texture_remap_channel" ) }
			, { uint32_t( CSCNSection::eTextureRemap ), cuT( "texture_remap" ) }
			, { uint32_t( CSCNSection::eClusters ), cuT( "clusters" ) }
			, { uint32_t( CSCNSection::eTexture ), cuT( "texture" ) }
			, { uint32_t( CSCNSection::eColourGrading ), cuT( "colour_grading" ) }
			, { uint32_t( CSCNSection::eLightingModel ), cuT( "lighting_model" ) }
			, { uint32_t( CSCNSection::eDefaultLightingModel ), cuT( "default_lighting_model" ) } };
	}

	uint32_t getSceneFileRootSection()
	{
		return uint32_t( CSCNSection::eRoot );
	}
}
