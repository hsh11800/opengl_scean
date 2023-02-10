
#version 150

in vec3 FN;
in vec3 FL;
in vec3 FV;
in float Fd;
in vec2 texCoord;
out vec4 fColor;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
uniform sampler2D texture;
void main() 
{ 
	

	vec3 N = normalize(FN);
	vec3 V = normalize(FV);
	vec3 L = normalize(FL);
	vec3 H = normalize(L+V);
	

	
	float attenuation = 1.0/(0.5+0.1*Fd+0.01*pow(Fd,2));
	
	vec4 ambient = AmbientProduct;
	float kd=max(dot(L,N),0.0);
	vec4 diffuse=kd*DiffuseProduct;
	float ks=pow(max(dot(N,H),0.0),Shininess);
	vec4 specular=ks*SpecularProduct;

	//texture mapping

	fColor = (ambient+ attenuation * (diffuse + specular)) * texture2D( texture, texCoord );
	//fColor = (ambient+ attenuation * (diffuse + specular));
	//fColor = specular;
	//fColor = ambient;
	//fColor = (diffuse);
	//fColor = vec4(1.0,0.0,0.0,1.0);
} 

