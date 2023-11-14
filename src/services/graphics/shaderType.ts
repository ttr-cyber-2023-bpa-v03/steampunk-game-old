export enum ShaderType {
    Vertex,
    Fragment
}

export function idFromShaderType(gl: WebGLRenderingContext, type: ShaderType): number {
    switch (type) {
    case ShaderType.Vertex:
        return gl.VERTEX_SHADER;
    case ShaderType.Fragment:
        return gl.FRAGMENT_SHADER;
    }
}