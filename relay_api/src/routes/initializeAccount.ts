import { createClient } from "@/client/index.js";
import { getInitializeInstructionAsync } from "@/instructionClient";
import { getMintAddress } from "@/utils/mintUtils";
import { getUserAccountPda } from "@/utils/userUtils";
import {
  createTransactionMessage,
  setTransactionMessageFeePayerSigner,
  TransactionSigner,
  pipe,
  appendTransactionMessageInstruction,
  setTransactionMessageLifetimeUsingBlockhash,
  signTransactionMessageWithSigners,
  assertIsSendableTransaction,
  assertIsTransactionWithBlockhashLifetime,
  getSignatureFromTransaction,
} from "@solana/kit";

export interface InitializeUserAccountArgs {
  userAccountId: number;
  userAccountName: string;
  user: TransactionSigner;
}

export async function initializeUserAccount({
  userAccountId,
  userAccountName,
  user,
}: InitializeUserAccountArgs) {
  const client = createClient();
  const levMint = getMintAddress();
  const [userAccountPda, _] = await getUserAccountPda(
    client.wallet.address,
    userAccountId,
  );

  const instruction = await getInitializeInstructionAsync({
    index: userAccountId,
    accountName: userAccountName,
    user,
    levMint,
    userAccountPda,
  });

  const { value: latestBlockhash } = await client.rpc
    .getLatestBlockhash()
    .send();

  const txMessage = pipe(
    createTransactionMessage({ version: 0 }),
    (tx) => setTransactionMessageFeePayerSigner(client.wallet, tx),
    (tx) => setTransactionMessageLifetimeUsingBlockhash(latestBlockhash, tx),
    (tx) => appendTransactionMessageInstruction(instruction, tx),
  );
  const tx = await signTransactionMessageWithSigners(txMessage);
  assertIsSendableTransaction(tx);
  assertIsTransactionWithBlockhashLifetime(tx);

  client.sendAndConfirmTransaction(tx, { commitment: "confirmed" });

  return getSignatureFromTransaction(tx);
}
