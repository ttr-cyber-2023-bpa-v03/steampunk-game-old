import { Engine } from "./engine";
import { Job } from "./scheduler";

export type ServiceFactory<T extends Service> = new (engine: Engine) => T;

export abstract class Service {
    public engine: Engine;

    private _running: boolean = false;
    public get running(): boolean { return this._running; }

    public constructor(engine: Engine) {
        this.engine = engine;
    }

    public async _startInternal(): Promise<void> {
        this._running = true;
        await this.start();
        if (this.step !== Service.prototype.step) 
            this.engine.scheduleJob(this.step.bind(this));
    }

    public async _stopInternal(): Promise<void> {
        await this.stop();
        this._running = false;
    }

    public abstract start(): Promise<void>;

    public abstract stop(): Promise<void>;

    public async step(): Promise<void> {
        throw new Error("Not implemented");
    }
}