# CSCN File Format

CSCN files are easily editable text files (since the syntax is understood).

The data types used in scene files are these ones :
- *boolean* : a boolean (*true* or *false*).
- *int* : a simple integer.
- *mask_1* : an hexadecimal integer defining a mask for one component from a colour (*0x000000FF*, for instance).
- *mask_3* : an hexadecimal integer defining a mask for three components from a colour  (*0x00FFFFFF*, for instance).
- *real* : a floating point number, using dot ( . ) as the decimals separator.
- *2, 3, 4 ints* : 2, 3 or 4 integers, separated by commas ( , ) or spaces ( ).
- *2, 3, 4 reals* : 2, 3 or 4 floating point numbers, separated by commas ( , ) or spaces ( ).
- *size* : 2 integers greater than or equal to 0.
- *2x2, 3x3, 4x4 reals matrix* : 2, 3 or 4 groups separated by semi colons ( ; ) of 2, 3 or 4 floating point numbers separated by commas ( *,* ) or spaces ( ).
- *rgb_colour* : the RGB components of colour, expressed in floating point numbers between 0.0 and 1.0.
- *rgba_colour* : the RGBA components of colour, expressed in floating point numbers between 0.0 and 1.0.
- *rgb_hdr_colour* : the RGB components of colour, expressed in floating point numbers greater than or equal to 0.0.
- *rgba_hdr_colour* : the RGBA components of colour, expressed in floating point numbers greater than or equal to 0.0.
- *value* : a string corresponding to a predefined value.
- *name* : a string, wrapped into double quotes ( &quot; ).
- *file* : a string, wrapped into double quotes ( &quot; ), containing a file name and path.
- *folder* : a string, wrapped into double quotes ( &quot; ), containing a folder path.

## Description

The file is split into sections, defined as follows :

    [section-type] "[section-name]"
    {
        // Section description
    }

Example:

    light "Light0"
    {
        type directional
        colour 1.0 1.0 1.0
        intensity 0.8 1.0
    }

Some sections can have child subsections :

    material "Bronze"
    {
        pass
        {
            ambient 0.2125 0.1275 0.054
            diffuse 0.714 0.4284 0.12144
            emissive 0.0
            specular 0.393548 0.271906 0.166721
            shininess 25.6
        }
    }

## Root section

- **debug_overlays** : *boolean*  
  Enables or disables debug overlays.
- **materials** : *value*  
  Defines the material type used in the whole file. The possible values are :
  - *phong* : Phong materials.
  - *blinn_phong* : Blinn-Phong materials.
  - *pbr* : PBR materials.
- **default_unit** : *value*  
  Defines the unit length used by the engine. The possible values are :
  - *mm* : Millimetres.
  - *cm* : Centimetres.
  - *m* : Metres.
  - *km* : Kilometres.
  - *yd* : Yards.
  - *ft* : Feet.
  - *in* : Inches.
- **max_image_size**: *int*
  Allows limitation of the loaded images (keeping their aspect ratio).
- **sampler** : *section*  
  Defines a texture sampler object.
- **material** : *section*  
  Defines a material.
- **loading_screen** : *section*  
  Redefines the loading screen.
- **font** : *section*  
  Defines a font used in text overlays.
- **window** : *section*  
  Defines a render window.
- **panel_overlay** : *section*  
  Defines a simple panel overlay.
- **border_panel_overlay** : *section*  
  Defines a panel overlay with a border.
- **text_overlay** : *section*  
  Defines a panel overlay with a text.
- **scene** : *section*  
  Defines a whole scene.

## sampler section

- **min_filter** : *value*  
  Value used for minification function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **mag_filter** : *value*  
  Value used for magnification function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **mip_filter** : *value*  
  Value used for mipmapping function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **min_lod** : *real*  
  Defines minimum level of detail value.
- **max_lod** : *real*  
  Defines maximum level of detail value.
- **lod_bias** : *real*  
  Defines the base MIP-Level.
- **u_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the U component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **v_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the V component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **w_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the W component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **border_colour** : *rgba_colour*  
  Defines the non textured border colour.
  - *float_transparent_black* : Transparent black.
  - *int_transparent_black* : Transparent black.
  - *float_opaque_black* : Opaque black.
  - *int_opaque_black* : Opaque black.
  - *float_opaque_white* : Opaque white.
  - *int_opaque_white* : Opaque white.
- **anisotropic_filtering** : *booléen*  
  Defines if anisotropic filtering is enabled (if supported).
- **max_anisotropy** : *real*  
  Defines the maximum degree of anisotropy.
- **comparison_mode** : *value*  
  Defines the sampler comparison mode. The possible values are :
  - *none* : Traditional sampler.
  - *ref_to_texture* : Shadow sampler.
