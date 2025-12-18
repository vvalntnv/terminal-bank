import { createClient } from "@/client";
import { getUserAccountPda, getUserAta } from "@/utils/userUtils";
import { Address } from "@solana/kit";
import { Request, Response, NextFunction } from "express";

export async function accountBalanceController(
  req: Request<{ accountId: string }>,
  res: Response,
  next: NextFunction,
) {
  try {
    const client = createClient();
    // validate the data
    const accountId = Number(req.params.accountId);

    const [pda] = await getUserAccountPda(client.wallet.address, accountId);

    const ata = await getUserAta(pda);
    const ataAddress = ata.toBase58() as Address;
    // const ataAddress = await getAddressFromPublicKey(ata);
    const { value: accountData } = await client.rpc
      .getTokenAccountBalance(ataAddress)
      .send();

    const totalLevs = accountData.uiAmountString;
    res.json({ amount: totalLevs }).send();
  } catch (error) {
    next(error);
  }
}
