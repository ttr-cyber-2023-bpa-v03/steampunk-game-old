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

    public render(o: any) {
        // import gl, canvas
        const gl = this.context.gl;
        const canvas = this.context.canvas;
        const program = this.context.program;

        // Set the resolution
        gl.viewport(0, 0, canvas.clientWidth, canvas.clientHeight);
        gl.uniform2f(this.glResolution, canvas.clientWidth, canvas.clientHeight);

        // Clear the screen
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT);

        // Draw the rectangle
        gl.drawArrays(gl.TRIANGLES, 0, 6);
    }
}