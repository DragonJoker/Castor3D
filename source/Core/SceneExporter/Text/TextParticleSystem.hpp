/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextParticleSystem_H___
#define ___CSE_TextParticleSystem_H___

#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ParticleSystem >
		: public TextWriterT< castor3d::ParticleSystem >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::ParticleSystem const & obj
			, castor::StringStream & file )override;
	};
}

#endif
