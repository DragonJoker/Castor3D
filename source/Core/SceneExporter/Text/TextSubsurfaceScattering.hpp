/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSubsurfaceScattering_H___
#define ___CSE_TextSubsurfaceScattering_H___

#include <Castor3D/Material/Pass/SubsurfaceScattering.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SubsurfaceScattering >
		: public TextWriterT< castor3d::SubsurfaceScattering >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::SubsurfaceScattering const & pass
			, castor::StringStream & file )override;
	};
}

#endif
