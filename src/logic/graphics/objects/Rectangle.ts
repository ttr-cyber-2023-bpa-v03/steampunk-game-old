import { Shader } from "..";
import { RenderObject } from "./RenderObject";
import { vec3 } from "gl-matrix"

export class Rectangle extends RenderObject {
    public color: vec3 = vec3.fromValues(1.0, 0.0, 1.0);
    public vertexBuffer: WebGLBuffer | null = null;

    public shader!: Shader;

    public async allocate() {
        const gl = await this.scene.gl;

        // Load shaders
        this.shader = await this.scene.getShader("shaders/vertex.glsl", "shaders/solid.glsl");
        console.debug("[Rectangle.allocate] Loaded shaders");
        
        this.vertexBuffer = gl.createBuffer();
        if (this.vertexBuffer == null)
            throw new Error("Failed to create vertex buffer");
    }

    public async update() {
        const gl = await this.scene.gl;

        // Update the vertex buffer
        const vertices = new Float32Array([
            this.position[0] - this.size[0] * this.anchor[0], this.position[1] - this.size[1] * this.anchor[1],
            this.position[0] + this.size[0] * (1 - this.anchor[0]), this.position[1] - this.size[1] * this.anchor[1],
            this.position[0] + this.size[0] * (1 - this.anchor[0]), this.position[1] + this.size[1] * (1 - this.anchor[1]),
            this.position[0] - this.size[0] * this.anchor[0], this.position[1] + this.size[1] * (1 - this.anchor[1])
        ]);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
    }

    public async draw() {
        const gl = await this.scene.gl;

        this.shader.use();
        gl.uniform2f(gl.getUniformLocation(this.shader.program, "u_resolution"), this.scene.canvas.width, this.scene.canvas.height);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(0);

        gl.uniform3fv(this.shader.uniform("uColor"), this.color);
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        gl.disableVertexAttribArray(0);
    }
}