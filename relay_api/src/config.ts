import dotenv from "dotenv";

dotenv.config();

interface Config {
  port: number;
  nodeEnv: string;
  solanaRpcEndpoint: string;
  mintAddress: string;
  mintAuthAddress: string;
}

const conf: Config = {
  port: Number(process.env.PORT || "8080"),
  nodeEnv: process.env.NODE_ENV || "development",
  solanaRpcEndpoint: process.env.SOLANA_RPC || "http://localhost:8899", // default test-validator port
  mintAddress:
    process.env.MINT_ADDRESS || "DP7EEP8wPWK6WbBVxHfLXEEW1Hg3kxV5iRE4eUwUQqdd",
  mintAuthAddress:
    process.env.MINT_AUTH_ADDRESS ||
    "8Qru4etbqJDGBWSA2UfMStnuwAPQ2q9wCaG1VG58S812",
};

export default conf;
