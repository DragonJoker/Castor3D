# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

bl_info = {
	"name": "Castor3D CSCN format (.cscn)",
	"author": "Sylvain Doremus",
	"version ": (2016, 0, 9, 0),
	"blender": (2, 58, 0),
	"location": "File > Export",
	"description": "Exports scene into CSCN file format",
	"warning": "",
	"wiki_url": "",
	"tracker_url": "",
	"support": 'COMMUNITY',
	"category": "Import-Export"}

if "bpy" in locals():
	import imp
	if "export_cscn" in locals():
		imp.reload(export_cscn)

# ###############################
#                               #
# ######## EXPORTATION ######## #
#                               #
# ###############################
import os
import time
import bpy
import mathutils
import bpy_extras.io_utils
from mathutils import Color

map_ambient = 0
map_diffuse = 1
map_specular = 2
map_shininess = 3
map_normal = 4
map_height = 5
map_opacity = 6


class MyVertex:
	def __init__(self, index, position, normal, uv):
		# vertex index in the mesh
		self.meshIndex = index
		self.meshIndex = index
		# vertex position
		self.position = position
		# vertex normal
		self.normal = normal
		# vertex UV
		self.uv = uv


class MyFace:
	def __init__(self, idx0, idx1, idx2):
		self.index0 = idx0
		self.index1 = idx1
		self.index2 = idx2


def name_compat(name):
	if name is None:
		return 'None'


def mesh_triangulate(mesh):
	import bmesh
	bm = bmesh.new()
	bm.from_mesh(mesh)
	bmesh.ops.triangulate(bm, faces=bm.faces)
	bm.to_mesh(mesh)
	bm.free()


def write_texture(channel, filepath, file):
	fw = file.write
	fw('\n\t\ttexture_unit\n')
	fw('\t\t{\n')
	fw('\t\t\timage "%s"\n' % filepath)  # Diffuse mapping image
	fw('\t\t\tcolour 1.0 1.0 1.0 1.0\n')
	if channel == map_ambient:
		fw('\t\t\tchannel ambient\n')
	elif channel == map_diffuse:
		fw('\t\t\tchannel diffuse\n')
	elif channel == map_specular:
		fw('\t\t\tchannel specular\n')
	elif channel == map_shininess:
		fw('\t\t\tchannel gloss\n')
	elif channel == map_normal:
		fw('\t\t\tchannel normal\n')
	elif channel == map_height:
		fw('\t\t\tchannel height\n')
	elif channel == map_opacity:
		fw('\t\t\tchannel opacity\n')
	fw('\t\t}\n')


def write_material(material, file, world_amb, src_dir, dst_dir, copy_set):
	fw = file.write
	fw('\nmaterial "%s"\n' % material.name)  # Define a new material: matname_imgname
	fw('{\n')
	fw('\tpass\n')
	fw('\t{\n')
	fw('\t\tambient %.6f %.6f %.6f\n' % (material.ambient * world_amb)[:])  # Ambient, uses mirror color,
	fw('\t\tdiffuse %.6f %.6f %.6f\n' % (material.diffuse_intensity * material.diffuse_color)[:])  # Diffuse
	fw('\t\tspecular %.6f %.6f %.6f\n' % (material.specular_intensity * material.specular_color)[:])  # Specular
	# convert from blenders spec (0-511) to 0 - 128 range.
	fw('\t\tshininess %.6f\n' % (material.specular_hardness * 0.250489236))
	'''
	if material.alpha < 1.0:
		fw('\t\talpha %.6f\n' % material.alpha)  # Alpha
		fw('\t\tblend_func src_alpha inv_src_alpha\n')
		fw('\t\ttwo_sided true\n')
	'''
	# Write images!
	image_map = {}
	# backwards so topmost are highest priority
	for mtex in reversed(material.texture_slots):
		if mtex and mtex.texture and mtex.texture.type == 'IMAGE':
			image = mtex.texture.image
			if image:
				# texface overrides others
				if (mtex.use_map_color_diffuse and (mtex.use_map_warp is False) and (mtex.texture_coords != 'REFLECTION')):
					image_map[map_diffuse] = image
				if mtex.use_map_ambient:
					image_map[map_ambient] = image
				if mtex.use_map_color_spec or mtex.use_map_specular:  # specular color
					image_map[map_specular] = image
				if mtex.use_map_hardness:  # specular hardness/glossiness
					image_map[map_shininess] = image
				if mtex.use_map_alpha or mtex.use_map_translucency:
					image_map[map_opacity] = image
				if mtex.use_map_normal and (mtex.texture.use_normal_map is True):
					image_map[map_normal] = image
				if mtex.use_map_normal and (mtex.texture.use_normal_map is False):
					image_map[map_height] = image

	for key, image in image_map.items():
		file_path = bpy_extras.io_utils.path_reference(image.filepath, src_dir, dst_dir, 'AUTO', "", copy_set, image.library)
		write_texture(key, file_path, file)

	fw('\t}\n')
	fw('}\n')


