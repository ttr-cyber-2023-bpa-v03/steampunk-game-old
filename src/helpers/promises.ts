// @helpers/promises.ts
// Provides generic types, methods, and classes for working with promises or thenables.

/**
 * A function that resolves a promise.
 * 
 * @param value The value that the promise should be resolved with.
 * @returns A value that is used to resolve the promise.
 */
export type Resolver<T> = (value: T | PromiseLike<T>) => void;

/**
 * A function that rejects a promise.
 * 
 * @param err The error that the promise should be rejected with.
 * @returns A value that is used to resolve the promise.
 */
export type Rejector = (err?: any) => void;

/**
 * A callback that is called when a promise is resolved.
 * 
 * @param value The value that the promise was resolved with.
 * @returns A value that is used to resolve the promise.
 */
export type FulfillmentCallback<T, TResult1 = T> = (value: T) => TResult1 | PromiseLike<TResult1>;

/**
 * A callback that is called when a promise is rejected.
 * 
 * @param reason The reason why the promise was rejected.
 * @returns A value that is used to resolve the promise.
 */
export type RejectionCallback<T> = (reason: any) => T | PromiseLike<T>;

/**
 * Provides an interface for resolving a promise from external scopes.
 * The `await` keyword can be used with an object that implements this interface.
 * 
 * I only created this because TypeScript doesn't provide me with a Thenable interface.
 */
export interface Thenable<T> {
    then<TResult1 = T, TResult2 = never>(
        onfulfilled?: FulfillmentCallback<T, TResult1>,
        onrejected?: RejectionCallback<TResult2>
    ): Promise<TResult1 | TResult2>;
}

/**
 * Provides a toolset for wrapping a promise.
 * 
 * @implements {Thenable<T>}
 */
export class PromiseWrapper<T> implements Thenable<T> {
    private promise: Promise<T>;

    /**
     * Method that resolves the inner promise.
     */
    protected _resolve!: Resolver<T>;

    /**
     * Method that rejects the inner promise.
     */
    protected _reject!: Rejector;

    /**
     * The value that the promise was resolved with.
     * Unsafe to use; use for optimization purposes only.
     * 
     * @see {Promise.value}
     */
    public value!: T;

    // Set whenever the promise is finished.
    private _finished: boolean = false;

    /**
     * Whether or not the promise is finished.
     */
    public get finished(): boolean {
        return this._finished;
    }

    /**
     * Creates a new OuterPromise.
     */
    public constructor() {
        // Create a new promise, then assign the resolve/reject methods to the class.
        // Wait for the promise to resolve, then set resolved and marshal the value.
        // Getting the promise to propagate errors correctly is annoying...
        this.promise = new Promise<T>((resolve, reject) => {
            this._resolve = resolve;
            this._reject = reject;
        }).then(this.middleMarshal.bind(this)).catch((err: any) => {
            this.middleMarshal.bind(this, err)();
            throw err;
        });
    }

    // Wait for resolve/reject then marshal the value and set finished.
    private middleMarshal(value: any): T {
        this._finished = true;
        this.value = value;
        return value;
    }

    /**
     * @see {Promise.then}
     * @see {Thenable.then}
     */
    public then<TResult1 = T, TResult2 = never>(
          onfulfilled?: FulfillmentCallback<T, TResult1>,
          onrejected?: RejectionCallback<TResult2>): Promise<TResult1 | TResult2> {
        return this.promise.then(onfulfilled, onrejected);
    }

    /**
     * @see {Promise.catch}
     */
    public catch(onrejected?: RejectionCallback<T>): Promise<T> {
        return this.promise.catch(onrejected);
    }

    /**
     * @see {Promise.finally}
     */
    public finally(callback: () => void): Promise<T> {
        return this.promise.finally(callback);
    }
}

/**
 * Provides an interface for resolving a promise from external scopes.
 * This is especially useful for resolving promises in classes.
 * 
 * @extends {PromiseWrapper<T>}
 * @implements {Thenable<T>}
 */
export class ExternalPromise<T> extends PromiseWrapper<T> implements Thenable<T> {
    public get resolve(): Resolver<T> { return this._resolve; }
    public get reject(): Rejector { return this._reject; }
}