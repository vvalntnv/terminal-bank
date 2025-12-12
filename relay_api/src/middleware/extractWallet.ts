import { BadRequestError, ForbiddenError } from "@/errors/generalErrors.js";
import { asyncLocalStorage } from "@/utils/asyncStorage.js";
import { createKeyPairSignerFromBytes, getBase58Encoder } from "@solana/kit";
import { Request, Response, NextFunction } from "express";

export async function extractWalletDataMiddleware(
  req: Request,
  _res: Response,
  next: NextFunction,
) {
  const privateKey = req.headers["x-priv-key"];
  const publicKey = req.headers["x-pub-key"];

  if (privateKey === undefined || publicKey === undefined) {
    throw new ForbiddenError("Wallet data not full");
  }
  if (typeof privateKey !== "string" || typeof publicKey !== "string") {
    throw new BadRequestError("Wallet format not correct");
  }

  try {
    const keypairBytes = getBase58Encoder().encode(privateKey);
    const keypair = await createKeyPairSignerFromBytes(keypairBytes);

    if (keypair.address !== publicKey) {
      throw new ForbiddenError("Public key mismatch");
    }

    asyncLocalStorage.run({ keypair }, () => {
      next();
    });
  } catch (error) {
    if (error instanceof ForbiddenError || error instanceof BadRequestError) {
      throw error;
    }
    // Catch invalid base58 or keypair creation errors
    throw new BadRequestError("Invalid wallet credentials");
  }
}
