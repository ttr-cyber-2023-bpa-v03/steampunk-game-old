import { Service } from "@logic/service";
import { Scene } from "./scene";

export class GraphicsService extends Service {
    private scene!: Scene;

    public async start(): Promise<void> {
        this.scene = new Scene();
        await this.scene.init(document.getElementById("frame") as HTMLCanvasElement);
    }

    public async stop(): Promise<void> {
        console.log("Stop graphics service");
    }

    public async step(): Promise<void> {
        this.scene.render();
    }
}