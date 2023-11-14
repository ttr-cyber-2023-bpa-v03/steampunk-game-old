import { ExternalPromise } from "@helpers/promises";
import { Engine } from "./engine";

export type ServiceCreator<T extends Service> = new (engine: Engine) => T;

// Expose internal methods for the engine
export interface ServiceInternalHack {
    _startInternal(): Promise<void>;
    _stopInternal(): Promise<void>;
}

export abstract class Service {
    public engine: Engine;
    public ready: ExternalPromise<void> = new ExternalPromise<void>();

    private _running: boolean = false;
    public get running(): boolean { return this._running; }

    public constructor(engine: Engine) {
        this.engine = engine;
        this.ready = new ExternalPromise<void>();
    }

    // eslint-disable-next-line no-unused-vars
    private async _startInternal(): Promise<void> {
        this._running = true;
        await this.start();

        // Hacky but basically it checks if the step function is overwritten
        // and if it is it'll schedule
        if (this.step !== Service.prototype.step) 
            this.engine.scheduleJob(this.step.bind(this));

        this.ready.resolve();
    }

    public async _stopInternal(): Promise<void> {
        this.ready = new ExternalPromise<void>();
        await this.stop();
        this._running = false;
    }

    public abstract start(): Promise<void>;

    public abstract stop(): Promise<void>;

    public async step(): Promise<void> {
        throw new Error("Not implemented");
    }
}