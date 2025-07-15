/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBackground_H___
#define ___CSE_TextBackground_H___

#include <Castor3D/Scene/Background/Background.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SceneBackground >
		: public TextWriterT< SceneBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder );
		bool operator()( SceneBackground const & overlay
			, StringStream & file )override;

	private:
		Path m_folder;
	};
}

#endif
