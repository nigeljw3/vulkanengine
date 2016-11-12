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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 3) in vec4 inNormal;
layout(location = 2) in float inHeight;

layout(binding = 0) uniform UBO {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 lightPos;
} ubo;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outEyePos;
layout(location = 3) out vec3 outLightVec;
layout(location = 4) out vec4 outAmbientLight;
layout(location = 5) out vec4 outDiffuseLight;
layout(location = 6) out vec4 outSpecularLight;
layout(location = 7) out float outSpecularConst;

void main() {
	//gl_Position = vec4(inPosition, 1.0);
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    
	//gl_Position = ubo.mvp * vec4(inPosition.x, inHeight, inPosition.z, 1.0);
    outColor = inColor;
	outAmbientLight = vec4(0.3, 0.3, 0.3, 1.0);
	outDiffuseLight = vec4(0.7, 0.7, 0.7, 1.0);
	outSpecularConst = 0.25;
	outSpecularLight = vec4(0.5, 0.5, 0.5, 1.0); 

	outNormal = normalize(inNormal.xyz);
	mat4 modelView = ubo.view * ubo.model;
	vec4 pos = modelView * vec4(inPosition.x, inHeight, inPosition.z, 1.0);	
	outEyePos = vec3(modelView * pos);
	vec4 lightPos = vec4(ubo.lightPos, 1.0) * modelView;
	outLightVec = normalize(ubo.lightPos.xyz - outEyePos);
	gl_Position = ubo.proj * pos;
}