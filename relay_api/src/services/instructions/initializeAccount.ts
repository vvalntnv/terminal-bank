import { createClient } from "@/client/index.js";
import { getInitializeInstructionAsync } from "@/instructionClient/index.js";
import { getMintAddress } from "@/utils/mintUtils.js";
import { getUserAccountPda } from "@/utils/userUtils.js";
import { InitializeAccountSchema } from "@/schemas/initializeAccount.js";
import { sendTransaction } from "@/utils/transactionUtils.js";

export async function initializeAccountService(args: InitializeAccountSchema) {
  const client = createClient();
  const levMint = getMintAddress();
  const [userAccountPda, _] = await getUserAccountPda(
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

  return await sendTransaction(client, instruction);
}
