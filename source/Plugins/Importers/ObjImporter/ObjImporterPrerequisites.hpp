/*
See LICENSE file in root folder
*/
#ifndef ___OBJ_IMPORTER_PREREQUISITES_H___
#define ___OBJ_IMPORTER_PREREQUISITES_H___

#include <Castor3D/Mesh/Importer.hpp>
#include <Castor3D/Mesh/SubmeshComponent/FaceIndices.hpp>

namespace Obj
{
	struct stNORMAL;
	struct stVERTEX;
	struct stUV;
	struct stUVW;
	struct stGROUP;

	typedef std::vector< stVERTEX > VertexArray;
	typedef std::vector< stNORMAL > NormalArray;
	typedef std::vector< stUV > UvArray;
	typedef std::vector< stUVW > UvwArray;
	typedef std::vector< castor3d::FaceIndices > FaceArray;
	typedef std::map< uint32_t, FaceArray > FaceArrayGrpMap;
	typedef std::map< uint32_t, uint32_t > UIntUIntMap;
	typedef std::vector< stGROUP * > GroupPtrArray;
	class ObjImporter;
}

#endif
