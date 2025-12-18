import { Router } from "express";
import { validate } from "@/middleware/validate.js";
import { initializeAccountSchema } from "@/schemas/initializeAccount.js";
import { depositSchema } from "@/schemas/deposit.js";
import { withdrawSchema } from "@/schemas/withdraw.js";
import { internalTransferSchema } from "@/schemas/internalTransfer.js";
import { externalTransferSchema } from "@/schemas/externalTransfer.js";

import { initializeAccountController } from "@/controllers/initializeAccountController.js";
import { depositController } from "@/controllers/depositController.js";
import { withdrawController } from "@/controllers/withdrawController.js";
import {
  internalTransferController,
  externalTransferController,
} from "@/controllers/transferController.js";
import { airdropSchema } from "@/schemas/airdrop";
import { airdropController } from "@/controllers/airdropController";
import { accountBalanceSchema } from "@/schemas/balance";
import { accountBalanceController } from "@/controllers/accountBalanceController";

const router = Router();

router.get("/account-balance/:accountId", accountBalanceController);

router.post("/airdrop-sol", validate(airdropSchema), airdropController);

router.post(
  "/initialize-account",
  validate(initializeAccountSchema),
  initializeAccountController,
);

router.post("/deposit", validate(depositSchema), depositController);

router.post("/withdraw", validate(withdrawSchema), withdrawController);

router.post(
  "/transfer/internal",
  validate(internalTransferSchema),
  internalTransferController,
);

router.post(
  "/transfer/external",
  validate(externalTransferSchema),
  externalTransferController,
);

export default router;
