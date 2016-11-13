/**
 * Copyright (C) 2016 Nigel Williams
 *
 * Vulkan Free Surface Modeling Engine (VFSME) is free software:
 * you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inEyePos;
layout(location = 3) in vec3 inLightVec;
layout(location = 4) in vec4 inAmbientLight;
layout(location = 5) in vec4 inDiffuseLight;
layout(location = 6) in vec4 inSpecularLight;
layout(location = 7) in float inSpecularConst;

layout(location = 0) out vec4 outColor;

void main() { 
	vec4 diffuseLight = inDiffuseLight * max(dot(inNormal, inLightVec), 0.0);
	vec3 eyeVec = normalize(-inEyePos);
	vec3 reflectedLight = normalize(reflect(-inLightVec, inNormal));
	float specularIntensity = pow(max(dot(reflectedLight, eyeVec), 0.0), 0.8);
	vec4 specularLight = inSpecularLight * specularIntensity * inSpecularConst; 

	outColor = (inAmbientLight + diffuseLight) * vec4(inColor, 1.0) + specularLight;
}