- **comparison_func** : *value*  
  Defines the sampler comparison function. The possible values are :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its alpha component is less than the second parameter.
  - *less_or_equal* : The sample colour is applied if its alpha component is less than or equal to the second parameter.
  - *equal* : The sample colour is applied if its alpha component is equal to the second parameter.
  - *not_equal* : The sample colour is applied if its alpha component is different from the second parameter.
  - *greater_or_equal* : The sample colour is applied if its alpha component is greater than or equal to the second parameter.
  - *greater* : The sample colour is applied if its alpha component is greater than the second parameter.
  - *never* : The sample colour is never applied.

## material section

Materials can be multi-pass, so you can declare more than one pass subsection.
- **pass** : *section*  
  Defines a new section describing a texture.
- **render_pass** : *name*  
  Defines the name of the render pass used to render objects using this material.

### pass section

- **colour_srgb** : *rgb_colour*  
  Defines the pass colour.
- **colour_hdr** : *rgb_hdr_colour*  
  Defines the pass colour.
- **specular** : *rgb_colour*  
  Defines specular colour.
- **metalness** : *real (between 0 and 1)*  
  Defines the metalness.
- **shininess** : *real (between 0 and 128)*  
  Defines the specular exponent.
- **glossiness** : *real (between 0 and 1)*  
  Defines the glossiness.
- **roughness** : *real (between 0 and 1)*  
  Defines the roughness.
- **emissive_colour** : *rgb_colour*  
  Defines emissive colour.
- **emissive_factor** : *real*  
  Defines emissive factor.
- **attenuation_colour** : *rgb_colour*  
  Defines the attenuation colour.
- **attenuation_distance** : *real*  
  Defines the attenuation distance.
- **thickness_factor** : *real*  
  Defines the thickness factor.
- **transmission** : *real*  
  Defines the transmission factor.
- **clearcoat_factor** : *real*  
  Defines the clearcoat factor.
- **clearcoat_roughness_factor** : *real*  
  Defines the clearcoat roughness.
- **iridescence_factor** : *real*  
  Defines the iridescence factor.
- **iridescence_ior** : *real*  
  Defines the iridescence layer index of refraction.
- **iridescence_min_thickness** : *real*  
  Defines the iridescence layer minimum thickness.
- **iridescence_max_thickness** : *real*  
  Defines the iridescence layer minimum thickness.
- **sheen_colour** : *real*  
  Defines the sheen layer colour.
- **sheen_roughness** : *real*  
  Defines the sheen layer roughness.
- **opacity** : *real (between 0 and 1)*  
  Defines the opacity factor.
- **two_sided** : *boolean*  
  Tells the pass is two sided (default is false).
- **reflections** : *boolean*  
  Enables reflections (default is false).  
- **fractal** : *boolean*  
  Enables fractal UV mapping (default is false).  
- **untile** : *boolean*  
  Enables untiled UV mapping (default is false).  
- **lighting** : *boolean*  
  Enables lighting (default is true).  
- **pickable** : *boolean*  
  Enables picking (default is true).  
- **texture_unit** : *section*  
  Defines a new section describing a texture unit.
- **alpha_blend_mode** : *value*  
  Alpha blending mode name, can be one of:
  - *none* : No alpha blending.
  - *additive* : Source and destination alpha values are added.
  - *multiplicative* : Source and destination alpha values are multiplied.
  - *interpolative* : Source and destination alpha values are interpolated.
- **colour_blend_mode** : *value*  
  Colour blending mode name, can be one of:
  - *none* : No colour blending.
  - *additive* : Source and destination colour values are added.
  - *multiplicative* : Source and destination colour values are multiplied.
  - *interpolative* : Source and destination colour values are interpolated.
- **alpha_func** : func : *value* ref-val: *real*  
  Defines the way alpha rejection is applied to the pass. The second parameter is the reference value used in alpha rejection function. The first parameter values can be :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its alpha component is less than the second parameter.
  - *less_or_equal* : The sample colour is applied if its alpha component is less than or equal to the second parameter.
  - *equal* : The sample colour is applied if its alpha component is equal to the second parameter.
  - *not_equal* : The sample colour is applied if its alpha component is different from the second parameter.
  - *greater_or_equal* : The sample colour is applied if its alpha component is greater than or equal to the second parameter.
  - *greater* : The sample colour is applied if its alpha component is greater than the second parameter.
  - *never* : The sample colour is never applied.
