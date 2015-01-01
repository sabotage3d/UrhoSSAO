#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
 
#ifdef BASE
    varying vec4 vColor;
#endif
#ifdef EDGE
    varying vec4 vScreenPos;
#endif
 
#ifdef COMPILEPS
float color_difference(in vec4 sc, in vec4 nc){
  return abs(sc.r-nc.r)+abs(sc.g-nc.g)+abs(sc.b-nc.b);
}
 
vec4 get_pixel(in sampler2D tex, in vec2 coords, in float dx, in float dy) {
 return texture2D(tex,coords + vec2(dx, dy));
}
 
// returns pixel color
float IsEdge(in sampler2D tex, in vec2 coords){
  float dxtex = 1.0 / 1920.0; //image width;
  float dytex = 1.0 / 1080.0; //image height;
  float cd[8];
 
  vec4 sc = get_pixel(tex,coords,float(0)*dxtex,float(0)*dytex);
  cd[0] = color_difference( sc, get_pixel(tex,coords,float(-1)*dxtex,float(-1)*dytex) );//color of itself
  cd[1] = color_difference( sc, get_pixel(tex,coords,float(-1)*dxtex,float(0)*dytex) );
  cd[2] = color_difference( sc, get_pixel(tex,coords,float(-1)*dxtex,float(1)*dytex) );
  cd[3] = color_difference( sc, get_pixel(tex,coords,float(0)*dxtex,float(1)*dytex) );
 
  vec4 alt1 = get_pixel(tex,coords,float(1)*dxtex,float(1)*dytex);
  vec4 alt2 = get_pixel(tex,coords,float(1)*dxtex,float(0)*dytex);
  vec4 alt3 = get_pixel(tex,coords,float(1)*dxtex,float(-1)*dytex);
  vec4 alt4 = get_pixel(tex,coords,float(0)*dxtex,float(-1)*dytex);
 
  if( length(alt1.rgb) < 0.1 ){ cd[4] = color_difference( sc, alt1 ); }else{ cd[4]=0.0; }
  if( length(alt2.rgb) < 0.1 ){ cd[5] = color_difference( sc, alt2 ); }else{ cd[5]=0.0; }
  if( length(alt3.rgb) < 0.1 ){ cd[6] = color_difference( sc, alt3 ); }else{ cd[6]=0.0; }
  if( length(alt4.rgb) < 0.1 ){ cd[7] = color_difference( sc, alt4 ); }else{ cd[7]=0.0; }
 
  return cd[0]+cd[1]+cd[2]+cd[3]+cd[4]+cd[5]+cd[6]+cd[7];
}
#endif
 
void VS(){
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
 
    #ifdef EDGE
      vScreenPos = GetScreenPos(gl_Position);
    #endif
 
    #ifdef BASE
        vec3 n = iNormal+vec3(1.0);
        n*=0.5;
        vColor = vec4(n,1.0);
    #endif
}
 
void PS(){
    #ifdef BASE
        vec4 diffColor = vColor;
        gl_FragColor = diffColor;
    #endif
 
    #ifdef EDGE
      vec4 color = vec4(0.0,0.0,0.0,1.0);
      if(IsEdge(sEnvMap,vScreenPos.xy / vScreenPos.w)>=2.0){
        color.rgba = vec4(1.0);
        //color = get_pixel(sEnvMap,vScreenPos.xy / vScreenPos.w,float(0)*(1.0/1920.0),float(0)*(1.0/1080.0));
      }
      gl_FragColor = color;
    #endif
}