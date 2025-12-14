import { Request, Response, NextFunction } from "express";
import { 
  internalTransferService, 
  externalTransferService 
} from "@/services/instructions/transfer.js";
import { InternalTransferSchema } from "@/schemas/internalTransfer.js";
import { ExternalTransferSchema } from "@/schemas/externalTransfer.js";

export async function internalTransferController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as InternalTransferSchema;
    const signature = await internalTransferService(data);
    res.json({ signature }).send();
  } catch (error) {
    next(error);
  }
}

export async function externalTransferController(
  req: Request,
  res: Response,
  next: NextFunction,
) {
  try {
    const data = req.body as ExternalTransferSchema;
    const signature = await externalTransferService(data);
    res.json({ signature }).send();
  } catch (error) {
    next(error);
  }
}
