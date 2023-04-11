#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

uniform vec3 u_CameraOrigin;
uniform vec2 u_Resolution;
uniform float u_Time;

uniform mat4 u_InverseView;
uniform mat4 u_InverseProjection;

void main(){
    vec2 frag_Coord = vec2(gl_FragCoord.x / u_Resolution.x, gl_FragCoord.y / u_Resolution.y);

    vec3 color = vec3(frag_Coord, 0.0);

    color = pow(color, vec3(0.4545));
    o_Color = vec4(color, 1.0);
}