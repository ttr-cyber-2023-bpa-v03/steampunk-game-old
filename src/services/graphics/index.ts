import { Service } from "@logic/service";
import { Scene } from "./scene";

export class GraphicsService extends Service {
    public scene!: Scene;

    private resizeViewport(): void {
        this.scene.setViewportSize(window.innerWidth, window.innerHeight);
    }

    public async start(): Promise<void> {
        this.scene = new Scene();
        await this.scene.init(document.getElementById("frame") as HTMLCanvasElement);

        this.resizeViewport();
        window.addEventListener("resize", this.resizeViewport.bind(this));
    }

    public async stop(): Promise<void> {
        console.log("Stop graphics service");
    }

    public async step(): Promise<void> {
        this.scene.render();
    }
}