import { getWalletFromRequest } from "@/utils/walletUtils";
import {
  createSolanaRpc,
  sendAndConfirmTransactionFactory,
  createSolanaRpcSubscriptions,
  type MessageSigner,
  type TransactionSigner,
  Rpc,
  RpcSubscriptions,
  SolanaRpcApi,
  SolanaRpcSubscriptionsApi,
  appendTransactionMessageInstruction,
  BaseTransactionMessage,
  TransactionMessageWithFeePayer,
} from "@solana/kit";

import {
  estimateComputeUnitLimitFactory,
  getSetComputeUnitLimitInstruction,
} from "@solana-program/compute-budget";

export type Client = {
  rpc: Rpc<SolanaRpcApi>;
  rpcSubscriptions: RpcSubscriptions<SolanaRpcSubscriptionsApi>;
  wallet: TransactionSigner & MessageSigner;
  sendAndConfirmTransaction: ReturnType<
    typeof sendAndConfirmTransactionFactory
  >;
  estimateAndSetComputeUnitLimit: ReturnType<
    typeof estimateAndSetComputeUnitLimitFactory
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
    const estimateAndSetComputeUnitLimit =
      estimateAndSetComputeUnitLimitFactory({ rpc });

    client = {
      wallet: getWalletFromRequest(),
      rpc,
      estimateAndSetComputeUnitLimit,
      rpcSubscriptions,
      sendAndConfirmTransaction,
    };
  } else {
    client.wallet = getWalletFromRequest();
  }
  return client;
}

function estimateAndSetComputeUnitLimitFactory(
  ...params: Parameters<typeof estimateComputeUnitLimitFactory>
) {
  const estimateComputeUnitLimit = estimateComputeUnitLimitFactory(...params);
  return async <
    T extends BaseTransactionMessage & TransactionMessageWithFeePayer,
  >(
    transactionMessage: T,
  ) => {
    const computeUnitsEstimate =
      await estimateComputeUnitLimit(transactionMessage);
    return appendTransactionMessageInstruction(
      getSetComputeUnitLimitInstruction({ units: computeUnitsEstimate }),
      transactionMessage,
    );
  };
}
