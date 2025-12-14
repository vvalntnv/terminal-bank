import { z } from "zod";

export const externalTransferSchema = z.object({
  senderIndex: z.number().int().min(0),
  to: z.string().min(1, "Recipient address is required"),
  amount: z.coerce.bigint().nonnegative(),
});

export type ExternalTransferSchema = z.infer<typeof externalTransferSchema>;
