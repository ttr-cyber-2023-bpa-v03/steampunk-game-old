/**
 * Determines what action the scheduler should take after the job runs its step
 * function.
 */
export enum JobOperation {
    /**
     * Reschedule job for next frame.
     */
    Resume,

    /**
     * Job is finished, detach from scheduler.
     */
    Detach,

    /**
     * Fatal error in job, abort scheduler and throw error.
     */
    Abort
}

/**
 * A function that runs on each frame.
 */
export type JobTask = () => Promise<JobOperation>;

export class Job {
    public readonly name: string;
    
    /**
     * Creates a new job object.
     * @param name The name of the job.
     */
    public constructor(name: string) {
        this.name = name;
    }

    /**
     * Logic to run on each frame.
     */
    public step(): Promise<JobOperation> {
        throw new Error("Not implemented");
    }
}