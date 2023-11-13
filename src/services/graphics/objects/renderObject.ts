import { vec2 } from "gl-matrix"
import { Scene } from "../scene";
import { Shader } from "../shader";

export abstract class RenderObject {
    /**
     * The parent scene of the object.
     */
    public scene: Scene;

    public shader!: Shader;

    /**
     * This determines the order in which objects are rendered. Objects with a higher
     * z-index will render on top of objects with a lower z-index.
     */
    public zIndex: number = 0;

    public constructor(scene: Scene) {
        this.scene = scene;
    }

    /**
     * The position of the object in the scene. This will determine where in the scene
     * the object will be rendered.
     */
    public position: vec2 = vec2.fromValues(0, 0);

    /**
     * The anchor point of an object. This is the relative placement point. For example,
     * if the anchor point is (0.5, 0.5), then the object will be centered at its position.
     */
    public anchor: vec2 = vec2.fromValues(0, 0);

    /**
     * The size of the object. This will determine how large the object will be rendered.
     */
    public size: vec2 = vec2.fromValues(100, 100);
    
    /**
     * The visibility of the object. If this is false, then the object will be skipped
     * during rendering.
     */
    public visible: boolean = false;

    /**
     * Allocates the object's resources.
     */
    public abstract allocate(): Promise<void>;

    /**
     * Renders the object.
     */
    public abstract draw(): void;
}