/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSkeleton_H___
#define ___C3D_TextSkeleton_H___

#include "Castor3D/Model/Skeleton/Skeleton.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Skeleton >
		: public TextWriterT< castor3d::Skeleton >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs
			, String const & subfolder );
		C3D_API bool operator()( castor3d::Skeleton const & material
			, castor::StringStream & file )override;

	private:
		String m_subfolder;
	};
}

#endif