- **blend_alpha_func** : func : *value* ref-val: *real*  
  Defines the way alpha blending is applied to the pass. The second parameter is the reference value used in alpha rejection function. The first parameter values can be :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its alpha component is less than the second parameter.
  - *less_or_equal* : The sample colour is applied if its alpha component is less than or equal to the second parameter.
  - *equal* : The sample colour is applied if its alpha component is equal to the second parameter.
  - *not_equal* : The sample colour is applied if its alpha component is different from the second parameter.
  - *greater_or_equal* : The sample colour is applied if its alpha component is greater than or equal to the second parameter.
  - *greater* : The sample colour is applied if its alpha component is greater than the second parameter.
  - *never* : The sample colour is never applied.
- **mixed_interpolation** : *section*  
  Helper that sets **alpha_blend_mode** to *interpolative*, **blend_alpha_func** to *less_or_equal* and **alpha_func** to *greater* with a ref. value to 0.95.
- **refraction_ratio** : *real*  
  Defines the refraction ratio. Note that if there is no refraction map, the refraction is still applied, using only the skybox.
- **subsurface_scattering** : *section*  
  Defines a new section describing subsurface scattering for the pass.
- **parallax_occlusion** : *value*  
  Enables or disables parallax occlusion mapping (needs a normal map and a height map), can be one of:
  - *none* : Disabled.
  - *one* : No tiling.
  - *repeat* : Repeated tiling.
- **bw_accumulation** : *int between 0 and 5*  
  Defines the accumulation function, for blended weighted rendering.

#### texture_unit section

- **image** : *file*  
  Defines the image file name.
- **render_target** : *section*  
  Defines a new section describing a render target.
- **channel** : *value {| value}*  
  The channels to which the texture is bound. Can be one of the following :
  - *diffuse* : Diffuse lighting colour.
  - *albedo* : Base colour.
  - *specular* : Specular colour.
  - *metallic* : Metallic factor.
  - *shininess* : Specular exponent.
  - *glossiness* : Glossiness factor.
  - *roughness* : Roughness factor.
  - *opacity* : Opacity factor.
  - *emissive* : Emissive colour.
  - *transmittance* : Transmittance factor (for subsurface scattering).
  - *occlusion* : Occlusion factor.
  - *normal* : Normals.
  - *height* : Height.
- **sampler** : *name*  
  Defines the sampler object used by the texture.
- **level_count** : *entier*
  Defines the maximum mip levels count.
- **diffuse_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **albedo_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **colour_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **specular_mask** : *mask_3*  
  Defines the components from the texture used for the specular colour.
- **metalness_mask** : *mask_1*  
  Defines the component from the texture used for the metallic factor
- **shininess_mask** : *mask_1*  
  Defines the component from the texture used for the specular exponent.
- **glossiness_mask** : *mask_1*  
  Defines the component from the texture used for the glossiness factor.
- **roughness_mask** : *mask_1*  
  Defines the component from the texture used for the roughness factor.
- **opacity_mask** : *mask_1*  
  Defines the component from the texture used for the opacity factor.
- **emissive_mask** : *mask_3*  
  Defines the components from the texture used for the emissive colour.
- **transmittance_mask** : *mask_1*  
  Defines the component from the texture used for the transmittance factor (for subsurface scattering).
- **occlusion_mask** : *mask_1*  
  Defines the component from the texture used for the occlusion factor.
- **normal_mask** : *mask_3*  
  Defines the components from the texture used for the normals.
- **normal_directx** : *booléen*  
  Tells if the texture normals are expressed for DirectX (green component will then be inverted).
- **height_mask** : *mask_1*  
  Defines the component from the texture used for the height.
- **height_factor** : *real*  
  The height factor multiplier.
- **thickness_mask** : *mask_1*  
  Defines the component from the texture used for the thickness factor.
- **transmission_mask** : *mask_1*  
  Defines the component from the texture used for the transmission factor.
- **clearcoat_normal_mask** : *mask_3*  
  Defines the component from the texture used for the clearcoat layer normal map.
- **clearcoat_mask** : *mask_1*  
  Defines the component from the texture used for the clearcoat factor.
- **clearcoat_roughness_mask** : *mask_1*  
  Defines the component from the texture used for the clearcoat layer roughness factor.
- **iridescence_mask** : *mask_1*  
  Defines the component from the texture used for the iridescence factor.
- **iridescence_thickness_mask** : *mask_1*  
  Defines the component from the texture used for the iridescence layer thickness factor.
- **sheen_mask** : *mask_3*  
  Defines the component from the texture used for the sheen layer colour.
- **sheen_roughness_mask** : *mask_1*  
  Defines the component from the texture used for the sheen layer roughness factor.
- **invert_y** : *boolean*  
  Defines if the image must be inverted along Y axis.
- **transform** : *section*  
  Defines the texture base transformation.
- **tileset** : *2 ints*  
  Defines the tile set dimensions of the texture.
- **tiles** : *int*  
  Defines the tile count of the texture.
- **animation** : *section*  
  Defines the texture transformation animation.
