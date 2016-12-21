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
# Cycles deferred code.
#

VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DEFERRED_FIRST = 4

deferredGeneralDefineGLSL = """#define VKTS_GAMMA 2.2
#define VKTS_INV_GAMMA (1.0/VKTS_GAMMA)"""

deferredGeneralBufferGLSL = """"""

deferredGeneralTextureGLSL = """"""

deferredGeneralFunctionsGLSL = """vec3 colorToLinear(vec3 c)
{
    return pow(c, vec3(VKTS_GAMMA, VKTS_GAMMA, VKTS_GAMMA));
}

vec3 colorToNonLinear(vec3 c)
{
    return pow(c, vec3(VKTS_INV_GAMMA, VKTS_INV_GAMMA, VKTS_INV_GAMMA));
}"""

deferredOutDeclareGLSL = """layout (location = 2) out vec4 ob_emissiveAmbientOcclusion;   // Emissive and ambient occlusion.
layout (location = 1) out vec4 ob_normalRoughness;      // Normal and roughness.
layout (location = 0) out vec4 ob_colorMetallic;        // Color and metallic."""

deferredOutAssignGLSL = """ob_emissiveAmbientOcclusion = vec4(Emissive_0.rgb, AmbientOcclusion_0);
    ob_normalRoughness = vec4(Normal_0.xyz * 0.5 + 0.5, Roughness_0);
    ob_colorMetallic = vec4(Color_0.rgb, Metallic_0);"""