def write_materials(scene, filepath, copy_set, materials):
	world = scene.world
	if world:
		world_amb = world.ambient_color
	else:
		world_amb = Color((0.0, 0.0, 0.0))
	src_dir = os.path.dirname(bpy.data.filepath)
	dst_dir = os.path.dirname(filepath)
	file = open(filepath, "w", encoding="utf8", newline="\n")
	fw = file.write
	fw('//////////////////////////////////////////////\n')
	fw('// Blender CMTL File: %r\n' % (os.path.basename(bpy.data.filepath) or "None"))
	fw('// Material Count: %i\n' % len(materials))
	fw('//////////////////////////////////////////////\n\n')
	# Write material/image combinations we have used.
	for material in materials:
		# Get the Blender data for the material and the image.
		# Having an image named None will make a bug, dont do it :)
		if material:
			write_material(material, file, world_amb, src_dir, dst_dir, copy_set)

	file.close()


def write_cameras(cameras,file):
	fw = file.write
	for cam_obj,cam_mtx in cameras:
		camera = cam_obj.data
		fw('\n\tcamera_node "%s"\n' % camera.name)
		fw('\t{\n')
		fw('\t\tposition %.6f %.6f %.6f\n' % cam_obj.location[:])
		fw('\t\tscale %.6f %.6f %.6f\n' % cam_obj.scale[:])
		fw('\t\torientation %.6f %.6f %.6f %.6f\n' % cam_obj.rotation_quaternion[:])
		fw('\t}\n')
		fw('\tcamera "%s"\n' % camera.name)
		fw('\t{\n')
		fw('\t\tparent "%s"\n' % camera.name)
		fw('\t\tviewport\n')
		fw('\t\t{\n')
		fw('\t\t\tnear %.6f\n' % camera.clip_start)
		fw('\t\t\tfar %.6f\n' % camera.clip_end)
		if camera.type == 'PERSP':
			fw('\t\t\tfov_y %.6f\n' % camera.angle_y)

		fw('\t\t}\n')
		fw('\t}\n')


def write_lights(lights,file):
	fw = file.write
	for lgt_obj,lgt_mtx in lights:
		light = lgt_obj.data
		fw('\n\tscene_node "%s"\n' % light.name)
		fw('\t{\n')
		location = [lgt_obj.location[0], lgt_obj.location[1], lgt_obj.location[2]]
		if light.type == 'SUN':
			location[0] = -location[0]
			location[2] = -location[2]
		else:
			location[1] = -location[1]
		fw('\t\tposition')
		fw(' %.6f'   % location[0])
		fw(' %.6f'   % location[1])
		fw(' %.6f\n' % location[2])
		if light.type != 'SUN':
			fw('\t\tscale %.6f %.6f %.6f\n' % lgt_obj.scale[:])
			fw('\t\torientation %.6f %.6f %.6f %.6f\n' % lgt_obj.rotation_quaternion[:])
		fw('\t}\n')
		fw('\tlight "%s"\n' % light.name)
		fw('\t{\n')
		fw('\t\tparent "%s"\n' % light.name)
		if light.type == 'SUN':
			fw('\t\ttype directional\n')
		elif light.type == 'POINT' or light.type == 'HEMI':
			fw('\t\ttype point_light\n')
		elif light.type == 'SPOT' or light.type == 'AREA':
			fw('\t\ttype spot_light\n')
		fw('\t\tambient %.6f %.6f %.6f 1.0\n' % light.color[:])
		fw('\t\tdiffuse %.6f %.6f %.6f 1.0\n' % light.color[:])
		fw('\t\tspecular %.6f %.6f %.6f 1.0\n' % light.color[:])
		'''
		if light.type == 'POINT':
		elif light.type == 'HEMI':            
		elif light.type == 'SPOT':
		elif light.type == 'AREA':
		'''
		fw('\t}\n')


