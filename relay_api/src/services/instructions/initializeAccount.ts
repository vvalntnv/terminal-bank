import { createClient } from "@/client/index.js";
import { getInitializeInstructionAsync } from "@/instructionClient/index.js";
import { getMintAddress } from "@/utils/mintUtils.js";
import { getUserAccountPda, getUserAta } from "@/utils/userUtils.js";
import { InitializeAccountSchema } from "@/schemas/initializeAccount.js";
import { sendTransaction } from "@/utils/transactionUtils.js";
import { getAddressFromPublicKey } from "@solana/kit";

export async function initializeAccountService(args: InitializeAccountSchema) {
  const client = createClient();
  const levMint = getMintAddress();
  const [userAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.userAccountId,
  );

  const instruction = await getInitializeInstructionAsync({
    index: args.userAccountId,
    accountName: args.userAccountName,
    user: client.wallet,
    levMint,
    userAccountPda,
  });

  await sendTransaction(client, instruction);

  const [pda] = await getUserAccountPda(
    client.wallet.address,
    args.userAccountId,
  );
  const userAta = await getUserAta(pda);
  return userAta.toString();
}
