import { ExternalPromise } from "@helpers/promises";
import { Shader } from "./shader";
import { RenderObject } from "./objects/renderObject";
import { ShaderType, idFromShaderType } from "./shaderType";

/**
 * Scenes will hold all the objects that will be rendered.
 * They will also determine the mathematical transformations from
 * 2D vector sizes/positions to matrices.
 */
export class Scene {
    public ready = new ExternalPromise<void>();
    public gl!: WebGLRenderingContext;
    public canvas!: HTMLCanvasElement;
    private shaders: Map<string, Shader> = new Map();

    // Objects based on their shader program
    private objects: Map<Shader, RenderObject[]> = new Map();

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
        this.gl = gl;

        console.debug("[Scene.init] Scene initialized");
        this.ready.resolve();
    }

    /**
     * Resize the viewport and canvas.
     * 
     * @param width The new width of the viewport.
     * @param height The new height of the viewport.
     */
    public setViewportSize(width: number, height: number) {
        this.canvas.width = window.innerWidth;
        this.canvas.height = window.innerHeight;
        this.gl.viewport(0, 0, width, height);
    }

    private async glCompileShader(shaderId: number, sourceFile: string): Promise<WebGLShader> {
        const gl = this.gl;

        // Read the contents from the source via fetch
        const sourceReq = await fetch(sourceFile, { method: "GET" });
        const body = await sourceReq.text();
    
        // Create a shader object of type shaderId
        const shader = gl.createShader(shaderId);
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

    public async loadShader(id: string, shaders: [ ShaderType, string ][]): Promise<void> {
        // Compile the shaders and create a shader program
        const compiledShaders = shaders.map(x => this.glCompileShader(idFromShaderType(this.gl, x[0]), x[1]));
        const shader = new Shader(this.gl, id, await Promise.all(compiledShaders));

        // Cache the shader
        this.shaders.set(id, shader);
    }

    public getShader(id: string): Shader {
        const shader = this.shaders.get(id);
        if (shader == null)
            throw new Error(`Shader ${id} is not loaded`);
        return shader;
    }

    public addObject(object: RenderObject) {
        // Check if there's a shader entry for this object
        if (!this.objects.has(object.shader))
            this.objects.set(object.shader, []);

        const objects = this.objects.get(object.shader)!;
        let index = 0;

        // Find location in the array where z-index is less than the object's
        // z-index
        for (let i = 0; i < objects.length; i++) {
            if (objects[i].zIndex < object.zIndex) index++;
            else break;
        }

        // Add the object to the scene in that index
        objects.splice(index, 0, object);
    }

    public removeObject(object: RenderObject) {
        // Check if there's a shader entry for this object
        const objects = this.objects.get(object.shader);
        if (objects == null)
            return;

        // Find the object in the array
        const index = objects.indexOf(object);
        if (index == -1)
            return;

        // Remove the object from the scene
        objects.splice(index, 1);
    }

    public render() {
        const gl = this.gl;

        // Clear the screen
        gl.clearColor(0, 0, 0, 1);
        gl.clear(this.gl.COLOR_BUFFER_BIT);

        // Render each object
        for (let [ shader, objects ] of this.objects) {
            gl.useProgram(shader.program);
            for (let object of objects)
                object.draw();
        }
    }
}