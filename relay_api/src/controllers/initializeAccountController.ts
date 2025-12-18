import { Request, Response, NextFunction } from "express";
import { initializeAccountService } from "@/services/instructions/initializeAccount.js";
import { InitializeAccountSchema } from "@/schemas/initializeAccount.js";

export async function initializeAccountController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as InitializeAccountSchema;
    const ataAddress = await initializeAccountService(data);
    res.json({ address: ataAddress.toString() }).send();
  } catch (error) {
    next(error);
  }
}
