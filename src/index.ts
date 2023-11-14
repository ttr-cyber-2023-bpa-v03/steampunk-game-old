import "@styles/index.scss";

import { Engine } from "@logic/engine";
import { GraphicsService } from "services/graphics";
import { Service } from "@logic/service";
import { Rectangle } from "services/graphics/objects/rectangle";
import { ShaderType } from "services/graphics/shaderType";

/*const canvas = document.getElementById("frame") as HTMLCanvasElement;

const scheduler = new Scheduler();
scheduler.start();

const scene = new Scene();
scene.init(canvas);

const engine = new Engine();
engine.loadService<GraphicsService>();*/

const engine = new Engine();
await engine.start();

const graphics = await engine.getService(GraphicsService);
await graphics.ready;
await graphics.scene.loadShader("solid", [
    [ ShaderType.Vertex, "shaders/vertex.glsl" ],
    [ ShaderType.Fragment, "shaders/solid.glsl" ],
]);

const rect = new Rectangle(graphics.scene);
rect.position = [ 100, 100 ];
rect.size = [ 100, 200 ];
rect.color = [ 1, 0, 0 ];
rect.visible = true;
await rect.allocate();

window.onmousemove = async (e) => {
    rect.position = [ e.clientX, e.clientY ];
    await rect.update();
}

graphics.scene.addObject(rect);