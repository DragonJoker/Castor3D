#include "PyCastor3D/PyCastor3DPch.hpp"

#include "PyCastor3D/PyCastor3DPrerequisites.hpp"

#include <Castor3D/Cache/LightCache.hpp>
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"

using namespace castor;
using namespace castor3d;

#define CACHE_GETTER( owner, name )\
	name##Cache & ( owner::*resget##name##Cache )() = &owner::get##name##Cache

void ExportCastor3D()
{
	// Make "from castor.gfx import <whatever>" work
	py::object l_module( py::handle<>( py::borrowed( PyImport_AddModule( "castor.gfx" ) ) ) );
	// Make "from castor import gfx" work
	py::scope().attr( "gfx" ) = l_module;
	// set the current scope to the new sub-module
	py::scope l_scope = l_module;
	//@}
	/**@group_name eLIGHT_TYPE */
	//@{
	py::enum_< LightType >( "LightType" )
		.value( "DIRECTIONAL", LightType::eDirectional )
		.value( "POINT", LightType::ePoint )
		.value( "SPOT", LightType::eSpot )
		;
	//@}
	/**@group_name MovableType */
	//@{
	py::enum_< MovableType >( "MovableType" )
		.value( "CAMERA", MovableType::eCamera )
		.value( "GEOMETRY", MovableType::eGeometry )
		.value( "LIGHT", MovableType::eLight )
		.value( "BILLBOARD", MovableType::eBillboard )
		;
	//@}
	/**@group_name eOVERLAY_TYPE */
	//@{
	py::enum_< OverlayType >( "OverlayType" )
		.value( "PANEL", OverlayType::ePanel )
		.value( "BORDER_PANEL", OverlayType::eBorderPanel )
		.value( "TEXT", OverlayType::eText )
		;
	//@}
	/**@group_name eVIEWPORT_TYPE */
	//@{
	py::enum_< ViewportType >( "ViewportType" )
		.value( "ORTHO", ViewportType::eOrtho )
		.value( "PERSPECTIVE", ViewportType::ePerspective )
		.value( "FRUSTUM", ViewportType::eFrustum )
		;
	//@}
	/**@group_name TextureFlag */
	//@{
	py::enum_< TextureFlag >( "TextureFlag" )
		.value( "DIFFUSE", TextureFlag::eDiffuse )
		.value( "NORMAL", TextureFlag::eNormal )
		.value( "OPACITY", TextureFlag::eOpacity )
		.value( "SPECULAR", TextureFlag::eSpecular )
		.value( "HEIGHT", TextureFlag::eHeight )
		.value( "EMISSIVE", TextureFlag::eEmissive )
		.value( "GLOSS", TextureFlag::eGlossiness )
		.value( "ALL", TextureFlag::eAll )
		;
	//@}
	/**@group_name TextureType */
	//@{
	py::enum_< VkImageType >( "ImageType" )
		.value( "1D", VkImageType::VK_IMAGE_TYPE_1D )
		.value( "2D", VkImageType::VK_IMAGE_TYPE_2D )
		.value( "3D", VkImageType::VK_IMAGE_TYPE_3D )
		;
	//@}
	/**@group_name WrapMode */
	//@{
	py::enum_< VkSamplerAddressMode >( "WrapMode" )
		.value( "REPEAT", VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT )
		.value( "MIRRORED_REPEAT", VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT )
		.value( "CLAMP_TO_BORDER", VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER )
		.value( "CLAMP_TO_EDGE", VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE )
		;
	//@}
	/**@group_name Filter */
	//@{
	py::enum_< VkFilter >( "Filter" )
		.value( "NEAREST", VkFilter::VK_FILTER_NEAREST )
		.value( "LINEAR", VkFilter::VK_FILTER_LINEAR )
		;
	//@}
	/**@group_name MipmapMode */
	//@{
	py::enum_< VkSamplerMipmapMode >( "MipmapMode" )
		.value( "NEAREST", VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST )
		.value( "LINEAR", VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR )
		;
	//@}
	/**@group_name PrimitiveTopology */
	//@{
	py::enum_< VkPrimitiveTopology >( "PrimitiveTopology" )
		.value( "POINT_LIST", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST )
		.value( "LINE_LIST", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST )
		.value( "LINE_STRIP", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP )
		.value( "TRIANGLE_LIST", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST )
		.value( "TRIANGLE_STRIP", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP )
		.value( "TRIANGLE_FAN", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN )
		.value( "LINE_LIST_ADJACENCY", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)
		.value( "LINE_STRIP_ADJACENCY", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY )
		.value( "TRIANGLE_LIST_ADJACENCY", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY )
		.value( "TRIANGLE_STRIP_ADJACENCY", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP )
		.value( "PATCH_LIST", VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST )
		;
	//@}
	/**@group_name Sampler */
	//@{
	py::class_< Sampler, boost::noncopyable >( "Sampler", py::no_init )
		.add_property( "name", py::make_function( &Sampler::getName, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Sampler::setName ), "The sampler name" )
		.add_property( "min_filter", cpy::make_getter( &Sampler::getMinFilter ), cpy::make_setter( &Sampler::setMinFilter ), "The sampler min filter" )
		.add_property( "mag_filter", cpy::make_getter( &Sampler::getMagFilter ), cpy::make_setter( &Sampler::setMagFilter ), "The sampler mag filter" )
		.add_property( "mip_filter", cpy::make_getter( &Sampler::getMipFilter ), cpy::make_setter( &Sampler::setMipFilter ), "The sampler mip filter" )
		.add_property( "u_wrap", cpy::make_getter( &Sampler::getWrapS ), cpy::make_setter( &Sampler::setWrapS ), "The sampler U wrap mode" )
		.add_property( "v_wrap", cpy::make_getter( &Sampler::getWrapT ), cpy::make_setter( &Sampler::setWrapT ), "The sampler V wrap mode" )
		.add_property( "w_wrap", cpy::make_getter( &Sampler::getWrapR ), cpy::make_setter( &Sampler::setWrapR ), "The sampler W wrap mode" )
		.add_property( "max_anisotropy", &Sampler::getMaxAnisotropy, &Sampler::setMaxAnisotropy, "The sampler max anisotropy" )
		.add_property( "min_lod", &Sampler::getMinLod, &Sampler::setMinLod, "The sampler min LOD" )
		.add_property( "max_lod", &Sampler::getMaxLod, &Sampler::setMaxLod, "The sampler max LOD" )
		.add_property( "lod_bias", &Sampler::getLodBias, &Sampler::setLodBias, "The sampler LOD bias" )
		.add_property( "border_colour", py::make_function( &Sampler::getBorderColour ), cpy::make_setter( &Sampler::setBorderColour ), "The sampler border colour" )
		.def( "initialise", &Sampler::initialise )
		.def( "cleanup", &Sampler::cleanup )
		;
	//@}
	/**@group_name TextureView */
	//@{
	py::class_< TextureView, boost::noncopyable >( "TextureImage", py::no_init )
		//.add_property( "buffer", &TextureView::getBuffer, "The texture image buffer" )
		;
	//@}
	/**@group_name TextureLayout */
	//@{
	TextureView &( TextureLayout::*textureLayoutImagegetter )() = &TextureLayout::getDefaultView;
	void ( TextureLayout::*dynamicTextureImagesetter )( PxBufferBaseSPtr, bool ) = &TextureLayout::setSource;
	void ( TextureLayout::*staticTextureImagesetter )( Path const &, Path const & ) = &TextureLayout::setSource;
	py::class_< TextureLayout, boost::noncopyable >( "TextureLayout", py::no_init )
		.add_property( "type", &TextureLayout::getType, "The texture type" )
		.def( "image", py::make_function( textureLayoutImagegetter, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "initialise", &TextureLayout::initialise )
		.def( "cleanup", &TextureLayout::cleanup )
		.def( "set_source", staticTextureImagesetter, "Sets the texture image" )
		.def( "set_source", dynamicTextureImagesetter, "Sets the texture image" )
		;
	//@}
	/**@group_name TextureUnit */
	//@{
	py::class_< TextureUnit, boost::noncopyable >( "TextureUnit", py::no_init )
		.add_property( "texture", &TextureUnit::getTexture, &TextureUnit::setTexture, "The unit texture" )
		.add_property( "channel", &TextureUnit::getFlags, "The texture channel" )
		.def( "set_texture", &TextureUnit::setTexture )
		;
	//@}
	/**@group_name PhongPass */
	//@{
	TextureUnitSPtr( Pass::*passChannelTextureUnitgetter )( uint32_t )const = &Pass::getTextureUnit;
	typedef TextureUnitPtrArrayIt( Pass::*TextureUnitPtrArrayItFunc )( );
	py::class_< Pass, boost::noncopyable >( "Pass", py::no_init )
		.add_property( "diffuse", cpy::make_getter( &PhongPass::getDiffuse, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PhongPass::setDiffuse ), "The pass diffuse colour" )
		.add_property( "specular", cpy::make_getter( &PhongPass::getSpecular, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PhongPass::setSpecular ), "The pass specular colour" )
		.add_property( "emissive", cpy::make_getter( &PhongPass::getEmissive ), cpy::make_setter( &PhongPass::setEmissive ), "The pass emissive colour" )
		.add_property( "shininess", &PhongPass::getShininess, &PhongPass::setShininess, "The pass shininess value" )
		.add_property( "two_sided", &PhongPass::IsTwoSided, &PhongPass::setTwoSided, "The pass two sided status" )
		.add_property( "alpha", &PhongPass::getOpacity, &PhongPass::setOpacity, "The pass global alpha value" )
		.def( "create_texture_unit", &PhongPass::addTextureUnit )
		.def( "destroy_texture_unit", &PhongPass::removeTextureUnit )
		.def( "get_texture_unit_at_channel", passChannelTextureUnitgetter )
		.def( "units", py::range< TextureUnitPtrArrayItFunc, TextureUnitPtrArrayItFunc >( &PhongPass::begin, &PhongPass::end ) )
		;
	//@}
	/**@group_name Material */
	//@{
	typedef PassPtrArrayIt( Material::*PassPtrArrayItFunc )( );
	py::class_< Material, boost::noncopyable >( "Material", py::init< String const &, Engine &, MaterialType >() )
		.add_property( "pass_count", &Material::getPassCount, "The material's passes count" )
		.def( "initialise", &Material::initialise )
		.def( "cleanup", &Material::cleanup )
		.def( "create_pass", &Material::createPass )
		.def( "destroy_pass", &Material::destroyPass )
		.def( "passes", py::range< PassPtrArrayItFunc, PassPtrArrayItFunc >( &Material::begin, &Material::end ) )
		;
	//@}
	/**@group_name IndexMapping */
	//@{
	py::class_< IndexMapping, boost::noncopyable >( "IndexMapping", py::no_init )
		;
	//@}
	/**@group_name TriFaceMapping */
	//@{
	py::class_< TriFaceMapping, py::bases< IndexMapping >, boost::noncopyable >( "TriFaceMapping", py::no_init )
		.add_property( "faces_count", &TriFaceMapping::getCount, "The total mapping's faces count" )
		.def( "add_face", cpy::FaceAdder() )
		;
	//@}
	/**@group_name LinesMapping */
	//@{
	py::class_< LinesMapping, py::bases< IndexMapping >, boost::noncopyable >( "LinesMapping", py::no_init )
		.add_property( "lines_count", &TriFaceMapping::getCount, "The total mapping's lines count" )
		.def( "add_line", cpy::LineAdder() )
		;
	//@}
	/**@group_name Submesh */
	//@{
	py::class_< Submesh, boost::noncopyable >( "Submesh", py::no_init )
		.add_property( "vertex_count", &Submesh::getPointsCount, "The total submesh's vertices count" )
		.add_property( "topology", &Submesh::getTopology, &Submesh::setTopology, "The submesh topology" )
		.def( "add_point", cpy::PointAdder() )
		;
	//@}
	/**@group_name Mesh */
	//@{
	typedef SubmeshPtrArrayIt( Mesh::*SubmeshPtrArrayItFunc )( );
	Animation & ( Mesh::*meshAnimationgetter )( castor::String const & ) = &Mesh::getAnimation;
	py::class_< Mesh, boost::noncopyable >( "Mesh", py::no_init )
		.add_property( "submesh_count", &Mesh::getSubmeshCount, "The total mesh's submeshes count" )
		.add_property( "faces_count", &Mesh::getFaceCount, "The total mesh's faces count" )
		.add_property( "vertex_count", &Mesh::getVertexCount, "The total mesh's vertices count" )
		.add_property( "submeshes", py::range< SubmeshPtrArrayItFunc, SubmeshPtrArrayItFunc >( &Mesh::begin, &Mesh::end ) )
		.def( "create_submesh", &Mesh::createSubmesh )
		.def( "delete_submesh", &Mesh::deleteSubmesh )
		.def( "animations", cpy::make_map_wrapper< AnimationPtrStrMap >( "Animations", &Mesh::getAnimations ) )
		.def( "create_animation", py::make_function( &Mesh::createAnimation, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "get_animation", py::make_function( meshAnimationgetter, py::return_value_policy< py::copy_non_const_reference >() ) )
		;
	//@}
	/**@group_name SceneNodeCache */
	//@{
	SceneNodeSPtr( SceneNodeCache::*sceneNodeCreator )( castor::String const &, SceneNodeSPtr ) = &SceneNodeCache::add;
	py::class_< SceneNodeCache, boost::noncopyable >( "SceneNodeCache", py::no_init )
		.def( "add", sceneNodeCreator, "Adds a SceneNode to the cache" )
		.def( "remove", &SceneNodeCache::remove, "Finds a SceneNode" )
		.def( "has", &SceneNodeCache::has, "Tells if the cache has a SceneNode" )
		.def( "find", &SceneNodeCache::find, "Finds a SceneNode in the cache" )
		;
	//@}
	/**@group_name CameraCache */
	//@{
	CameraSPtr( CameraCache::*cameraAdder )( castor::String const &, CameraSPtr ) = &CameraCache::add;
	py::class_< CameraCache, boost::noncopyable >( "CameraCache", py::no_init )
		.def( "add", cameraAdder, "Adds a Camera to the cache" )
		.def( "remove", &CameraCache::remove, "Finds a Camera" )
		.def( "has", &CameraCache::has, "Tells if the cache has a Camera" )
		.def( "find", &CameraCache::find, "Finds a Camera in the cache" )
		;
	//@}
	/**@group_name GeometryCache */
	//@{
	GeometrySPtr( GeometryCache::*geometryCreator )( castor::String const &, SceneNode &, MeshSPtr ) = &GeometryCache::add;
	py::class_< GeometryCache, boost::noncopyable >( "GeometryCache", py::no_init )
		.def( "add", geometryCreator, "Adds a Geometry to the cache" )
		.def( "remove", &GeometryCache::remove, "Finds a Geometry" )
		.def( "has", &GeometryCache::has, "Tells if the cache has a Geometry" )
		.def( "find", &GeometryCache::find, "Finds a Geometry in the cache" )
		;
	//@}
	/**@group_name LightCache */
	//@{
	LightSPtr( LightCache::*lightCreator )( castor::String const &, SceneNode &, LightType ) = &LightCache::add;
	py::class_< LightCache, boost::noncopyable >( "LightCache", py::no_init )
		.def( "add", lightCreator, "Adds a Light to the cache" )
		.def( "remove", &LightCache::remove, "Removes a Light from the cache" )
		.def( "has", &LightCache::has, "Tells if the cache has a Light" )
		.def( "find", &LightCache::find, "Finds a Light in the cache" )
		;
	/**@group_name MeshCache */
	//@{
	py::class_< MeshCache, boost::noncopyable >( "MeshCache", py::no_init )
		.def( "add", &MeshCache::add<>, "Adds a Mesh to the cache" )
		.def( "remove", &MeshCache::remove, "Removes a Mesh from the cache" )
		.def( "has", &MeshCache::has, "Tells if the cache has a Mesh" )
		.def( "find", &MeshCache::find, "Finds a Mesh in the cache" )
		;
	//@}
	/**@group_name RenderWindowCache */
	//@{
	py::class_< RenderWindowCache, boost::noncopyable >( "RenderWindowCache", py::no_init )
		.def( "add", &RenderWindowCache::add<>, "Adds a RenderWindow to the cache" )
		.def( "remove", &RenderWindowCache::remove, "Removes a RenderWindow from the cache" )
		.def( "has", &RenderWindowCache::has, "Tells if the cache has a RenderWindow" )
		.def( "find", &RenderWindowCache::find, "Finds a RenderWindow in the cache" )
		;
	//@}
	/**@group_name MaterialCache */
	//@{
	MaterialSPtr( MaterialCache::*mtlmgrCreate )( castor::String const &, castor3d::MaterialType ) = &MaterialCache::add;
	py::class_< MaterialCache, boost::noncopyable >( "MaterialCache", py::no_init )
		.def( "add", mtlmgrCreate, "Adds a Material to the cache" )
		.def( "remove", &MaterialCache::remove, "Removes a Material from the cache" )
		.def( "has", &MaterialCache::has, "Tells if the cache has a Material" )
		.def( "find", &MaterialCache::find, "Finds a Material in the cache" )
		;
	//@}
	/**@group_name SceneCache */
	//@{
	SceneSPtr( SceneCache::*scnmgrCreate )( castor::String const & ) = &SceneCache::add;
	py::class_< SceneCache, boost::noncopyable >( "SceneCache", py::no_init )
		.def( "add", scnmgrCreate, "Adds a Scene to the cache" )
		.def( "remove", &SceneCache::remove, "Removes a Scene from the cache" )
		.def( "has", &SceneCache::has, "Tells if the cache has a Scene" )
		.def( "find", &SceneCache::find, "Finds a Scene in the cache" )
		;
	//@}
	/**@group_name OverlayCache */
	//@{
	OverlaySPtr( OverlayCache::*ovlCreate )( castor::String const &, OverlayType, SceneSPtr, OverlaySPtr ) = &OverlayCache::add;
	py::class_< OverlayCache, boost::noncopyable >( "OverlayCache", py::no_init )
		.def( "add", ovlCreate, "Adds an Overlay to the cache" )
		.def( "remove", &OverlayCache::remove, "Removes a Overlay from the cache" )
		.def( "has", &OverlayCache::has, "Tells if the cache has a Overlay" )
		.def( "find", &OverlayCache::find, "Finds a Overlay in the cache" )
		;
	//@}
	/**@group_name PluginCache */
	//@{
	PluginSPtr( PluginCache::*pluginLoader )( castor::Path const & ) = &PluginCache::loadPlugin;
	py::class_< PluginCache, boost::noncopyable >( "PluginCache", py::no_init )
		.def( "load_plugin", pluginLoader )
		.def( "load_plugins", &PluginCache::loadAllPlugins )
		.def( "get_plugins", &PluginCache::getPlugins )
		;
	//@}
	/**@group_name SceneBackground */
	//@{
	py::class_< SceneBackground, boost::noncopyable >( "SceneBackground", py::no_init )
		;
	//@}
	/**@group_name ColourBackground */
	//@{
	py::class_< ColourBackground, py::bases< SceneBackground >, boost::noncopyable >( "ColourBackground", py::init< Engine &, Scene & >() )
		;
	//@}
	/**@group_name ImageBackground */
	//@{
	py::class_< ImageBackground, py::bases< SceneBackground >, boost::noncopyable >( "ImageBackground", py::init< Engine &, Scene & >() )
		.def( "image", &ImageBackground::loadImage )
		;
	//@}
	/**@group_name SkyboxBackground */
	//@{
	py::class_< SkyboxBackground, py::bases< SceneBackground >, boost::noncopyable >( "SkyboxBackground", py::init< Engine &, Scene & >() )
		.def( "left_image", &SkyboxBackground::loadLeftImage )
		.def( "right_image", &SkyboxBackground::loadRightImage )
		.def( "top_image", &SkyboxBackground::loadTopImage )
		.def( "bottom_image", &SkyboxBackground::loadBottomImage )
		.def( "front_image", &SkyboxBackground::loadFrontImage )
		.def( "back_image", &SkyboxBackground::loadBackImage )
		.def( "equi_image", &SkyboxBackground::loadEquiTexture )
		.def( "cross_image", &SkyboxBackground::loadCrossTexture )
		;
	//@}
	/**@group_name Scene */
	//@{
	CACHE_GETTER( Scene, SceneNode );
	CACHE_GETTER( Scene, Camera );
	CACHE_GETTER( Scene, Geometry );
	CACHE_GETTER( Scene, Light );
	CACHE_GETTER( Scene, Mesh );
	py::class_< Scene, boost::noncopyable >( "Scene", py::no_init )
		.add_property( "background_colour", py::make_function( &Scene::getBackgroundColour, py::return_value_policy< py::copy_const_reference >() ), &Scene::setBackgroundColour, "The background colour" )
		.add_property( "name", py::make_function( &Scene::getName, py::return_value_policy< py::copy_const_reference >() ), &Scene::setName, "The scene name" )
		.add_property( "ambient_light", py::make_function( &Scene::getAmbientLight, py::return_value_policy< py::copy_const_reference >() ), &Scene::setAmbientLight, "The ambient light colour" )
		.add_property( "root_node", &Scene::getRootNode, "The root scene node" )
		.add_property( "root_object_node", &Scene::getObjectRootNode, "The objects root scene node" )
		.add_property( "root_camera_node", &Scene::getCameraRootNode, "The cameras root scene node" )
		.def( "cleanup", &Scene::cleanup )
		.def( "nodes", py::make_function( resgetSceneNodeCache, py::return_value_policy< py::copy_non_const_reference >() ), "The SceneNodes cache" )
		.def( "geometries", py::make_function( resgetGeometryCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Geometries cache" )
		.def( "cameras", py::make_function( resgetCameraCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Cameras cache" )
		.def( "lights", py::make_function( resgetLightCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Lights cache" )
		.def( "meshes", py::make_function( resgetMeshCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Meshs cache" )
		.def( "get_background", &Scene::getBackground )
		.def( "set_background", &Scene::setBackground )
		;
	//@}
	//@}
	/**@group_name RenderWindow */
	//@{
	void( RenderWindow::*RenderWindowResizer )( Size const & ) = &RenderWindow::resize;
	py::class_< RenderWindow, boost::noncopyable >( "RenderWindow", py::no_init )
		.add_property( "camera", &RenderWindow::getCamera, &RenderWindow::setCamera, "The window camera" )
		.add_property( "viewport", &RenderWindow::getViewportType, &RenderWindow::setViewportType, "The viewport type" )
		.add_property( "scene", &RenderWindow::getScene, &RenderWindow::setScene, "The rendered scene" )
		.add_property( "pixel_format", &RenderWindow::getPixelFormat, &RenderWindow::setPixelFormat, "The window pixel format" )
		.add_property( "size", &RenderWindow::getSize, "The window size" )
		.add_property( "name", py::make_function( &RenderWindow::getName, py::return_value_policy< py::copy_const_reference >() ), "The window name" )
		//.def( "initialise", &RenderWindow::initialise )
		.def( "cleanup", &RenderWindow::cleanup )
		.def( "resize", RenderWindowResizer )
		;
	//@}
	/**@group_name MovableObject */
	//@{
	py::class_< MovableObject, boost::noncopyable >( "MovableObject", py::no_init )
		.add_property( "name", py::make_function( &MovableObject::getName, py::return_value_policy< py::copy_const_reference >() ), "The object name" )
		.add_property( "type", &MovableObject::getType, "The movable type" )
		.add_property( "scene", py::make_function( &MovableObject::getScene, py::return_value_policy< py::return_by_value >() ), "The object's parent scene" )
		.def( "attach", &MovableObject::attachTo )
		.def( "detach", &MovableObject::detach )
		;
	//@}
	/**@group_name LightCategory */
	//@{
	py::class_< LightCategory, boost::noncopyable >( "LightCategory", py::no_init )
		.add_property( "colour", cpy::make_getter( &LightCategory::getColour, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LightCategory::setColour ), "The light colour" )
		.add_property( "diffuse_intensity", cpy::make_getter( &LightCategory::getDiffuseIntensity ), cpy::make_setter( &LightCategory::setDiffuseIntensity ), "The light diffuse intensity" )
		.add_property( "specular_intensity", cpy::make_getter( &LightCategory::getSpecularIntensity ), cpy::make_setter( &LightCategory::setSpecularIntensity ), "The light specular intensity" )
		;
	//@}
	/**@group_name DirectionalLight */
	//@{
	py::class_< DirectionalLight, py::bases< LightCategory >, boost::noncopyable >( "DirectionalLight", py::no_init )
		;
	//@}
	/**@group_name PointLight */
	//@{
	py::class_< PointLight, py::bases< LightCategory >, boost::noncopyable >( "PointLight", py::no_init )
		.add_property( "attenuation", cpy::make_getter( &PointLight::getAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::setAttenuation ), "The light attenuation values (constant, linear and quadratic)" )
		;
	//@}
	/**@group_name SpotLight */
	//@{
	py::class_< SpotLight, py::bases< LightCategory >, boost::noncopyable >( "SpotLight", py::no_init )
		.add_property( "attenuation", cpy::make_getter( &PointLight::getAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::setAttenuation ), "The light attenuation values (constant, linear and quadratic)" )
		.add_property( "exponent", &SpotLight::getExponent, &SpotLight::setExponent, "The light exponent value" )
		.add_property( "cut_off", cpy::make_getter( &SpotLight::getCutOff, py::return_value_policy< py::copy_const_reference >() ), &SpotLight::setCutOff, "The light cut off value" )
		;
	//@}
	/**@group_name Camera */
	//@{
	void ( Camera::*cameraResizer )( Size const & ) = &Camera::resize;
	py::class_< Camera, py::bases< MovableObject >, boost::noncopyable >( "Camera", py::no_init )
		.add_property( "viewport", &Camera::getViewportType, &Camera::setViewportType, "The camera viewport type" )
		.add_property( "width", &Camera::getWidth, "The camera horizontal resolution" )
		.add_property( "height", &Camera::getHeight, "The camera vertical resolution" )
		.def( "resize", cameraResizer )
		;
	//@}
	/**@group_name Light */
	//@{
	py::class_< Light, py::bases< MovableObject >, boost::noncopyable >( "Light", py::no_init )
		.add_property( "light_type", &Light::getLightType, "The light type" )
		.add_property( "directional", &Light::getDirectionalLight, "The directional light category" )
		.add_property( "point", &Light::getPointLight, "The point light category" )
		.add_property( "spot", &Light::getSpotLight, "The spot light category" )
		;
	//@}
	/**@group_name Geometry */
	//@{
	py::class_< Geometry, py::bases< MovableObject >, boost::noncopyable >( "Geometry", py::no_init )
		.add_property( "mesh", &Geometry::getMesh, &Geometry::setMesh, "The geometry's mesh" )
		.def( "get_material", &Geometry::getMaterial )
		.def( "set_material", &Geometry::setMaterial )
		;
	//@}
	/**@group_name SceneNode */
	//@{
	py::class_< SceneNode, std::shared_ptr< SceneNode >, boost::noncopyable >( "SceneNode", py::no_init )
		.add_property( "position", cpy::make_getter( &SceneNode::getPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setPosition ), "The node position" )
		.add_property( "orientation", cpy::make_getter( &SceneNode::getOrientation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setOrientation ), "The node orientation" )
		.add_property( "scale", cpy::make_getter( &SceneNode::getScale, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setScale ), "The node scale" )
		.def( "attach_object", &SceneNode::attachObject )
		.def( "detach_object", &SceneNode::detachObject )
		.def( "attach_to", &SceneNode::attachTo )
		.def( "detach_from", &SceneNode::detach )
		.def( "yaw", &SceneNode::yaw )
		.def( "pitch", &SceneNode::pitch )
		.def( "roll", &SceneNode::roll )
		.def( "rotate", cpy::make_setter( &SceneNode::rotate ) )
		.def( "translate", cpy::make_setter( &SceneNode::translate ) )
		.def( "scale", cpy::make_setter( &SceneNode::scale ) )
		;
	//@}
	/**@group_name OverlayCategory */
	//@{
	py::class_< OverlayCategory, boost::noncopyable >( "OverlayCategory", py::no_init )
		.add_property( "type", &OverlayCategory::getType, "The overlay type" )
		.add_property( "position", cpy::make_getter( &OverlayCategory::getPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::setPosition ), "The overlay position, relative to its parent" )
		.add_property( "size", cpy::make_getter( &OverlayCategory::getSize, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::setSize ), "The overlay size, relative to its parent" )
		.add_property( "visible", &Overlay::isVisible, &Overlay::setVisible, "The overlay visibility" )
		.add_property( "material", &Overlay::getMaterial, &Overlay::setMaterial, "The overlay material" )
		;
	//@}
	/**@group_name PanelOverlay */
	//@{
	py::class_< PanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "PanelOverlay", py::no_init );
	//@}
	/**@group_name BorderPanelOverlay */
	//@{
	py::class_< BorderPanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "BorderPanelOverlay", py::no_init )
		.add_property( "border_left", &BorderPanelOverlay::getLeftBorderSize, &BorderPanelOverlay::setLeftBorderSize, "The bottom border width" )
		.add_property( "border_right", &BorderPanelOverlay::getRightBorderSize, &BorderPanelOverlay::setRightBorderSize, "The right border width" )
		.add_property( "border_top", &BorderPanelOverlay::getTopBorderSize, &BorderPanelOverlay::setTopBorderSize, "The top border width" )
		.add_property( "border_bottom", &BorderPanelOverlay::getBottomBorderSize, &BorderPanelOverlay::setBottomBorderSize, "The bottom border width" )
		.add_property( "border_material", &BorderPanelOverlay::getBorderMaterial, &BorderPanelOverlay::setBorderMaterial, "The border material" )
		;
	//@}
	/**@group_name TextOverlay */
	//@{
	void( TextOverlay::*captionsetter )( castor::String const & ) = &TextOverlay::setCaption;
	py::class_< TextOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "TextOverlay", py::no_init )
		.add_property( "font", py::make_function( &TextOverlay::getFontName, py::return_value_policy< py::copy_const_reference >() ), &TextOverlay::setFont, "The text font" )
		.add_property( "caption", &TextOverlay::getCaption, captionsetter, "The text caption" )
		;
	//@}
	/**@group_name Overlay */
	//@{
	uint32_t( Overlay::*ovgetChildsCount )( )const = &Overlay::getChildrenCount;
	typedef Overlay::iterator( Overlay::*OverlayPtrIntMapItFunc )( );
	py::class_< Overlay, boost::noncopyable >( "Overlay", py::no_init )
		.add_property( "panel", &Overlay::getPanelOverlay, "The panel overlay category" )
		.add_property( "border_panel", &Overlay::getBorderPanelOverlay, "The border panel overlay category" )
		.add_property( "text", &Overlay::getTextOverlay, "The text overlay category" )
		.add_property( "name", py::make_function( &Overlay::getName, py::return_value_policy< py::copy_const_reference >() ), &Overlay::setName, "The overlay name" )
		.add_property( "childs", py::range< OverlayPtrIntMapItFunc, OverlayPtrIntMapItFunc >( &Overlay::begin, &Overlay::end ) )
		.def( "childs_count", ovgetChildsCount )
		.def( "add_child", &Overlay::addChild )
		;
	//@}
	/**@group_name AnimatedSkeleton */
	//@{
	typedef AnimationPtrStrMapIt( AnimatedSkeleton::*AnimatedSkeletonAnimationsItFunc )( );
	py::class_< AnimatedSkeleton, boost::noncopyable >( "AnimatedSkeleton", py::no_init )
		.def( "start_all_animations", &AnimatedSkeleton::startAllAnimations )
		.def( "stop_all_animations", &AnimatedSkeleton::stopAllAnimations )
		.def( "pause_all_animations", &AnimatedSkeleton::pauseAllAnimations )
		.def( "start_animation", &AnimatedSkeleton::startAnimation )
		.def( "stop_animation", &AnimatedSkeleton::stopAnimation )
		.def( "pause_animation", &AnimatedSkeleton::stopAnimation )
		.def( "get_animation", py::make_function( &AnimatedSkeleton::getAnimation, py::return_value_policy< py::copy_non_const_reference >{} ) )
		;
	//@}
	/**@group_name AnimatedMesh */
	//@{
	typedef AnimationPtrStrMapIt( AnimatedMesh::*AnimatedMeshAnimationsItFunc )( );
	py::class_< AnimatedMesh, boost::noncopyable >( "AnimatedMesh", py::no_init )
		.def( "start_all_animations", &AnimatedMesh::startAllAnimations )
		.def( "stop_all_animations", &AnimatedMesh::stopAllAnimations )
		.def( "pause_all_animations", &AnimatedMesh::pauseAllAnimations )
		.def( "start_animation", &AnimatedMesh::startAnimation )
		.def( "stop_animation", &AnimatedMesh::stopAnimation )
		.def( "pause_animation", &AnimatedMesh::stopAnimation )
		.def( "get_animation", py::make_function( &AnimatedMesh::getAnimation, py::return_value_policy< py::copy_non_const_reference >{} ) )
		;
	//@}
	/**@group_name AnimatedObjectGroup */
	//@{
	typedef castor::StrSetIt( AnimatedObjectGroup::*AnimatedObjectGroupAnimationsItFunc )( );
	typedef AnimatedObjectPtrStrMapIt( AnimatedObjectGroup::*AnimatedObjectGroupObjectsItFunc )( );
	py::class_< AnimatedObjectGroup >( "AnimatedObjectGroup", py::no_init )
		.add_property( "scene", py::make_function( &AnimatedObjectGroup::getScene, py::return_value_policy< py::reference_existing_object >() ) )
		//.def( "objects", &AnimatedObjectGroup::getObjects )
		.def( "start_all_animations", &AnimatedObjectGroup::startAllAnimations )
		.def( "stop_all_animations", &AnimatedObjectGroup::stopAllAnimations )
		.def( "pause_all_animations", &AnimatedObjectGroup::pauseAllAnimations )
		.def( "start_animation", &AnimatedObjectGroup::startAnimation )
		.def( "stop_animation", &AnimatedObjectGroup::stopAnimation )
		.def( "pause_animation", &AnimatedObjectGroup::stopAnimation )
		.def( "add_animation", &AnimatedObjectGroup::addAnimation )
		.def( "set_animation_looped", &AnimatedObjectGroup::setAnimationLooped )
		;
	//@}
	/**@group_name Engine */
	//@{
	CACHE_GETTER( Engine, Scene );
	CACHE_GETTER( Engine, Plugin );
	CACHE_GETTER( Engine, Material );
	CACHE_GETTER( Engine, Overlay );
	CACHE_GETTER( Engine, RenderWindow );
	py::class_< Engine, boost::noncopyable >( "Engine", py::init< castor::String, bool >() )
		.def( "initialise", &Engine::initialise )
		.def( "cleanup", &Engine::cleanup )
		.def( "load_renderer", &Engine::loadRenderer )
		.def( "scenes", py::make_function( resgetSceneCache, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "plugins", py::make_function( resgetPluginCache, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "materials", py::make_function( resgetMaterialCache, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "overlays", py::make_function( resgetOverlayCache, py::return_value_policy< py::copy_non_const_reference >() ) )
		.def( "render_windows", py::make_function( resgetRenderWindowCache, py::return_value_policy< py::copy_non_const_reference >() ), "The RenderWindows cache" )
		;
	//@}
}