- **texcoord_set** : *section*  
  Defines the target texture coordinates set this texture will be using.

##### transform section

- **rotate** : *réel*  
  Defines the texture rotation.
- **translate** : *2 réels*  
  Defines the texture translation.
- **scale** : *2 réels*  
  Defines the texture scaling.
- **tile** : *2 entiers*  
  Defines the selected tile.

##### animation section

- **rotate** : *réel*  
  Defines the texture rotation animation speed.
- **translate** : *2 réels*  
  Defines the texture translation animation speed.
- **scale** : *2 réels*  
  Defines the texture scaling animation speed.
- **tile** : *2 entiers*  
  Defines if the tiles are animated.

#### shader_program section

- **vertex_program** : *section*  
  Defines a new section describing the vertex program.
- **pixel_program** : *section*  
  Defines a new section describing the pixel program.
- **geometry_program** : *section*  
  Defines a new section describing the geometry program.
- **hull_program** : *section*  
  Defines a new section describing the hull (tessellation control) program.
- **domain_program** : *section*  
  Defines a new section describing the domain (tessellation evaluation) program.
- **constants_buffer** : *section*  
  Defines a new section dexcribing a constants buffer (uniform buffer).

#### vertex/pixel/geometry/hull/domain_program section

- **file** : *file*  
  Shader file name
- **sampler** : *name*  
  Creates a new variable of sample (1D, 2D, …) type, for the pixel shader.
- **input_type** : *value*  
  Defines the input faces data type, for geometry shader. Can be one of the following :
  - *points* : Points.
  - *lines* : Disjoint lines.
  - *line_loop* : Joint lines loop.
  - *line_strip* : Joint lines.
  - *triangles* : Disjoint triangles.
  - *triangle_strip* : Joint triangles.
  - *triangle_fan* : Triangles joint using the first point.
  - *quads* : Disjoint quads.
  - *quad_strip* : Joint quads.
  - *polygon* : Polygons.
- **output_type** : *value*  
  Defines the geometry chader output data type. Can be one of the following :
  - *points* : Points.
  - *line_strip* : Joint lines.
  - *triangle_strip* : Joint triangles.
  - *quad_strip* : Joint quads.
- **output_vtx_count** : *int*  
  Defines the geometry shader output vertices.
- **constants_buffer** : *section*  
  Defines a new section describing a constants buffer.

#### constants_buffer section

- **shaders** : *bitwise ORed values*  
  Shader types to which this buffer applies, can be one of:
  - *vertex*
  - *hull*
  - *domain*
  - *geometry*
  - *pixel*
  - *compute*
- **variable** : *name*, *section*  
  Defines a new section describing a variable for this buffer.

#### variable section

- **type** : *value*  
  Variable type name, can be :
  - *int* : 1 signed integer.
  - *uint* : 1 unsigned integer.
  - *float* : 1 simple precision floating point number.
  - *double* : 1 double precision floating point number.
  - *vec2i* : 2 signed integers.
  - *vec3i* : 3 signed integers.
  - *vec4i* : 4 signed integers.
  - *vec2f* : 2 simple precision floating point numbers.
  - *vec3f* : 3 simple precision floating point numbers.
  - *vec4f* : 4 simple precision floating point numbers.
  - *vec2d* : 2 double precision floating point numbers.
  - *vec3d* : 3 double precision floating point numbers.
  - *vec4d* : 4 double precision floating point numbers.
  - *mat2x2i* : 2x2 signed integers matrix.
  - *mat2x3i* : 2x3 signed integers matrix.
  - *mat2x4i* : 2x4 signed integers matrix.
  - *mat3x2i* : 3x2 signed integers matrix.
  - *mat3x3i* : 3x3 signed integers matrix.
  - *mat3x4i* : 3x4 signed integers matrix.
  - *mat4x2i* : 4x2 signed integers matrix.
  - *mat4x3i* : 4x3 signed integers matrix.
  - *mat4x4i* : 4x4 signed integers matrix.
  - *mat2x2f* : 2x2 simple precision floating point numbers matrix.
  - *mat2x3f* : 2x3 simple precision floating point numbers matrix.
  - *mat2x4f* : 2x4 simple precision floating point numbers matrix.
  - *mat3x2f* : 3x2 simple precision floating point numbers matrix.
  - *mat3x3f* : 3x3 simple precision floating point numbers matrix.
  - *mat3x4f* : 3x4 simple precision floating point numbers matrix.
  - *mat4x2f* : 4x2 simple precision floating point numbers matrix.
  - *mat4x3f* : 4x3 simple precision floating point numbers matrix.
  - *mat4x4f* : 4x4 simple precision floating point numbers matrix.
  - *mat2x2d* : 2x2 double precision floating point numbers matrix.
  - *mat2x3d* : 2x3 double precision floating point numbers matrix.
  - *mat2x4d* : 2x4 double precision floating point numbers matrix.
  - *mat3x2d* : 3x2 double precision floating point numbers matrix.
  - *mat3x3d* : 3x3 double precision floating point numbers matrix.
  - *mat3x4d* : 3x4 double precision floating point numbers matrix.
  - *mat4x2d* : 4x2 double precision floating point numbers matrix.
  - *mat4x3d* : 4x3 double precision floating point numbers matrix.
  - *mat4x4d* : 4x4 double precision floating point numbers matrix.
