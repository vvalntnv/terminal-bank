import { createClient } from "@/client/index.js";
import { getWithdrawInstructionAsync } from "@/instructionClient/index.js";
import { getMintAddress } from "@/utils/mintUtils.js";
import { getUserAccountPda } from "@/utils/userUtils.js";
import { WithdrawSchema } from "@/schemas/withdraw.js";
import { sendTransaction } from "@/utils/transactionUtils.js";

export async function withdrawService(args: WithdrawSchema) {
  const client = createClient();
  const levMint = getMintAddress();

  const [userAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.index,
  );

  const instruction = await getWithdrawInstructionAsync({
    index: args.index,
    amount: args.amount,
    user: client.wallet,
    levMint,
    userAccountPda,
  });

  return await sendTransaction(client, instruction);
}
