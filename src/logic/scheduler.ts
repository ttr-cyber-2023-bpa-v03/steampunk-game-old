const DEFAULT_FRAME_DELAY: number = 1000 / 60;

import { ExternalPromise } from "@helpers/promises";

export type JobRet = Promise<void>;
export type Job = () => JobRet;

/**
 * Schedules tasks for the game. This class contains the core game loop.
 */
export class Scheduler {
    private jobs: Set<Job> = new Set();
    private stepPool: JobRet[] = [];

    // The promise that will be resolved when the scheduler is stopped and rejected if
    // the scheduler crashes.
    public running = new ExternalPromise<void>();

    // This is the ID of the scheduler's next runthrough. This is used to cancel
    // the scheduler if needed.
    private animationFrameId: number | null = null;

    // micro-optimization: bind once, use many times
    private boundLoop: () => void = this.loop.bind(this);

    // The time of the last frame
    private _frameTime: number = 0;

    /**
     * The minimum amount of elapsed time between frames. This is measured in
     * milliseconds.
     */
    public frameDelay: number = DEFAULT_FRAME_DELAY;

    /**
     * Starts the scheduler.
     */
    public start() {
        this._frameTime = performance.now();
        this.loop();
    }

    /**
     * Stops the scheduler.
     */
    public async stop() {
        if (this.animationFrameId !== null) {
            cancelAnimationFrame(this.animationFrameId);
            this.animationFrameId = null;
        }

        // Wait for all jobs to complete
        await Promise.all(this.stepPool);
        this.running.resolve();
    }

    /**
     * Kills the scheduler and propagates an error to the promise.
     * 
     * @param err The error to propagate.
     */
    private killScheduler(err: Error) {
        // Stop the scheduler and propagate the error to the promise
        if (this.animationFrameId !== null) {
            cancelAnimationFrame(this.animationFrameId);
            this.running.reject(err);
            this.animationFrameId = null;
        }
    }

    /**
     * Schedules a job to run on each frame.
     * 
     * @param job The job to schedule.
     */
    public schedule(job: Job) {
        this.jobs.add(job);
    }

    /**
     * The core game loop. This is called on each frame.
     */
    private async loop() {
        const currentTime = performance.now();
        const elapsed = currentTime - this._frameTime;

        // Respect the frame delay by rescheduling the loop if the delay has not
        // elapsed yet.
        if (elapsed < this.frameDelay) {
            if (!this.running.finished)
                this.animationFrameId = requestAnimationFrame(this.boundLoop);
            return;
        }

        // Run scheduled jobs
        for (const job of this.jobs) {
            this.stepPool.push(job().catch((err: Error) => {
                this.killScheduler(new Error("Job threw an exception.", { cause: err }));
            }));
        }

        // Wait for all step operations to complete then clear the pool
        await Promise.all(this.stepPool);
        this.stepPool.length = 0;

        // As long as the loop isn't stopped, reschedule it for the next frame.
        if (!this.running.finished) {
            this._frameTime = currentTime - (elapsed % this.frameDelay);
            this.animationFrameId = requestAnimationFrame(this.boundLoop);
        }
    }
}
