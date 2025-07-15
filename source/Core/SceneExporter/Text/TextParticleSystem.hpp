/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextParticleSystem_H___
#define ___CSE_TextParticleSystem_H___

#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ParticleSystem >
		: public TextWriterT< ParticleSystem >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ParticleSystem const & obj
			, StringStream & file )override;
	};
}

#endif