def write_meshes(scene, filepath, meshes, file, EXPORT_NORMALS, EXPORT_UV, EXPORT_MTL, OBJECTS_AS_SUBMESHES):
	from collections import defaultdict
	fw = file.write
	# A Dict of Materials
	# (material.name, image.name):matname_imagename
	mtl_dict = {}
	material_names = {}
	material_list = []
	
	mtx = mathutils.Matrix()

	def roundVec3(v):
		return round(v.x, 6), round(v.y, 6), round(v.z, 6)

	def roundVec2(v):
		return round(v[0], 6), round(v[1], 6)

	if OBJECTS_AS_SUBMESHES:
		fw('\n\tscene_node "%s"\n' % scene.name)
		fw('\t{\n')
		fw('\t}\n')
		fw('\tobject "%s"\n' % scene.name)
		fw('\t{\n')
		fw('\t\tparent "%s"\n' % scene.name)
		fw('\t\tmesh "%s"\n' % scene.name)
		fw('\t\t{\n')
		fw('\t\t\ttype custom\n')
	for mesh_obj,mesh_mtx in meshes:
		#mesh = mesh_obj.data
		mesh = mesh_obj.to_mesh(scene, False, 'PREVIEW', calc_tessface=False)
		mesh.transform(mtx * mesh_mtx)
		mesh_triangulate(mesh)
		uv_layer = mesh.uv_layers.active.data
		if EXPORT_NORMALS:
			mesh.calc_normals()
		if EXPORT_UV:
			faceuv = len(mesh.uv_textures) > 0
			if faceuv:
				uv_texture = mesh.uv_textures.active.data[:]
				uv_layer = mesh.uv_layers.active.data[:]
		else:
			faceuv = False
		if not OBJECTS_AS_SUBMESHES:
			fw('\tscene_node "%s"\n' % mesh.name)
			fw('\t{\n')
			fw('\t\tposition %.6f %.6f %.6f\n' % mesh_obj.location[:])
			fw('\t\tscale %.6f %.6f %.6f\n' % mesh_obj.scale[:])
			fw('\t\torientation %.6f %.6f %.6f %.6f\n' % mesh_obj.rotation_quaternion[:])
			fw('\t}\n')
			fw('\tobject "%s"\n' % mesh.name)
			fw('\t{\n')
			fw('\t\tparent "%s"\n' % mesh.name)
			fw('\t\tmesh "%s"\n' % mesh.name)
			fw('\t\t{\n')
			fw('\t\t\ttype custom\n')
		polygons = defaultdict(list)
		for polygon in mesh.polygons:
			polygons[polygon.material_index].append(polygon)

		for material_id, polygon_list in polygons.items():
			material = mesh.materials[material_id]
			if not material_names.get(material.name):
				material_names[material.name] = material
				material_list.append(material)
			vertex_list = []
			vcount = 0
			if OBJECTS_AS_SUBMESHES:
				fw('\n\t\t\tsubmesh "%s"\n' % mesh.name)
			else:
				fw('\n\t\t\tsubmesh\n')
			fw('\t\t\t{\n')
			iVertexIdx0 = 0
			iVertexIdx1 = 0
			iVertexIdx2 = 0
			face_list = []
			vertex_indx = defaultdict(list)
			iVertexIdx = 0
			for polygon in polygon_list:
				# Worst but working solution : we duplicate each vertex used by the face
				# First vertex
				fw('\t\t\t\tvertex %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[0]].co[:])
				if EXPORT_NORMALS:
					fw('\t\t\t\tnormal %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[0]].normal[:])
				if EXPORT_UV:
					uv=uv_layer[polygon.loop_indices[0]].uv
					uvCoord=uv[0], uv[1]
					fw('\t\t\t\tuv %.6f %.6f\n'   % uvCoord[:])
				# Second vertex
				fw('\t\t\t\tvertex %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[1]].co[:])
				if EXPORT_NORMALS:
					fw('\t\t\t\tnormal %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[1]].normal[:])
				if EXPORT_UV:
					uv=uv_layer[polygon.loop_indices[1]].uv
					uvCoord=uv[0], uv[1]
					fw('\t\t\t\tuv %.6f %.6f\n'   % uvCoord[:])
				# Third vertex
				fw('\t\t\t\tvertex %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[2]].co[:])
				if EXPORT_NORMALS:
					fw('\t\t\t\tnormal %.6f %.6f %.6f\n' % mesh.vertices[polygon.vertices[2]].normal[:])
				if EXPORT_UV:
					uv=uv_layer[polygon.loop_indices[2]].uv
					uvCoord=uv[0], uv[1]
					fw('\t\t\t\tuv %.6f %.6f\n'   % uvCoord[:])
				fw('\t\t\t\tface')
				fw(' %d'   % iVertexIdx)
				iVertexIdx += 1
				fw(' %d'   % iVertexIdx)
				iVertexIdx += 1
				fw(' %d\n' % iVertexIdx)
				iVertexIdx += 1

			if material:
				fw('\t\t\t\tmaterial "%s"\n' % material.name)
			fw('\t\t\t}\n')

		if not OBJECTS_AS_SUBMESHES:
			fw('\t\t}\n')
			fw('\t}\n')
		bpy.data.meshes.remove(mesh)

	if OBJECTS_AS_SUBMESHES:
		fw('\t\t}\n')
		fw('\t}\n')
	if EXPORT_MTL:
		copy_set = set()
		write_materials(scene, filepath, copy_set, material_list)
		# copy all collected files.
		bpy_extras.io_utils.path_reference_copy(copy_set)


def write_file(filepath, scene, meshes, lights, cameras,
			   BACKGROUND_COLOUR, RESOLUTION,
			   EXPORT_NORMALS, EXPORT_UV, EXPORT_MTL,
			   OBJECTS_AS_SUBMESHES,
			   ):
	#Basic write function. The context and options must be already set
	#This can be accessed externaly
	#eg.
	#write( 'c:\\test\\foobar.cscn', Blender.Object.GetSelected() ) # Using default options.

	mtxGlobal = mathutils.Matrix()

	def veckey3d(v):
		return round(v.x, 6), round(v.y, 6), round(v.z, 6)

	def veckey2d(v):
		return round(v[0], 6), round(v[1], 6)

	print('CSCN Export path: %r' % filepath)

	time1 = time.time()

	file = open(filepath, "w", encoding="utf8", newline="\n")
	fw = file.write
	world = scene.world
	if world:
		world_amb = world.ambient_color
	else:
		world_amb = Color((0.0, 0.0, 0.0))

	# Write Header
	fw('//////////////////////////////////////////////\n')
	fw('// Blender v%s CSCN File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
	fw('// www.blender.org\n')
	fw('//////////////////////////////////////////////\n\n')

	# Tell the cscn file what material file to use.
	mtlfilepath = os.path.splitext(filepath)[0] + ".cmtl"
	fw('mtl_file "%s"\n\n' % repr(os.path.basename(mtlfilepath))[1:-1])  # filepath can contain non utf8 chars, use repr

	fw('scene "Blender Scene"\n')
	fw('{\n')
	fw('\tbackground_colour %.6f %.6f %.6f 1.0\n' % BACKGROUND_COLOUR[:]);
	fw('\tambient_light %.6f %.6f %.6f 1.0\n' % world_amb[:]);

	write_cameras(cameras, file)
	write_lights(lights, file)
	write_meshes(scene, mtlfilepath, meshes, file, EXPORT_NORMALS, EXPORT_UV, EXPORT_MTL, OBJECTS_AS_SUBMESHES)

	fw('}\n')

	fw('\nwindow "Window"\n')
	fw('{\n')
	fw('\trender_target\n')
	fw('\t{\n')
	fw('\t\tformat	argb32\n')
	fw('\t\tdepth	depth24s8\n')
	fw('\t\tsize	%d %d\n' % RESOLUTION[:])
	fw('\t\tscene	"Blender Scene"\n')
	fw('\t}\n')
	fw('\tvsync		true\n')
	fw('}\n')
 
	file.close()

	print("CSCN Export time: %.2f" % (time.time() - time1))

'''
Currently the exporter lacks these features:
* multiple scene export (only active scene is written)
* particles
* animations
'''
def save(filepath, context,
		 background_colour, resolution,
		 export_normals, export_uvs, export_materials,
		 objects_as_submeshes
		 ):
	base_name, ext = os.path.splitext(filepath)
	context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension
	scene = context.scene

	# Exit edit mode before exporting, so current object states are exported properly.
	if bpy.ops.object.mode_set.poll():
		bpy.ops.object.mode_set(mode='OBJECT')

	meshes = []
	cameras = []
	lights = []

	for obj in scene.objects:
		# ignore dupli children
		if obj.parent and obj.parent.dupli_type in {'VERTS', 'FACES'}:
			print(obj.name, 'is a dupli child - ignoring')
		else:
			if obj.dupli_type != 'NONE':
				print('creating dupli_list on', obj.name)
				obj.dupli_list_create(scene)
				for dob in obj.dupli_list:
					if obj.type == 'CAMERA':
						cameras.append([dob.object, dob.matrix])
					elif obj.type == 'LAMP':
						lights.append([dob.object, dob.matrix])
					elif obj.type == 'MESH':
						meshes.append([dob.object, dob.matrix])
				print(obj.name, 'has', len(obj.dupli_list), 'dupli children')
			else:
				print('no dupli_list on', obj.name)
				if obj.type == 'CAMERA':
					cameras.append([obj, obj.matrix_world])
				elif obj.type == 'LAMP':
					lights.append([obj, obj.matrix_world])
				elif obj.type == 'MESH':
					meshes.append([obj, obj.matrix_world])

	full_path = ''.join(context_name)

	# EXPORT THE FILE.
	write_file(full_path, scene, meshes, lights, cameras,
			   background_colour, resolution,
			   export_normals, export_uvs, export_materials,
			   objects_as_submeshes,
			   )

	# Restore old active scene.
#   orig_scene.makeCurrent()
#   Window.WaitCursor(0)

# ###############################
#                               #
# ######## REGISTERING ######## #
#                               #
# ###############################

from bpy.props import BoolProperty, FloatProperty, StringProperty, EnumProperty

# Save a Castor3D CSCN File
class ExportCSCN(bpy.types.Operator):
	bl_idname = "export_scene.cscn"
	bl_label = "Export CSCN scene"
	bl_description= "Import a Castor3D scene file"
	bl_options = {'PRESET', 'UNDO'}

	filepath                = bpy.props.StringProperty(         name="File Path",               description="Filepath used for exporting the CSCN file",    default="unnamed.cscn",     maxlen=1024 )
	objects_as_submeshes    = bpy.props.BoolProperty(           name="Objects as Submeshes",    description="Export Blender objects as CSCN Submeshes",     default=True)
	resolution              = bpy.props.IntVectorProperty(      name="Resolution",              description="Scene render window resolution",               default=(800,600),          size=2 )
	background_colour       = bpy.props.FloatVectorProperty(    name="Background color",        description="Scene background colour",                      default=(0.5, 0.5, 0.5),    size=3 )
	export_normals          = bpy.props.BoolProperty(           name="Include Normals",         description="Export normals too",                           default=True )
	export_uvs              = bpy.props.BoolProperty(           name="Include UVs",             description="Write out the active UV coordinates",          default=True )
	export_materials        = bpy.props.BoolProperty(           name="Write Materials",         description="Write out the MTL file",                       default=True )

	def execute(self, context):
		save(self.filepath, context,
			 self.background_colour,
			 self.resolution,
			 self.export_normals,
			 self.export_uvs,
			 self.export_materials,
			 self.objects_as_submeshes)
		return {'FINISHED'}

	def invoke(self, context, event):
		wm= context.window_manager
		wm.fileselect_add( self )
		return {'RUNNING_MODAL'}


def menu_func_export(self, context):
	self.layout.operator(ExportCSCN.bl_idname, text="Castor3D Scene (.cscn)")


def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
	register()
