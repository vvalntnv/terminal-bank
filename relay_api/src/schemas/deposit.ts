import { z } from "zod";

export const depositSchema = z.object({
  index: z.number().int().min(0),
  amount: z.coerce.bigint().nonnegative(),
});

export type DepositSchema = z.infer<typeof depositSchema>;
