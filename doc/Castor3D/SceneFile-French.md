# Format de fichier CSCN

Les fichiers CSCN sont au format texte et sont donc modifiables assez facilement (pour peu que l’on comprenne quelque chose à  la syntaxe :P)

Les types de données qui apparaissent dans les fichiers de scène sont les suivants :

- *booléen* : un booléen (*true* ou *false*).
- *entier* : un entier.
- *masque_1* : un entier hexadécimal représentant un masque pour une composante de couleur (*0x000000FF* ou autre).
- *masque_3* : un entier hexadécimal représentant un masque pour trois composantes de couleur (*0x00FFFFFF* ou autre).
- *réel* : un nombre réel, le séparateur des décimales est le point ( . ).
- *2, 3, 4 entiers* : 2, 3 ou 4 entiers, séparés par des virgules ( , ) ou des espaces ( ).
- *2, 3, 4 réels* : 2, 3 ou 4 réels, séparés par des virgules ( , ) ou des espaces ( ).
- *taille* : 2 entiers supérieurs ou égaux à 0.
- *matrice de 2x2, 3x3, 4x4 réels* : 2, 3 ou 4 groupes séparés par des points virgules ( ; ) de 2, 3 ou 4 réels séparés par des virgules ( *,* ) ou des espaces ( )
- *couleur_rvb* : les composantes RVB d’une couleur, exprimées en réels compris entre 0.0 et 1.0.
- *couleur_rvba* : les composantes RVBA d’une couleur, exprimées en réels compris entre 0.0 et 1.0.
- *couleur_hdr_rvb* : les composantes RVB d’une couleur, exprimées en réels supérieurs ou égaux à 0.0.
- *couleur_hdr_rvba* : les composantes RVBA d’une couleur, exprimées en réels supérieurs ou égaux à 0.0.
- *valeur* : chaîne de caractère représentant une valeur prédéfinie.
- *nom* : chaîne de caractères, entourée de guillemets ( &quot; ).
- *fichier* : chaîne de caractères représentant un chemin d’accès à un fichier, entourée de guillemets ( &quot; ).
- *dossier* : chaîne de caractères représentant un chemin d’accès à un dossier, entourée de guillemets ( &quot; ).


## Description

Le fichier est décomposé en sections décrites de la manière suivante:

    [section-type] "[section-name]"
    {
        // Section description
    }

Exemple:

    light "Light0"
    {
        type directional
        colour 1.0 1.0 1.0
        intensity 0.8 1.0
    }

Certaines sections peuvent avoir des sous-sections :

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

## Liste des sections

Les différentes sections possibles sont les suivantes :
- **sampler**  
  Permet de définir un objet d’échantillonnage de texture.
- **material**  
  Permet la définition d’un matériau.
- **mesh**  
  Permet la définition d’un maillage.
- **font**  
  Permet la définition d’une police utilisée dans les incrustations texte.
- **window**  
  Permet la définition d’une fenêtre de rendu.
- **panel_overlay**  
  Permet de définir une incrustation globale de type panneau simple.
- **border_panel_overlay**  
  Permet de définir une incrustation globale de type panneau avec bordure.
- **text_overlay**  
  Permet de définir une incrustation globale de type panneau avec texte.
- **scene**  
  Permet de définir une scène.

## Section sampler

- **min_filter** : *valeur*  
  Valeur pour la fonction de minification. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **mag_filter** : *valeur*  
  Valeur pour la fonction de magnification. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **mip_filter** : *valeur*  
  Valeur pour la fonction de mipmapping. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **min_lod** : *réel*  
  Définit la valeur minimale du niveau de détail.
- **max_lod** : *réel*  
  Définit la valeur maximale du niveau de détail.
- **lod_bias** : *réel*  
  Définit le MIP-Level minimal utilisé.
- **u_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en U. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **v_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en V. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **w_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en W. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **border_colour** : *valeur*  
  Définit la couleur des bords non texturés.
  - *float_transparent_black* : Noir transparent.
  - *int_transparent_black* : Noir transparent.
  - *float_opaque_black* : Noir opaque.
  - *int_opaque_black* : Noir opaque.
  - *float_opaque_white* : Blanc opaque.
  - *int_opaque_white* : Blanc opaque.
- **max_anisotropy** : *réel*  
  Définit le degré maximal d’anisotropie.

## Section material

Les matériaux pouvant être multi-passes, il est possible de définir plusieurs sous-sections de passe.

- **pass** : *section*  
  Commence un nouvelle section décrivant les propriétés de la passe.


### Section pass

- **diffuse** : *couleur_rvb*  
  Définit la couleur diffuse de cette passe (matériaux phong uniquement).
