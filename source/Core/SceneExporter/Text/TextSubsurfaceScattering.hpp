/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSubsurfaceScattering_H___
#define ___CSE_TextSubsurfaceScattering_H___

#include <Castor3D/Material/Pass/SubsurfaceScattering.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SubsurfaceScattering >
		: public TextWriterT< SubsurfaceScattering >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SubsurfaceScattering const & pass
			, StringStream & file )override;
	};
}

#endif
