/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextCamera_H___
#define ___CSE_TextCamera_H___

#include <Castor3D/Scene/Camera.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Camera >
		: public TextWriterT< Camera >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Camera const & camera
			, StringStream & file )override;
	};
}

#endif
