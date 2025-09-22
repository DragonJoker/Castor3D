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
#include "Castor3D/Scene/Light/LightGroup.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

using c3d::operator<<;

namespace c3d
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
					blockContext->scene->scene->addNewFont( blockContext->name
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
					blockContext->scene->scene->addNewFont( blockContext->name
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

		static void addFontParsers( AttributeParsers & result )
		{
			BlockParserContextT< FontContext > context{ result, CSCNSection::eFont };

			context.addParser( cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
			context.addPopParser( cuT( "}" ), parserFontEnd );
		}

		static void addSdfFontParsers( AttributeParsers & result )
		{
			BlockParserContextT< FontContext > context{ result, CSCNSection::eSdfFont };

			context.addParser( cuT( "file" ), parserSdfFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addPopParser( cuT( "}" ), parserSdfFontEnd );
		}
	}

	void fillMeshImportParameters( FileParserContext & context
		, String const & meshParams
		, Parameters & parameters )
	{
		for ( auto const & param : string::split( meshParams, cuT( "-" ), 20, false ) )
		{
			if ( param.find( cuT( "smooth_normals" ) ) == 0 )
			{
				String strNml = cuT( "smooth" );
				parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
			}
			else if ( param.find( cuT( "flat_normals" ) ) == 0 )
			{
				String strNml = cuT( "flat" );
				parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
			}
			else if ( param.find( cuT( "tangent_space" ) ) == 0 )
			{
				parameters.add( cuT( "tangent_space" ), true );
			}
			else if ( param.find( cuT( "pitch" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != String::npos )
				{
					float value;
					string::parse< float >( param.substr( eqIndex + 1 ), value );
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
					eqIndex != String::npos )
				{
					float value;
					string::parse< float >( param.substr( eqIndex + 1 ), value );
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
					eqIndex != String::npos )
				{
					float value;
					string::parse< float >( param.substr( eqIndex + 1 ), value );
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
					eqIndex != String::npos )
				{
					float value;
					string::parse< float >( param.substr( eqIndex + 1 ), value );
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
					eqIndex != String::npos )
				{
					float value;
					string::parse< float >( param.substr( eqIndex + 1 ), value );
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
					eqIndex != String::npos )
				{
					if ( String value = param.substr( eqIndex + 1 );
						value.size() > 2 && value.front() == '\"' && value.back() == '\"' )
					{
						String prefix;
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
			else if ( param.find( cuT( "no_validation" ) ) == 0 )
			{
				parameters.add( cuT( "no_validation" ), true );
			}
			else if ( param.find( cuT( "invert_normals" ) ) == 0 )
			{
				parameters.add( cuT( "invert_normals" ), true );
			}
			else if ( param.find( cuT( "preferred_importer" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != String::npos )
				{
					if ( String value = param.substr( eqIndex + 1 );
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
			else if ( param.find( cuT( "submesh" ) ) == 0 )
			{
				if ( auto eqIndex = param.find( cuT( '=' ) );
					eqIndex != String::npos )
				{
					uint32_t value;
					string::parse< uint32_t >( param.substr( eqIndex + 1 ), value );
					parameters.add( cuT( "submesh" ), value );
				}
				else
				{
					CU_ParsingError( cuT( "Malformed parameter -submesh=<uint>." ) );
				}
				}
		}
	}

	AdditionalParsers createSceneFileParsers( Engine const & engine )
	{
		AttributeParsers parsers;
		scnprs::addFontParsers( parsers );
		scnprs::addSdfFontParsers( parsers );
		Engine::addParsers( parsers );
		Scene::addParsers( parsers );
		RenderWindow::addParsers( parsers );
		Sampler::addParsers( parsers );
		Camera::addParsers( parsers );
		Light::addParsers( parsers );
		LightGroup::addParsers( parsers );
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
		SsaoConfig::addParsers( parsers );
		VctConfig::addParsers( parsers );
		Material::addParsers( parsers, engine.getPassComponentsRegister().getTextureChannels() );

		return AdditionalParsers{ parsers
			, registerSceneFileSections()
			, nullptr };
	}

	StrSectionIdMap registerSceneFileSections()
	{
		return { { SectionId( CSCNSection::eRoot ), String{} }
			, { SectionId( CSCNSection::eScene ), cuT( "scene" ) }
			, { SectionId( CSCNSection::eWindow ), cuT( "window" ) }
			, { SectionId( CSCNSection::eSampler ), cuT( "sampler" ) }
			, { SectionId( CSCNSection::eCamera ), cuT( "camera" ) }
			, { SectionId( CSCNSection::eViewport ), cuT( "viewport" ) }
			, { SectionId( CSCNSection::eLight ), cuT( "light" ) }
			, { SectionId( CSCNSection::eNode ), cuT( "scene_node" ) }
			, { SectionId( CSCNSection::eObject ), cuT( "object" ) }
			, { SectionId( CSCNSection::eObjectMaterials ), cuT( "materials" ) }
			, { SectionId( CSCNSection::eFont ), cuT( "font" ) }
			, { SectionId( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ) }
			, { SectionId( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ) }
			, { SectionId( CSCNSection::eTextOverlay ), cuT( "text_overlay" ) }
			, { SectionId( CSCNSection::eMesh ), cuT( "mesh" ) }
			, { SectionId( CSCNSection::eSubmesh ), cuT( "submesh" ) }
			, { SectionId( CSCNSection::eMaterial ), cuT( "material" ) }
			, { SectionId( CSCNSection::ePass ), cuT( "pass" ) }
			, { SectionId( CSCNSection::eTextureUnit ), cuT( "texture_unit" ) }
			, { SectionId( CSCNSection::eRenderTarget ), cuT( "render_target" ) }
			, { SectionId( CSCNSection::eShaderProgram ), cuT( "shader_program" ) }
			, { SectionId( CSCNSection::eShaderStage ), cuT( "shader_object" ) }
			, { SectionId( CSCNSection::eUBOVariable ), cuT( "variable" ) }
			, { SectionId( CSCNSection::eBillboard ), cuT( "billboard" ) }
			, { SectionId( CSCNSection::eBillboardList ), cuT( "positions" ) }
			, { SectionId( CSCNSection::eAnimGroup ), cuT( "animated_object_group" ) }
			, { SectionId( CSCNSection::eAnimation ), cuT( "animation" ) }
			, { SectionId( CSCNSection::eSkybox ), cuT( "skybox" ) }
			, { SectionId( CSCNSection::eParticleSystem ), cuT( "particle_system" ) }
			, { SectionId( CSCNSection::eParticle ), cuT( "particle" ) }
			, { SectionId( CSCNSection::eSsao ), cuT( "ssao" ) }
			, { SectionId( CSCNSection::eHdrConfig ), cuT( "hdr_config" ) }
			, { SectionId( CSCNSection::eShadows ), cuT( "shadows" ) }
			, { SectionId( CSCNSection::eMeshDefaultMaterials ), cuT( "default_materials" ) }
			, { SectionId( CSCNSection::eLpv ), cuT( "lpv_config" ) }
			, { SectionId( CSCNSection::eRaw ), cuT( "raw_config" ) }
			, { SectionId( CSCNSection::ePcf ), cuT( "pcf_config" ) }
			, { SectionId( CSCNSection::eVsm ), cuT( "vsm_config" ) }
			, { SectionId( CSCNSection::eRsm ), cuT( "rsm_config" ) }
			, { SectionId( CSCNSection::eTextureAnimation ), cuT( "texture_animation" ) }
			, { SectionId( CSCNSection::eVoxelConeTracing ), cuT( "voxel_cone_tracing" ) }
			, { SectionId( CSCNSection::eTextureTransform ), cuT( "texture_transform" ) }
			, { SectionId( CSCNSection::eSceneImport ), cuT( "import" ) }
			, { SectionId( CSCNSection::eSkeleton ), cuT( "skeleton" ) }
			, { SectionId( CSCNSection::eMeshAnimation ), cuT( "mesh_animation" ) }
			, { SectionId( CSCNSection::eTextureRemapChannel ), cuT( "texture_remap_channel" ) }
			, { SectionId( CSCNSection::eTextureRemap ), cuT( "texture_remap" ) }
			, { SectionId( CSCNSection::eClusters ), cuT( "clusters" ) }
			, { SectionId( CSCNSection::eTexture ), cuT( "texture" ) }
			, { SectionId( CSCNSection::eColourGrading ), cuT( "colour_grading" ) }
			, { SectionId( CSCNSection::eLightingModel ), cuT( "lighting_model" ) }
			, { SectionId( CSCNSection::eDefaultLightingModel ), cuT( "default_lighting_model" ) }
			, { SectionId( CSCNSection::eLightGroup ), cuT( "light_group" ) }
			, { SectionId( CSCNSection::eLightGroupInstances ), cuT( "instances" ) }
			, { SectionId( CSCNSection::eLightGroupShadows ), cuT( "group_shadows" ) }
			, { SectionId( CSCNSection::eLightGroupShadowsLpv ), cuT( "group_lpv_config" ) }
			, { SectionId( CSCNSection::eLightGroupShadowsRaw ), cuT( "group_raw_config" ) }
			, { SectionId( CSCNSection::eLightGroupShadowsPcf ), cuT( "group_pcf_config" ) }
			, { SectionId( CSCNSection::eLightGroupShadowsVsm ), cuT( "group_vsm_config" ) }
			, { SectionId( CSCNSection::eLightGroupShadowsRsm ), cuT( "group_rsm_config" ) }
			, { SectionId( CSCNSection::eUpscaling ), cuT( "upscaling" ) }
			, { SectionId( CSCNSection::eSkeletonArmature ), cuT( "armature" ) }
			, { SectionId( CSCNSection::eSkeletonNode ), cuT( "armature_node" ) }
			, { SectionId( CSCNSection::eSkeletonBone ), cuT( "armature_bone" ) }
			, { SectionId( CSCNSection::eSkeletonAnimation ), cuT( "skeleton_animation" ) }
			, { SectionId( CSCNSection::eSkeletonAnimationObject ), cuT( "skeleton_animation_object" ) }
			, { SectionId( CSCNSection::eSkeletonAnimationKeyframe ), cuT( "skeleton_animation_keyframe" ) }
			, { SectionId( CSCNSection::eSkeletonAnimationKeyframeObject ), cuT( "skeleton_animation_keyframe_object" ) }
			, { SectionId( CSCNSection::eMeshAnimationKeyframe ), cuT( "mesh_animation_keyframe" ) }
			, { SectionId( CSCNSection::eMeshAnimationKeyframeWeights ), cuT( "mesh_animation_keyframe_weights" ) }
		};
	}

	uint32_t getSceneFileRootSection()
	{
		return uint32_t( CSCNSection::eRoot );
	}
}
