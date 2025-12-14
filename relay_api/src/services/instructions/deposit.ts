import { createClient } from "@/client/index.js";
import { getDepositInstructionAsync } from "@/instructionClient/index.js";
import { getMintAddress, getMintAuthAddress } from "@/utils/mintUtils.js";
import { getUserAccountPda } from "@/utils/userUtils.js";
import { DepositSchema } from "@/schemas/deposit.js";
import { sendTransaction } from "@/utils/transactionUtils.js";

export async function depositService(args: DepositSchema) {
  const client = createClient();
  const levMint = getMintAddress();
  const mintAuthorityPda = getMintAuthAddress();

  const [userAccountPda] = await getUserAccountPda(
    client.wallet.address,
    args.index,
  );

  const instruction = await getDepositInstructionAsync({
    index: args.index,
    amount: args.amount,
    user: client.wallet,
    levMint,
    userAccountPda,
    mintAuthorityPda,
  });

  return await sendTransaction(client, instruction);
}
