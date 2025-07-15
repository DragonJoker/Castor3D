/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextMaterial_H___
#define ___CSE_TextMaterial_H___

#include <Castor3D/Material/Material.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Material >
		: public TextWriterT< Material >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder );
		bool operator()( Material const & material
			, StringStream & file )override;

	private:
		Path m_folder;
		String m_subfolder;
	};
}

#endif
