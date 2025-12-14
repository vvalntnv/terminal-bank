import { Request, Response, NextFunction } from "express";
import { withdrawService } from "@/services/instructions/withdraw.js";
import { WithdrawSchema } from "@/schemas/withdraw.js";

export async function withdrawController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as WithdrawSchema;
    const signature = await withdrawService(data);
    res.json({ signature }).send();
  } catch (error) {
    next(error);
  }
}
