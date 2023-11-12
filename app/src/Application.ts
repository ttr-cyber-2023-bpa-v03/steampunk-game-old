import { BrowserWindow } from "electron";
import * as path from "path";

export default class Application {
    static window: Electron.BrowserWindow | null;
    static application: Electron.App;
    static BrowserWindow: typeof BrowserWindow;

    private static onWindowAllClosed() {
        if (process.platform !== "darwin")
            Application.application.quit();
    }

    private static onClose() {
        Application.window = null;
    }

    private static onReady() {
        // Initialize window
        Application.window = new Application.BrowserWindow({ width: 800, height: 600 });
        Application.window.on("closed", Application.onClose);

        let gameUrl;
        if (process.env.NODE_ENV === "development")
            gameUrl = "http://localhost:3000";
        else
            gameUrl = "file://" + path.join(process.resourcesPath, "game", "index.html");

        // Load game HTML
        Application.window.loadURL(gameUrl);
    }

    static main(app: Electron.App, window: typeof BrowserWindow) {
        Application.BrowserWindow = window;
        Application.application = app;
        Application.application.on("window-all-closed", Application.onWindowAllClosed);
        Application.application.on("ready", Application.onReady);
    }
}