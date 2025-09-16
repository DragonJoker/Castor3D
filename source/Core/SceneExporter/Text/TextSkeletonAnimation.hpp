/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSkeletonAnimation_H___
#define ___CSE_TextSkeletonAnimation_H___

#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SkeletonAnimation >
		: public TextWriterT< SkeletonAnimation >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SkeletonAnimation const & object
			, StringStream & file )override;
	};
}

#endif
