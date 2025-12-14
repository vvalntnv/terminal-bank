import {
  createTransactionMessage,
  setTransactionMessageFeePayerSigner,
  pipe,
  appendTransactionMessageInstruction,
  setTransactionMessageLifetimeUsingBlockhash,
  signTransactionMessageWithSigners,
  assertIsSendableTransaction,
  assertIsTransactionWithBlockhashLifetime,
  getSignatureFromTransaction,
  Instruction,
} from "@solana/kit";
import { createClient } from "@/client/index.js";

export async function sendTransaction(
  client: ReturnType<typeof createClient>,
  instruction: Instruction,
) {
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

  try {
    await client.sendAndConfirmTransaction(tx, { commitment: "confirmed" });
  } catch (e) {
    console.error("Transaction failed:", e);
    throw e;
  }

  return getSignatureFromTransaction(tx);
}
