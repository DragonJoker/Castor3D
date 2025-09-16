/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextMeshAnimation_H___
#define ___CSE_TextMeshAnimation_H___

#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< MeshAnimation >
		: public TextWriterT< MeshAnimation >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( MeshAnimation const & object
			, StringStream & file )override;
	};
}

#endif