- **count** : *int*  
  Variable occurences count (array size).
- **value** :  
  Variable value, depends on the chosen type.

#### subsurface_scattering section

- **strength** : *real*  
  Defines the strength of the effect.
- **gaussian_width** : *real*  
  Defines the width of the Gaussian blur.
- **transmittance_profile** : *section*  
  Defines a new section describing the transmittance profile.

#### transmittance_profile section

- **factor** : *vec4f*  
  Defines the three RGB components of the colour, and the fourth component is used for the exponent of that colour.

## font section

- **file** : *file*  
  Defines the file holding the font.
- **height** : *int*  
  Defines the height (precision) of the font.

## scene section

- **ambient_light** : *colour*  
  Defines the ambient lighting colour. For PBR materials, defines the influence of the IBL on the scene.
- **background_colour** : *colour*  
  Defines the background colour.
- **background_image** : *file*  
  Defines the background image.
- **import** : *section*  
  Allows scene import from a CSCN file or another file format supported by Castor3D importer plug-ins.
- **scene_node** : *section*  
  Defines a new section describing a scene node for objects, lights or billboards.
- **camera_node** : *section*  
  Defines a new section describing a scene node for cameras.
- **light** : *section*  
  Defines a new section describing a light source.
- **object** : *section*  
  Defines a new section describing an object.
- **billboard** : *section*  
  Defines a new section describing a billboards list.
- **camera** : *section*  
  Defines a new section describing a camera.
- **panel_overlay** : *section*  
  Defines a new section describing a simple panel overlay.
- **border_panel_overlay** : *section*  
  Defines a new section describing a simple panel overlay with a border.
- **text_overlay** : *section*  
  Defines a new section describing a simple panel overlay with text.
- **animated_object_group** : *section*  
  Defines a new section describing an animated object group, with common animations.
- **skeleton** : *section*  
  Defines a new section describing a skeleton, that can be used with one or more meshes.
- **mesh** : *section*  
  Defines a new section describing a mesh, that can be used with one or more objects.
- **particle_system** : *section*  
  Defines a new section describing a particle system.
- **skybox** : *section*  
  Defines a new section describing the skybox.
- **include** : *file*  
  Includes a scene file, allowing you to split your scene in multiple files.
- **sampler** : *section*  
  Defines a new section describing a sampler.
- **fog_type** : *value*  
  Defines the fog type for the scene. Possible values are:
  - *linear*: Fog intensity increases linearly with distance to camera.
  - *exponential*: Fog intensity increases exponentially with distance to camera.
  - *squared_exponential*: Fog intensity increases even more with distance to camera.
- **fog_density** : *real*  
  Defines the fog density, which is multiplied by the distance, according to chosen fog type.

### import section

- **file** : *path*  
  Imports the scene from the given file name.
- **file_anim** : *path*  
  Imports animations from the given file name.
- **prefix** : *text*  
  Defines the imported objects' name prefix, used to prevent names clash.
- **rescale** : *real*  
  Rescales the imported objects by given factor, on three axes.
- **pitch** : *réel*  
  Rotates the imported objects by given angle (in degrees) along X axis.
- **yaw** : *réel*  
  Rotates the imported objects by given angle (in degrees) along Y axis.
- **roll** : *réel*  
  Rotates the imported objects by given angle (in degrees) along Z axis.

### scene_node and camera_node sections

- **parent** : *name*  
  Defines this node’s parent. The default parent node is the root node.
- **position** : *3 reals*  
  Node position relative to its parent.
- **orientation** : *4 reals*  
  A quaternion holding node orientation relative to its parent.
- **direction** : *3 reals*  
  Holds node direction relative to its parent.
- **scale** : *3 reals*  
  Node scale relative to its parent.

### light section

- **type** : *value*  
  Three light source types exist in Castor3D :
  - *directional* : directional light (like the sun).
  - *point_light* : a positioned source which emits in all directions.
  - *spot_light* : a positioned source which emits in an oriented cone.
- **colour** : *rgb_colour*  
  Defines the colour for this source.
- **intensity** : *2 reals*  
  Defines the diffuse and specular intensities for this source.
