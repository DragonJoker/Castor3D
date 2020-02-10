/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_RENDER_H___
#define ___C3D_PREREQUISITES_RENDER_H___

#include "Castor3D/Texture/TextureConfiguration.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{

	struct NonTexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
		};

		Vertex vertex[6];
	};

	struct TexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
			castor::Point2f texture;
		};

		Vertex vertex[6];
	};

	struct NonTexturedCube
	{
		struct Quad
		{
			struct Vertex
			{
				castor::Point3f position;
			};

			Vertex vertex[6];
		};

		Quad faces[6];
	};

	struct TexturedCube
	{
		struct Quad
		{
			struct Vertex
			{
				castor::Point3f position;
				castor::Point2f texture;
			};

			Vertex vertex[6];
		};

		Quad faces[6];
	};
	class RenderColourCubeLayerToTexture;
	class RenderColourCubeToTexture;
	class RenderColourLayerToTexture;
	class RenderColourToCube;
	class RenderQuad;
	class RenderDepthCubeLayerToTexture;
	class RenderDepthCubeToTexture;
	class RenderDepthLayerToTexture;
	class RenderDepthToCube;
	class TextureProjection;

	CU_DeclareSmartPtr( RenderColourCubeLayerToTexture );
	CU_DeclareSmartPtr( RenderColourCubeToTexture );
	CU_DeclareSmartPtr( RenderColourLayerToTexture );
	CU_DeclareSmartPtr( RenderColourToCube );
	CU_DeclareSmartPtr( RenderQuad );
	CU_DeclareSmartPtr( RenderDepthCubeLayerToTexture );
	CU_DeclareSmartPtr( RenderDepthCubeToTexture );
	CU_DeclareSmartPtr( RenderDepthLayerToTexture );
	CU_DeclareSmartPtr( RenderDepthToCube );
	CU_DeclareSmartPtr( TextureProjection );


	//@}
}

#endif