- **albedo** : *couleur_rvb*  
  Defines la couleur d’albédo de cette passe (non disponible pour les matériaux phong).
- **specular** : *couleur_rvb*  
  Définit la couleur réfléchie par cette passe (non disponible pour les matériaux metallic/roughness).
- **metallic** : *réel (entre 0 et 1)*  
  Definit la metallitude de cette pass (matériaux metallic/roughness uniquement).
- **shininess** : *réel (entre 0 et 128)*  
  Définit la façon dont la lumière est réfléchie (matériaux phong uniquement).
- **glossiness** : *réel (entre 0 et 1)*  
  Définit l’éclat de la surface (matériaux specular/glossiness materials uniquement).
- **roughness** : *réel (entre 0 et 1)*  
  Définit la rugosité de la surface (matériaux metallic/roughness uniquement).
- **ambient** : *réel (entre 0 et 1)*  
  Définit la couleur ambiante de cette passe (matériaux phong seulement).
- **emissive** : *réel (entre 0 et 1)*  
  Définit la couleur émise par cette passe.
- **alpha** : *réel (entre 0 et 1)*  
  Définit la valeur d’alpha des couleurs du material.
- **two_sided** : *booléen*  
  Définit si le material est double face (true) ou pas (false).
- **texture_unit** : *section*  
  Définit une nouvelle section concernant une texture.
- **alpha_blend_mode** : *valeur*  
  Nom du mode de mélange alpha, au choix parmi :
  - *none* : Pas de mélange alpha.
  - *additive* : Les alphas de la source et de la destination s’additionnent.
  - *multiplicative* : Les alphas de la source et de la destination se multiplient.
- **colour_blend_mode** : *valeur*  
  Nom du mode de mélange couleur, au choix parmi :
  - *none* : Pas de mélange couleur.
  - *additive* : Les couleurs de la source et de la destination s’additionnent.
  - *multiplicative* : Les couleurs de la source et de la destination se multiplient.
- **alpha_func** : func : *valeur* ref-val : *réel*  
  Définit la fonction de gestion de l’alpha rejection pour la texture. Le second paramètre est la valeur de référence pour les calculs de transparence. Les valeurs possibles pour le premier paramètre sont :
  - *always* : la couleur de l’échantillon est toujours appliquée.
  - *less* : La couleur de l’échantillon est appliquée si sa transparence est inférieure au 2ème paramètre.
  - *less_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est inférieure ou égal au 2ème paramètre.
  - *equal* : La couleur de l’échantillon est appliquée si sa transparence est égale au 2ème paramètre.
  - *not_equal* : La couleur de l’échantillon est appliquée si sa transparence est différente du 2ème paramètre.
  - *greater_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est supérieure ou égal au 2ème paramètre.
  - *greater* : La couleur de l’échantillon est appliquée si sa transparence est supérieure au 2ème paramètre.
  - *never* : La couleur de l’échantillon n’est jamais appliquée.
- **refraction_ratio** : *réel*  
  Définit le ratio de réfraction de la passe. Notez que même s’il n’y a pas de refraction map, la réfraction sera appliquée tout de même, en utilisant seulement la skybox.
- **subsurface_scattering** : *section*  
  Définit une nouvelle section décrivant le subsurface scattering pour cette passe.
- **parallax_occlusion** : *booléen*  
  Active ou désactive le parallax occlusion mapping (nécessite une normal map et une height map).
- **bw_accumulation** : *entier compris entre 0 et 5*  
  Définit la fonction d'accumulation, pour le blended weighted rendering.
- **reflections** : Active les réflexions.
- **refractions** : Active la réfraction.

#### Section texture_unit

- **image** : *fichier*  
  Définit le chemin où trouver l’image.
- **render_target** : *section*  
  Permet de définir la texture en cible de rendu et de configurer cette cible.
- **channel** : *value {| value}*  
  Les canaux auxquels la texture peut être associée. Peut prendre les valeurs suivantes :
  - *diffuse* : Couleur d'éclairage diffus (Phong uniquement).
  - *albedo* : Couleur de base (PBR uniquement).
  - *specular* : Couleur spéculaire (Phong et PBR Specular/Glossiness).
  - *metallic* : Facteur métallique (PBR Metallic/Roughness uniquement).
  - *shininess* : Exposant spéculaire (Phong uniquement).
  - *glossiness* : Facteur de glossiness (PBR Specular/Glossiness uniquement).
  - *roughness* : Facteur de roughness (PBR Metallic/Roughness uniquement).
  - *opacity* : Facteur d'opacité.
  - *emissive* : Couleur émissive.
  - *transmittance* : Facteur de transmission (pour le subsurface scattering).
  - *occlusion* : Facteur d'occlusion.
  - *normal* : Normales.
  - *height* : Hauteur.
