/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBackground_H___
#define ___CSE_TextBackground_H___

#include <Castor3D/Scene/Background/Background.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SceneBackground >
		: public TextWriterT< castor3d::SceneBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder );
		bool operator()( castor3d::SceneBackground const & overlay
			, castor::StringStream & file )override;

	private:
		Path m_folder;
	};
}

#endif
