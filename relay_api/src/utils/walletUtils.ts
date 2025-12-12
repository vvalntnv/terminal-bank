import { ForbiddenError } from "@/errors/generalErrors.js";
import { asyncLocalStorage } from "./asyncStorage.js";

export function getWalletFromRequest() {
  const wallet = asyncLocalStorage.getStore()?.keypair;

  if (wallet === undefined) {
    throw new ForbiddenError("NO wallet provided");
  }

  return wallet;
}
