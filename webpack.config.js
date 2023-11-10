const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const TsconfigPathsPlugin = require("tsconfig-paths-webpack-plugin");

const dotenv = require("dotenv");
dotenv.config();

const dev = process.env.NODE_ENV !== "production";
if (!process.env.DEV_PORT && dev) {
    console.error("Please specify a port with the DEV_PORT environment variable.");
    process.exit(1);
}

module.exports = {
    entry: "./src/index.ts",
    module: {
        rules: [
            {
                test: /\.ts?$/,
                use: "ts-loader",
                exclude: /node_modules/
            },
            {
                test: /\.s[ac]ss$/i,
                use: [ "style-loader", "css-loader", "sass-loader" ]
            }
        ]
    },
    devtool: dev ? "eval-cheap-module-source-map" : "source-map",
    resolve: {
        extensions: [ ".ts", ".js" ],
        plugins: [
            new TsconfigPathsPlugin({
                configFile: path.resolve(__dirname, "tsconfig.json")
            })
        ]
    },
    output: {
        filename: "bundle.js",
        path: path.resolve(__dirname, "static")
    },
    plugins: [
        new HtmlWebpackPlugin({
            template: "./src/index.html",
            favicon: "./static/favicon.ico"
        })
    ],
    devServer: {
        static: path.resolve(__dirname, "static"),
        compress: true,
        port: process.env.DEV_PORT
    }
};