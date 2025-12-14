import { z } from "zod";

export const withdrawSchema = z.object({
  index: z.number().int().min(0),
  amount: z.coerce.bigint().nonnegative(),
});

export type WithdrawSchema = z.infer<typeof withdrawSchema>;
