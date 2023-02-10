
#version 150

in  vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
out vec3 FN;
out vec3 FV;
out vec3 FL;
out float Fd;
uniform mat4 Projection;
uniform mat4 Model;
uniform mat4 View;
uniform vec4 rquat;
out vec2 texCoord;
uniform vec4 LightPosition;

vec4 multq(vec4 a, vec4 b)
{
	vec3 aa = vec3(a[1], a[2], a[3]);
	vec3 bb = vec3(b[1], b[2], b[3]);
	vec3 cc = a[0] * bb + b[0] * aa + cross(bb, aa);
	return vec4(b[0] * a[0] - dot(aa, bb), cc);
}
vec4 invq(vec4 a){
	return (vec4(a.x,-a.yzw)/dot(a,a));
}

void main() 
{


	//vertex coordinate caculation
	vec4 model_co= View*Model*vPosition;
	vec4 pquat = vec4(0.0, model_co.xyz);//position quatanion
	pquat = multq(rquat, multq(pquat,invq(rquat)));
	vec3 pos = vec3(pquat[1],pquat[2],pquat[3]);
	gl_Position =  Projection * vec4(pos,1.0);
	
	//shading caculation	
	vec4 viewpos = View*vec4(0.0,0.0,0.0,1.0);
	vec4 vquat = vec4(0.0,viewpos.xyz);//view quatanion
	vquat = multq(rquat, multq(vquat , invq(rquat)));
	//viewpos = vec4(vquat[1], vquat[2], vquat[3], 1.0);

	//normal 
	vec4 npos = (View * Model * vec4(vNormal,0.0));
	vec4 nquat = vec4(0.0,npos.xyz);
	nquat = multq(rquat,multq(nquat,invq(rquat)));
	FN = nquat.yzw;
	//FN = npos.xyz;
	FV = viewpos.xyz - pos;
	FL=(LightPosition).xyz;
	
	Fd = sqrt(dot(LightPosition.xyz-pos,LightPosition.xyz-pos));
	if (LightPosition.w!=0.0){
		
		//FL = (View * LightPosition).xyz;
		vec4 lquat = vec4(0.0,( LightPosition).xyz);//light quatanion
		lquat = multq(rquat, multq(lquat , invq(rquat)));
		FL = vec3(lquat.yzw);
		
		
	}

	//texture mapping
	texCoord = vTexCoord;
} 
