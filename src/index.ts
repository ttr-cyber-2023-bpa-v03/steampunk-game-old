import "@styles/index.scss";

import { JobOperation, Scheduler } from "@logic/scheduler";
import { Scene } from "@logic/graphics";
import { Rectangle } from "@logic/graphics/objects/Rectangle";
import { ShaderType } from "@logic/graphics/ShaderType";

const scheduler = new Scheduler();
scheduler.start();

scheduler.catch((err) => {
    if (err.cause)
        console.error(err.message, err.cause);
});

const scene = new Scene();
scene.init(document.getElementById("frame") as HTMLCanvasElement).then(async () => {
    await scene.loadShader("solid", [
        [ ShaderType.Vertex, "shaders/vertex.glsl" ],
        [ ShaderType.Fragment, "shaders/solid.glsl" ]
    ]);

    await scene.loadShader("light", [
        [ ShaderType.Vertex, "shaders/vertex.glsl" ],
        [ ShaderType.Fragment, "shaders/light.glsl" ]
    ]);

    scene.setViewportSize(window.innerWidth, window.innerHeight);

    const rect = new Rectangle(scene);
    rect.visible = true;
    rect.position = [10, 10];
    rect.size = [100, 100];
    rect.zIndex = 1;
    rect.color = [0.0, 1.0, 0.0];
    await rect.allocate();
    
    scene.addObject(rect);

    scheduler.schedule(Scheduler.createJob("render", async () => {
        scene.render();
        return JobOperation.Resume;
    }));

    // window mouse event
    window.addEventListener("mousemove", (ev) => {
        rect.position = [ev.clientX, ev.clientY];
        rect.update();
    });

    window.addEventListener("resize", () => {
        scene.setViewportSize(window.innerWidth, window.innerHeight);
    });
});