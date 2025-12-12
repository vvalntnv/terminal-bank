import {
  createSolanaRpc,
  createSolanaRpcSubscriptions,
  type MessageSigner,
  type TransactionSigner,
} from "@solana/kit";
import {
  Rpc,
  RpcSubscriptions,
  SolanaRpcApi,
  SolanaRpcSubscriptionsApi,
} from "@solana/kit";

export type Client = {
  rpc: Rpc<SolanaRpcApi>;
  rpcSubscriptions: RpcSubscriptions<SolanaRpcSubscriptionsApi>;
  wallet: TransactionSigner & MessageSigner;
};

let client: Client | undefined;
export function createClient(): Client {
  if (!client) {
    client = {
      rpc: createSolanaRpc("http://127.0.0.1:8899"),
      rpcSubscriptions: createSolanaRpcSubscriptions("ws://127.0.0.1:8900"),
    };
  }
  return client;
}
