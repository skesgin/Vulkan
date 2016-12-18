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

# <pep8-80 compliant>

#
# Cycles materials.
#

fragmentGLSL = """#version 450 core

#define VKTS_PARALLAX_SCALE 0.002
#generalDefine#
#generalBuffer#

layout (location = 0) in vec3 v_f_normal;
#nextAttribute#
layout (location = 4) in vec3 v_f_incident;
layout (location = 5) in vec4 v_f_vertex;
#generalTexture#
#nextTexture#
#generalFunctions#
#outDeclare#

mat4 translate(vec3 t)
{
    return mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, t.x, t.y, t.z, 1.0);
}

mat4 rotateRzRyRx(vec3 rotate)
{
    if (rotate.x == 0.0 && rotate.y == 0.0 && rotate.z == 0.0)
    {
        return mat4(1.0);
    }

    float rz = radians(rotate.z);
    float ry = radians(rotate.y);
    float rx = radians(rotate.x);
    float sx = sin(rx);
    float cx = cos(rx);
    float sy = sin(ry);
    float cy = cos(ry);
    float sz = sin(rz);
    float cz = cos(rz);

    return mat4(cy * cz, cy * sz, -sy, 0.0, -cx * sz + cz * sx * sy, cx * cz + sx * sy * sz, cy * sx, 0.0, sz * sx + cx * cz * sy, -cz * sx + cx * sy * sz, cx * cy, 0.0, 0.0, 0.0, 0.0, 1.0);
}

mat4 scale(vec3 s)
{
    return mat4(s.x, 0.0, 0.0, 0.0, 0.0, s.y, 0.0, 0.0, 0.0, 0.0, s.z, 0.0, 0.0, 0.0, 0.0, 1.0);
}

float fresnelNode(float eta, float theta)
{
    float c = abs(theta);
    float g = eta * eta - 1 + c * c;
    
    if(g > 0)
    {
        g = sqrt(g);
        
        float A = (g - c)/(g + c);
        float B = (c * (g + c) - 1)/(c * (g - c) + 1);
        
        return 0.5 * A * A * (1 + B * B);
    }
	
    return 1.0;
}

vec2 parallaxMappingNode(vec2 texCoord, vec3 view, float height)
{
    return texCoord - view.xy * height * VKTS_PARALLAX_SCALE;
}

vec3 bumpMappingNode(vec3 normal, float height, float distance, float strength)
{
    float finalHeight = height * distance; 

    vec3 x = normalize(vec3(1.0, 0.0, dFdx(finalHeight) * strength));
    vec3 y = normalize(vec3(0.0, 1.0, dFdy(finalHeight) * strength));
    vec3 z = cross(x, y);

    return mat3(x, y, z) * normal;
}

void main()
{ 
    vec3 normal = normalize(v_f_normal);
    vec3 incident = normalize(v_f_incident);
    #nextTangents#
    #nextTexCoord#
    #previousMain#

    if (round(Mask_0) == 1.0)
    {
        discard;
    }

    #outAssign#
}"""

nextTexCoord = """vec3 texCoord = vec3(v_f_texCoord, 0.0);"""


nextTangents = """vec3 bitangent = normalize(v_f_bitangent);
    vec3 tangent = normalize(v_f_tangent);
    
    mat3 objectToWorldMatrix = mat3(tangent, bitangent, normal);"""


normalMapAttribute = """layout (location = 1) in vec3 v_f_bitangent;
layout (location = 2) in vec3 v_f_tangent;
#nextAttribute#"""


texCoordAttribute = """layout (location = 3) in vec2 v_f_texCoord;
#nextAttribute#"""


texImageFunction = """layout (binding = %d) uniform sampler2D u_texture%d;
#nextTexture#"""

parallaxMain = """#previousMain#

    mat3 worldToTangentMatrix = transpose(mat3(tangent, bitangent, normal));
    texCoord = vec3(parallaxMappingNode(texCoord.xy, worldToTangentMatrix * incident, %s), 0.0);"""

#########
# Nodes #
#########

#
# Group
#

pbrMain = """#previousMain#
    
    // PBR start

    // In
    vec4 %s = %s;
    float %s = %s;
    float %s = %s;
    float %s = %s;
    float %s = %s;
    vec3 %s = %s;
    vec4 %s = %s;
    
    // PBR end"""
        
#
# Converter
#

combineRgbMain = """#previousMain#
    
    // Combine RGB start

    // In
    float %s = %s;
    float %s = %s;
    float %s = %s;
    
    // Out
    vec4 %s = vec4(%s, %s, %s, 1.0);
    
    // end""" 

combineXyzMain = """#previousMain#
    
    // Combine XYZ start

    // In
    float %s = %s;
    float %s = %s;
    float %s = %s;
    
    // Out
    vec3 %s = vec3(%s, %s, %s);
    
    // end""" 

