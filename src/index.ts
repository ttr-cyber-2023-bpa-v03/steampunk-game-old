import "@styles/index.scss";

import { Scheduler } from "@logic/scheduler";

import { Renderer } from "@logic/graphics";
import { JobOperation } from "@logic/scheduler/Job";

const canvas = document.getElementById("frame") as HTMLCanvasElement;

const scheduler = new Scheduler();
scheduler.start();

scheduler.schedule(Scheduler.createJob("init", async () => {
    console.log("running");
    return JobOperation.Abort;
}));
