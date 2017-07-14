#version  150  core

uniform  mat4  uViewMtx;
uniform  mat4  uProjMtx;

in   vec3  position;

out  Vertex {
    vec3  normal;
} OUT;

void  main(void)       {
    vec3  tempPos    = position  - vec3 (0,0,1);
    OUT.normal      = transpose(mat3(uViewMtx )) * normalize(tempPos );
   //OUT.normal       = inverse(mat3(viewMatrix )) * normalize(tempPos );
    gl_Position     = uProjMtx * vec4(tempPos , 1.0);
}
