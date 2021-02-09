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

		if ( auto block = beginBlock( cuT( "particle_system" ), obj.getName(), file ) )
		{
			result = writeName( "parent", obj.getParent()->getName(), file )
				&& write( cuT( "particles_count" ), obj.getMaxParticlesCount(), file )
				&& write( cuT( "dimensions" ), obj.getDimensions(), file )
				&& writeName( cuT( "material" ), obj.getMaterial()->getName(), file );

			if ( result )
			{
				if ( auto partBlock = beginBlock( "particle", file ) )
				{
					if ( result && !obj.getParticleType().empty() )
					{
						result = writeName( cuT( "type" ), obj.getParticleType(), file );
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
				result = write( cuT( "cs_shader_program" ), obj.getCompute().getUpdateProgram(), file );
			}
		}

		return result;
	}
}
