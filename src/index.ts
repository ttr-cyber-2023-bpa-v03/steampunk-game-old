import "@styles/index.scss";

import { Engine } from "@logic/engine";
import { GraphicsService } from "services/graphics";
import { Service } from "@logic/service";

/*const canvas = document.getElementById("frame") as HTMLCanvasElement;

const scheduler = new Scheduler();
scheduler.start();

const scene = new Scene();
scene.init(canvas);

const engine = new Engine();
engine.loadService<GraphicsService>();*/

const engine = new Engine();
await engine.start();

engine.getService(GraphicsService);