export class Shader {
    public readonly program: WebGLProgram;
    private gl: WebGLRenderingContext;

    public constructor(gl: WebGLRenderingContext, vertexShader: WebGLShader, fragmentShader: WebGLShader) {
        // Create a program
        const program = gl.createProgram();
        if (program == null)
            throw new Error("Failed to create WebGL program object");

        // Attach the shaders
        gl.attachShader(program, vertexShader);
        gl.attachShader(program, fragmentShader);

        // Link the program
        gl.linkProgram(program);

        // Check for success
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            // Unload the program and throw an error
            gl.deleteProgram(program);
            throw new Error("Failed to link program: " + gl.getProgramInfoLog(program));
        }

        this.program = program;
        this.gl = gl;
    }

    public attr(name: string): number {
        const attr = this.gl.getAttribLocation(this.program, name);
        if (attr == -1)
            throw new Error("Failed to get attribute location: " + name);
        return attr;
    }

    public uniform(name: string): WebGLUniformLocation {
        const uniform = this.gl.getUniformLocation(this.program, name);
        if (uniform == null)
            throw new Error("Failed to get uniform location: " + name);
        return uniform;
    }

    public use() {
        this.gl.useProgram(this.program);
    }

    public delete() {
        this.gl.deleteProgram(this.program);
    }
}