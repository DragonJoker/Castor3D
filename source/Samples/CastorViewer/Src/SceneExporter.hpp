#ifndef ___CV_SCENE_EXPORTER_H___
#define ___CV_SCENE_EXPORTER_H___

#include <GuiCommonPrerequisites.hpp>

#include <Path.hpp>

namespace CastorViewer
{
	class SceneExporter
	{
	public:
		SceneExporter();
		~SceneExporter();

		virtual void ExportScene( Castor3D::Engine * p_castor3d, Castor3D::Scene const & p_scene, Castor::Path const & p_fileName ) = 0;
	};

	class ObjSceneExporter
	{
	public:
		ObjSceneExporter();
		~ObjSceneExporter();

		virtual void ExportScene( Castor3D::Engine * p_castor3d, Castor3D::Scene const & p_scene, Castor::Path const & p_fileName );

	private:
		Castor::String DoExportMaterial( Castor::Path const & p_pathMtlFolder, Castor3D::Material const & p_material )const;
		Castor::String DoExportMesh( Castor3D::Mesh const & p_mesh, uint32_t & p_offset, uint32_t & p_count )const;
	};

	class CscnSceneExporter
	{
	public:
		CscnSceneExporter();
		~CscnSceneExporter();

		virtual void ExportScene( Castor3D::Engine * p_castor3d, Castor3D::Scene const & p_scene, Castor::Path const & p_fileName );
	};
}

#endif
