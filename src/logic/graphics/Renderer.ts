import { ExternalPromise } from "@helpers/promises";

import { vec2, mat4 } from "gl-matrix";

export class Renderer {
    private gl = new ExternalPromise<WebGLRenderingContext>();

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

        // Expose the context and set up the viewport
        this.gl.resolve(gl as WebGLRenderingContext);
        gl.viewport(0, 0, canvas.width, canvas.height);


    }
}