- **attenuation** : *3 reals*  
  Defines the three attenuation components : constant, linear and quadratic. This attenuation is computed from the distance to the light source.  
  Only for spot_light and point_light.
- **inner_cut_off** : *real*  
  Defines the inner angle of the emission cone.  
  Only for spot_light.
- **outer_cut_off** : *real*  
  Defines the outer angle of the emission cone.  
  Only for spot_light.
- **exponent** : *real*  
  Defines the attenuation computed with the distance from the emission cone centre.  
  Only for spot_light.
- **parent** : *name*  
  Defines the node which this light source is attached to.
- **shadows** : *section*  
  Defines a new section describing the shadows for the light source.

#### shadows section

- **producer** : *booléen*  
  Tells if the light source produces shadows (*true*) or not (*false*).
- **filter** : *valeur*  
  Defines the type of shadows to use. Can be one of the following:
  - *raw* : No filter.
  - *pcf* : PCF filter.
  - *variance* : Variance Shadow Map.
- **min_offset** : *real*  
  Defines the minimal offset to apply to the shadow value.
- **max_slope_offset** : *real*  
  Defines the mawimal offset, slope wise, to apply to the shadow value.
- **variance_max** : *real*  
  Defines the minimal variance factor to apply to the shadow value.
- **variance_bias** : *real*  
  Defines the variance bias to apply to the shadow value.
- **volumetric_steps** : *int*  
  Defines the number of steps performed by the ray to compute the volumetric light scattering.
- **volumetric_scattering** : *real*  
  Defines the volumetric light scattering factor.

### object section

- **parent** : *name*  
  Defines the node which this object is attached to.
- **mesh** : *name*  
  Defines the mesh this object uses.
- **mesh** : *name* *section*  
  Defines a new section describing a mesh with the given name.
- **material** : *name*  
  Name of a material, defined in a .cmtl file or in this file. Applies this material too all the submeshes.
- **materials** : *section*  
  New section used to specify each submesh’s material.
- **cast_shadows** : *boolean*  
  Defines if the object casts shadows (*true*, default value) or not (*false*).
- **receive_shadows** : *boolean*  
  Defines if the object receives shadows (*true*, default value) or not (*false*).
- **cullable** : *boolean*  
  Defines if the object is cullable (*true*, default value) or not (*false*).

#### materials section

- **material** : *int*, *name*  
  Submesh index, and material name for this submesh.

### billboard section

Allows the definition of billboards that share the same material and dimensions.

- **parent** : *name*  
  Defines the parent scene node.
- **type** : *value*  
  Defines the type of billboard. Possible values are:
  - *cylindrical*: The billboard faces the camera, except for their Y axis, which remains still.
  - *spherical*: The billboard faces the camera on all axes.
- **positions** : *section*  
  Defines a new section describing each billboard position.
- **material** : *name*  
  Defines the material used by every billboard of this list.
- **dimensions** : *size*  
  Defines billboards dimensions.
- **size** : *value*  
  Defines the billboards sizing. Possible values are:
  - *dynamic*: The size varies, depending on the distance to camera.
  - *fixed*: The size is fixed, where the camera is.

#### positions section

- **pos** : *3 reals*  
  Defines the relative position of a billboard.

### camera section

- **parent** : *name*  
  Defines the parent CameraNode.
- **primitive** : *value*  
  Defines the display type. Can be one of :
  - *points* : Points.
  - *lines* : Disjointed lines.
  - *line_loop* : Jointed lines loop.
  - *line_strip* : Joined lines.
  - *triangles* : Disjointed triangles.
  - *triangle_strip* : Jointed triangles.
  - *triangle_fan* : Triangles jointed using the first point.
  - *quads* : Disjointed quads.
  - *quad_strip* : Jointed quads.
  - *polygon* : Polygons.
- **viewport** : *section*  
  Defines the camera view port.
- **hdr_config** : *section*  
  Defines a new section describing the HDR configuration.

#### viewport section

- **type** : *value*  
  Viewport type. Can be one of :
  - *ortho* : Orthographic viewport.
  - *perspective* : Perspective viewport.
  - *frustum* : Frustum viewport.
- **left** : *real*  
  Defines the minimum displayed X coordinate.
- **right** : *real*  
  Defines the maximum displayed X coordinate.
- **top** : *real*  
  Defines the minimum displayed Y coordinate.
- **bottom** : *real*  
  Defines the maximum displayed Y coordinate.
- **near** : *real*  
  Defines the minimum displayed Z coordinate.
- **far** : *real*  
  Defines the maximum displayed Z coordinate.
- **size** : *size*  
  Defines the window display size (in pixels).
- **fov_y** : *real*  
  Defines the vertical field of view angle, in radians.
- **aspect_ratio** : *real*  
  Defines the global window aspect ratio (1.33333 for 4/3, 1.77777 for 16/9 … ).

