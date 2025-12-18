import { createClient } from "@/client";
import { LAMPORTS_PER_SOL } from "@/constants";
import { AirdropType } from "@/schemas/airdrop";
import { airdropFactory, assertIsLamports } from "@solana/kit";
import { Request, Response, NextFunction } from "express";

export async function airdropController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as AirdropType;

    const client = createClient();
    const airdrop = airdropFactory({
      rpc: client.rpc,
      rpcSubscriptions: client.rpcSubscriptions,
    });

    const amount = BigInt(data.amountToAirdrop) * LAMPORTS_PER_SOL;
    assertIsLamports(amount);

    await airdrop({
      recipientAddress: client.wallet.address,
      lamports: amount,
      commitment: "confirmed",
    });
    res.json({ status: "airdropped" }).send();
  } catch (error) {
    next(error);
  }
}
