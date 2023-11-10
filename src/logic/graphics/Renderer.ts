import { OuterPromise } from "@helpers/promises";
import { RenderContext } from "./RenderContext";

import { vec2, mat4 } from "gl-matrix";

export class Renderer {
    private gl = new OuterPromise<WebGLRenderingContext>();

    /**
     * Initializes WebGL and the renderer.
     * 
     * @throws {Error} If there was a problem initializing WebGL.
     * @param canvas The HTML `canvas` element to render to.
     */
    public async init(canvas: HTMLCanvasElement): Promise<RenderContext> {
        // Load WebGL
        const gl = canvas.getContext("webgl");
        if (gl == null)
            throw new Error("WebGL is not supported");

        // Expose the context and set up the viewport
        this.gl.resolve(gl as WebGLRenderingContext);
        gl.viewport(0, 0, canvas.width, canvas.height);

        // Start compiling shaders (note that these are returning promises)
        const vShader = this.compileShader(gl.VERTEX_SHADER, "shaders/vertex.glsl");
        const fShader = this.compileShader(gl.FRAGMENT_SHADER, "shaders/fragment.glsl");

        // Set up the program
        const program = await this.loadProgram([ await vShader, await fShader ]);
        gl.useProgram(program);

        // Finally create a context
        return { canvas, gl, program };
    }
    
    private async compileShader(shaderEnum: number, sourceFile: string) {
        const gl = await this.gl;

        // Read the contents from the source via fetch
        const sourceReq = await fetch(sourceFile, { method: "GET" });
        const body = await sourceReq.text();
    
        // Create a shader object of type shaderEnum
        const shader = gl.createShader(shaderEnum);
        if (shader == null)
            throw new Error("Failed to create WebGL shader object");
    
        // Send the shader to WebGL and compile it
        gl.shaderSource(shader, body);
        gl.compileShader(shader);
    
        // Check for success
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {    
            // Unload the shader and throw an error
            gl.deleteShader(shader);
            throw new Error("Failed to compile shader: " + gl.getShaderInfoLog(shader));
        }

        return shader;
    }

    private async loadProgram(shaders: WebGLShader[]): Promise<WebGLProgram> {
        const gl = await this.gl;

        // Create a program object and validate it
        const program = gl.createProgram();
        if (program == null)
            throw new Error("Failed to create WebGL program object");

        // Attach all shaders to the program, then link the program
        for (const shader of shaders)
            gl.attachShader(program, shader);
        gl.linkProgram(program);

        // Check for success
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            // Unload the program and throw an error
            gl.deleteProgram(program);
            throw new Error("Failed to link program: " + gl.getProgramInfoLog(program));
        }

        return program;
    }
}