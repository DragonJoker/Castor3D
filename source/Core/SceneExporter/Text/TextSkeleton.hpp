/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSkeleton_H___
#define ___CSE_TextSkeleton_H___

#include <Castor3D/Model/Skeleton/Skeleton.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Skeleton >
		: public TextWriterT< Skeleton >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & subfolder
			, bool forceText );
		bool operator()( Skeleton const & material
			, StringStream & file )override;

	private:
		String m_subfolder;
		bool m_forceText;
	};
}

#endif
