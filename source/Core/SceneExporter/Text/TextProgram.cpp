#include "TextProgram.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d
{
	namespace prgm
	{
		static String getShaderBlockName( ast::ShaderStage stage )
		{
			switch ( stage )
			{
			case ast::ShaderStage::eVertex: return cuT( "vertex_program" );
			case ast::ShaderStage::eTessellationControl: return cuT( "tessellation_control_program" );
			case ast::ShaderStage::eTessellationEvaluation: return cuT( "tessellation_evaluation_program" );
			case ast::ShaderStage::eGeometry: return cuT( "geometry_program" );
			case ast::ShaderStage::eFragment: return cuT( "fragment_program" );
			case ast::ShaderStage::eCompute: return cuT( "compute_program" );
			case ast::ShaderStage::eTaskNV: return cuT( "taskNV_program" );
			case ast::ShaderStage::eMeshNV: return cuT( "meshNV_program" );
			case ast::ShaderStage::eTask: return cuT( "task_program" );
			case ast::ShaderStage::eMesh: return cuT( "mesh_program" );
			case ast::ShaderStage::eRayGeneration: return cuT( "ray_generation_program" );
			case ast::ShaderStage::eRayAnyHit: return cuT( "rayAnyHit_program" );
			case ast::ShaderStage::eRayClosestHit: return cuT( "ray_closest_hit_program" );
			case ast::ShaderStage::eRayMiss: return cuT( "ray_miss_program" );
			case ast::ShaderStage::eRayIntersection: return cuT( "ray_intersection_program" );
			case ast::ShaderStage::eCallable: return cuT( "callable_program" );
			default: CU_Exception( "Unsupported ShaderStage" );
			}
		}
	}

	TextWriter< ShaderProgram >::TextWriter( String const & tabs, Path const & folder )
		: TextWriterT< ShaderProgram >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< ShaderProgram >::operator()( ShaderProgram const & object
		, StringStream & file )
	{
		bool result = true;

		for ( auto const & [stage, filePath] : object.m_files )
		{
			if ( result && !filePath.empty() )
			{
				result = false;
				if ( auto block{ beginBlock( file, prgm::getShaderBlockName( stage ) ) } )
					result = writeFile( file, cuT( "file" ), filePath, m_folder, cuT( "Shaders" ) );
			}
		}

		return result;
	}
}
