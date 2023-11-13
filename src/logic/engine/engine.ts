import { Job, Scheduler } from "@logic/scheduler";
import { Service, ServiceFactory as ServiceValue } from "@logic/service";
import { EngineState } from "./engineState";

export class Engine {
    private services = new Map<string, Service>();
    private scheduler: Scheduler = new Scheduler();

    private _state: EngineState = EngineState.Stopped;
    public get state(): EngineState { return this._state; }

    public async loadService<T extends Service>(serviceClass: ServiceValue<T>): Promise<void> {
        try {
            const service = new serviceClass(this);
            this.services.set(service.constructor.name, service);
            if (this.state == EngineState.Running)
                await service._startInternal();
            console.debug(`[Engine] Loaded service "${serviceClass.name}"`);
        }
        catch (error) {
            console.error(`[Engine] Failed to load service "${serviceClass.name}": ${error}`);
        }
    }

    public async getService<T extends Service>(serviceClass: ServiceValue<T>): Promise<T> {
        const service = this.services.get(serviceClass.name);

        // Check if the service loaded and if not, load it
        if (service === undefined) {
            await this.loadService(serviceClass);
            return this.services.get(serviceClass.name) as T;
        }

        return service as T;
    }

    public scheduleJob(job: Job) {
        this.scheduler.schedule(job);
    }

    public async start() {
        // Wait for the scheduler to finish the final frame then start
        this.scheduler.start();

        // Start all services
        for (const service of this.services.values())
            await service._startInternal();

        this._state = EngineState.Running;
    }

    public async stop() {
        // Stop all services
        for (const service of this.services.values())
            service.stop();

        // Stop the scheduler loop
        this.scheduler.stop();

        // Reset the engine state
        this._state = EngineState.Stopped;
    }

}