#### hdr_config section

- **exposure** : *real*  
  Defines the scene’s exposure.
- **gamma** : *real*  
  Defines the gamma correction.

### animated_object_group section

- **animated_object** : *name*  
  Adds the object with the given name to the group.
- **animated_mesh** : *name*  
  Adds the mesh from the object with the given name to the group.
- **animated_skeleton** : *name*  
  Adds the skeleton from the object with the given name to the group.
- **animated_node** : *name*  
  Adds the node with the given name to the group.
- **animation** : *name*  
  Adds the animation with the given name to the group’s common animations list.
- **start_animation** : *name*  
  Starts the given animation.
- **pause_animation** : *name*  
  Pauses the given animation (which must have been started first).

#### animation section

- **looped** : *boolean*  
  Defines if the animation is looped (*true*) or not (*false*, default value).
- **scale** : *real*  
  Defines the time scale of the animation (can be negative, the animation will then be played backwards).
- **start_at** : *real*  
  Defines start index of the animation.
- **stop_at** : *real*  
  Defines stop index of the animation.

## skeleton section

- **import** : *file* *&lt;options&gt;*  
  Allows import of skeleton data from a file, in CMSH file format or any format supported by Castor3D import plug-ins. Only if the mesh type is custom. This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting skeleton by given factor, on three axes.
- **import_anim** : *file* *&lt;options*&gt;  
  Allows import of skeleton animations from a file.  
  This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.

## mesh section

- **type** : *name*  
  Mesh type name, one of :
  - *custom* : manually defined mesh or imported mesh.
  - *cube* : a cube, user must then define its three dimensions.
  - *cone* : a cone, user must then define its radius and height.
  - *cylinder* : a cylinder, user must then define its radius and height.
  - *sphere* : a sphere with quad faces, user must then define the subdivisions count and the radius.
  - *icosahedron* : a sphere with triangular faces, user must then define the subdivisions count and the radius.
  - *torus* : a torus, user must then define the internal and external subdivisions count and the internal and external radius.
  - *plane* : a plane, user must then define the width and depth subdivisions count and the width and depth.
- **skeleton** : *name*  
  Defines the skeleton used by the mesh.
- **submesh** : *section*  
  Defines a new section describing a submesh. Only if the mesh type is custom.
- **import** : *file* *&lt;options&gt;*  
  Allows import of mesh data from a file, in CMSH file format or any format supported by Castor3D import plug-ins. Only if the mesh type is custom. This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *pitch*=*réel* : Rotates the resulting mesh by given angle (in degrees) along X axis.
  - *yaw*=*réel* : Rotates the resulting mesh by given angle (in degrees) along Y axis.
  - *roll*=*réel* : Rotates the resulting mesh by given angle (in degrees) along Z axis.
- **import_anim** : *file* *&lt;options*&gt;  
  Allows import of mesh animations from a file.  
  This directive must happen after a first import directive.  
  Available only if the mesh type is *custom*.  
  This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *pitch*=*réel* : Rotates the resulting mesh by given angle (in degrees) along X axis.
  - *yaw*=*réel* : Rotates the resulting mesh by given angle (in degrees) along Y axis.
  - *roll*=*réel* : Rotates the resulting mesh by given angle (in degrees) along Z axis.
- **division** : *name* *int*  
  Allows the mesh subdivision, using a supported Castor3D divider plug-in algorithm. The second parameter is the application count of the algorithm (its applied recursively).

### submesh section

- **vertex** : *3 reals*  
  Defines a vertex position.
- **uv** : *2 reals*  
  Defines the UV texture coordinates for the previously declared vertex.
- **uvw** : *3 reals*  
  Defines the UVW texture coordinates for the previously declared vertex.
- **normal** : *3 reals*  
  Defines the normal coordinates for the previously declared vertex.
- **tangent** : *3 reals*  
  Defines the tangent coordinates for the previously declared vertex.
- **face** : *3 or 4 integers*  
  Defines a face using the three or four vertices whose indices are given. If more than three indices are given, creates the appropriate count of triangular faces.
- **face_uv** : *as much uv as the face indices*  
  Defines the UV coordinates for each vertex of the previously declared face.
- **face_uvw** : *as much uvw as the face indices*  
  Defines the UVW coordinates for each vertex of the previously declared face.
- **face_normals** : *as much 3 reals groups as the face indices*  
  Defines the normals coordinates for each vertex of the previously declared face.
- **face_tangents** : *as much 3 reals groups as the face indices*  
  Defines the tangents coordinates for each vertex of the previously declared face.

## panel_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **uv** : *4 reals*  
  Defines the UV for the overlay (left, top, right, bottom).
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## border_panel_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **center_uv** : *4 reals*  
  Defines the UV for the center of the overlay (left, top, right, bottom).
