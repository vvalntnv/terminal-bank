import { Request, Response, NextFunction } from "express";
import { depositService } from "@/services/instructions/deposit.js";
import { DepositSchema } from "@/schemas/deposit.js";

export async function depositController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as DepositSchema;
    const signature = await depositService(data);
    res.json({ signature }).send();
  } catch (error) {
    next(error);
  }
}