mathMain = """#previousMain#
    
    // Math start

    // In
    float %s = %s;
    float %s = %s;
    
    // Out
    float %s = %s%s %s %s%s;
    
    // end"""

rgbToBwMain = """#previousMain#
    
    // RGB to BW start

    // In
    vec4 %s = %s;
    
    // Out
    float %s = %s.r * 0.2126 + %s.g * 0.7152 + %s.g * 0.0722;
    
    // end"""

separateRgbMain = """#previousMain#
    
    // Separate RGB start

    // In
    vec4 %s = %s;
    
    // Out
    float %s = %s.r;
    float %s = %s.g;
    float %s = %s.b;
    
    // end""" 

separateXyzMain = """#previousMain#
    
    // Separate XYZ start

    // In
    vec3 %s = %s;
    
    // Out
    float %s = %s.x;
    float %s = %s.y;
    float %s = %s.z;
    
    // end""" 

#
# Vector
#

bumpMain = """#previousMain#
    
    // Bump start

    // In
    float %s = %s;
    float %s = %s;
    float %s = %s;
    vec3 %s = %s;

    // Out
    vec3 %s = bumpMappingNode(%s, %s, %s * %s, %s);
    
    // Bump end"""

mappingMain = """#previousMain#
    
    // Mapping start

    // In
    vec4 %s = vec4(%s, 1.0);

    %s = %s;
    
    // Out
    vec3 %s = %s%s%s.xyz;
    
    // Mapping end"""

normalMain = """#previousMain#
    
    // Normal start

    // In
    vec3 %s = %s;
    
    // Out
    
    vec3 %s = %s;
    float %s = dot(%s, %s);
    
    // Normal end"""

normalMapMain = """#previousMain#
    
    // Normal map start

    // In
    float %s = %s;
    vec3 %s = objectToWorldMatrix * normalize(%s.xyz * 2.0 - 1.0);
    
    // Out
    
    vec3 %s = normalize(mix(normal, %s, %s));
    
    // Normal map end"""

#
# Color
#

gammaMain =  """#previousMain#
    
    // Gamma start

    // In
    vec4 %s = %s;
    float %s = clamp(%s, 0.001, 10.000);
    
    // Out
    vec4 %s = pow(%s, vec4(1.0 / %s));
    
    // Gamma end"""

invertMain = """#previousMain#
    
    // Invert start

    // In
    float %s = %s;
    vec4 %s = %s;
    
    // Out
    vec4 %s = mix(%s, vec4(1.0 - %s.r, 1.0 - %s.g, 1.0 - %s.b, 1.0 - %s.a), %s);
    
    // Invert end"""

# Mix

multiplyMain = """#previousMain#
    
    // Multiply start

    // In
    float %s = %s;
    vec4 %s = %s;
    vec4 %s = %s;
    
    // Out
    vec4 %s = %s%s * (1.0 - %s) + %s * %s * %s%s;
    
    // Multiply end"""

addSubtractMain = """#previousMain#
    
    // Add start

    // In
    float %s = %s;
    vec4 %s = %s;
    vec4 %s = %s;
    
    // Out
    vec4 %s = %s%s %s %s * %s%s;
    
    // Add end"""

mixMain = """#previousMain#
    
    // Mix start

    // In
    float %s = %s;
    vec4 %s = %s;
    vec4 %s = %s;
    
    // Out
    vec4 %s = %smix(%s, %s, %s)%s;
    
    // Mix end"""

#
# Texture
#

texCheckerMain = """#previousMain#
    
    // Checker texture start

    // In
    vec3 %s = %s;
    vec4 %s = %s;
    vec4 %s = %s;
    float %s = %s;

    bool %s = mod(floor(%s.s * %s), 2.0) == 1.0;
    bool %s = mod(floor(%s.t * %s), 2.0) == 1.0;
    
    // Out
    vec4 %s = %s;
    float %s = 0.0;
        
    if ((%s && !%s) || (!%s && %s))
    {
        %s = %s;
        %s = 1.0;
    }
    
    // Checker texture end"""

texImageMain = """#previousMain#
    
    // Image texture start

    // In
    vec3 %s = %s;
    
    // Out
    vec4 %s = texture(u_texture%d, %s.st).rgba;
    float %s = texture(u_texture%d, %s.st).a;
    
    // Image texture end"""

#
# Input
#

fresnelMain = """#previousMain#
    
    // Fresnel start

    // In
    float %s = max(%s, 1.00001);
    vec3 %s = %s;
    
    // Out
    float %s = fresnelNode(%s, dot(-incident, %s));
    
    // Fresnel end"""

rgbMain = """#previousMain#
    
    // RGB start

    // Out
    vec4 %s = %s;
    
    // end""" 

uvMapMain = """#previousMain#
    
    // UV map start

    // Out
    vec3 %s = texCoord;
    
    // UV map end"""

valueMain = """#previousMain#
    
    // Value start

    // Out
    float %s = %s;
    
    // end""" 
