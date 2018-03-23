precision mediump float;
uniform sampler2D material_texture_diffuse1;
in vec2 TexCoord0;
in float sIDs0;   
out vec4 FragColor;
                   
void main()
{                                                                                                       
	FragColor =  vec4(sIDs0, 0.0, 0.0, 1.0);//texture(material_texture_diffuse1, TexCoord0);
}