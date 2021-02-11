#include "Castor3D/Text/TextParticleSystem.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Castor3D/Text/TextProgram.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< ParticleSystem >::TextWriter( String const & tabs )
		: TextWriterT< ParticleSystem >{ tabs }
	{
	}

	bool TextWriter< ParticleSystem >::operator()( ParticleSystem const & obj
		, castor::TextFile & file )
	{
		log::info << tabs() << cuT( "Writing ParticleSystem " ) << obj.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "particle_system" ), obj.getName() ) } )
		{
			result = writeName( file, "parent", obj.getParent()->getName() )
				&& write( file, cuT( "particles_count" ), obj.getMaxParticlesCount() )
				&& write( file, cuT( "dimensions" ), obj.getDimensions() )
				&& writeName( file, cuT( "material" ), obj.getMaterial()->getName() );

			if ( result )
			{
				result = false;

				if ( auto partBlock{ beginBlock( file, "particle" ) } )
				{
					result = true;

					if ( !obj.getParticleType().empty() )
					{
						result = writeName( file, cuT( "type" ), obj.getParticleType() );
					}

					auto values = obj.getDefaultValues();

					for ( auto & var : obj.getParticleVariables() )
					{
						result = result
							&& file.writeText( tabs() + cuT( "variable \"" )
								+ var.m_name + cuT( "\" " )
								+ castor3d::getName( var.m_dataType ) + cuT( " " )
								+ values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
						checkError( result, "ParticleSystem particle variable" );
					}
				}
			}

			if ( result && obj.getCompute().hasUpdateProgram() )
			{
				result = write( file, cuT( "cs_shader_program" ), obj.getCompute().getUpdateProgram() );
			}
		}

		return result;
	}
}
