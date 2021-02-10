/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSubsurfaceScattering_H___
#define ___C3D_TextSubsurfaceScattering_H___

#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SubsurfaceScattering >
		: public TextWriterT< castor3d::SubsurfaceScattering >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::SubsurfaceScattering const & pass
			, castor::TextFile & file )override;
	};
}

#endif
