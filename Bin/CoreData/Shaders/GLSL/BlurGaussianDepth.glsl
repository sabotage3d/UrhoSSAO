
// Depth aware Gaussian filter using texture bilinear interpolation. For the Urho3D engine.
// Based on a paper by Daniel Rákos:
// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling

#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

#line 11

varying vec2 vTexCoord;

#ifdef COMPILEVS
void VS()
{
	mat4 modelMatrix = iModelMatrix;
	vec3 worldPos = GetWorldPos(modelMatrix);
	gl_Position = GetClipPos(worldPos);
   	vTexCoord = GetQuadTexCoord(gl_Position);
}
#endif

#ifdef COMPILEPS

uniform float cVar1;
uniform float cVar2;

// Type of data to read and process (see 'getValues')
#define VALUE_TYPE vec4

// Increase to make depth edges crisper, decrease to reduce flicker
#define EDGE_SHARPNESS      10000.0

// Gaussian weight offset
#define GAUSSIAN_OFFSET     0.0

// Filter direction
#ifdef VERTICAL
    #define AXIS y
    #define DIR yx
#else
    #define AXIS x
    #define DIR xy
#endif

void getValues(vec2 pos, out VALUE_TYPE value, out float depth)
{
    vec4 values = texture2D(sDiffMap, pos);
    value = values;
#if 1
    depth = DecodeDepth(values.gbb);
#else
    depth = DecodeDepth(texture2D(sDepthBuffer, pos).rgb);
#endif
}

/*
float getWeight(float depth, float centerDepth, float gaussianWeight, inout float totalWeight)
{
    float depthWeight = max(0.0, 1.0 - EDGE_SHARPNESS * abs(depth - centerDepth));
    float weight = (GAUSSIAN_OFFSET + gaussianWeight) * depthWeight;
    totalWeight += weight;
    return weight;
}
*/

VALUE_TYPE getResult(vec2 pos, float centerDepth, float gaussianWeight, inout float totalWeight)
{
    VALUE_TYPE value;
    float depth;
    getValues(pos, value, depth);
    
    float depthWeight = max(0.0, 1.0 - EDGE_SHARPNESS * cVar2 * abs(depth - centerDepth));
    float weight = (GAUSSIAN_OFFSET + gaussianWeight) * depthWeight;
    totalWeight += weight;
    return value * weight;
}

void PS()
{
    VALUE_TYPE centerValue;
    float centerDepth;
    getValues(vTexCoord, centerValue, centerDepth);
    
    // 2+1+2 samples
#if SAMPLES==2
    //vec2 weight = vec2( 0.29412, 0.35294 );                         //sigma = 1.483
    //vec2 offset = vec2( 0.0, 1.3333 ) * cGBufferInvSize.AXIS; 
    // sigma = 1.5
    // gauss = 0.292082 0.233881 0.120078
    vec2 weight = vec2( 0.292082 0.353959 );
    vec2 offset = vec2( 0.0, 1.33924 ) * cGBufferInvSize.AXIS;
 
    float totalWeight = GAUSSIAN_OFFSET + weight.x;
    VALUE_TYPE result = centerValue * totalWeight;
    result += getResult(vTexCoord - vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    
    // 4+1+4 samples
#elif SAMPLES==4
    //vec3 weight = vec3( 0.227027, 0.316217, 0.070270 );           //sigma = 1.7754
    //vec3 offset = vec3( 0.0, 1.3846, 3.2308 ) * cGBufferInvSize.AXIS;
    //vec3 weight = vec3( 0.250404, 0.320621, 0.0541771 );          //sigma = 1.6
    //vec3 offset = vec3( 0.0, 1.35757, 3.20307 ) * cGBufferInvSize.AXIS;
    // sigma = 2
    // gauss = 0.204164 0.180174 0.123832 0.0662822 0.0276306 
    vec4 weight = vec4( 0.204164, 0.304005, 0.0939128 );
    vec4 offset = vec4( 0.0, 1.40733, 3.29421 ) * cGBufferInvSize.AXIS;
 
    float totalWeight = GAUSSIAN_OFFSET + weight.x;
    VALUE_TYPE result = centerValue * totalWeight;
    result += getResult(vTexCoord - vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
    result += getResult(vTexCoord - vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
 
    // 6+1+6 samples
#elif SAMPLES==6
    //vec4 weight = vec4( 0.18571, 0.28870, 0.10364, 0.014805 );        //sigma = 2.15327
    //vec4 offset = vec4( 0.0, 1.4211, 3.3158, 5.2105 ) * cGBufferInvSize.AXIS;
    //vec4 weight = vec4( 0.167292, 0.271598, 0.118306, 0.0264486 );    //sigma = 2.4
    //vec4 offset = vec4( 0.0, 1.4353, 3.3526, 5.2779 ) * cGBufferInvSize.AXIS;
    // sigma = 3.5
    // gauss = 0.121569 0.116706 0.103256 0.0841947 0.0632704 0.0438191 0.0279688
    vec4 weight = vec4( 0.121569, 0.219963, 0.147465, 0.071788 );
    vec4 offset = vec4( 0.0, 1.46943, 3.42905, 5.3896 ) * cGBufferInvSize.AXIS;
 
    float totalWeight = GAUSSIAN_OFFSET + weight.x;
    VALUE_TYPE result = centerValue * totalWeight;
    result += getResult(vTexCoord - vec2(offset.w, 0.0).DIR, centerDepth, weight.w, totalWeight);
    result += getResult(vTexCoord - vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
    result += getResult(vTexCoord - vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
    result += getResult(vTexCoord + vec2(offset.w, 0.0).DIR, centerDepth, weight.w, totalWeight);
    
    // 8+1+8 samples
#elif SAMPLES==8
    // sigma = 5
    // gauss = 0.0875447 0.0858112 0.080814 0.0731235 0.0635705 0.0530986 0.0426126 0.0328565 0.0243407
    float weight0 = 0.087545;
    vec4 weight = vec4( 0.166625, 0.136694, 0.0957112, 0.0571972 );
    vec4 offset = vec4( 1.4850, 3.4651, 5.44522, 7.42556 ) * cGBufferInvSize.AXIS;

    float totalWeight = GAUSSIAN_OFFSET + weight0;
    VALUE_TYPE result = centerValue * totalWeight;
    result += getResult(vTexCoord - vec2(offset.w, 0.0).DIR, centerDepth, weight.w, totalWeight);
    result += getResult(vTexCoord - vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
    result += getResult(vTexCoord - vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord - vec2(offset.x, 0.0).DIR, centerDepth, weight.x, totalWeight);
    result += getResult(vTexCoord + vec2(offset.x, 0.0).DIR, centerDepth, weight.x, totalWeight);
    result += getResult(vTexCoord + vec2(offset.y, 0.0).DIR, centerDepth, weight.y, totalWeight);
    result += getResult(vTexCoord + vec2(offset.z, 0.0).DIR, centerDepth, weight.z, totalWeight);
    result += getResult(vTexCoord + vec2(offset.w, 0.0).DIR, centerDepth, weight.w, totalWeight);

#endif

    gl_FragColor = result / (totalWeight + 0.0001);
}
#endif

