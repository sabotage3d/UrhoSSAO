#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

varying vec3 vTexCoord;

varying float depth;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);

    depth = (-gl_Position.z-0.1)/(100.0-0.1);
    vTexCoord = vec3(GetTexCoord(iTexCoord), GetDepth(gl_Position));
}

void PS()
{

    gl_FragColor = vec4(1,0,0, 1.0) * depth;

}