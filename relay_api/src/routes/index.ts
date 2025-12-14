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
  externalTransferController 
} from "@/controllers/transferController.js";

const router = Router();

router.post(
  "/initialize-account",
  validate(initializeAccountSchema),
  initializeAccountController,
);

router.post(
  "/deposit",
  validate(depositSchema),
  depositController,
);

router.post(
  "/withdraw",
  validate(withdrawSchema),
  withdrawController,
);

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
