#version 330 core

in vec4 position;
in vec3 normals;
in vec2 tc;

uniform sampler2D tex;

void main(){
	vec3 diffuseTexture;
	diffuseTexture = texture(tex, tc).rgb;

	vec3 color;
	if(normals != vec3(0.0, 0.0, 0.0)){
		vec3 LightPosition = vec3(0.25, 0.5, 1);
		vec3 L = normalize(position.xyz - LightPosition);
		vec3 N = normalize(normals);

		float ambient = 0.5;
		float diffuse = max(dot(L, N), 0.0);

		color = diffuseTexture*ambient + diffuseTexture*diffuse;
	}
	else{
		color = diffuseTexture;
	}

	gl_FragColor = vec4(color, 1.0);
}
