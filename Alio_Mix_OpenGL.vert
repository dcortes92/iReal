varying vec3  texCoord;
void main(void)
{   
	vec3 multipliers;
	multipliers.x = 1.0;
	multipliers.y = 0.125;
	multipliers.z = 1.0;
	gl_Position = vec4(gl_Vertex.xy, 0.0 , 1.0);

	//gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z , 1.0);
	gl_Position = sign(gl_Position);
    
	// Texture coordinate for screen aligned (in correct range):	
	texCoord.xy = (gl_Position.xy + vec2( 1.0 )) / vec2(2.0);
	texCoord.z = texCoord.x * 1920.0 * 3.0 + texCoord.y * 1080.0 + 0.5;
	texCoord = texCoord * multipliers; 
}