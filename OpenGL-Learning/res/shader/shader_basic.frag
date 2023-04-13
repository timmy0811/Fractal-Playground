#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

uniform vec3 u_CameraOrigin;
uniform vec2 u_Resolution;
uniform float u_Time;

uniform mat4 u_InverseView;
uniform mat4 u_InverseProjection;

uniform float u_DI;
uniform float u_M2;
uniform float u_PowInv;
uniform float u_Pow;
uniform float u_Zoom;
uniform int u_Iterations;
uniform float u_Unknown;
uniform vec2 u_CPos;
uniform float u_Universal;
uniform vec2 u_ZoomCoord;

vec2 distanceToMandelbrot( vec2 c )
{
    #if 0
    {
        float c2 = dot(c, c);
        // skip computation inside M1 - https://iquilezles.org/articles/mset1bulb
        if( 256.0*c2*c2 - 96.0*c2 + 32.0*c.x - 3.0 < 0.0 ) return vec2(0.0, u_Iterations);
        // skip computation inside M2 - https://iquilezles.org/articles/mset2bulb
        if( 16.0*(c2+2.0*c.x+1.0) - 1.0 < 0.0 ) return vec2(0.0, u_Iterations);
    }
    #endif

    // iterate
    float di =  0.0;
    vec2 z  = vec2(0.0);
    float m2 = 0.0;
    vec2 dz = vec2(0.0);
    float iter = u_Iterations;
    for( int i=0; i<u_Iterations; i++ )
    {
        if( m2>u_Unknown ) { 
            di=0.0; 
            iter = float(i);
            break; 
            }

		// Z' -> 2·Z·Z' + 1
        dz = 2.0*vec2(z.x*dz.x-z.y*dz.y, z.x*dz.y + z.y*dz.x) + vec2(1.0,0.0);
			
        // Z -> Z² + c			
        z = vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y ) + c;
			
        m2 = dot(z,z);
    }

    // distance	
	// d(c) = |Z|·log|Z|/|Z'|
	float d = 0.5*sqrt(dot(z,z)/dot(dz,dz))*log(dot(z,z));
    if( di>0.5 ) d=0.0;
	
    return vec2(d, iter);
}

vec2 distanceToJulia(vec2 z){
    vec2 c = vec2(u_Universal);
    int i;
    for (i=0; i<u_Iterations; i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.x * z.y + z.x * z.y) + c.y;

        if ((x * x + y * y) > 10.0) break;
        z.x = x;
        z.y = y;
    }

    return vec2(0.0, i);
}

vec3 hsv2rgb(vec3 hsv) {
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float c = v * s;
    h = mod((h * 6.0), 6.0);
    float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
    vec3 rgb = vec3(0.0);

    if (0.0 <= h && h < 1.0) {
        rgb = vec3(c, x, 0.0);
    } else if (1.0 <= h && h < 2.0) {
        rgb = vec3(x, c, 0.0);
    } else if (2.0 <= h && h < 3.0) {
        rgb = vec3(0.0, c, x);
    } else if (3.0 <= h && h < 4.0) {
        rgb = vec3(0.0, x, c);
    } else if (4.0 <= h && h < 5.0) {
        rgb = vec3(x, 0.0, c);
    } else if (5.0 <= h && h < 6.0) {
        rgb = vec3(c, 0.0, x);
    }

    float m = v - c;
    return rgb + vec3(m);
}

void main(){
    vec2 frag_Coord = vec2(gl_FragCoord.x / u_Resolution.x, gl_FragCoord.y / u_Resolution.y) * 2.0 - 1.0;
    frag_Coord.x *= u_Resolution.x / u_Resolution.y;
    frag_Coord += u_CPos;

    float tz = 0.05 * u_Zoom;
    float zoo = pow(0.5, 13.0 * tz);
	vec2 c = u_ZoomCoord + frag_Coord * zoo;

    vec2 distToMb = distanceToMandelbrot(c);
    // vec3 color = vec3(pow(u_PowInv * distToMb.x / zoo, u_Pow));

    float hue = distToMb.y / u_Iterations;
    float saturation = 1.0;
    float value = distToMb.y < u_Iterations ? 1.0 : 0.0;
    vec3 hsvVal = vec3(hue, saturation, value);
    vec3 color = hsv2rgb(hsvVal);
    color = pow(color, vec3(0.4545));
    
    o_Color = vec4(color, 1.0);
}