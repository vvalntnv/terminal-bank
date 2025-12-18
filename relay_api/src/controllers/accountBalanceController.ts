import { createClient } from "@/client";
import { AccountBalanceRequest, accountBalanceSchema } from "@/schemas/balance";
import { getUserAccountPda, getUserAta } from "@/utils/userUtils";
import {
  Address,
  getAddressDecoder,
  getAddressFromPublicKey,
} from "@solana/kit";
import { Request, Response, NextFunction } from "express";

export async function accountBalanceController(
  req: Request<{ accountId: number }>,
  res: Response,
  next: NextFunction,
) {
  try {
    const client = createClient();
    // validate the data
    const data = accountBalanceSchema.parse(req.params);

    const [pda] = await getUserAccountPda(
      client.wallet.address,
      data.accountId,
    );

    const ata = await getUserAta(pda);
    const ataAddress = ata.toBase58() as Address;
    // const ataAddress = await getAddressFromPublicKey(ata);
    const { value: accountData } = await client.rpc
      .getTokenAccountBalance(ataAddress)
      .send();

    const totalLevs = accountData.uiAmountString;
    res.json({ amount: totalLevs });
  } catch (error) {
    next(error);
  }
}
