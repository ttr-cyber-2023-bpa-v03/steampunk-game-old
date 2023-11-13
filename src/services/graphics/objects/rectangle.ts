import { RenderObject } from "./renderObject";
import { vec3 } from "gl-matrix"

export class Rectangle extends RenderObject {
    public color: vec3 = vec3.fromValues(1.0, 1.0, 1.0);
    public vertexBuffer: WebGLBuffer | null = null;

    public async allocate() {
        this.shader = this.scene.getShader("solid");
        this.vertexBuffer = this.scene.gl.createBuffer();
        if (this.vertexBuffer == null)
            throw new Error("Failed to create vertex buffer");
    }

    public async update() {
        const gl = this.scene.gl;

        // Destructure position, size, and anchor for better readability
        const [ x, y ] = this.position;
        const [ width, height ] = this.size;
        const [ anchorX, anchorY ] = this.anchor;

        // Update the vertex buffer
        const vertices = new Float32Array([
            x - width * anchorX,       y - height * anchorY,       // Top left
            x + width * (1 - anchorX), y - height * anchorY,       // Top right
            x + width * (1 - anchorX), y + height * (1 - anchorY), // Bottom right
            x - width * anchorX,       y + height * (1 - anchorY)  // Bottom left
        ]);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
    }

    public draw() {
        const gl = this.scene.gl;
        gl.uniform2f(gl.getUniformLocation(this.shader.program, "u_resolution"), this.scene.canvas.width, this.scene.canvas.height);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(0);

        gl.uniform3fv(this.shader.uniform("uColor"), this.color);
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        gl.disableVertexAttribArray(0);
    }
}