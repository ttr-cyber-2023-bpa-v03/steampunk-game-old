import { ExternalPromise } from "@helpers/promises";
import { vec2, mat4 } from "gl-matrix";
import { Shader } from ".";
import { RenderObject } from "./objects/RenderObject";

/**
 * Scenes will hold all the objects that will be rendered.
 * They will also determine the mathematical transformations from
 * 2D vector sizes/positions to matrices.
 */
export class Scene {
    public gl = new ExternalPromise<WebGLRenderingContext>();
    public canvas!: HTMLCanvasElement;
    private shaderCache: Map<string, Shader> = new Map();

    /**
     * The objects in the scene.
     */
    public objects: RenderObject[] = [];

    /**
     * Initializes WebGL and the renderer.
     * 
     * @throws {Error} If there was a problem initializing WebGL.
     * @param canvas The HTML `canvas` element to render to.
     */
    public async init(canvas: HTMLCanvasElement): Promise<void> {
        // Load WebGL
        const gl = canvas.getContext("webgl");
        if (gl == null)
            throw new Error("WebGL is not supported");

        // Expose canvas
        this.canvas = canvas;
        
        // Expose the context and set up the viewport
        this.gl.resolve(gl as WebGLRenderingContext);
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        gl.viewport(0, 0, canvas.clientWidth, canvas.clientHeight);

        console.debug("[Scene.init] Scene initialized");
    }

    private async glCompileShader(shaderEnum: number, sourceFile: string): Promise<WebGLShader> {
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

        console.debug(`[Scene.glCompileShader] Compiled shader ${sourceFile}`);

        return shader;
    }

    public async getShader(vertexShader: string, fragmentShader: string): Promise<Shader> {
        const gl = await this.gl;
        const id =  `${vertexShader}${fragmentShader}`;

        // Check the cache
        if (this.shaderCache.has(id))
            return this.shaderCache.get(id)!;

        const vertShader = await this.glCompileShader(gl.VERTEX_SHADER, vertexShader);
        const fragShader = await this.glCompileShader(gl.FRAGMENT_SHADER, fragmentShader);
        const shader = new Shader(gl, vertShader, fragShader);

        // Cache the shader
        this.shaderCache.set(id, shader);

        return shader;
    }

    public async render() {
        const gl = await this.gl;

        // Clear the screen
        gl.clearColor(0, 0, 0, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);

        // Render each object
        for (const obj of this.objects) {
            if (!obj.visible)
                continue;

            // Draw the object
            await obj.draw();
        }
    }
}