- **sampler** : *nom*
  Définit l’échantillonneur pour la texture.
- **diffuse_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur diffuse (Phong uniquement).
- **albedo_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur de base (PBR uniquement).
- **specular_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur spéculaire (Phong ou PBR Specular/Glossiness).
- **metalness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur métallique (PBR Metallic/Roughness uniquement).
- **shininess_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour l'exposant spéculaire (Phong uniquement).
- **glossiness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de glossiness (PBR Specular/Glossiness uniquement).
- **roughness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de roughness (PBR Metallic/Roughness uniquement).
- **opacity** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur d'opacité.
- **emissive** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur émissive.
- **transmittance** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de transmittance (pour le subsurface scattering.
- **occlusion** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur d'occlusion.
- **normal** : *masque_3*  
  Définit les composantes de la texture utilisées pour les normales.
- **height** : *masque_1*  
  Définit la composante de la texture utilisée pour la hauteur.
- **height_factor** : *réel*  
  Définit le facteur de hauteur.
- **normal_directx** : *booléen*  
  Dit si les normales de la texture sont exprimées pour DirectX (la composante verte sera alors inversée).

#### Section shader_program

- **vertex_program** : *section*  
  Définit une nouvelle section concernant le vertex program.
- **pixel_program** : *section*  
  Définit une nouvelle section concernant le pixel program.
- **geometry_program** : *section*  
  Définit une nouvelle section concernant le geometry program.
- **hull_program** : *section*  
  Définit une nouvelle section concernant le hull (tessellation control) program.
- **domain_program** : *section*  
  Définit une nouvelle section concernant le domain (tessellation evaluation) program.
- **constants_buffer** : *section*  
  Définit une nouvelle section concernant les tampons de constantes (uniform buffers).

#### Section vertex/pixel/geometry/hull/domain_program

- **file** : *fichier*  
  Nom du fichier où se trouve le programme.
- **sampler** : *nom*  
  Crée une nouvelle variable de type sampler (1D, 2D, …) pour le pixel shader.
- **input_type** : *valeur*  
  Définit le type de données de faces en entrée du geometry shader. Peut ptrendre les valeurs suivantes :
  - *points* : Des points.
  - *lines* : Des lignes disjointes.
  - *line_loop* : Une boucle formée de lignes jointes.
  - *line_strip* : Des lignes jointes.
  - *triangles* : Des triangles disjoints.
  - *triangle_strip* : Des triangles joints.
  - *triangle_fan* : Des triangles joints par le premier point.
  - *quads* : Des quadrilatères disjoints.
  - *quad_strip* : Des quadrilatères joints.
  - *polygon* : Des polygones.
- **output_type** : *valeur*  
  Définit le type de données de faces en sortie du geometry shader. Peut ptrendre les valeurs suivantes :
  - *points* : Des points.
  - *line_strip* : Des lignes jointes.
  - *triangle_strip* : Des triangles joints.
  - *quad_strip* : Des quadrilatères joints.
- **output_vtx_count** : *entier*  
  Définit le nombre de sommets en sortie du geometry shader.
- **constants_buffer** : *section*  
  Définit une nouvelle section décrivant un constants buffer.

#### Section constants_buffer

- **shaders** : *combinaison binaire de valeurs*  
  Types de shaders pour lesquels ce tampon s’applique, parmi :
  - *vertex*
  - *hull*
  - *domain*
  - *geometry*
  - *pixel*
  - *compute*
- **variable** : *nom*, *section*  
  Définit une section de propriétés d’une variable à ajouter au tampon.

#### Section variable

- **type** : *valeur*  
  Nom du type de la variable, peut être :
  - *int* : 1 entier signé.
  - *uint* : 1 entier non signé.
  - *float* : 1 nombre flottant en simple précision.
  - *double* : 1 nombre flottant en double précision.
  - *vec2i* : 2 entiers signés.
  - *vec3i* : 3 entiers signés.
  - *vec4i* : 4 entiers signés.
  - *vec2f* : 2 nombres flottants en simple précision.
  - *vec3f* : 3 nombres flottants en simple précision.
  - *vec4f* : 4 nombres flottants en simple précision.
  - *vec2d* : 2 nombres flottants en double précision.
  - *vec3d* : 3 nombres flottants en double précision.
  - *vec4d* : 4 nombres flottants en double précision.
  - *mat2x2i* : Matrice 2x2 d’entiers signés.
  - *mat2x3i* : Matrice 2x3 d’entiers signés.
  - *mat2x4i* : Matrice 2x4 d’entiers signés.
  - *mat3x2i* : Matrice 3x2 d’entiers signés.
  - *mat3x3i* : Matrice 3x3 d’entiers signés.
  - *mat3x4i* : Matrice 3x4 d’entiers signés.
  - *mat4x2i* : Matrice 4x2 d’entiers signés.
  - *mat4x3i* : Matrice 4x3 d’entiers signés.
  - *mat4x4i* : Matrice 4x4 d’entiers signés.
  - *mat2x2f* : Matrice 2x2 de nombres flottants simple précision.
  - *mat2x3f* : Matrice 2x3 de nombres flottants simple précision.
  - *mat2x4f* : Matrice 2x4 de nombres flottants simple précision.
  - *mat3x2f* : Matrice 3x2 de nombres flottants simple précision.
  - *mat3x3f* : Matrice 3x3 de nombres flottants simple précision.
  - *mat3x4f* : Matrice 3x4 de nombres flottants simple précision.
  - *mat4x2f* : Matrice 4x2 de nombres flottants simple précision.
  - *mat4x3f* : Matrice 4x3 de nombres flottants simple précision.
  - *mat4x4f* : Matrice 4x4 de nombres flottants simple précision.
  - *mat2x2d* : Matrice 2x2 de nombres flottants double précision.
  - *mat2x3d* : Matrice 2x3 de nombres flottants double précision.
  - *mat2x4d* : Matrice 2x4 de nombres flottants double précision.
  - *mat3x2d* : Matrice 3x2 de nombres flottants double précision.
  - *mat3x3d* : Matrice 3x3 de nombres flottants double précision.
  - *mat3x4d* : Matrice 3x4 de nombres flottants double précision.
  - *mat4x2d* : Matrice 4x2 de nombres flottants double précision.
  - *mat4x3d* : Matrice 4x3 de nombres flottants double précision.
  - *mat4x4d* : Matrice 4x4 de nombres flottants double précision.
- **count** : *entier*  
  Nombre d’occurences de la variable (taille du tableau).
- **value**  
  Valeur de la variable, fonction du type choisi.

#### Section subsurface_scattering

- **strength** : *réel*  
  Définit la force de l’effet.
- **gaussian_width** : *réel*  
  Définit la largeur du flo Gaussien.
- **transmittance_profile** : *new section*  
  Définit une nouvelle section décrivant le profil de transmission.

#### Section transmittance_profile

- **factor** : *vec4f*  
  Définit les trois composantes RVB de la couleur, la quatrième composante définissant le facteur de multiplication de cette couleur.

## Section font

- **file** : *fichier*  
  Définit le fichier contenant la police.
- **height** : *entier*  
  Définit la hauteur des caractères (la précision).

## Section scene

- **ambient_light** : *couleur*  
  Définit la couleur de l’éclairage ambiant.
- **background_colour** : *couleur*  
  Définit la couleur de fond de la scène.
- **background_image** : *fichier*  
  Définit l’image de fond de la scène.
- **import** : *fichier*  
  Permet l’import d’une scène à partir d’un fichier de scène CSCN ou autre, supporté par les plug-ins d’import Castor3D.
- **scene_node** : *section*  
  Définit un noeud de scène.
- **camera_node** : *section*  
  Définit un noeud de scène spécifique aux caméras.
- **light** : *section*  
  Définit une source lumineuse.
- **object** : *section*  
  Définit un objet.
- **billboard** : *section*  
  Définit des billboards.
- **camera** : *section*  
  Définit une caméra.
- **panel_overlay** : *section*  
  Définit une incrustation de type panneau simple.
- **border_panel_overlay** : *section*  
  Définit une incrustation de type panneau avec bordure.
- **text_overlay** : *section*  
  Définit une incrustation de type panneau avec texte.
- **animated_object_group** : *section*  
  Définit un groupe d’objets animés avec des animations communes.
- **mesh** : *section*  
  Définit une nouvelle section décrivant un maillage, pouvant être utilisé pou un ou plusieurs objets.
- **particle_system** : *section*  
  Définit une nouvelle section décrivant un système de particules.
- **skybox** : *section*  
  Définit une nouvelle section décrivant la skybox.
- **include** : *fichier*  
  Inclut un fichier de scène, permettant de découper une scène en de multiples fichiers.
- **sampler** : *section*  
  Définit une nouvelle section décrivant un échantillonneur.
- **fog_type** : *valeur*  
  Définit le type de brouillard pour la scène. Les valeurs possibles sont :
  - *linear* : L’intensité du brouillard augmente linéairement, avec la distance à la caméra.
  - *exponential* : L’intensité du brouillard augmente exponentiellement, avec la distance à la caméra.
  - *squared_exponential* : L’intensité du brouillard augmente encore plus, avec la distance à la caméra.
- **fog_density** : *réel*  
  Définit la densité du brouillard, qui est multipliée par la distance, en fonction du type de brouillard.
- **hdr_config** : *section*  
  Définit une nouvelle section, décrivant la configuration HDR.

### Section hdr_config

- **exposure** : *réel*  
  Définit l’exposition de la scène.
- **gamma** : *réel*  
  Définit la correction gamma de la scène.

### Sections scene_node et camera_node

- **parent** : *nom*  
  Définit le Node parent de celui-ci. Par défaut le parent est le RootNode. Le fait de transformer (tranlate, rotate, scale) un node parent transforme ses enfants.
- **position** : *3 réels*  
  La position du node par rapport à son parent.
- **orientation** : *4 réels*  
  Quaternion représentant l’orientation du node par rapport à son parent.
- **scale** : *3 réels*  
  Echelle du node par rapport à son parent.

### Section light

- **type** : *valeur*  
  3 types de sources lumineuses existent dans Castor3D :
  - *directional* : lumière directionnelle (telle le soleil).
  - *point_light* : une source située à un endroit et émettant dans toutes les directions.
  - *spot_light* : une source située à un endroit et émettant dans un cône orienté dans une direction.
- **colour** : *3 réels*  
  Définit la couleur de la source lumineuse, au format RGB.
- **intensity** : *2 réels*  
  Définit les intensités diffuse et spéculaire de la source lumineuse.
- **attenuation** : *3 réels*  
  Définit les 3 composantes d’atténuation de la source lumineuse en fonction de l’éloignement de la source : constante, linéaire et quadratique.  
  spot_light et point_light uniquement.
- **cut_off** : *réel*  
  Ouverture de l’angle du cône du spot.  
  spot_light uniquement.
- **exponent** : *réel*  
  Attenuation fonction de la distance entre le point éclairé et le centre du cône lumineux.  
  spot_light uniquement.
- **parent** : *nom*  
  Définit le nom du SceneNode auquel la source lumineuse est attachée.
- **shadows** : *section*  
  Définit une nouvelle section décrivant les ombres produites par la source lumineuse.

#### Section shadows

- **producer** : *booléen*  
  Dit si la source lumineuse produit des ombres (*true*) ou pas (*false*).
- **filter** : *valeur*  
  Définit le type d'ombres à utiliser. Peut prendre les valeurs suivantes :
  - *raw* : Aucun filtre.
  - *pcf* : Filtre PCF.
  - *variance* : Variance Shadow Map.
- **min_offset** : *réel*  
  Définit l'offset minimal à appliquer à la valeur récupérée depuis la texture d'ombres.
- **max_slope_offset** : *réel*  
  Définit l'offset maximal, relatif à la pente, à appliquer à la valeur récupérée depuis la texture d'ombres.
- **variance_max** : *réel*  
  Définit le facteur maximal pour la variance, à appliquer à la valeur récupérée depuis la texture d'ombres.
- **variance_bias** : *réel*  
  Définit le décalage de variance à appliquer à la valeur récupérée depuis la texture d'ombres.
- **volumetric_steps** : *entier*  
  Le nombre d'étapes effectuées par le rayon pour calculer le volumetric light scattering.
- **volumetric_scattering** : *réel*  
  Définit le facteur de volumetric light scattering.

### Section object

- **parent** : *nom*  
  Nom du SceneNode auquel la géométrie est attachée.
- **mesh** : *nom*  
  Définit le maillage utilisé par cet objet.
- **mesh** : *nom*, *section*  
  Définit une section décrivant un maillage, avec le nom donné.
- **material** : *nom*  
  Nom d’un materiau défini dans un fichier .cmtl ou dans ce fichier. Applique le materiau à tous les sous-maillages.
- **materials** : *section*  
  Permet de définir le matériau pour chaque sous-maillage.
- **cast_shadows** : *booléen*  
  Définit si l’objet projette des ombres (*true*, valeur par défaut) ou pas (*false*).
- **receive_shadows** : *booléen*  
  Définit si l’objet reçoit des ombres (*true*, valeur par défaut) ou pas (*false*).

#### Section materials

- **material** : *entier*, *nom*  
  Index du sous-maillage et nom du matériau à utiliser.


### Section billboard

Permet de définir des billboards partageant le même matériau et faisant les mêmes dimensions.

- **parent** : *nom*  
  Définit le SceneNode auquel ces billboards seront attachés.
- **positions** : *section*  
  Permet de définir les positions relatives des différentes instances des billboards.
- **material** : *nom*  
  Définit le mtatériau utilisé pour l’affichage des billboards.
- **dimensions** : *taille*  
  Définit la taille des billboards.
- **type** : *valeur*  
  Définit le type de billboards. Les valeurs possibles sont :
  - *cylindrical*: Les billboards font face à la caméra, sauf pour leur axe Y, qui reste fixe.
  - *spherical*: Les billboards font face à la caméra sur tous les axes.
- **size** : *valeur*  
  Définit le redimensionnement des billboards. Les valeurs possibles sont :
  - *dynamic*: La taille varie, en fonction de la distance de la caméra.
  - *fixed*: La taille est fixe, quelle que soit la distance à la caméra.

#### Section positions

- **pos** : *3 réels*  
  Définit la position relative d’un billboard.

### Section camera

- **parent** : *nom*  
  Définit le CameraNode auquel la caméra est attachée.
- **primitive** : *valeur*  
  Définit le type d’affichage de la caméra, peut prendre les valeurs suivantes :
  - *points* : Des points.
  - *lines* : Des lignes disjointes.
  - *line_loop* : Une boucle formée de lignes jointes.
  - *line_strip* : Des lignes jointes.
  - *triangles* : Des triangles disjoints.
  - *triangle_strip* : Des triangles joints.
  - *triangle_fan* : Des triangles joints par le premier point.
  - *quads* : Des quadrilatères disjoints.
  - *quad_strip* : Des quadrilatères joints.
  - *polygon* : Des polygones.
- **viewport** : *section*
  Définit la fenêtre d’affichage de la caméra.

#### Section viewport

- **type** : *valeur*  
  Type d’affichage de la fenêtre, peut valoir 2d ou 3d.
- **left** : *réel*  
  Définit la coordonnée X minimale affichée.
- **right** : *réel*  
  Définit la coordonnée X maximale affichée.
- **top** : *réel*  
  Définit la coordonnée Y minimale affichée.
- **bottom** : *réel*  
  Définit la coordonnée Y maximale affichée.
- **near** : *réel*  
  Définit la coordonnée Z minimale affichée.
- **far** : *réel*  
  Définit la coordonnée Z maximale affichée.
- **size** : *taille*  
  Définit la taille de la fenêtre d’affichage (en pixels).
- **fov_y** : *réel*  
  Définit l’angle d’ouverture vertical, en radians.
- **aspect_ratio** : *réel*  
  Définit l’aspect global de la fenêtre (1.33333 pour 4/3, 1.77777 pour 16/9 … ).

### Section animated_object_group

- **animated_object** : *nom*  
  Définit le nom d’un objet à ajouter dans le groupe.
- **animation** : *nom*  
  Ajoute l’animation dont le nom est donné à la liste d’animations communes.
- **start_animation** : *nom*  
  Démarre l’animation donnée.
- **pause_animation** : *nom*  
  Met l’animation donnée en pause (elle doit avoir été démarrée au préalable).

#### Section animation

- **looped** : *booléen*  
  Définit si l’animation est bouclée (*true*) ou pas (*false*, valeur par défaut).
- **scale** : *réel*  
  Définit la vitesse de l’animation (peut être négative, l’animation sera alors jouée à l’envers).

## Section mesh

- **type** : *nom*  
  Nom du type de maillage. Peut être :
  - *custom* : maillage défini manuellement ou maillage importé.
  - *cube* : cube, il faut définir ses 3 dimensions par la suite.
  - *cone* : cône, il faut définir son rayon et sa hauteur par la suite.
  - *cylinder* : cylindre, dont il faut entrer ensuite le rayon et la hauteur.
  - *sphere* : sphère à faces “carrées”, il faut définir le nombre de subdivision et le rayon.
  - *icosahedron* : sphère à faces triangulaires, il faut définir le nombre de subdivision et le rayon.
  - *torus* : torre, il est nécessaire de définir le nombre de subdivisions internes, externes et les rayons interne et externe.
  - *plane* : un plan, il est nécessaire de définir le nombre de subdivisions en largeur et en profondeur ainsi que la largeur et la profondeur.
- **submesh** : *section*  
  Définit un sous-maillage, uniquement si le type du maillage est **custom**.
- **import** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant les données du maillage. Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D. Uniquement si le type du maillage est **custom**. Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *smooth_normals* : Génère les normales par sommet lors de l’import.
  - *flat_normals* : Génère les normales par face lors de l’import.
  - *tangent_space* : Génère les informations d’espace tangent (tangente et bitangente) lors de l’import.
  - *rescale*=*réel* : Met le maillage à l’échelle, sur les trois axes.
- **morph_import** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant les données du maillage, pour ajouter une animation par sommet.  
  Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D.  
  Disponible uniquement si le type du maillage est *custom*.  
  Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *smooth_normals* : Génère les normales par sommet lors de l’import.
  - *flat_normals* : Génère les normales par face lors de l’import.
  - *tangent_space* : Génère les informations d’espace tangent (tangente et bitangente) lors de l’import.
  - *rescale*=*réel* : Met le maillage à l’échelle, sur les trois axes.
- **division** : *nom* *entier*  
  Permet la subdivision du maillage en utilisant un algorithm défini par le nom donné (support en fonction des plugins). Le second paramètre est le nombre de fois où la division est effectuée (récursivement).

### Section submesh

- **vertex** : *3 réels*  
  Ajoute le sommet défini par les coordonnées au sous-maillage.
- **uv** : *2 réels*  
  Définit les uv à utiliser pour le sommet déclaré précédemment.
- **uvw** : *3 réels*  
  Définit les uvw à utiliser pour le sommet déclaré précédemment.
- **normal** : *3 réels*  
  Définit la normale à utiliser pour le sommet déclaré précédemment.
- **tangent** : *3 réels*  
  Définit la tangente à utiliser pour le sommet déclaré précédemment.
- **face** : *3 ou 4 entiers*  
  Définit une face qui utilise les 3 ou 4 indices de sommet. S’il y a plus de 3 indices, crée autant de faces triangulaires que nécessaire.
- **face_uv** : *autant d’uv que d’indices de la face*  
  Définit les uv à utiliser pour la face déclarée précédemment.
- **face_uvw** : *autant d’uvw que d’indices de la face*  
  Définit les uvw à utiliser pour la face déclarée précédemment.
- **face_normals** : *autant de groupes de 3 réels que d’indices de la face*  
  Définit les normales à utiliser pour la face déclarée précédemment.
- **face_tangents** : *autant de groupes de 3 réels que d’indices de la face*  
  Définit les tangentes à utiliser pour la face déclarée précédemment.

## Section panel_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 entiers*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **uv** : *4 réels*  
  Définit les UV pour l’incrustation (gauche, haut, droit, bas).
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section border_panel_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 réels*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 réels*  
  Définit la taille absolue de l’incrustation, en pixels.
- **center_uv** : *4 réels*  
  Définit les UV pour le centre de l’incrustation (gauche, haut, droit, bas).
- **border_material** : *nom*  
  Définit le matériau utilisé par la bordure du panneau.
- **border_position** : *valeur*  
  Définit la position de la bordure de l’incrustation, parmi les valeurs suivantes :
  - *internal* : La bordure à l’intérieur de l’inscrustation (elle ne dépasse pas de l’incrustation).
  - *middle* : La bordure est à moitié à l’intérieur et à moitié à l’extérieur de l’incrustation (elle dépasse de l’incrustation).
  - *external* : La bordure est à l’extérieur de l’incrustation (elle n’empiète pas sur le contenu de l’incrustation).
- **border_size** : *4 réels*  
  Définit la taille des bords (gauche, droite, haut , bas), par rapport au parent (ou à l’écran).
- **pxl_border_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **border_inner_uv** : *4 réels*  
  Définit les UV pour la bordure de l’incrustation, côté intérieur (gauche, haut, droit, bas).
- **border_outer_uv** : *4 réels*  
  Définit les UV pour la bordure de l’incrustation, côté extérieur (gauche, haut, droit, bas).
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section text_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 entiers*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **font** : *nom*  
  Définit la police utilisée par l’incrustation.
- **text** : *texte*  
  Définit le texte affiché.
- **text_wrapping** : *valeur*  
  Définit la manière dont le texte est découpé dans le cas où une ligne dépasse les dimensions de l’incrustation :
  - *none* : Le texte n’est pas découpé (ce qui dépasse n’est pas affiché).
  - *break* : Le texte est découpé à la lettre (les mots sont coupés).
  - *break_words* : Le texte est découpé au mot (les mots restent entiers).
- **vertical_align** : *valeur*  
  Définit la manière dont le texte est aligné verticalement, dans son incrustation :
  - *top* : Le texte est aligné en haut.
  - *center* : Le texte est centré.
  - *bottom* : Le texte est aligné en bas.
- **horizontal_align** : *valeur*  
  Définit la manière dont le texte est aligné horizontalement, dans son incrustation :
  - *left* : Le texte est aligné à gauche.
  - *center* : Le texte est centré.
  - *right* : Le texte est aligné à droite.
- **texturing_mode** : *valeur*  
  Définit la manière dont la texture est appliquée :
  - *letter* : La texture est appliquée sur chaque lettre.
  - *text* : La texture est appliquée sur tout le texte.
- **line_spacing_mode** : *valeur*  
  Définit la hauteur des lignes :
  - *own_height* : Chaque ligne a pour hauteur sa propre hauteur.
  - *max_lines_height* : Chaque ligne a pour hauteur la hauteur de la plus haute ligne.
  - *max_font_height* : Chaque ligne a pour hauteur la hauteur du plus haut caractère de la police.
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section window

- **render_target** : *section*  
  Permet de définir une nouvelle section décrivant la cible de rendu.
- **vsync** : *booléen*  
  Permet de dire si on veut activer la synchronisation verticale.
- **fullscreen** : *booléen*  
  Permet d’activer ou non l’affichage plein écran.

## Section render_target

- **scene** : *nom*  
  Permet de définir la scène rendue dans cette cible.
- **camera** : *nom*  
  Permet de définir la caméra utilisée pour rendre la scène.
- **size** : *taille*  
  Définit la taille du buffer interne de la cible de rendu.
- **format** : *valeur*  
  Définit le format des pixels du buffer couleur de la cible de rendu. Peut valoir :
  - *l8* : Luminance 8 bits, 1 nombre entier 8 bits.
  - *l16f* : Luminance 16 bits, 1 nombre en virgule flottante 16 bits (half float).
  - *l32f* : Luminance 32 bits, 1 nombre en virgule flottante 32 bits (float).
  - *al16* : Transparence + Luminance, 2 nombres entiers 8 bits.
  - *al16f* : Transparence + Luminance, 2 nombres en virgule flottante 16 bits (half float).
  - *al32f* : Transparence + Luminance, 2 nombres en virgule flottante 32 bits (float).
  - *argb1555* : ARGB 16 bits, 1 bit alpha et chaque composante sur un entier 5 bits.
  - *rgb565* : RGB 16 bits, R sur un entier 5 bits, G sur un entier 6 bits et B sur un entier 5 bits.
  - *argb16* : ARGB 16 bits, chaque composante sur un entier 4 bits.
  - *rgb24* : RGB 24 bits, chaque composante sur un entier 8 bits.
  - *argb32* : ARGB 32 bits, chaque composante sur un entier 8 bits.
  - *argb16f* : ARGB 64 bits, chaque composante sur un flottant 16 bits (half float).
  - *rgb32f* : RGB 96 bits, chaque composante sur un flottant 32 bits (float).
  - *argb32f* : ARGB 128 bits, chaque composante sur un flottant 32 bits (float).
- **depth** : *valeur*  
  Définit le format des pixels du buffer profondeur de la cible de rendu. Peut valoir :
  - *depth16* : Profondeur sur un entier en 16 bits.
  - *depth24* : Profondeur sur un entier en 24 bits.
  - *depth24s8* : Profondeur sur un entier en 24 bits + Stencil sur un entier 8 bits.
  - *depth32fs8* : Profondeur sur un nombre en virgule flottante en 32 bits + Stencil sur un entier 8 bits.
  - *depth32* : Profondeur sur un entier en 32 bits.
  - *depth32f* : Profondeur sur un nombre en virgule flottante en 32 bits.
  - *stencil1* : Stencil sur un bit.
  - *stencil8* : Stencil sur un entier en 8 bits.
- **postfx** : *nom*, *parametres optionnels*  
  Définit un effet post-rendu à utiliser. Les paramètres optionnels dépendent de l’effet choisi.
- **stereo** : *booléen*  
  Définit si on utilise l’affichage stéréoscopique.
- **tone_mapping** : *nom*  
  Définit l’opérateur de mappage de ton, à utiliser avec la cible de rendu.
- **ssao** : *section*  
  Définit une nouvelle section décrivant le Screen Space Ambient Occlusion.

### Section ssao

- **enabled** : *booléen*  
  Définit le statut d’activation du SSAO.
- **high_quality** : *booléen*  
  Définit la qalité de l’effet.
- **radius** : *réel*  
  Définit le rayon de l’effet (exprimé en mètres).
- **bias** : *réel*  
  Définit le bias de l’effet.
- **intensity** : *réel*  
  Définit l’intensité de l’effet.
- **num_samples** : *entier*  
  Définit le nombre d’échantillons par pixel.
- **edge_sharpness** : *réel*  
  Définit la netteté des contours lors de la passe de flou.
- **blur_step_size** : *entier*  
  Définit la taille d’une étape, durant la passe de flou.
- **blur_radius** : *entier*  
  Définit le rayon du flou.
