/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
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
    //outColor = vec4(inColor, 1.0);

	vec3 eyeVec = normalize(-inEyePos);
	vec3 reflected = normalize(reflect(-inLightVec, inNormal)); 
	vec4 diffuse = inDiffuseLight * max(dot(inNormal, inLightVec), 0.0);
	vec4 specular = inSpecularLight * pow(max(dot(reflected, eyeVec), 0.0), 0.8) * inSpecularConst; 

	outColor = (inAmbientLight + diffuse) * vec4(inColor, 1.0) + specular;
}