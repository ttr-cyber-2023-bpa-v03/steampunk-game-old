import { RenderContext } from "./RenderContext";

/**
 * This is the first abstraction of the graphics engine. It is responsible
 * for cooperating with WebGL to render the screen by doing calculations and
 * sending data to the GPU.
 */
export class Camera {
    private context: RenderContext;
    
    public glPosition: number;
    public glResolution: WebGLUniformLocation;

    constructor(ctx: RenderContext) {
        this.glPosition = ctx.gl.getAttribLocation(ctx.program, "a_position");
        this.glResolution = ctx.gl.getUniformLocation(ctx.program, "u_resolution")!;
        this.context = ctx;   
    }

    public render() {
        // import gl, canvas
        const gl = this.context.gl;
        const canvas = this.context.canvas;
        
        // Set the resolution
        gl.viewport(0, 0, canvas.clientWidth, canvas.clientHeight);
        gl.uniform2f(this.glResolution, canvas.clientWidth, canvas.clientHeight);

        // Clear the screen
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT);

        // Draw red square from 10,10 to 100,100
        // We're going to 100% need some abstraction for matrices
        const buffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
            10, 10,
            100, 10,
            10, 100,
            10, 100,
            100, 10,
            100, 100
        ]), gl.STATIC_DRAW);
        gl.enableVertexAttribArray(this.glPosition);
        gl.vertexAttribPointer(this.glPosition, 2, gl.FLOAT, false, 0, 0);

        // Draw the rectangle
        gl.drawArrays(gl.TRIANGLES, 0, 6);
    }
}