import "@styles/index.scss";

import { Camera, Renderer } from "@logic/graphics";

const canvas = document.getElementById("frame") as HTMLCanvasElement;

const renderer = new Renderer();
renderer.init(canvas).then(ctx => {
    const camera = new Camera(ctx);

    const o: any = {
        hue: 0
    };

    // TODO: we need a task scheduler for this
    setInterval(() => {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

        o.hue += 0.1;

        camera.render(o);
    }, 0);
});