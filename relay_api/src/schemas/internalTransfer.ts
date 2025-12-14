import { z } from "zod";

export const internalTransferSchema = z.object({
  senderIndex: z.number().int().min(0),
  receiverIndex: z.number().int().min(0),
  amount: z.coerce.bigint().nonnegative(),
});

export type InternalTransferSchema = z.infer<typeof internalTransferSchema>;
