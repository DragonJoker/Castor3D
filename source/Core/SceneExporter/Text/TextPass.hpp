/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPass_H___
#define ___CSE_TextPass_H___

#include <Castor3D/Material/Pass/Pass.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Pass >
		: public TextWriterT< Pass >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder );
		bool operator()( Pass const & pass
			, StringStream & file )override;

	private:
		Path m_folder;
		String m_subfolder;
	};
}

#endif
