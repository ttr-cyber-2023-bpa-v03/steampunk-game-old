import "@styles/index.scss";

import { JobOperation, Scheduler } from "@logic/scheduler";
import { Scene } from "@logic/graphics";
import { Rectangle } from "@logic/graphics/objects/Rectangle";

const scheduler = new Scheduler();
scheduler.start();

const scene = new Scene();
scene.init(document.getElementById("frame") as HTMLCanvasElement).then(async () => {
    const rect = new Rectangle(scene);
    rect.visible = true;
    rect.position = [10, 10];
    rect.size = [100, 100];
    await rect.allocate();

    scene.objects.push(rect);
    scheduler.schedule(Scheduler.createJob("render", async () => {
        await scene.render();
        return JobOperation.Resume;
    }));

    // window mouse event
    window.addEventListener("mousemove", (ev) => {
        rect.position = [ev.clientX, ev.clientY];
        rect.update();
    });
});