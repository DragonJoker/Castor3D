/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationModule_H___
#define ___C3D_MeshAnimationModule_H___

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	/**@name Mesh Animation */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Class which represents the mesh animation submeshes buffers.
	\~french
	\brief		Classe de représentation des tampons de sous-maillages d'animations de maillage.
	*/
	template< typename T >
	struct SubmeshAnimationBufferT
	{
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		std::vector< InterleavedVertexT< T > > m_buffer;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;
	};

	class MeshAnimation;
	class MeshAnimationKeyFrame;
	class MeshAnimationSubmesh;

	//@}
}

#endif
