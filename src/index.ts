import "@styles/index.scss";

import { Camera, Renderer } from "@logic/graphics";

const canvas = document.getElementById("frame") as HTMLCanvasElement;
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

const renderer = new Renderer();
renderer.init(canvas).then(ctx => {
    const camera = new Camera(ctx);

    // TODO: we need a task scheduler for this
    setInterval(() => {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        camera.render();
    }, 0);
});