- **border_material** : *name*  
  Defines the material used for the panel’s border.
- **border_position** : *value*  
  Defines the border’s position, can be one of:
  - *internal* : The border is inside the overlay.
  - *middle* : The border is half inside, half outside the overlay.
  - *external* : The border is outside the overlay.
- **border_size** : *4 reals*  
  Defines the borders sizes (left right, top, bottom).
- **pxl_border_size** : *2 ints*  
  Defines the absolute border size, in pixels.
- **border_inner_uv** : *4 reals*  
  Defines the UV for the border of the overlay, inner side (left, top, right, bottom).
- **border_outer_uv** : *4 reals*  
  Defines the UV for the border of the overlay, outer side (left, top, right, bottom).
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## text_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **font** : *name*  
  Defines the font used to display the text.
- **text** : *texte*  
  Defines the displayed text.
- **text_wrapping** : *value*  
  Defines the way the text is cut when a line overflows the overlay dimensions. Can be one of:
  - *none* : The text is not cut (the part that overflows won’t be displayed, though).
  - *break* : The text is cut at the letter (the words will be cut).
  - *break_words* : The text is cut at the word (the words remain uncut).
- **vertical_align** : *value*  
  Defines the text vertical alignment:
  - *top* : Align on top.
  - *center* : Vertically center.
  - *bottom* : Align on bottom.
- **horizontal_align** : *value*  
  Defines the text horizontal alignment:
  - *left* : Align on left.
  - *center* : Horizontally center.
  - *right* : Align on right.
- **texturing_mode** : *value*  
  Defines the way the texture is applied:
  - *letter* : The texture is applied on each letter.
  - *text* : The texture is applied on the whole text.
- **line_spacing_mode** : *value*  
  Defines the height of the lines:
  - *own_height* : Each line has its own height.
  - *max_lines_height* : Each line has the height of the line with the maximum height.
  - *max_font_height* : Each line has the height of the character with the maximum height in the font.
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## window section

- **render_target** : *section*  
  Defines a new section describing the render target.
- **vsync** : *boolean*  
  Defines the activation or deactivation of vertical synchronisation.
- **fullscreen** : *boolean*  
  Defines the activation or deactivation of full-screen display.

## render_target section

- **scene** : *nom*  
  Defines the scene rendered in this target.
- **camera** : *nom*  
  Defines the camera used to render the scene.
- **size** : *size*  
  Defines the internal buffer dimensions.
- **format** : *value*  
  Defines the colour buffer pixel format. Can be one of :
  - *l8* : Luminance 8 bits, one 8 bits integral number.
  - *l16f* : Luminance 16 bits, one 16 bits floating point number (half float).
  - *l32f* : Luminance 32 bits, one 32 bits floating point number(float).
  - *al16* : Alpha + Luminance, two 8 bits integral number.
  - *al16f* : Alpha + Luminance, two 16 bits floating point number (half float).
  - *al32f* : Alpha + Luminance, two 32 bits floating point number (float).
  - *argb1555* : ARGB 16 bits, 1 bit for alpha and each other component on a 5 bits integer.
  - *rgb565* : RGB 16 bits, R and B on a 5 bits integer, G on a 6 bits integer.
  - *argb16* : ARGB 16 bits, each component on a 4 bits integer.
  - *rgb24* : RGB 24 bits, each component on a 8 bits integer.
  - *argb32* : ARGB 32 bits, each component on a 8 bits integer.
  - *argb16f* : ARGB 64 bits, each component on a 16 bits floating point number (half float).
  - *rgb32f* : RGB 96 bits, each component on a 32 bits floating point number (half float).
  - *argb32f* : ARGB 128 bits, each component on a 32 bits floating point number (half float).
- **postfx** : *value*  
  Defines a post render effect to use. The parameters depend on the chosen effect.
- **tone_mapping** : *name*  
  Defines the tone mapping operator to use with the render target.
- **ssao** : *section*  
  Defines a new section describing the Screen Space Ambient Occlusion.

### ssao section

- **enabled** : *boolean*  
  Defines the activation status of SSAO.
- **high_quality** : *boolean*  
  Defines the quality of the effect.
- **radius** : *real*  
  Defines the radius of the effect (expressesd in meters).
- **bias** : *real*  
  Defines the bias of the effect.
- **intensity** : *real*  
  Defines the intensity of the effect.
- **num_samples** : *int*  
  Defines the number of samples per pixel.
- **edge_sharpness** : *real*  
  Defines the edge sharpness, in the blur pass.
- **blur_step_size** : *int*  
  Defines the size of a step in the blur pass.
- **blur_radius** : *int*  
  Defines the blur radius.
