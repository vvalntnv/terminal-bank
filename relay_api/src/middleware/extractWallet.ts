import { BadRequestError, ForbiddenError } from "@/errors/generalErrors.js";
import { asyncLocalStorage } from "@/utils/asyncStorage.js";
import { Request, Response, NextFunction } from "express";

export async function extractWalletDataMiddleware(
  req: Request,
  _res: Response,
  next: NextFunction,
) {
  const privateKey = req.headers["x-priv-key"];
  const publicKey = req.headers["x-priv-key"];

  if (privateKey === undefined || publicKey === undefined) {
    throw new ForbiddenError("Wallet data not full");
  }
  if (typeof privateKey !== "string" || typeof publicKey !== "string") {
    throw new BadRequestError("Wallet format not correct");
  }

  asyncLocalStorage.run(
    { walletPrivkey: privateKey, walletPubkey: publicKey },
    () => {
      next();
    },
  );
}
