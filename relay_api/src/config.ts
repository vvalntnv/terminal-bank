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
  solanaRpcEndpoint: process.env.SOLANA_RPC || "https://localhost:8899", // default test-validator port
  mintAddress:
    process.env.MINT_ADDRESS || "41TpbodVtBJCGckytTuMQHzjza63NYX2BNwJRCs7fhZR",
  mintAuthAddress:
    process.env.MINT_AUTH_ADDRESS ||
    "AzipKXtgdz6cf9K8MLBJSTAJQPQYWSwJJsMXumXwe5Mh",
};

export default conf;
