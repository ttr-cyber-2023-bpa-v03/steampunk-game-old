const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const TsconfigPathsPlugin = require("tsconfig-paths-webpack-plugin");
const CopyWebpackPlugin = require("copy-webpack-plugin");
const TerserPlugin = require("terser-webpack-plugin");


const dotenv = require("dotenv");
dotenv.config();

const isDev = process.env.NODE_ENV === "development";

module.exports = {
    entry: "./src/index.ts",
    target: "web",
    devtool: isDev ? "eval-cheap-module-source-map" : "source-map",
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
        path: path.resolve(__dirname, "dist")
    },
    plugins: [
        new HtmlWebpackPlugin({
            template: "./src/index.html",
            favicon: "./assets/favicon.ico"
        }),
        new CopyWebpackPlugin({
            patterns: [
                { from: "./assets", to: "./assets" },
                { from: "./src/shaders", to: "./shaders" }
            ]
        })
    ],
    optimization: {
        minimize: !isDev,
        minimizer: [
            new TerserPlugin({
                parallel: true,
                terserOptions: {
                    sourceMap: true,
                    mangle: true,
                    format: { comments: false },
                },
                extractComments: false
            })
        ]
    },
    devServer: {
        static: path.resolve(__dirname, "dist"),
        compress: true,
        port: 3000
    }
};