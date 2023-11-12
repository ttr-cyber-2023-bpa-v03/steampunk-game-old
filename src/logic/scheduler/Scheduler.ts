const DEFAULT_FRAME_DELAY: number = 1000 / 60;

import { PromiseWrapper } from "@helpers/promises";
import { Job, JobOperation, JobTask } from "./Job";

/**
 * Schedules tasks for the game. This class contains the core game loop.
 */
export class Scheduler extends PromiseWrapper<void> {
    private jobs: Set<Job> = new Set();

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
    public stop() {
        if (this.animationFrameId !== null) {
            cancelAnimationFrame(this.animationFrameId);
            this._resolve();
        }
    }

    private fatalStop(err: Error) {
        if (this.animationFrameId !== null) {
            cancelAnimationFrame(this.animationFrameId);
            this._reject(err);
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
     * Creates a new job object.
     * 
     * @param name The name of the job.
     * @param task The task to run on each frame.
     */
    public static createJob(name: string, task: JobTask): Job {
        const job = new Job(name);
        job.step = task;
        return job;
    }

    private async loop() {
        const currentTime = performance.now();
        const elapsed = currentTime - this._frameTime;

        // Respect the frame delay by rescheduling the loop if the delay has not
        // elapsed yet.
        if (elapsed < this.frameDelay) {
            if (!this.finished)
                this.animationFrameId = requestAnimationFrame(this.boundLoop);
            return;
        }

        let currentJob!: Job;
        try {
            // Run scheduled jobs
            for (currentJob of this.jobs) {
                const status = await currentJob.step();
                switch (status) {
                    // Throw an error to be caught
                    case JobOperation.Abort:
                        this.fatalStop(new Error(`Job '${currentJob.name}' aborted.`));

                    // Reschedule the job for the next frame
                    case JobOperation.Detach:
                        this.jobs.delete(currentJob);
                        break;
                }
            }
        }
        catch (cause) {
            // Stop the scheduler and propagate the error to the promise
            this.fatalStop(new Error(`Job '${currentJob.name}' threw an exception.`,
                { cause }));
        }

        // As long as the loop isn't stopped, reschedule it for the next frame.
        if (!this.finished) {
            this._frameTime = currentTime - (elapsed % this.frameDelay);
            this.animationFrameId = requestAnimationFrame(this.boundLoop);
        }
    }
}
