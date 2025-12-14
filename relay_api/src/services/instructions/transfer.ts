import { createClient } from "@/client/index.js";
import {
  getInternalTransferInstructionAsync,
  getExternalTransferInstructionAsync,
} from "@/instructionClient/index.js";
import { getMintAddress } from "@/utils/mintUtils.js";
import { getUserAccountPda } from "@/utils/userUtils.js";
import { InternalTransferSchema } from "@/schemas/internalTransfer.js";
import { ExternalTransferSchema } from "@/schemas/externalTransfer.js";
import { address } from "@solana/kit";
import { sendTransaction } from "@/utils/transactionUtils.js";

export async function internalTransferService(args: InternalTransferSchema) {
  const client = createClient();
  const levMint = getMintAddress();

  // Sender setup
  const [senderAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.senderIndex,
  );

  // Receiver setup
  const [receiverAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.receiverIndex,
  );

  const instruction = await getInternalTransferInstructionAsync({
    senderIndex: args.senderIndex,
    receiverIndex: args.receiverIndex,
    amount: args.amount,
    user: client.wallet,
    levMint,
    senderAccountPda,
    receiverAccountPda,
  });

  return await sendTransaction(client, instruction);
}

export async function externalTransferService(args: ExternalTransferSchema) {
  const client = createClient();
  const levMint = getMintAddress();

  // Sender setup
  const [senderAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.senderIndex,
  );
  const receiverAddress = address(args.to);

  // Receiver setup (External transfers always go to account index 1)
  const [receiverPdaAccount] = await getUserAccountPda(
    receiverAddress,
    1,
  );

  const instruction = await getExternalTransferInstructionAsync({
    senderIndex: args.senderIndex,
    amount: args.amount,
    user: client.wallet,
    levMint,
    senderPdaAccount: senderAccountPda,
    receiverPdaAccount,
    receiverAddress,
  });

  return await sendTransaction(client, instruction);
}
