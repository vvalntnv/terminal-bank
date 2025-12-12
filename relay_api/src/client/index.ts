import { getWalletFromRequest } from "@/utils/walletUtils.js";
import {
  createSolanaRpc,
  sendAndConfirmTransactionFactory,
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
  sendAndConfirmTransaction: ReturnType<
    typeof sendAndConfirmTransactionFactory
  >;
};

let client: Client | undefined;
export function createClient(): Client {
  if (!client) {
    const rpc = createSolanaRpc("http://127.0.0.1:8899");
    const rpcSubscriptions = createSolanaRpcSubscriptions(
      "ws://127.0.0.1:8900",
    );

    const sendAndConfirmTransaction = sendAndConfirmTransactionFactory({
      rpc,
      rpcSubscriptions,
    });

    client = {
      wallet: getWalletFromRequest(),
      rpc,
      rpcSubscriptions,
      sendAndConfirmTransaction,
    };
  } else {
    client.wallet = getWalletFromRequest();
  }
  return client;
}
