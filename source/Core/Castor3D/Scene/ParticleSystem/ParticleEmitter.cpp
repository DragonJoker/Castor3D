#include "Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp"

#include "Castor3D/Scene/ParticleSystem/Particle.hpp"

namespace castor3d
{
	ParticleEmitter::ParticleEmitter( ParticleDeclaration const & decl )
		: m_decl{ decl }
	{
	}

	castor3d::Particle ParticleEmitter::emit( ParticleValues const & value )
	{
		assert( m_decl.count() == value.size() );
		castor3d::Particle particle{ m_decl };
		uint32_t index = 0;

		for ( auto decl : m_decl )
		{
			switch ( decl.m_dataType )
			{
			case ParticleFormat::eInt:
				particle.setValue< ParticleFormat::eInt >( index, value.get< int32_t >( index ) );
				break;
			case ParticleFormat::eVec2i:
				particle.setValue< ParticleFormat::eVec2i >( index, value.get< castor::Point2i >( index ) );
				break;
			case ParticleFormat::eVec3i:
				particle.setValue< ParticleFormat::eVec3i >( index, value.get< castor::Point3i >( index ) );
				break;
			case ParticleFormat::eVec4i:
				particle.setValue< ParticleFormat::eVec4i >( index, value.get< castor::Point4i >( index ) );
				break;
			case ParticleFormat::eUInt:
				particle.setValue< ParticleFormat::eUInt >( index, value.get< uint32_t >( index ) );
				break;
			case ParticleFormat::eVec2ui:
				particle.setValue< ParticleFormat::eVec2ui >( index, value.get< castor::Point2ui >( index ) );
				break;
			case ParticleFormat::eVec3ui:
				particle.setValue< ParticleFormat::eVec3ui >( index, value.get< castor::Point3ui >( index ) );
				break;
			case ParticleFormat::eVec4ui:
				particle.setValue< ParticleFormat::eVec4ui >( index, value.get< castor::Point4ui >( index ) );
				break;
			case ParticleFormat::eFloat:
				particle.setValue< ParticleFormat::eFloat >( index, value.get< float >( index ) );
				break;
			case ParticleFormat::eVec2f:
				particle.setValue< ParticleFormat::eVec2f >( index, value.get< castor::Point2f >( index ) );
				break;
			case ParticleFormat::eVec3f:
				particle.setValue< ParticleFormat::eVec3f >( index, value.get< castor::Point3f >( index ) );
				break;
			case ParticleFormat::eVec4f:
				particle.setValue< ParticleFormat::eVec4f >( index, value.get< castor::Point4f >( index ) );
				break;
			case ParticleFormat::eMat2f:
				particle.setValue< ParticleFormat::eMat2f >( index, value.get< castor::Matrix2x2f >( index ) );
				break;
			case ParticleFormat::eMat3f:
				particle.setValue< ParticleFormat::eMat3f >( index, value.get< castor::Matrix3x3f >( index ) );
				break;
			case ParticleFormat::eMat4f:
				particle.setValue< ParticleFormat::eMat4f >( index, value.get< castor::Matrix4x4f >( index ) );
				break;
			}

			++index;
		}

		onEmit( particle );
		return particle;
	}
}
