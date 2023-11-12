import { app as electronApp, BrowserWindow } from "electron";
import Application from "./Application";

electronApp.setName("steampunk-game");
Application.main(electronApp, BrowserWindow);