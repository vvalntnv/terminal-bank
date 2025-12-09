import dotenv from "dotenv";

dotenv.config();

interface Config {
  port: number;
  nodeEnv: string;
  solanaRpcEndpoint: string;
}

const conf: Config = {
  port: Number(process.env.PORT || "8080"),
  nodeEnv: process.env.NODE_ENV || "development",
  solanaRpcEndpoint: process.env.SOLANA_RPC || "https://localhost:8899", // default test-validator port
};

export default conf;
