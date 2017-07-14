#version  150  core

uniform  samplerCube   cubeTex;

in  Vertex {
    vec3  normal;
} IN;

out  vec4  FragColor;

void  main(void)    {
    FragColor = texture(cubeTex ,normalize(IN.normal ));
}
