/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSkeleton_H___
#define ___CSE_TextSkeleton_H___

#include <Castor3D/Model/Skeleton/Skeleton.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Skeleton >
		: public TextWriterT< castor3d::Skeleton >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & subfolder );
		bool operator()( castor3d::Skeleton const & material
			, castor::StringStream & file )override;

	private:
		String m_subfolder;
	};
}

#endif
